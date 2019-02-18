#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "uart.h"

Uart u;
char buf[1024];

void handler(int sig){
	uart_close(&u);
	_exit(0);
}
int main(int argc,char** argv){
	speed_t baud;
	struct sigaction sa;
	FILE *fp;

	if(argc != 2){
		fprintf(stderr,"wrong args\n");
		return 1;
	}

	baudnum2sym(115200,&baud);
	if(uart_open(&u,"/dev/ttyUSB1",&baud) == -1){
		return 1;
	}
	memset(&sa,0,sizeof(sa));
	sa.sa_handler = handler;

	if(sigaction(SIGINT,&sa,NULL) == -1){
		perror("sigaction failed");
		uart_close(&u);
		return 1;
	}
	
	fp = fopen(argv[1],"rb");
	if(fp == NULL){
		fprintf(stderr,"fopen failed\n");
		uart_close(&u);
		return 1;
	}
	
	int counter = 0;	
	while(1){
		int r = fread(buf,1,1024,fp);
		if(r <= 0){
			break;
		}
		else{
			counter++;
			printf("%dbyte\n",counter);
		}
		int res = uart_write(&u,buf,r);
		if(res == -1){
			uart_close(&u);
			fclose(fp);
			return 1;
		}
	}
	
	fclose(fp);
	uart_close(&u);
	return 0;
}

