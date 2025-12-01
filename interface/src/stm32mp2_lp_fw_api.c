/**
  ******************************************************************************
  * @file    stm32mp2_lp_fw_api.c
  * @author  MCD Application Team
  * @brief   This file contains the public API of STM32MP LP firmware
  *          with the shared data.
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

#include "stm32mp2_lp_fw_api.h"
#include <stm32mp2xx.h>
#include <string.h>

#define _RISAF_KEY_SIZE_IN_BYTES (256 / 8)
#define SHARED_DATA_TFM_LP_MAGIC 0xBAADCAFE

struct shared_data_tfm_lp {
	uint32_t magic;
	stm32mp2_lp_fw_suspend_mode_t lpmode;
	uint8_t mkey[_RISAF_KEY_SIZE_IN_BYTES];
};

#ifndef STM32MP2_LP_FW_SHARED_DATA_SECTION
static struct shared_data_tfm_lp * const shared_data_ptr =
	(struct shared_data_tfm_lp *)STM32MP2_LP_FW_SHARED_ADDRESS;
#else
struct shared_data_tfm_lp shared_data __section(".shared_data_section");
static struct shared_data_tfm_lp *const shared_data_ptr = &shared_data;
#endif

// Returns the lpmode desired for this lowpower request
stm32mp2_lp_fw_suspend_mode_t stm32mp2_lp_fw_get_lpmode(void)
{
	return shared_data_ptr->lpmode;
}

// Set the lpmode desired for this lowpower request
void stm32mp2_lp_fw_set_lpmode(stm32mp2_lp_fw_suspend_mode_t lpmode)
{
	shared_data_ptr->lpmode = lpmode;
}

// Set the magic within the shared data struct to ensure that the data are ready to use
void stm32mp2_lp_fw_mark_data_valid(void)
{
	shared_data_ptr->magic = SHARED_DATA_TFM_LP_MAGIC;
}

// Test the magic within the shared data struct to ensure that the data are ready to use
bool stm32mp2_lp_fw_check_data_valid(void)
{
	return (shared_data_ptr->magic == SHARED_DATA_TFM_LP_MAGIC);
}

// In case of error the size would be set to 0 and *mkey = NULL
// In case of success the size would be set to the mkey size and *mkey = uint8_t mkey[size];
void stm32mp2_lp_fw_get_mkey(uint8_t **mkey, size_t *size)
{
	if (!stm32mp2_lp_fw_check_data_valid()) {
		*mkey = NULL;
		*size = 0;
		return;
	}
	*mkey = shared_data_ptr->mkey;
	*size = _RISAF_KEY_SIZE_IN_BYTES;
}

// Copy the mkey[size] array within the shared data.
// Checks if the size passed as args fit with the reserved size for the mkey
bool stm32mp2_lp_fw_set_mkey(uint8_t *mkey, size_t size)
{
	/*
	 * Handle the case where the key size is null
	 * In this case we don't want DDR encryption
	 */
	if (!size) {
		memset(shared_data_ptr->mkey, 0, _RISAF_KEY_SIZE_IN_BYTES);
		return true;
	} else if (size != _RISAF_KEY_SIZE_IN_BYTES) {
		return false;
	}
	memcpy(shared_data_ptr->mkey, mkey, size);

	return true;
}

int stm32mp2_lp_fw_exec(void)
{
	int ret;

	/*
	 * Jump to Reset Handler with the offset=4 of LP FW Cortex-M33
	 * vector table (start firmware, so start of RETRAM)
	 */
	void **reset_addr = (void **)(RETRAM_BASE + 4);
	int (*lp_fw_entry)(void) = *reset_addr;

	ret = lp_fw_entry();

	/* Clear magic, it is a protection for unexpected firmware execution */
	shared_data_ptr->magic = 0xDEADBEEF;

	return ret;
}
