#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>
#include <fluent.hpp>
#include "hvcp2-lib.h"
#include <sqlite3.h>
const char* serialPath = "/dev/hvcp2";
const int baudrate = 9600;
int sendCommandBytes = 0; //カメラへ送信するコマンドが何バイトなのか保存
const char* dbName = "user.db";

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
        if( responseBytes == 1){
          printf("ヘッダー部がおかしかった\n");
        }else{
          //正常にレスポンスがあるので中身を見ていく
          if(responseBytes > 4){
            getResponseImage(fd);
						delay(300);
            //機器のROMに登録
            command = getRegisterToRom(&sendCommandBytes);
            sendCommand(sendCommandBytes, fd, command);
            delay(500);             
            if(serialDataAvail(fd)){
              checkResponse(fd);
            }
						printf("saved to ROM\n");
            //ホストのアルバムに保存
            command = saveAlbumToHost(&sendCommandBytes);
            sendCommand(sendCommandBytes, fd, command);
            delay(700);
            if(serialDataAvail(fd)){
							int dataSize, albumSize, CRC;
							dataSize = checkResponse(fd);		
              if(dataSize > 1){
								albumSize = getAlbumSize(fd);
								printf("albumSize %d \n",albumSize);
								CRC = getAlbumSize(fd);
								printf("CRC %d \n", CRC);
								char text[128] = {'\0'} ;
								snprintf(text, 128, "echo %d,%d,%d > album.txt", dataSize,albumSize,CRC);
								system(text);
								while(serialDataAvail(fd)){
									int albumData = serialGetchar(fd);
									snprintf(text, 128, "echo %d >> album.txt", albumData);
									system(text);
								}
              }
            }else{
							printf("cannot save host's album\n");
							return -1;
						}
						//DBに保存
						sqlite3 *db=NULL;
						char* errMsg = NULL;
						int err;
						sqlite3_stmt* stmt = NULL;
						if( sqlite3_open(dbName, &db) != SQLITE_OK){
							printf("db err\n");
							return -1;
						}
						err = sqlite3_prepare16(db, "INSERT INTO users (id, name) VALUES (?, ?)",-1, &stmt, NULL);
						if( err != SQLITE_OK){
							printf("err db exec\n");
							return -1;
						}
						//DB コマンド実行
						sqlite3_bind_int(stmt, 1, atoi(argv[1]) );
						sqlite3_bind_text(stmt, 2, argv[3], strlen(argv[3]), SQLITE_STATIC);
						while(SQLITE_DONE != sqlite3_step(stmt)){}
						sqlite3_finalize(stmt);
						printf("ADD %s to id %d\n",argv[3], atoi(argv[1]));
						if(sqlite3_close(db) != SQLITE_OK){
							printf("err close\n");
							return -1;
						}
						printf("db closed\n");
            printf("登録した\n");
            break;
          }
        }
      }
    }
  }
}
