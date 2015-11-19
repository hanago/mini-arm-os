#include <stddef.h>
#include <stdint.h>
#include <ctype.h>

#include "reg.h"
#include "threads.h"

#include "asm.h"
#include "usart.h"
#include "string.h"

/* Homework: Simple shell */
void thread_fibonacci(void *userdata){
	int result = fibonacci(str2int(userdata));
	print_str("fibonacci(");
	print_str(userdata);
	print_str(") = ");
	print_int(result);
	print_c('\n');
}

void simple_shell()
{
	char buffer[100];
	char *argv[5];
	int argc;

	while(1){
		*(argv[0]) = '\0';
		print_str("$ ");
		get_str(buffer, 99);

		/* Get arguments. */
		argv[0] = cmdtok(buffer);
		argc = 1;
		while(*buffer && argc < 5){
			argv[argc] = cmdtok(NULL);
			argc++;
		}

		/* Decode and execute command. */
		if(*(argv[0]) == '\0')
			continue;

		else if (strcmp(argv[0],"fibonacci_thread") == 0)
			thread_create(thread_fibonacci, (void *) argv[1]);

		else if (strcmp(argv[0],"fibonacci") == 0)
			thread_fibonacci((void *) argv[1]);

		else if(strcmp(argv[0],"greeting") == 0)
            print_str("Hello, i'm simple shell!\n");

		else if(strcmp(argv[0],"exit") == 0)
            print_str("Press \"Ctrl + a\" and then \"x\" to terminate qemu.\n");

		else if(strcmp(argv[0],"help") == 0)
		{
            print_str("\tfibonacci_thread %d\n");
            print_str("\tfibonacci %d\n");
            print_str("\thelp\n");
            print_str("\tgreeting\n");
            print_str("\texit\n");
		}
		else{
			print_str(argv[0]);
			print_str(":Command not found\n");
		}
	}

}

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
