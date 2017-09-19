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

//順番通りに並び替え，数値にして返す
int getMSBLSB(int fd){
	int lsb = serialGetchar(fd);
	int msb = serialGetchar(fd);
	return lsb + (msb << 8);
}

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
        unsigned char* command;
        int sendCommandBytes;
        command = resetROMData(&sendCommandBytes);
        sendCommand(sendCommandBytes, fd, command);
				delay(500);
        if(checkResponse(fd) == 1){
          //err
          printf("error header\n");
        }else{
          printf("ok\n");
        }
      }else if(commandNum == 1){
				unsigned char* command;
				int sendCommandBytes;
				int albumSize, dataSize, CRC;
				command = readAlbumToCamera(&sendCommandBytes);
				sendCommand(sendCommandBytes, fd, command);
				delay(500);
				if(checkResponse(fd) == 1){
				}else{
					dataSize = getAlbumSize(fd);
					printf("data size is %d bytes\n", dataSize);
					albumSize = getAlbumSize(fd);
					printf("album size is %d bytes\n", albumSize);
					CRC = getAlbumSize(fd);
					printf("CRC is %d\n", CRC);
					for(int i=0; i<albumSize; i++){
						printf("%d ", serialGetchar(fd));
					}
				}
			}
    }
  }		
	serialClose(fd);
  return 0;
}
