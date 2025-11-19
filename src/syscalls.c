/**
  *****************************************************************************
  **
  **  File        : syscalls.c
  **
  **  Abstract    : System Workbench Minimal System calls file
  **
  **              For more information about which c-functions
  **                need which of these lowlevel functions
  **                please consult the Newlib libc-manual
  **
  **  Environment : System Workbench for MCU
  **
  **  Distribution: The file is distributed as is, without any warranty
  **                of any kind.
  **
  **  (c)Copyright System Workbench for MCU.
  **  You may use this file as-is or modify it according to the needs of your
  **  project. Distribution of this file (unmodified or modified) is not
  **  permitted. System Workbench for MCU permit registered System Workbench(R) users the
  **  rights to distribute the assembled, compiled & linked contents of this
  **  file as part of an application binary file, provided that it is built
  **  using the System Workbench for MCU toolchain.
  **
  *****************************************************************************
  */

/* Includes */
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

#include <main.h>

/* Variables */
extern int errno;

/* Functions */
int _read(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		*ptr++ = io_getchar();
	}

	return len;
}

int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		io_putchar(*ptr++);
	}

	return len;
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _open(char *path, int flags, ...)
{
	/* Pretend like we always fail */
	return -1;
}
