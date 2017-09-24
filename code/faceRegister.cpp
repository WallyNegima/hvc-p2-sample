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
		//登録するID=userid
		//登録するデータ番号=dataid : 一人につき100枚まで登録可能
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
            if(serialDataAvail(fd)){
              checkResponse(fd);
            }
						printf("saved to ROM\n");
            //ホストのアルバムに保存
            command = saveAlbumToHost(&sendCommandBytes);
            sendCommand(sendCommandBytes, fd, command);
            if(serialDataAvail(fd)){
							int dataSize, albumSize, CRC;
							dataSize = checkResponse(fd);		
              if(dataSize > 1){
								albumSize = getAlbumSize(fd);
								printf("albumSize %d \n",albumSize);
								CRC = getAlbumSize(fd);
								printf("CRC %d \n", CRC);
								//album.txtに全データを書き込む
								char text[128] = {'\0'} ;
								snprintf(text, 128, "echo %d,%d,%d > album.txt", dataSize,albumSize,CRC);
								system(text);
								while(serialDataAvail(fd)){
									int  albumData = serialGetchar(fd);
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
						int userID, userData; 
						char* userName;
						//  登録ID, 登録データ,登録名前
						userID = atoi(argv[1]);
						userData = atoi(argv[2]);
						userName = argv[3];
						sqlite3_stmt* stmt = NULL;
						if( sqlite3_open(dbName, &db) != SQLITE_OK){
							printf("db err\n");
							return -1;
						}
						//DBに登録されているIDかどうかをチェック
						err = sqlite3_prepare_v2(db, "SELECT * FROM users", 64, &stmt, NULL);
						if(err != SQLITE_OK){
							printf("select is not ok\n");
							return -1;
						}
						//データの抽出
						int count=0;
						while(SQLITE_ROW == (err = sqlite3_step(stmt)) ){
							int id = sqlite3_column_int(stmt, 0);
							const unsigned char* name = sqlite3_column_text(stmt, 1);
							printf("id:%d, name:%s\n", id, name);
							count++;
						}
						if(err != SQLITE_DONE){
							printf("抽出するところでエラー\n");
							return -1;
						}
						sqlite3_finalize(stmt);

						char query[256] = {'\0'};
						//idとnameを登録
						if(count == 0){
							snprintf(query, 256, "INSERT INTO users(id, name) VALUES(%d, \"%s\")", userID,userName);
						}else{
							snprintf(query, 256, "UPDATE users SET id=%d, name=\"%s\" ", userID, userName);
						}
						//err = sqlite3_prepare16(db, "INSERT INTO users(id, name) VALUES(?, ?)",-1, &stmt, NULL);
						printf("%s\n",query);
						err = sqlite3_exec(db, query, NULL, NULL, &errMsg);
						if( err != SQLITE_OK){
							printf("err db exec\n");
							return -1;
						}
						printf("ADD %s to id %d\n",userName, userID);
						//DBを閉じる
						if(sqlite3_close(db) != SQLITE_OK){
							printf("err close\n");
							return -1;
						}
						printf("db closed\n");
            printf("register all success\n");
            break;
          }
        }
      }
    }
  }
}
