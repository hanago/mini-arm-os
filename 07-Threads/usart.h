#ifndef __USART_H__
#define __USART_H__

#define USART_FLAG_TXE	((uint16_t) 0x0080)
#define USART_FLAG_RXNE	((uint16_t) 0x0020)

void usart_init(void);

void print_c(const char c);
void print_str(const char *str);
void print_int(int num);

char get_c();
int get_str(char *buffer, int size);

#endif