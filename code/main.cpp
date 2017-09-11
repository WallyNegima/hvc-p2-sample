#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>
#include <fluent.hpp>

//検出結果を格納する構造体
typedef struct{
	long posX;
	long posY;
	long size; //物体を正方形の領域で検出する．その時の一辺のピクセル数
	long confidence; //信頼度
}RESULT;

const char* serialPath = "/dev/hvcp2";
const int baudrate = 9600;

int main(){
	int fd; //シリアル通信のID的なもの

	/*シリアルオープン*/
	fd = serialOpen(serialPath, baudrate);
	if(fd < 0){
		//シリアルオープンに失敗
		printf("can not open %s \n", serialPath);
	}else{
		//シリアルオープンに成功
		printf("open %s \n",serialPath);
	}

	serialClose(fd);
}
