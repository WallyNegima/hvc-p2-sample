#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>

const char* serialPath = "/dev/ttyACM0";
const int baudrate = 9600;

int main(){
  /*シリアルオープン*/
  int fd = serialOpen(serialPath, baudrate);
  if(fd<0){
    printf("can not open serialPort");
  }else{
    printf("connected! fd:%d\n", fd);
  }
	unsigned char *command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*4);
	command[0] = 0xFE;
	command[1] = 0x00;
	command[2] = 0x00;
	command[3] = 0x00;
	//unsigned char* command = { 0xFE, 0x00, 0x00, 0x00 };

	int i;
	for(i=0; i<4; i++){
		serialPutchar(fd, command[i]);
	}
	delay(1000);
	//serialPuts(fd,command);
	//delay(1000);
	for(i=0; i<4; i++){
		printf("%x ", command[i]	);
	}
	//int aaa;
	//scanf("%d", &aaa);
	printf("some data arrived!\n");
	if(!serialDataAvail(fd)){
		printf("kitenai\n");
	}
	while(serialDataAvail(fd)){
		printf("%c ", serialGetchar(fd));
		//fflush(stdout);
	}
	serialClose(fd);

	printf("serial closed\n");
	return 0;
}

  
