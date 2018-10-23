#define UART_SHOW_ERROR_LOG
#include <stdio.h>
#include <string.h>

#include "uart.h"


Uart u;
speed_t sp;
int res;

char buf[10] = {0};
char buf2[10] = {0};

int main(){
	res = baudnum2sym(115200,&sp);
	if(res == -1){
		puts("conv");
		return 1;
	}

	res = uart_open(&u,"/dev/ttyUSB1",&sp);
	if(res == -1){
		puts("open");
		return 1;
	}

	buf[0] = 0x30;
	buf[1] = 0x31;
	buf[2] = 0x32;
	res = uart_write(&u,buf,3);
	if(res == -1){
		puts("write");
		return 1;
	}

	res = uart_read(&u,buf2,1);
	res = uart_read(&u,buf2+1,1);
	res = uart_read(&u,buf2+2,1);
	if(res == -1){
		puts("read");
		return 1;
	}
	printf("%s\n",buf2);


	uart_close(&u);
	return 0;
}
