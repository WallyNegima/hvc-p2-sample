#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

/*
コマンドを返すだけのライブラリ
*/

//intにはコマンドのバイト数を格納
//コマンドのバイト列を返す
unsigned char* getRegisterAlbum(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 4;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x20;
  command[2] = 0x00;
  command[3] = 0x00;

  return command;
}

//ex. getSetCameraAngle(sendCBytes, 0); -> 0度に設定
//ex. getSetCameraAngle(sendCBytes, 1); -> 90度に設定
unsigned char* getSetCameraAngle(int* sendCommandBytes, int angle){
  unsigned char* command;
  *sendCommandBytes = 5;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x01;
  command[2] = 0x01;
  command[3] = 0x00;
  if(angle == 0){
    command[4] = 0x00;
  }else if(angle == 1){
    command[4] = 0x01;
  }else if(angle == 2){
    command[4] == 0x02;
  }else{
    command[4] = 0x03;
  }

  return command;
}

unsigned char* getReadCameraAngle(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 4;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x02;
  command[2] = 0x00;
  command[3] = 0x00;
                          
  return command; 
}

