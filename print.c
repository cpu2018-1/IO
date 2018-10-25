#include <stdio.h>

#include "uart.h"


int main(){
	speed_t baud;
	Uart u;

	baudnum2sym(115200,&baud);
	int res = uart_open(&u,"/dev/ttyUSB1",&baud);
	if(res == -1){
		printf("err open\n");
		return 1;
	}

	char buf[2];
	while(1){
		int r = uart_read(&u,buf,1);
		if(r == 1){
			putchar(buf[0]);
			fflush(stdout);
		}
	}
	return 0;
}
