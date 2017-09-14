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
unsigned char* getRegisterCommand(){
	unsigned char* command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*7);
	command[0] = 0xFE;
	command[1] = 0x10;
	command[2] = 0x03;
	command[3] = 0x00;
	command[4] = 0x02; //IDのLSB
	command[5] = 0x00; //IDのMSB
	command[6] = 0x00; //データID

	sendCommandBytes = 7;
	return command;
}

unsigned char* getRegisterToRom(){
	unsigned char* command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*4);
	command[0] = 0xFE;
	command[1] = 0x22;
	command[2] = 0x00;
	command[3] = 0x00;

	sendCommandBytes = 4;
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


//LSB MSB の順に値を取得し
//順番通りに並び替え，数値にして返す
int getMSBLSB(int fd){
		int lsb = serialGetchar(fd);
		int msb = serialGetchar(fd);
		return lsb + (msb << 8);
}

//画像データを取得
void getResponseImage(int fd){
	unsigned char imageHead[4];
	int imageWidth, imageHeight;
	int pixelWidth, pixelHeight;
	int pixel;
	imageWidth = getMSBLSB(fd);
	imageHeight = getMSBLSB(fd);
	printf("width:%d height:%d¥n", imageWidth, imageHeight);
	for(pixelHeight=0; pixelHeight<imageHeight; pixelHeight++){
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
			command = getRegisterCommand();
			printf("sended\n");

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
                        if(responseBytes > 4){

                            printf("responseBytes = %d\n", responseBytes);
                            getResponseImage(fd);

                            //機器のROMに登録
                            command = getRegisterToRom();
                            serialFlush(fd);
                            for(int i=0; i<sendCommandBytes; i++){
                                serialPutchar(fd, command[i]);
                            }
                                            
                            if(serialDataAvail(fd)){
                                if(responseIsErr(fd) == 1){
                                    printf("ヘッダー部がおかしかった\n");
                                }else{
                                    printf("%d\n",getResponseBytes(fd));
                                }
                            }

                            printf("登録しました\n");
                            break;
                        }
					}
				}
			}
    }
}
