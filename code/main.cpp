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
int sendCommandBytes = 0; //カメラへ送信するコマンドが何バイトなのか保存


//顔や体などを検出する際に用いるコマンドを返す
unsigned char* getSeachCommand(){
	unsigned char* command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*7);
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

	sendCommandBytes = 7;
	return command;
}

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

void getObjectResult(RESULT* result, int fd){
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

/*
カメラの画面を下のように9分割し，
どこに物体が存在するかを数値で返す
0|1|2
-----
3|4|5
-----
6|7|8
*/
int getFacePos(RESULT* result){
	int pos = 0;
	if(result->posX <= 500){
		pos += 0;
	}
	if(result->posX <= 1100){
		pos += 1;
	}
	if(result->posX > 1100){
		pos += 1;
	}

	if(result->posY <= 400){
		pos += 0;
	}
	if(result->posY <= 800){
		pos += 3;
	}
	if(result->posY > 800){
		pos += 3;
	}

	return pos;
}

//LSB MSB の順に値を取得し
//順番通りに並び替え，数値にして返す
int getMSBLSB(int fd){
	int lsb = serialGetchar(fd);
	int msb = serialGetchar(fd);
	return lsb + (msb << 8);
}

int getAge(int fd){
	int age = serialGetchar(fd);
	int reliability = getMSBLSB(fd);
	return age;
}
int getSex(int fd){
	int sex = serialGetchar(fd);
	int reliability = getMSBLSB(fd);
	return sex;
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
		printf("open %s \n",serialPath);
		command = getSeachCommand();
		
		while(1){
			//送信中のデータなどは一度破棄する
			serialFlush(fd);
			for(int i=0; i<sendCommandBytes; i++){
				serialPutchar(fd, command[i]);
			}
			delay(100);

			if(serialDataAvail(fd)){
				//結果が帰ってきたあとの処理
				
				//ヘッダー部を解析してエラーが出たら終了
				if(responseIsErr(fd) == 1){
					printf("ヘッダー部がおかしかった\n");
				}else{
					//正常にレスポンスがあるので中身を見ていく
					//帰ってきたデータ長を求める
					unsigned int responseBytes;
					responseBytes = getResponseBytes(fd);
					printf("responseBytes = %d\n", responseBytes);

					//データの処理
					//体，手，顔の検出数を取得
					int bodyNum = 0;
					bodyNum = serialGetchar(fd);
					int handNum = 0;
					handNum = serialGetchar(fd);
					int faceNum = 0;
					faceNum = serialGetchar(fd);

					//予約で0固定分
					serialGetchar(fd);

					//体の検出結果
					RESULT* bodyResult = (RESULT*)malloc(sizeof(RESULT)*bodyNum);
					for(int i=0; i<bodyNum; i++){
						//検出結果を格納していく
						getObjectResult(&bodyResult[i], fd);
						printf("x=%d y=%d size=%d confidence=%d\n", bodyResult[i].posX, bodyResult[i].posY, bodyResult[i].size, bodyResult[i].confidence);
					}

					//手の検出結果
					RESULT* handResult = (RESULT*)malloc(sizeof(RESULT)*handNum);
					for(int i=0; i<handNum; i++){
						getObjectResult(&handResult[i],fd);
						printf("x=%d y=%d size=%d confidence=%d\n", handResult[i].posX, handResult[i].posY, handResult[i].size, handResult[i].confidence);
					}

					//顔の検出結果
					RESULT* faceResult = (RESULT*)malloc(sizeof(RESULT)*faceNum);
					for(int i=0; i<faceNum; i++){
						fluent::Logger *logger = new fluent::Logger();
						logger->new_forward("localhost", 24224);
						fluent::Message *msg = logger->retain_message("camera");
						getObjectResult(&faceResult[i],fd);//これで座標とサイズと信頼度がわかる
						msg->set("posX", std::to_string(faceResult[i].posX));
						msg->set("posY", std::to_string(faceResult[i].posY));
						msg->set("size", std::to_string(faceResult[i].size));
						msg->set("confidence", std::to_string(faceResult[i].confidence));

						//顔の位置を9分割して判定
						int facePos = getFacePos(&faceResult[i]);
						msg->set("pos", std::to_string(facePos));
						if(facePos == 4){
							printf("object is center\n");
						}

						if(0b00010000 & command[4]){
							//年齢検出
							int age = 0;
							age = getAge(fd);
							printf("age:%d\n", age);
							msg->set("age", std::to_string(age));
						}
						if(0b00100000 & command[4]){
							int sex = -1;
							sex = getSex(fd);
							printf("sex:%d\n", sex);
							msg->set("sex", std::to_string(sex));
						}
						if(0b01000000 & command[4]){
							//視線検出
						}
						if(0b10000000 & command[4]){
							//目つむり検出
						}
						if(0b00000001 & command[5]){
							//表情
							int noneEmotion, joyEmotion, surprizeEmotion, angerEmotion, sorrowEmotion, totalEmotion;
							noneEmotion = serialGetchar(fd);
							joyEmotion = serialGetchar(fd);
							surprizeEmotion = serialGetchar(fd);
							angerEmotion = serialGetchar(fd);
							sorrowEmotion = serialGetchar(fd);
							totalEmotion = serialGetchar(fd);
							printf("無表情感:%d\n喜び:%d\n驚き:%d\n怒り:%d\n悲しみ:%d\n表情度:%d\n",
							noneEmotion, joyEmotion, surprizeEmotion, angerEmotion, sorrowEmotion, totalEmotion-100); 
							msg->set("noneEmo", std::to_string(noneEmotion));
							msg->set("joyEmo", std::to_string(joyEmotion));
							msg->set("surprizeEmo", std::to_string(surprizeEmotion));
							msg->set("angerEmo", std::to_string(angerEmotion));
							msg->set("sorrowEmo", std::to_string(sorrowEmotion));
							msg->set("totalEmo", std::to_string(totalEmotion));
						}
						if(0b00000010 & command[5]){
							//顔認証
							int userid, similarity;
							userid = getMSBLSB(fd);
							similarity = getMSBLSB(fd);
							if(userid > 100 || userid < 0){
								//おかしい
							}else{
								printf("userid:%d\n",userid);
								msg->set("userid", std::to_string(userid));
							}
						}
						logger->emit(msg);
						delete logger;
					}

					free(bodyResult);
					free(handResult);
					free(faceResult);

					if(faceNum > 0){
						delay(5000);
					}
				}
				delay(2000);
			}
		}
	}
	serialClose(fd);
}
