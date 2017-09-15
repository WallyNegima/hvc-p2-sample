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


unsigned char* getSeachCommand(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 7;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
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

  return command;
}
