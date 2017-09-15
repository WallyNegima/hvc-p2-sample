#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>
#include <fluent.hpp>
#include "hvcp2-lib.h"

const char* serialPath = "/dev/hvcp2";
const int baudrate = 9600;



//ヘッダー部が正しければ1を返す
int responseIsErr(int fd){
	if(serialGetchar(fd) != 0xFE){
		return 1;
	}
	if(serialGetchar(fd) != 0x00){
		return 1;
	}
	return -1;
}

//返ってきたデータのバイト数を返す
unsigned int getResponseBytes(int fd){
	unsigned char tmp_datasize[4];
	unsigned int datasize;
	//printf("datasize:");
	for(int i=0; i<4; i++){
	tmp_datasize[i] = serialGetchar(fd);
	}
	datasize = (long)tmp_datasize[0];
	datasize = datasize | (tmp_datasize[1] << 8) | (tmp_datasize[2] << 16) | (tmp_datasize[3] << 24);
	return datasize;
}

//順番通りに並び替え，数値にして返す
int getMSBLSB(int fd){
	int lsb = serialGetchar(fd);
	int msb = serialGetchar(fd);
	return lsb + (msb << 8);
}

int main(){
	int fd; //シリアル通信のID的なもの
	unsigned char* command;
	/*シリアルオープン*/
	fd = serialOpen(serialPath, baudrate);
	if(fd < 0){
		//シリアルオープンに失敗
		printf("can not open %s \n", serialPath);
	}else{
		//シリアルオープンに成功
    int sendCommandBytes=0;
		printf("open %s \n",serialPath);
		command = getReadCameraAngle(&sendCommandBytes);
    for(int i=0; i<sendCommandBytes; i++){
      serialPutchar(fd, command[i]);
    }
    while(serialDataAvail(fd)){
      printf("%d ", serialGetchar(fd));
    }
  }		
	serialClose(fd);
}
