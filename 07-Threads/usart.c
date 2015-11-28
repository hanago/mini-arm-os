#include "usart.h"

#include <stdint.h>
#include "reg.h"
#include "string.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
void usart_init(void)
{
	*(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
	*(RCC_APB1ENR) |= (uint32_t) (0x00020000);

	/* USART2 Configuration, Rx->PA3, Tx->PA2 */
	*(GPIOA_CRL) = 0x00004B00;
	*(GPIOA_CRH) = 0x44444444;
	*(GPIOA_ODR) = 0x00000000;
	*(GPIOA_BSRR) = 0x00000000;
	*(GPIOA_BRR) = 0x00000000;

	*(USART2_CR1) = 0x0000000C;
	*(USART2_CR2) = 0x00000000;
	*(USART2_CR3) = 0x00000000;
	*(USART2_CR1) |= 0x2000;
}

/* Print */
void print_c(const char c){
	while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (c & 0xFF);
}

void print_str(const char *str)
{
	while (*str) {
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
		str++;
	}
}

void print_int(int num)
{
	print_str(itoa(num));
}

/* Get */
char get_c() {
	char c;
    while(!(*(USART2_SR) & USART_FLAG_RXNE));
        c = (char)(*(USART2_DR) & 0xFF);
    print_c(c);

    return c;
}

int get_str(char *buffer, int size) {
	int i = 0;
	char c = '\0';

	while(c != 0xD){
    	c = get_c();
    	if(c != 0xD && i < size)
    		*(buffer+i++) = c;

    }
    *(buffer+i) = '\0';
    print_c('\n');

    return 1;
}