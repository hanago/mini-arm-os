#include "os.h"

#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include "reg.h"
#include "threads.h"
#include "usart.h"
#include "string.h"
#include "shell.h"

/* 72MHz */
#define CPU_CLOCK_HZ 72000000

/* 100 ms per tick. */
#define TICK_RATE_HZ 10

int main(void)
{
	const char *str1 = "simple_shell";

	usart_init();

	if (thread_create(simple_shell, (void *) str1) == -1)
		print_str("simple_shell creation failed\r\n");

	/* SysTick configuration */
	*SYSTICK_LOAD = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
	*SYSTICK_VAL = 0;
	*SYSTICK_CTRL = 0x07;

	thread_start();

	return 0;
}
