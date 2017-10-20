#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>
#include <fluent.hpp>
#include "hvcp2-lib.h"
#include <string.h>

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
    int sendCommandBytes=0;
		printf("open %s \n",serialPath);
    while(1){
      printf("-----------\n");
      printf("使いたいコマンドを選んで\n");
      printf("0:ROMデータを初期化\n");
			printf("1:アルバムデータをホストからカメラへ\n");
			printf("2:カメラ内のアルバムデータをROMへ\n");
      printf("99:プログラムを終了\n");

      char input[16];
      fgets(input, sizeof(input), stdin);
      //文字列が問題ないかチェック
      for(int i=0; i<sizeof(input); i++){
        if(input[i] == '\0' || input[i] == '\n'){
          break;
        }else if(isdigit(input[i])){
        }else{
          printf("数値でない文字列が入っています at %d\n", i);
          return -1;
        }
        if(i>2){
          printf("文字数が多すぎる\n");
          return -1;
        }
      }
      
      int commandNum = atoi(input);
      printf("%d\n", commandNum);
      if(commandNum == 99){
        printf("finish\n");
        return 0;
      }
      if(commandNum == 0){
        //カメラモジュールのROMデータを消去
        unsigned char* command;
        int sendCommandBytes;
        command = resetROMData(&sendCommandBytes);
        sendCommand(sendCommandBytes, fd, command);
        if(checkResponse(fd) == 1){
          //err
          printf("error header\n");
        }else{
          printf("ok\n");
        }
      }else if(commandNum == 1){
				//アルバムデータをカメラへ読み込む
        unsigned char* command;
				int sendCommandBytes;
				command = readAlbumToCamera(&sendCommandBytes, fd);
				//sendCommand(sendCommandBytes, fd, command);
				if(checkResponse(fd) == 1){
					return -1;
				}
				printf("read album to cam\n");
			}else if(commandNum == 2){
				//ROMにカメラのデータを書き込む
        unsigned char* command;
				int sendCommandBytes;
				command = getRegisterToRom(&sendCommandBytes);
				sendCommand(sendCommandBytes, fd, command);
				if(checkResponse(fd) == 1){
					printf("err response \n");
					return -1;
				}
				printf("register to cam's ROM\n");
			}
		}
  }		
	serialClose(fd);
  return 0;
}
