/**
  ******************************************************************************
  * @file    src/main.c
  * @author  MCD Application Team
  * @brief   Main program body of Cortex-M33 Low-Power firmware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdint.h>
#include <stm32mp2xx.h>
#include <stm32mp2xx_hal.h>
#include <stm32mp2xx_hal_ddr.h>
#include <stm32mp2xx_hal_ddr_ddrphy_phyinit.h>
#include <stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom.h>
#include <stm32mp2xx_ll_exti.h>
#include <stm32mp2xx_ll_pwr.h>
#include <stm32mp2xx_ll_usart.h>
#include <stm32mp2_lp_fw_api.h>

#include <main.h>
#include <version.h>

#define RIF_CID1	0x1
#define RIF_CID2	0x2

uint8_t retreg[4096]  __section(".uninit_data");

struct {
	uint32_t CR;
	uint32_t IACR;
	RISAF_Illegal_TypeDef IAR[2];
	RISAF_Region_TypeDef REG[15];
} ddr_risaf_cfg  __section(".uninit_data");

struct {
	uint32_t ISER[16U];
	uint32_t ITNS[16U];
	uint8_t  IPR[496U];
} nvic_cfg __section(".uninit_data");

struct {
	uint32_t ICSR;
	uint32_t VTOR;
	uint32_t AIRCR;
	uint32_t SCR;
	uint32_t CCR;
	uint8_t  SHPR[12U];
	uint32_t SHCSR;
	uint32_t CFSR;
	uint32_t HFSR;
	uint32_t DFSR;
} scb_cfg __section(".uninit_data");

struct {
	USART_TypeDef *uart;
	bool enable;
} uart_cfg __section(".uninit_data");

struct {
	uint32_t EXTI1_C2IMR1;
	uint32_t EXTI1_C2IMR2;
	uint32_t EXTI1_C2IMR3;
	uint32_t EXTI2_C2IMR1;
	uint32_t EXTI2_C2IMR2;
	uint32_t EXTI2_C2IMR3;
	uint32_t exti1_ciden[3];
	uint32_t exti2_ciden[3];
} exti_cfg __section(".uninit_data");

struct {
	uint32_t ciden;
	uint32_t irq_en;
} rtc_cfg __section(".uninit_data");

/*
 * Bitfield of EXTI interruption for the CPU1 wakeup to copy on CPU2 wakeup
 * 1 for each "reserved" interruption or for each wake-up interruption
 * not for both CPU1 and CPU2 (i.e. when copy is not allowed)
 */
#if defined(STM32MP21xxxx)
const uint32_t exti1_rsvd_mask[3] = {
	GENMASK_32(25, 24) | BIT(20),
	GENMASK_32(63 - 32, 58 - 32) | GENMASK_32(51 - 32, 49 - 32) |
	GENMASK_32(46 - 32, 45 - 32) | BIT(42 - 32) |
	GENMASK_32(35 - 32, 33 - 32),
	GENMASK_32(95 - 64, 77 - 64) | BIT(73 - 64) | BIT(71 - 64) |
	GENMASK_32(67 - 64, 64 - 64),
};

const uint32_t exti2_rsvd_mask[3] = {
	GENMASK_32(24, 27) | GENMASK_32(25, 16),
	GENMASK_32(62 - 32, 53 - 32) | GENMASK_32(50 - 32, 38 - 32) |
	BIT(36 - 32) | GENMASK_32(34 - 32, 32 - 32),
	GENMASK_32(95 - 64, 64 - 64),
};
#endif
#if defined(STM32MP23xxxx) || defined(STM32MP25xxxx)
const uint32_t exti1_rsvd_mask[3] = {
	BIT(20),
	GENMASK_32(63 - 32, 58 - 32) | BIT(51 - 32) | BIT(35 - 32),
	GENMASK_32(95 - 64, 85 - 64) | GENMASK_32(82 - 64, 77 - 64) |
	BIT(73 - 64) | BIT(71 - 64) | BIT(69 - 64) |
	GENMASK_32(67 - 64, 64 - 64),};

const uint32_t exti2_rsvd_mask[3] = {
	BIT(28) | GENMASK_32(24, 16),
	GENMASK_32(63 - 32, 62 - 32) | GENMASK_32(60 - 32, 52 - 32) |
	BIT(49 - 32) |  BIT(47 - 32) | GENMASK_32(45 - 32, 34 - 32) |
	BIT(32 - 32),
	GENMASK_32(95 - 64, 77 - 64) | GENMASK_32(75 - 64, 71 - 64) |
	GENMASK_32(69 - 64, 66 - 64),};
#endif

static bool exti_is_allowed(const uint32_t *rsv, uint8_t intr)
{
	uint8_t i = intr / 32;
	uint32_t mask = BIT(intr % 32);

	return (rsv[i] & mask) == 0;
}

static void wakeup_configure(void)
{
	bool exti1_c1_filtering = false;
	bool exti2_c1_filtering = false;
	uint8_t i, j;
	uint32_t mask;
	uint32_t val;

	exti_cfg.EXTI1_C2IMR1 = READ_REG(EXTI1_S->C2IMR1);
	exti_cfg.EXTI1_C2IMR2 = READ_REG(EXTI1_S->C2IMR2);
	exti_cfg.EXTI1_C2IMR3 = READ_REG(EXTI1_S->C2IMR3);

	exti_cfg.EXTI2_C2IMR1 = READ_REG(EXTI2_S->C2IMR1);
	exti_cfg.EXTI2_C2IMR2 = READ_REG(EXTI2_S->C2IMR2);
	exti_cfg.EXTI2_C2IMR3 = READ_REG(EXTI2_S->C2IMR3);

	if (LL_EXTI_IsEnabledC1ProcessorCidFiltering(EXTI1_S)) {
		LL_EXTI_DisableC1ProcessorCidFiltering(EXTI1_S);
		exti1_c1_filtering = true;
	}
	if (LL_EXTI_IsEnabledC1ProcessorCidFiltering(EXTI2_S)) {
		LL_EXTI_DisableC1ProcessorCidFiltering(EXTI2_S);
		exti2_c1_filtering = true;
	}

	for (i = 0; i < 3; i++) {
		exti_cfg.exti1_ciden[i] = 0;
		exti_cfg.exti2_ciden[i] = 0;
	}
	for (i = 0; i < 96; i++) {
		j = i / 32;
		mask = BIT(i % 32);
		if (LL_EXTI_IsEnabledEventCidFiltering(EXTI1_S, i)) {
			LL_EXTI_DisableEventCidFiltering(EXTI1_S, i);
			exti_cfg.exti1_ciden[j] |= mask;
		}
		if (LL_EXTI_IsEnabledEventCidFiltering(EXTI2_S, i)) {
			LL_EXTI_DisableEventCidFiltering(EXTI2_S, i);
			exti_cfg.exti2_ciden[j] |= mask;
		}
	}
	/* Add CPU1 wakeup for CPU2, as CPU1 is in hold boot */
	SET_BIT(EXTI1_S->C2IMR1, READ_REG(EXTI1_S->C1IMR1) & ~exti1_rsvd_mask[0]);
	SET_BIT(EXTI1_S->C2IMR2, READ_REG(EXTI1_S->C1IMR2) & ~exti1_rsvd_mask[1]);
	SET_BIT(EXTI1_S->C2IMR3, READ_REG(EXTI1_S->C1IMR3) & ~exti1_rsvd_mask[2]);
	SET_BIT(EXTI2_S->C2IMR1, READ_REG(EXTI2_S->C1IMR1) & ~exti2_rsvd_mask[0]);
	SET_BIT(EXTI2_S->C2IMR2, READ_REG(EXTI2_S->C1IMR2) & ~exti2_rsvd_mask[1]);
	SET_BIT(EXTI2_S->C2IMR3, READ_REG(EXTI2_S->C1IMR3) & ~exti2_rsvd_mask[2]);

	/* Manage specific wake-up interrupt only for CPU1 */
	/* 17: RTC NS CPU1 => 19: RTC NS CPU2 */
	if (READ_REG(EXTI2_S->C1IMR1) & BIT(17))
		SET_BIT(EXTI2_S->C2IMR1, BIT(19));
	/* 22: RTC S CPU1  => 24: RTC S CPU2 */
	if (READ_REG(EXTI2_S->C1IMR1) & BIT(22))
		SET_BIT(EXTI2_S->C2IMR1, BIT(24));

	/* restore EXTI CID filtering */
	if (exti1_c1_filtering)
		LL_EXTI_EnableC1ProcessorCidFiltering(EXTI1_S);
	if (exti2_c1_filtering)
		LL_EXTI_EnableC1ProcessorCidFiltering(EXTI2_S);

	/*
	 * Disable CID filtering in RTC for each interruption:
	 * enable the interrupt request for CPU2 even if for CPU1 assigned IT
	 */
	rtc_cfg.ciden = 0;
	rtc_cfg.irq_en = 0;
	for (i = 0; i < 6; i++) {
		val = READ_REG(RTC->RCIDCFGR[i]);
		if (val & RTC_RCIDCFGR_CFEN) {
			rtc_cfg.ciden |= BIT(i);
			WRITE_REG(RTC->RCIDCFGR[i], val & ~RTC_RCIDCFGR_CFEN);
		}
	}
	if (!NVIC_GetEnableIRQ(RTC_S_IRQn)) {
		NVIC_EnableIRQ(RTC_S_IRQn);
		rtc_cfg.irq_en |= BIT(0);
	}
	if (!NVIC_GetEnableIRQ(RTC_IRQn)) {
		NVIC_EnableIRQ(RTC_IRQn);
		rtc_cfg.irq_en |= BIT(1);
	}

	/* Enable RCC wakup interrupt */
	LL_RCC_ClearFlag_WKUP();
	LL_RCC_EnableIT_WKUP();
	NVIC_ClearPendingIRQ(RCC_WAKEUP_IRQn);
	NVIC_EnableIRQ(RCC_WAKEUP_IRQn);
	NVIC_SetPriority(RCC_WAKEUP_IRQn, 0);
}

static void wakeup_restore(void)
{
	uint8_t i, j;
	uint32_t mask;

	/* Restore saved IMR values */
	WRITE_REG(EXTI1_S->C2IMR1, exti_cfg.EXTI1_C2IMR1);
	WRITE_REG(EXTI1_S->C2IMR2, exti_cfg.EXTI1_C2IMR2);
	WRITE_REG(EXTI1_S->C2IMR3, exti_cfg.EXTI1_C2IMR3);
	WRITE_REG(EXTI2_S->C2IMR1, exti_cfg.EXTI2_C2IMR1);
	WRITE_REG(EXTI2_S->C2IMR2, exti_cfg.EXTI2_C2IMR2);
	WRITE_REG(EXTI2_S->C2IMR3, exti_cfg.EXTI2_C2IMR3);

	/* Restore RIF configuration: enable CIDEN when disabled */
	for (i = 0; i < 96; i++) {
		j = i / 32;
		mask = BIT(i % 32);
		if (exti_is_allowed(exti1_rsvd_mask, i) &&
		    (exti_cfg.exti1_ciden[j] & mask))
			LL_EXTI_EnableEventCidFiltering(EXTI1_S, i);
		if (exti_is_allowed(exti2_rsvd_mask, i) &&
		    (exti_cfg.exti1_ciden[j] & mask))
			LL_EXTI_EnableEventCidFiltering(EXTI2_S, i);
	}

	/* Restore RTC CID filtering */
	for (i = 0; i < 6; i++) {
		if (rtc_cfg.ciden & BIT(i)) {
			SET_BIT(RTC->RCIDCFGR[i], RTC_RCIDCFGR_CFEN);
		}
	}
	/* Restore RTC IRQ in NVIC */
	if (rtc_cfg.irq_en & BIT(0))
		NVIC_DisableIRQ(RTC_S_IRQn);
	if (rtc_cfg.irq_en & BIT(1))
		NVIC_DisableIRQ(RTC_IRQn);

	/* Disable RCC wakup interrupt */
	NVIC_DisableIRQ(RCC_WAKEUP_IRQn);
	NVIC_ClearPendingIRQ(RCC_WAKEUP_IRQn);
	LL_RCC_DisableIT_WKUP();
	LL_RCC_ClearFlag_WKUP();
}

void uart_init(void)
{
	uint32_t uart_dir;

	uart_cfg.enable = false;
	uart_cfg.uart = (USART_TypeDef *)stm32mp2_lp_fw_get_uart_addr();
	if (uart_cfg.uart) {
		/* Check if the transmitter is enabled */
		uart_dir = LL_USART_GetTransferDirection(uart_cfg.uart);
		if (uart_dir != LL_USART_DIRECTION_TX &&
		    uart_dir != LL_USART_DIRECTION_TX_RX) {
			uart_cfg.uart = NULL;
		} else {
			uart_cfg.enable = true;
		}
	}
}

/* low level LibC access for project */
int io_putchar(int ch)
{
	if (uart_cfg.enable && uart_cfg.uart) {
		while (!LL_USART_IsActiveFlag_TXE_TXFNF(uart_cfg.uart))
			;
		LL_USART_TransmitData8(uart_cfg.uart, ch);
	}

	return ch;
}

int io_getchar(void)
{
	uint8_t ch = 0;
	return ch;
}

__NO_RETURN void Error_Handler(void)
{
	printf("ERROR: system error in low power firmware\r\n");
#ifndef STM32MP2_LP_HALT_ON_ERROR
	/* software reset: set SYSRST in RCC Global Reset Control Set Register */
	WRITE_REG(RCC->GRSTCSETR, RCC_GRSTCSETR_SYSRST_Msk);
#endif
	while (true)
		;
}

static bool is_cpu1_in_reset(void)
{
	uint32_t cpu1_status = HAL_PWR_CPU1State();
	uint32_t cpu1_hold_boot = READ_REG(PWR->CPU1D1SR) & PWR_CPU1D1SR_HOLD_BOOT_Msk;

	return ((cpu1_status == PWR_CPU_CRESET) && !!cpu1_hold_boot);
}

static void save_RISAF4(void)
{
	ddr_risaf_cfg.CR = READ_REG(RISAF4->CR);
	ddr_risaf_cfg.IACR = READ_REG(RISAF4->IACR);

	for (int i = 0; i < 2; i++)
		ddr_risaf_cfg.IAR[i] = READ_REG(RISAF4->IAR[i]);

	for (int i = 0; i < 15; i++) {
		ddr_risaf_cfg.REG[i].CFGR    = READ_REG(RISAF4->REG[i].CFGR);
		ddr_risaf_cfg.REG[i].STARTR  = READ_REG(RISAF4->REG[i].STARTR);
		ddr_risaf_cfg.REG[i].ENDR    = READ_REG(RISAF4->REG[i].ENDR);
		ddr_risaf_cfg.REG[i].CIDCFGR = READ_REG(RISAF4->REG[i].CIDCFGR);
		ddr_risaf_cfg.REG[i].ACFGR   = READ_REG(RISAF4->REG[i].ACFGR);
		ddr_risaf_cfg.REG[i].ASTARTR = READ_REG(RISAF4->REG[i].ASTARTR);
		ddr_risaf_cfg.REG[i].AENDR   = READ_REG(RISAF4->REG[i].AENDR);
		ddr_risaf_cfg.REG[i].ANESTR  = READ_REG(RISAF4->REG[i].ANESTR);
		ddr_risaf_cfg.REG[i].BCFGR   = READ_REG(RISAF4->REG[i].BCFGR);
		ddr_risaf_cfg.REG[i].BSTARTR = READ_REG(RISAF4->REG[i].BSTARTR);
		ddr_risaf_cfg.REG[i].BENDR   = READ_REG(RISAF4->REG[i].BENDR);
		ddr_risaf_cfg.REG[i].BNESTR  = READ_REG(RISAF4->REG[i].BNESTR);
	}
}

static void restore_RISAF4(void)
{
	uint32_t *mkey;
	size_t size;

	for (int i = 0; i < 2; i++)
		WRITE_REG(RISAF4->IAR[i], ddr_risaf_cfg.IAR[i]);

	for (int i = 0; i < 15; i++) {
		WRITE_REG(RISAF4->REG[i].STARTR,  ddr_risaf_cfg.REG[i].STARTR);
		WRITE_REG(RISAF4->REG[i].ENDR,    ddr_risaf_cfg.REG[i].ENDR);
		WRITE_REG(RISAF4->REG[i].CIDCFGR, ddr_risaf_cfg.REG[i].CIDCFGR);
		WRITE_REG(RISAF4->REG[i].ACFGR,   ddr_risaf_cfg.REG[i].ACFGR);
		WRITE_REG(RISAF4->REG[i].ASTARTR, ddr_risaf_cfg.REG[i].ASTARTR);
		WRITE_REG(RISAF4->REG[i].AENDR,   ddr_risaf_cfg.REG[i].AENDR);
		WRITE_REG(RISAF4->REG[i].ANESTR,  ddr_risaf_cfg.REG[i].ANESTR);
		WRITE_REG(RISAF4->REG[i].BCFGR,   ddr_risaf_cfg.REG[i].BCFGR);
		WRITE_REG(RISAF4->REG[i].BSTARTR, ddr_risaf_cfg.REG[i].BSTARTR);
		WRITE_REG(RISAF4->REG[i].BENDR,   ddr_risaf_cfg.REG[i].BENDR);
		WRITE_REG(RISAF4->REG[i].BNESTR,  ddr_risaf_cfg.REG[i].BNESTR);

		/* Activate only when everything is prepared */
		WRITE_REG(RISAF4->REG[i].CFGR, ddr_risaf_cfg.REG[i].CFGR);
	}

	stm32mp2_lp_fw_get_mkey((uint8_t **)&mkey, &size);

	for (int i = 0; i < 4; i++)
		WRITE_REG(RISAF4->KEYR[i], mkey[i]);

	while (!(RISAF4->SR & RISAF_SR_KEYVALID))
		;

	WRITE_REG(RISAF4->CR, ddr_risaf_cfg.CR);
	WRITE_REG(RISAF4->IACR, ddr_risaf_cfg.IACR);
}

static void save_nvic_cfg(void)
{
	for (int i = 0; i < 16; i++) {
		nvic_cfg.ISER[i] = READ_REG(NVIC->ISER[i]);
		nvic_cfg.ITNS[i] = READ_REG(NVIC->ITNS[i]);
	}

	for (int i = 0; i < 496; i++)
		nvic_cfg.IPR[i] = READ_REG(NVIC->IPR[i]);
}

static void restore_nvic_cfg(void)
{
	for (int i = 0; i < 16; i++)
		WRITE_REG(NVIC->ITNS[i], nvic_cfg.ITNS[i]);

	for (int i = 0; i < 496; i++)
		WRITE_REG(NVIC->IPR[i], nvic_cfg.IPR[i]);

	for (int i = 0; i < 16; i++)
		WRITE_REG(NVIC->ISER[i], nvic_cfg.ISER[i]);
}

static void save_scb_cfg(void)
{
	for (int i = 0; i < 12; i++)
		scb_cfg.SHPR[i] = READ_REG(SCB->SHPR[i]);

	scb_cfg.ICSR  = READ_REG(SCB->ICSR);
	scb_cfg.VTOR  = READ_REG(SCB->VTOR);
	scb_cfg.AIRCR = READ_REG(SCB->AIRCR);
	scb_cfg.SCR   = READ_REG(SCB->SCR);
	scb_cfg.CCR   = READ_REG(SCB->CCR);
	scb_cfg.SHCSR = READ_REG(SCB->SHCSR);
	scb_cfg.CFSR  = READ_REG(SCB->CFSR);
	scb_cfg.HFSR  = READ_REG(SCB->HFSR);
	scb_cfg.DFSR  = READ_REG(SCB->DFSR);
}

static void restore_scb_cfg(void)
{
	for (int i = 0; i < 12; i++)
		scb_cfg.SHPR[i] = READ_REG(SCB->SHPR[i]);

	WRITE_REG(SCB->ICSR,  scb_cfg.ICSR);
	WRITE_REG(SCB->VTOR,  scb_cfg.VTOR);
	WRITE_REG(SCB->AIRCR, (0x05fa0000 | (scb_cfg.AIRCR & 0xffff)));
	WRITE_REG(SCB->SCR,   scb_cfg.SCR);
	WRITE_REG(SCB->CCR,   scb_cfg.CCR);
	WRITE_REG(SCB->SHCSR, scb_cfg.SHCSR);
	WRITE_REG(SCB->CFSR,  scb_cfg.CFSR);
	WRITE_REG(SCB->HFSR,  scb_cfg.HFSR);
	WRITE_REG(SCB->DFSR,  scb_cfg.DFSR);
}

static void ddr_init(bool wakeup_from_standby, bool pmic_init)
{
	/* Init DDR */
	static DDR_InitTypeDef DDR_InitStructure;

	if (wakeup_from_standby) {
		HAL_DDR_SR_SetMode(HAL_DDR_SW_SELF_REFRESH_MODE);
		DDR_InitStructure.wakeup_from_standby = true;
		DDR_InitStructure.self_refresh = true;
	} else {
		DDR_InitStructure.wakeup_from_standby = false;
		DDR_InitStructure.self_refresh = false;
	}

	DDR_InitStructure.zdata = 0;
	DDR_InitStructure.clear_bkp = false;

	HAL_DDR_Init(&DDR_InitStructure);
}

static void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		Error_Handler();
}

static void platform_reinit(void)
{
	/* The console uart is lost after Standby */
	uart_cfg.enable = false;

	/* Mask interrupt (primask seems reset by standby) */
	__TZ_set_PRIMASK_NS(1);
	__set_PRIMASK(1);
	restore_scb_cfg();
	/* Configure the system clock */
	SystemClock_Config();
	ddr_init(true, false);
	restore_RISAF4();
	restore_nvic_cfg();
	HAL_SuspendTick();
}

static void platform_init(void)
{
	HAL_DDR_SR_SetMode(HAL_DDR_SW_SELF_REFRESH_MODE);

	uart_cfg.enable = false;
	/* uart initialized and not suspend, i.e. cleared after init */
	if (uart_cfg.uart && stm32mp2_lp_fw_get_uart_addr())
		uart_cfg.enable = true;

	save_RISAF4();
	save_nvic_cfg();
	save_scb_cfg();
	wakeup_configure();
}

static void platform_restore(void)
{
	wakeup_restore();
}

static void save_CPU1CR_access(uint32_t *config, uint32_t *filtering)
{
	*config = LL_PWR_GetAuthorisedCidNSharable(LL_R2);
	*filtering = LL_PWR_IsCidFilteringNSharableEnabled(LL_R2);

	/*
	 * Filtering for PWR resource 2 (CA35)
	 * Enable CID2 (CM33, resource 3) to modify resource 2
	 */
	LL_PWR_SetAuthorisedCidNSharable(LL_R2, RIF_CID2);
	LL_PWR_EnableCidFilteringNSharable(LL_R2);
}

static void restore_CPU1CR_access(uint32_t config, uint32_t filtering)
{
	/*
	 * Filtering for PWR resource 2 (CA35)
	 * Enable CID2 (CM33, resource 3) to modify resource 2
	 */
	LL_PWR_SetAuthorisedCidNSharable(LL_R2, config);
	if (filtering)
		LL_PWR_EnableCidFilteringNSharable(LL_R2);
	else
		LL_PWR_DisableCidFilteringNSharable(LL_R2);
}

int main(void)
{
	uint32_t config;
	uint32_t filtering;
	uint32_t lpmode;

	if (!stm32mp2_lp_fw_check_data_valid())
		Error_Handler();

	if (HAL_DDR_SetRetentionAreaBase((intptr_t)&retreg) != HAL_OK) {
		printf("[LP FW] Error failed to set retention area base\r\n");
		Error_Handler();
	}

	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == RESET) {

		if (stm32mp2_lp_fw_get_lpmode() == STM32MP2_LP_FW_LPMODE_INIT) {
			uart_init();
			printf("INFO: low power firmware " VERSION_FULLSTR "\r\n");

			ddrphy_phyinit_usercustom_saveretregs();

			return 0;
		}

		platform_init();

		/*
		 * If Cortex-A35 is in RESET with HOLD_BOOT,
		 * steal CPU1CR rights to force STANDBY mode
		 * later for this core (Allow{D1,D2}standby).
		 */
		if (is_cpu1_in_reset()) {
			save_CPU1CR_access(&config, &filtering);
			/*
			 * Modify PWR resource 2 power control (CPU1 == CA35)
			 * Set D1 STANDBY mode only for STOP2, LPSTOP2 & LPLVSTOP2
			 * Set D1&D2 STANDBY mode for STANDBY
			 */
			LL_PWR_CPU1_AllowD1standby();
			if (stm32mp2_lp_fw_get_lpmode() == STM32MP2_LP_FW_LPMODE_STANDBY1)
				LL_PWR_CPU1_AllowD2standby();

			/* Reset filtering control */
			restore_CPU1CR_access(config, filtering);
		}

		__HAL_PWR_CLEAR_FLAG();

		if (stm32mp2_lp_fw_get_lpmode() == STM32MP2_LP_FW_LPMODE_STANDBY1) {
			/* DDR won't be accessible after this point */
			if (HAL_DDR_STDBY_SR_Entry() != HAL_OK) {
				printf("[LP FW] Error failed to set DDR in self-refresh for standby\r\n");
				Error_Handler();
			}
			HAL_PWREx_EnableRetRamContentStandbyRetention(PWR_RETENTION_RAM_SUPPLY_ON_STDBY_VBAT);
			HAL_PWR_EnterSTANDBYMode(PWR_STANDBY_1);
			/* We should never go this far in the LP-firmware ! */
			Error_Handler();
		} else {
			if (HAL_DDR_SR_Entry(NULL) != HAL_OK) {
				printf("[LP FW] Error failed to set DDR in self-refresh\r\n");
				Error_Handler();
			}

			switch (stm32mp2_lp_fw_get_lpmode()) {
			case STM32MP2_LP_FW_LPMODE_STOP2:
				lpmode = PWR_REGULATOR_LP_OFF;
				break;
			case STM32MP2_LP_FW_LPMODE_LP_STOP2:
				lpmode = PWR_REGULATOR_LP_ON_LV_OFF;
				break;
			case STM32MP2_LP_FW_LPMODE_LPLV_STOP2:
				lpmode = PWR_REGULATOR_LP_ON_LV_ON;
				break;
			default:
				Error_Handler();
			}
			HAL_PWR_EnterSTOPMode(lpmode, PWR_STOPENTRY_WFI);

			platform_restore();

			if (HAL_DDR_SR_Exit() != HAL_OK) {
				printf("[LP FW] Error DDR failed to get out of self-refresh\r\n");
				Error_Handler();
			}
			/* DDR is now readable/writable */
		}
	} else {
		/* Standby Exit */
		platform_reinit();
		if (HAL_DDR_SR_Exit() != HAL_OK) {
			printf("[LP FW] Error DDR failed to get out of self-refresh\r\n");
			Error_Handler();
		}
	}

	return 0;
}
