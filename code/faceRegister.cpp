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
int sendCommandBytes = 0; //カメラへ送信するコマンドが何バイトなのか保存
/*
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
*/

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
		


int main(int argc, char* argv[]){
  int fd; //シリアル通信のID的なもの
  unsigned char* command;

  if(argc != 3 ){
    printf("userid, userdata を入力して実行して下さい\n");
  }

  /*シリアルオープン*/
  fd = serialOpen(serialPath, baudrate);
  if(fd < 0){
    //シリアルオープンに失敗
    printf("can not open %s \n", serialPath);
  }else{
    //シリアルオープンに成功
    printf("open %s \n",serialPath);
    unsigned short userid;
    unsigned char dataid;
    userid = atoi(argv[1]);
    dataid = atoi(argv[2]);
    command = getRegisterCommand(&sendCommandBytes, userid, dataid);

    while(1){
      //送信中のデータなどは一度破棄する
      sendCommand(sendCommandBytes, fd, command);
      printf("sended\n");
      delay(100);

      if(serialDataAvail(fd)){
        //結果が帰ってきたあとの処理
    
        //ヘッダー部を解析してエラーが出たら終了
        unsigned int responseBytes;
        responseBytes = checkResponse(fd);
        if( responseBytes == 0){
          printf("ヘッダー部がおかしかった\n");
        }else{
          //正常にレスポンスがあるので中身を見ていく
          if(responseBytes > 4){
            getResponseImage(fd);

            //機器のROMに登録
            command = getRegisterToRom(&sendCommandBytes);
            sendCommand(sendCommandBytes, fd, command);
            delay(100);             
            if(serialDataAvail(fd)){
              checkResponse(fd);
            }
            delay(100);
            //ホストのアルバムに保存
            command = saveAlbumToHost(&sendCommandBytes);
            sendCommand(sendCommandBytes, fd, command);
            delay(100);
            if(serialDataAvail(fd)){
              if(checkResponse(fd) > 0){
                for(int i=0; i<responseBytes; i++){
                  printf("%d ", serialGetchar(fd));
                }
              }
            }
            printf("登録した\n");
            break;
          }
        }
      }
    }
  }
}
