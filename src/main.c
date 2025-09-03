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

#include "stm32mp2_lp_fw_api.h"

void platform_init()
{
	if( ! stm32mp2_lp_fw_check_data_valid())
		while(true);
}

int main(void)
{
	platform_init();
	return -1;
}
