/**
  ******************************************************************************
  * @file    stm32mp2_lp_fw_api.h
  * @author  MCD Application Team
  * @brief   This file contains the functions prototypes for the public API
  *          of STM32MP LP firmware.
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

#ifndef STM32MP2_LP_FW_API
#define STM32MP2_LP_FW_API

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * enum suspend mode
 * @STM32MP2_LP_FW_LPMODE_STOP2
 * @STM32MP2_LP_FW_LPMODE_LP_STOP2
 * @STM32MP2_LP_FW_LPMODE_LPLV_STOP2
 * @STM32MP2_LP_FW_LPMODE_STANDBY1
 * @STM32MP2_LP_FW_LPMODE_OFF
 * @STM32MP2_LP_FW_LPMODE_INIT
 */
typedef enum stm32mp2_lp_fw_suspend_mode_e {
	STM32MP2_LP_FW_LPMODE_STOP2 = 0,
	STM32MP2_LP_FW_LPMODE_LP_STOP2,
	STM32MP2_LP_FW_LPMODE_LPLV_STOP2,
	STM32MP2_LP_FW_LPMODE_STANDBY1,
	STM32MP2_LP_FW_LPMODE_OFF,
	STM32MP2_LP_FW_LPMODE_INIT,
} stm32mp2_lp_fw_suspend_mode_t;

//Returns the lpmode desired for this lowpower request
stm32mp2_lp_fw_suspend_mode_t stm32mp2_lp_fw_get_lpmode(void);

//set the lpmode desired for this lowpower request
void stm32mp2_lp_fw_set_lpmode(stm32mp2_lp_fw_suspend_mode_t lpmode);

//set the magic within the sahred data struct to ensure that the data are ready to use
void stm32mp2_lp_fw_mark_data_valid(void);

//test the magic within the shared data struct to ensure that the data are ready to use
bool stm32mp2_lp_fw_check_data_valid(void);

//in case of error the size would be set to 0 and *mkey = NULL
//in case of success the size would be set to the mkey szie and *mkey = uint8_t mkey[size];
void stm32mp2_lp_fw_get_mkey(uint8_t **mkey, size_t *size);

//Copy the mkey[size] array within the shared data.
//Checks if the size passed as args fit with the reserved size for the mkey
bool stm32mp2_lp_fw_set_mkey(uint8_t *mkey, size_t size);

//Execute the low power firmware to reach the suspend mode
int stm32mp2_lp_fw_exec(void);

#endif//STM32MP2_LP_FW_API
