#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "uart.h"


int main(){
	speed_t baud;
	Uart u;

	baudnum2sym(460800,&baud);
	int res = uart_open(&u,"/dev/ttyUSB1",&baud);
	if(res == -1){
		printf("failed at opening\n");
		return 1;
	}

	char rbuf[2];
	char wbuf[128];

	pid_t pid;

	pid = fork();
	if(pid == -1){
		printf("fork error\n");
		uart_close(&u);
		return 1;
	} else if (pid == 0){ // child reading
		while(1){
			int r = uart_read(&u,rbuf,1);
			if(r == 1){
				printf("%c",rbuf[0]);
				fflush(stdout);
			} else if (r == -1){
				printf("read error\n");
				uart_close(&u);
				exit(1);
			}
		}
	} else { // parent writeing
		while(1){
			memset(wbuf,'\0',128);
			fgets(wbuf,128,stdin);
			int wlen = strlen(wbuf);
			int w = uart_write(&u,wbuf,wlen);
			if(w != wlen){
				puts("write error\n");
				uart_close(&u);
				kill(pid,SIGINT);
				int stat;
				wait(&stat);
				exit(1);
			}
		}
/*
			char * cp = wbuf;
			while (*cp != '\0' && *cp != '\n'){
				while(1){
					int w = uart_write(&u,cp,1);
					if(w == 1){
						break;
					} else if (w == -1){
						printf("write error\n");
						uart_close(&u);
						kill(pid,SIGINT);
						int stat;
						wait(&stat);
						exit(1);
					}
				}
				cp++;
			}
		}
*/
	}

	uart_close(&u);
	return 0;
}
