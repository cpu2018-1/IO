#ifndef UART_H
#define UART_H

#include <termios.h>

// if UART_SHOW_ERR_LOG is defined then perror(error)

typedef struct {
	int fd;
	int old_valid;
	speed_t speed;
	struct termios oldio;
	struct termios newio;
} Uart;

int baudnum2sym(int num_baud,speed_t *sym_baud);
// succ->0 fail->-1
int uart_open(Uart* uart_data,char* port_name,speed_t* sym_baud);
int uart_close(Uart* uart_data);
int uart_write(Uart *uart_data,char* buf,size_t count);
int uart_read(Uart *uart_data,char* buf,size_t);
#endif
