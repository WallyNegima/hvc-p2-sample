#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>

//検出結果を格納する構造体
typedef struct{
	long   posX;       /* Center x-coordinate */
	long   posY;       /* Center y-coordinate */
	long   size;       /* Size */
	long   confidence; /* Degree of confidence */
}RESULT;

const char* serialPath = "/dev/ttyACM0";
const int baudrate = 9600;
int commandBytes = 4+3;
int fd;
int lsb, msb;

int main(){
  /*シリアルオープン*/
  fd = serialOpen(serialPath, baudrate);
  if(fd<0){
    printf("can not open serialPort");
  }else{
    printf("connected! fd:%d\n", fd);
  }
	unsigned char *command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*commandBytes);
	command[0] = 0xFE;
	command[1] = 0x04;
	command[2] = 0x03;
	command[3] = 0x00;
	command[4] = 0b00110100;
	/*
	bit7:目つむり
	bit6:視線
	bit5:性別
	bit4:年齢
	bit3:顔向き
	bit2:顔検出
	bit1:手検出
	bit0:人体検出
	*/
	command[5] = 0b00000000;
	/*
	bit1:顔認証
	bit0:表情
	*/
	command[6] = 0x01;

	int i;
	for(i=0; i<commandBytes; i++){
		serialPutchar(fd, command[i]);
	}
	delay(1500);
	for(i=0; i<commandBytes; i++){
		printf("%x ", command[i]	);
	}
	printf("some data arrived!\n");
		serialClose(fd);
	free(command);

	printf("serial closed\n");
	return 0;
}

void getResult(RESULT* result){
	int receiveData[8];
	int i;
	for(i=0; i<8; i++){
		receiveData[i] = (int)serialGetchar(fd);
	}
	result->posX = (long)(receiveData[0] + receiveData[1]<<8);
	result->posY = (long)(receiveData[2] + receiveData[3]<<8);
	result->size = (long)(receiveData[4] + receiveData[5]<<8);
	result->confidence = (long)(receiveData[6] + receiveData[7]<<8);
}
