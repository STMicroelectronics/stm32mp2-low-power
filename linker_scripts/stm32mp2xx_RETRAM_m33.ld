/**
  ******************************************************************************
  * @file    stm32mp25xx_RETRAM_m33.ld
  * @author  MCD Application Team
  * @brief   Linker script for Cortex-M33 low-power firmware in RETRAM
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

NSC_SIZE             = 8K;
MEMORY
{
  RETRAM (rwx)      : ORIGIN = RETRAM_BASE,    LENGTH = 128K
  RETRAM_APP (rwx)  : ORIGIN = ORIGIN(RETRAM), LENGTH = LENGTH(RETRAM) - NSC_SIZE
}

MEMORY
{
  m_text (rx)       : ORIGIN = ORIGIN(RETRAM_APP),                                                     LENGTH = 38K
  m_data (rwx)      : ORIGIN = ORIGIN(RETRAM_APP) + LENGTH(m_text),                                    LENGTH = 16K
  m_stack (rwx)     : ORIGIN = ORIGIN(RETRAM_APP) + LENGTH(m_text) + LENGTH(m_data),                   LENGTH = 16K
  m_remaining (rwx) : ORIGIN = ORIGIN(RETRAM_APP) + LENGTH(m_text) + LENGTH(m_data)+ LENGTH(m_stack),  LENGTH = 50K
}

REGION_ALIAS("TEXT", m_text);
REGION_ALIAS("DATA", m_data);
REGION_ALIAS("STACK", m_stack);
REGION_ALIAS("REMAINING", m_remaining);

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

ENTRY(Reset_Handler)

SECTIONS
{

  .text :
  {
    KEEP(*(.vectors))
    __Vectors_End = .;
    __Vectors_Size = __Vectors_End - __Vectors;
    __end__ = .;

    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)

    KEEP(*(.eh_frame*))
  } > TEXT

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > TEXT

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > TEXT
  __exidx_end = .;

  /* To copy multiple ROM to RAM sections,
   * uncomment .copy.table section and,
   * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
  /*
  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;
    LONG (__etext)
    LONG (__data_start__)
    LONG (__data_end__ - __data_start__)
    __copy_table_end__ = .;
  } > TEXT
  */

  /* To clear multiple BSS sections,
   * uncomment .zero.table section and,
   * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
  /*
  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    LONG (__bss_start__)
    LONG (__bss_end__ - __bss_start__)
    __zero_table_end__ = .;
  } > TEXT
  */

  __etext = .;

  .FWDDR : {
      FW_DDR_START = .;
      KEEP(*stm32mp2xx_hal_ddr_ddrphy_firmware_ddr_pmu_train.o (.data))
  } > TEXT

  .data :
  {
    __data_start__ = .;
    *(vtable)
    *(.data*)
    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;
  } > DATA

  .init_data :
  {
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    . = ALIGN(4);
  } > DATA

  .uninit_data :
  {
      KEEP(*(.uninit_data))
  } > DATA

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > DATA

  .heap (COPY):
  {
    __HeapBase = .;
    __end__ = .;
    end = __end__;
    KEEP(*(.heap*))
    __HeapLimit = .;
  } > DATA

  /* .stack_dummy section doesn't contains any symbols. It is only
   * used for linker to calculate size of stack sections, and assign
   * values to stack symbols later */
  .stack_dummy (COPY):
  {
    KEEP(*(.stack*))
  } > STACK

  /* Set stack top to end of RAM, and stack limit move down by
   * size of stack_dummy section */
  __StackTop = ORIGIN(STACK) + LENGTH(STACK);
  __StackLimit = __StackTop - SIZEOF(.stack_dummy);
  PROVIDE(__stack = __StackTop);

  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

  /* Remaining memory at end of RETRAM, not reserved for firmware execution */
  .shared_data_section :
  {
    KEEP(*(.shared_data_section))
  } > REMAINING
}
