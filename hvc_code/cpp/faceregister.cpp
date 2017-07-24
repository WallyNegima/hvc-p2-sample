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

//検出した座標，その大きさ，信頼度を求めてRESULT型の構造体に格納
void getObjectResult(RESULT* result){
	int receiveData[8];
	int i;
	for(i=0; i<8; i++){
		receiveData[i] = (int)serialGetchar(fd);
	}
	result->posX = (long)(receiveData[0] | receiveData[1]<<8);
	result->posY = (long)(receiveData[2] | receiveData[3]<<8);
	result->size = (long)(receiveData[4] | receiveData[5]<<8);
	result->confidence = (long)(receiveData[6] | receiveData[7]<<8);
}

//ビットを反転させる
long bitswap(long data){
	long swaped = 0;
	int i = 0;
	swaped =  data;
	for(i=0; i<16; i++){
		swaped <<= 1;
		data >>= 1;
		swaped |= (data & 1);
	}
	return swaped;
}

int main(){
  /*シリアルオープン*/
  fd = serialOpen(serialPath, baudrate);
  if(fd<0){
    printf("can not open serialPort");
  }else{
  //  printf("connected! fd:%d\n", fd);
  }
	unsigned char *command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*commandBytes);
	command[0] = 0xFE;
	command[1] = 0x10;
	command[2] = 0x03;
	command[3] = 0x00;
	command[4] = 0x01;
	command[5] = 0x00;
	command[6] = 0x00;

	while(1){
		int i;
		serialFlush(fd); //コマンド送る前に送信途中のデータは破棄
		for(i=0; i<commandBytes; i++){
			serialPutchar(fd, command[i]);
		}
		delay(500);
		if(!serialDataAvail(fd)){
		//	printf("kitenai\n");
		}
		if(serialDataAvail(fd)){
			//結果を受け取った後の処理
			//fluentdに流すやつを用意
			fluent::Logger *logger  = new fluent::Logger();
			logger->new_forward("localhost", 24224);

			//ヘッダー部を解析
			for(i=0; i<2; i++){
				//printf("%x ", serialGetchar(fd));
				serialGetchar(fd);
			}
			//printf("\n");
			
			//データ長をもとめる
			unsigned char tmp_datasize[4];
			unsigned char l_lsb, l_msb, h_lsb, h_msb;
			unsigned long datasize;
			//printf("datasize:");
			for(i=0; i<4; i++){
				tmp_datasize[i] = serialGetchar(fd);
				//printf("%x ", tmp_datasize[i]);
			}
			h_msb = tmp_datasize[3];
			h_lsb = tmp_datasize[2];
			l_msb = tmp_datasize[1];
			l_lsb = tmp_datasize[0];

			datasize = (long)l_lsb;
			datasize = datasize | (l_msb << 8) | (h_lsb << 16) | (h_msb << 24);
			printf("total = %d byte\n", datasize);


			//画像データを取得
			unsigned char imageHead[4];
			int imageWidth, imageHeight;
			int pixelWidth, pixelHeight;
			int pixel;
			imageWidth = 64;
			imageHeight = 64;
			serialGetchar(fd);
			serialGetchar(fd);
			serialGetchar(fd);
			serialGetchar(fd);
			for(pixelHeight=0; i<imageHeight; pixelHeight++){
				for(pixelWidth=0; pixelWidth<imageWidth; pixelWidth++){
					if(serialDataAvail(fd)){
						pixel  = serialGetchar(fd);
						printf("%x ", pixel);
					}else{
					}
				}
				if(serialDataAvail(fd)){
					printf("\n");
				}else{
					printf("x=%d y=%d で終了\n", pixelWidth, pixelHeight);
					break;
				}
			}
			delete logger;
		}
		printf("registed");
		break;
	}

	//ROMに書き込む
	command[0] = 0xFE;
	command[1] = 0x22;
	command[2] = 0x00;
	command[3] = 0x00;
	int i=0;
	serialFlush(fd);
	for(i=0; i<4; i++){
		serialPutchar(fd, command[i]);
	}
	
	if(serialDataAvail){
		for(i=0; i<6; i++){
			serialGetchar(fd);
		}
	}


	serialClose(fd);
	free(command);
}
