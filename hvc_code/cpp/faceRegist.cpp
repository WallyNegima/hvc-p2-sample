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
	command[1] = 0x04;
	command[2] = 0x03;
	command[3] = 0x00;
	command[4] = 0b00000100;
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
	command[6] = 0x00;
	/*
	0x00 no image
	0x01 320*240
	0x02 160*120
	*/

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
			//printf("total = %d byte\n", datasize);

			//データの処理
			//人体の検出数を求める
			int bodyNum = 0;
			bodyNum = serialGetchar(fd);
			printf("body:%d\n", bodyNum);
			//手の検出数を求める
			int handNum = 0;
			handNum = serialGetchar(fd);
			printf("hand:%d\n", handNum);
			//顔の検出数を求める		
			int faceNum = 0;
			faceNum = serialGetchar(fd);
			printf("face:%d\n", faceNum);
			//予約
			int buf = 0;
			buf = serialGetchar(fd);
			//printf("buf:%d\n", buf);

			//検出結果を取り出す
			//人体の検出結果
			RESULT* bodyResult = (RESULT*)malloc(sizeof(RESULT)*bodyNum);
			for(i=0; i<bodyNum; i++){
				//検出結果を格納していく
				getObjectResult(&bodyResult[i]);
				printf("x=%d y=%d size=%d confidence=%d\n", bodyResult[i].posX, bodyResult[i].posY, bodyResult[i].size, bodyResult[i].confidence);
			}

			//手の検出結果
			RESULT* handResult = (RESULT*)malloc(sizeof(RESULT)*handNum);
			for(i=0; i<handNum; i++){
				getObjectResult(&handResult[i]);
				printf("x=%d y=%d size=%d confidence=%d\n", handResult[i].posX, handResult[i].posY, handResult[i].size, handResult[i].confidence);
			}

			//顔の検出結果
			RESULT* faceResult = (RESULT*)malloc(sizeof(RESULT)*faceNum);
			for(i=0; i<faceNum; i++){
				getObjectResult(&faceResult[i]);
				printf("%d %d %d %d", faceResult[i].posX, faceResult[i].posY, faceResult[i].size, faceResult[i].confidence);
				
				//認識した顔を登録！！
				unsigned char* faceRegistCommand;
				faceRegistCommand = (unsigned char*)malloc(sizeof(unsigned char)*commandBytes);
				faceRegistCommand[0] = 0xFE;
				faceRegistCommand[1] = 0x10;
				faceRegistCommand[2] = 0x03;
				faceRegistCommand[3] = 0x00;
				faceRegistCommand[4] = 0x01;
				faceRegistCommand[5] = 0x00;
				/*
				ユーザーIDを指定
				4,5はユーザーID
				4がLSB,5がMSB
				*/
				faceRegistCommand[6] = 0x01;//データIDを指定
				//受信できるデータは全て破棄して，登録用コマンドを送る
				serialFlush(fd);
				for(i=0; i<commandBytes; i++){
					serialPutchar(fd, command[i]);
				}
				delay(300);
				printf("\n");

				if(serialDataAvail(fd)){
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

					//画像データを取得
					unsigned char imageHead[4];
					int imageWidth, imageHeight;
					int pixelWidth, pixelHeight = 0;
					int pixel;
					imageWidth = 64;
					imageHeight = 64;
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
				}

				//メッセージインスタンス作成
				fluent::Message *msg = logger->retain_message("tag.camera");
				msg->set("posX", std::to_string(faceResult[i].posX));
				msg->set("posY", std::to_string(faceResult[i].posY));
				msg->set("size", std::to_string(faceResult[i].size));
				msg->set("confidence", std::to_string(faceResult[i].confidence));
				msg->set("userId", std::to_string(1));
				msg->set("dataId", std::to_string(1));	
				//ログを送信
				logger->emit(msg);
				printf("registed\n");
				free(faceRegistCommand);
				delete logger;
				break;
			}
			free(bodyResult);
			free(handResult);
			free(faceResult);
			delete logger;
		}
		delay(5000);
		printf("a\n");
	}
	serialClose(fd);
	free(command);
}
