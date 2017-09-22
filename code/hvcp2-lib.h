#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <wiringPi.h>
#include <wiringSerial.h>

/*
コマンドを返すだけのライブラリ
*/

//intにはコマンドのバイト数を格納
//コマンドのバイト列を返す
unsigned char* saveAlbumToHost(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 4;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x20;
  command[2] = 0x00;
  command[3] = 0x00;

  return command;
}

//アルバムをホストからカメラへ引っ張る
unsigned char* readAlbumToCamera(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 4;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x21;
  command[2] = 0x04;
  command[3] = 0x00;

  return command;
}

 int getAlbumSize(int fd){
	int albumSize;
	int lsb_l, lsb_m, msb_l, msb_m; //msb_m msb_l lsb_m lsb_l の順番
	lsb_l = serialGetchar(fd);
	lsb_m = serialGetchar(fd);
	msb_l = serialGetchar(fd);
	msb_m = serialGetchar(fd);
	albumSize =lsb_l;
	albumSize = albumSize | (lsb_m << 8) | (msb_l << 16) | (msb_m << 24);
	return albumSize;
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

//カメラアングルの情報を取得
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

//顔や体を検出するコマンド
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


//顔を登録するコマンドを返す
unsigned char* getRegisterCommand(int* sendCommandBytes, unsigned short userid, unsigned char dataid){
  unsigned char* command;
  *sendCommandBytes = 7;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x10;
  command[2] = 0x03;
  command[3] = 0x00;
  command[4] = userid & 0b00001111; //IDのLSB
  command[5] = userid & 0b11110000; //IDのMSB
  command[6] = dataid; //データID

  return command;
}

//ROMに顔データを保存
unsigned char* getRegisterToRom(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 4;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x22;
  command[2] = 0x00;
  command[3] = 0x00;

  return command;
}
//ROMのアルバムデータをフォーマット
unsigned char* resetROMData(int* sendCommandBytes){
  unsigned char* command;
  *sendCommandBytes = 4;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x30;
  command[2] = 0x00;
  command[3] = 0x00;

  return command;
}
//コマンドを送信
void sendCommand(int sendCommandBytes, int fd, unsigned char* command){
  serialFlush(fd);
  for(int i=0; i<sendCommandBytes; i++){
    serialPutchar(fd, command[i]);
  }
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
int getResponseBytes(int fd){
  unsigned char tmp_datasize[4];
  int datasize;
  //printf("datasize:");
  for(int i=0; i<4; i++){
    tmp_datasize[i] = serialGetchar(fd);
  }
  datasize = (long)tmp_datasize[0];
  datasize = datasize | (tmp_datasize[1] << 8) | (tmp_datasize[2] << 16) | (tmp_datasize[3] << 24);
  return datasize;
}

//レスポンスをチェックする
int checkResponse(int fd){
  int responseBytes;
  if(responseIsErr(fd) == 1){
    printf("header err\n");
    return 1;
  }else{
    responseBytes = getResponseBytes(fd);
    printf("seponse is %d bytes\n", responseBytes);
		return responseBytes;
  }

  return responseBytes;
}

