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
//const char* dbName = "userFace.db";
//sqlite3 *db = NULL;
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
	command[5] = 0b00000011;
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

	//** db
    /*
	int dbNum = sqlite3_open(dbName, &db);
	if(SQLITE_OK != dbNum){
		printf("db no good");
	}
    */

	//*/

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
			printf("--------------------\n");
			//データの処理
			//人体の検出数を求める
			int bodyNum = 0;
			bodyNum = serialGetchar(fd);
			printf("\nbody:%d\n", bodyNum);
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
				printf("x座標:%d\n", faceResult[i].posX);
				printf("y座標:%d\n", faceResult[i].posY); 
				printf("サイズ:%d\n", faceResult[i].size);
				printf("信頼度:%d\n", faceResult[i].confidence);
				//メッセージインスタンス作成
				fluent::Message *msg = logger->retain_message("tag.camera");
				msg->set("posX", std::to_string(faceResult[i].posX));
				msg->set("posY", std::to_string(faceResult[i].posY));
				msg->set("size", std::to_string(faceResult[i].size));
				msg->set("confidence", std::to_string(faceResult[i].confidence));
				if( 0b00001000 & command[4]){
					//顔向き検出
				}
				if( 0b00010000 & command[4]){
					//年齢検出
					int age;
					int reliability;
					age = serialGetchar(fd);
					lsb = serialGetchar(fd);
					msb = serialGetchar(fd);
					reliability = lsb + msb<<8;
					printf("年齢:%d\n", age-10);
					msg->set("age", std::to_string(age));
				}
				if( 0b00100000 & command[4]){
					//性別測定結果
					int sex;
					sex = serialGetchar(fd);
					lsb = serialGetchar(fd);
					msb = serialGetchar(fd);
					if( sex == 1){
						printf("性別:男性\n");
					}else{
						printf("性別:女性\n");
					}
					msg->set("sex", std::to_string(sex));
				}
				if( 0b01000000 & command[4]){
					//視線検出
				}
				if( 0b10000000 & command[4]){
					//目つむり検出
				}
				if( 0b00000001 & command[5]){
					//表情
					int noneEmotion, joyEmotion, surprizeEmotion, angerEmotion, sorrowEmotion, totalEmotion;
					noneEmotion = serialGetchar(fd);
					joyEmotion = serialGetchar(fd);
					surprizeEmotion = serialGetchar(fd);
					angerEmotion = serialGetchar(fd);
					sorrowEmotion = serialGetchar(fd);
					totalEmotion = serialGetchar(fd);
					printf("無表情感:%d\n喜び:%d\n驚き:%d\n怒り:%d\n悲しみ:%d\n表情度:%d\n",noneEmotion, joyEmotion, surprizeEmotion, angerEmotion, sorrowEmotion, totalEmotion-100); 
					msg->set("noneEmo", std::to_string(noneEmotion));
					msg->set("joyEmo", std::to_string(joyEmotion));
					msg->set("surprizeEmo", std::to_string(surprizeEmotion));
					msg->set("angerEmo", std::to_string(angerEmotion));
					msg->set("sorrowEmo", std::to_string(sorrowEmotion));
					msg->set("totalEmo", std::to_string(totalEmotion));
				}
				if( 0b00000010 & command[5]){
					//顔認証
					int userid, similarity;
					lsb = serialGetchar(fd);
					msb = serialGetchar(fd);
					userid = lsb + (msb << 8);
					lsb = serialGetchar(fd);
					msb = serialGetchar(fd);
					similarity = lsb + (msb << 8); 		
					if( userid == 0xFF80 ){
							printf("cannot ninsho\n");
					}else if( userid == 0xFF81){
							printf("album is none\n");
					}else{
						printf(" %d %d", userid, similarity);
                        msg->set("userid", std::to_string(userid));
					}
				}

				//ログを送信
				logger->emit(msg);
			}


			//画像を取得する
			if( command[6] & 0x11){
				unsigned char imageHead[4];
				int imageWidth, imageHeight;
				for(i=0; i<4; i++){
					imageHead[i] = serialGetchar(fd);
				}
				imageWidth = imageHead[0] + (imageHead[1]<<8);
				imageHeight = imageHead[2] + (imageHead[3]<<8);
				printf("\nwidth = %d\nheight = %d\n", imageWidth, imageHeight);
					
				int pixelWidth, pixelHeight = 0;
				int pixel;
				for(pixelHeight=0; i<imageHeight; pixelHeight++){
					for(pixelWidth=0; pixelWidth<imageWidth; pixelWidth++){
						if(serialDataAvail(fd)){
							pixel  = serialGetchar(fd);
		//					printf("%x ", pixel);
						}else{
						}
					}
					if(serialDataAvail(fd)){
		//				printf("\n");
					}else{
						printf("x=%d y=%d で終了\n", pixelWidth, pixelHeight);
						break;
					}
				}
			}
					
			free(bodyResult);
			free(handResult);
			free(faceResult);
			delete logger;
		}
		delay(5000);
	}
	serialClose(fd);
}
