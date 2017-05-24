#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>

#define serialPath "/dev/ttyACM0"
#define baudrate (9600)


int main(){
  /*シリアルオープン*/
  int fd = serialOpen(serialPath, baudrate);
  if(fd<0){
    printf("can not open serialPort");
  }else{
    printf("connected!");
  }
	uint8_t command[5];
	//command = (unsigned char*)malloc(sizeof(unsigned char)*4);
	command[0] = 0xFE;
	command[1] = 0x00;
	command[2] = 0x00;
	command[3] = 0x00;
	serialPuts(fd, (char*)command);
	while(1){
		if(serialDataAvail(fd) != 0){
			while(serialDataAvail(fd)){
			//printf("%x\n", serialGetchar(fd));
			uint8_t x;
			read(fd, &x, 1);
			printf("%x\n",x);
			printf(" - geted data\n");
			fflush(stdout);
			}	
			break;
		}
	}
	
	serialClose(fd);
	printf("serial closed\n");
	return 0;
}

  
