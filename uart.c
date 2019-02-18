#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "uart.h"


int baudnum2sym(int num_baud, speed_t *sym_baud) {
     int ret;
     ret = 0;
     switch (num_baud) {
     case 9600:
   *sym_baud = B9600;
   break;
     case 115200:
   *sym_baud = B115200;
   break;
     case 230400:
   *sym_baud = B230400;
   break;
     case 460800:
   *sym_baud = B460800;
   break;
     case 500000:
   *sym_baud = B500000;
   break;
     case 576000:
   *sym_baud = B576000;
   break;
     case 921600:
   *sym_baud = B921600;
   break;
     case 1000000:
   *sym_baud = B1000000;
   break;
     default:
   ret = -1;
   break;
     }
     return(ret);
 }
 
 // return file descriptor , if err then return -1
int uart_open(Uart *uart_data,char* port_name,speed_t *sym_baud){
	uart_data->old_valid = 0;
	// file open
	int fd;
	fd = open(port_name,O_RDWR);
	uart_data->fd = fd;
	if(fd == -1){
#ifdef UART_SHOW_ERROR_LOG
		perror("fail at opening discriptor");
#endif
		return -1;
	}
	
	int res;
	// store curr config to oldio
	res = tcgetattr(fd,&(uart_data->oldio));
	if(res == -1){
#ifdef UART_SHOW_ERROR_LOG
		perror("fail at tcgetattr");
#endif
		return -1;
	}
	uart_data->newio = uart_data->oldio;

	//set speed
	res = cfsetspeed(&(uart_data->newio),*sym_baud);
	if (res == -1){
#ifdef UART_SHOW_ERROR_LOG
		perror("fail at cfsetspeed");
#endif
		return -1;
	}
	uart_data->speed = *sym_baud;

	// activate the setting
	res = tcflush(fd,TCIFLUSH);
	if (res == -1){
#ifdef UART_SHOW_ERROR_LOG
		perror("fail at tcflush");
#endif
		return -1;
	}
	res = tcsetattr(fd,TCSANOW,&(uart_data->newio));
	if (res == -1){
#ifdef UART_SHOW_ERROR_LOG
		perror("fail at tcsetattr");
#endif
		return -1;
	}

	res = ioctl(fd, TCSETS, &(uart_data->newio));
	if (res == -1){
#ifdef UART_SHOW_ERROR_LOG
		perror("fail at ioctrl");
#endif
		return -1;
	}


	uart_data->old_valid = 1;
	return 0;
}

int uart_close(Uart *uart_data){
	int res;
	int ans = 0;
	if(uart_data->old_valid){
		res = tcsetattr(uart_data->fd,TCSANOW,&(uart_data->oldio));
		if (res == -1){
			while(1){
				res = tcsetattr(uart_data->fd,TCSANOW,&(uart_data->oldio));
				if(res != -1){
					break;
				}
				if(errno != EINTR){
#ifdef UART_SHOW_ERROR_LOG
					perror("fail at tcsetattr");
#endif
					ans = -1;
					break;
				}
			}
		}
	}
	if(uart_data->fd != -1){
		close(uart_data->fd);
		return ans;
	}

  if(uart_data->old_valid){
     res = ioctl(uart_data->fd,TCSETS,&(uart_data->oldio));
     if (res == -1){
       while(1){
         res = ioctl(uart_data->fd,TCSETS,&(uart_data->oldio));
         if(res != -1){
           break;
         }
         if(errno != EINTR){
 #ifdef UART_SHOW_ERROR_LOG
           perror("fail at tcsetattr");
 #endif
           ans = -1;
           break;
         }
       }
     }
   }
   if(uart_data->fd != -1){
     close(uart_data->fd);
   }
								

	return ans;
}
//succ->0, err->-1
int uart_write(Uart *uart_data,char* buf,size_t count){
	int res;
	int tmp_c = count;
	while(count > 0){
		res = write(uart_data->fd,buf,count);
		if(res == -1 && errno != EINTR){
#ifdef UART_SHOW_ERROR_LOG
			perror("fail at write");
#endif
			return -1;
		}
		else if (res == -1 && (errno == EINTR)){
			continue;
		}
		else{
			buf += res;
			count -= res;
		}
	}
	return tmp_c;
}

//err->-1 succ->num of read_data
int uart_read(Uart *uart_data,char* buf,size_t count){
	int res;
	while(count > 0){
		res = read(uart_data->fd,buf,count);
		if(res == -1 && errno != EINTR && errno != EAGAIN){
#ifdef UART_SHOW_ERROR_LOG
			printf("%d ",errno);
			perror("fail at read");
#endif
			return -1;
		}
		else if(res == -1 && (errno == EINTR || errno == EAGAIN)){
			continue;
		}
		else{
			buf += res;
			count -= res;
		}
	}
	return res;
}
