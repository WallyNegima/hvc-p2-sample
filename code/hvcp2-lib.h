#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <math.h>
/*
コマンドを返すだけのライブラリ
*/

//コマンドを送信
void sendCommand(int sendCommandBytes, int fd, unsigned char* command){
  serialFlush(fd);
  for(int i=0; i<sendCommandBytes; i++){
    serialPutchar(fd, command[i]);
  }
	delay(500);
}

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

//int型整数のnbit目から8bitを取得して返す
unsigned char getBitFromN(int data, int n){
	unsigned char bit=0b00000000;
	for(int i=0; i<32; i++){
		if(n == i){
			for(int j = 0; j<8; j++){
				bit += data & (int)pow(2.0, (double)j);
			}
			break;
		}
		data = data >> 1;
	}
	return bit;

}

//アルバムをホストからカメラへ引っ張る
unsigned char* readAlbumToCamera(int* sendCommandBytes, int fd){
  unsigned char* command;
	FILE *fp;
	const char* album = "album.txt";
	//アルバムデータを開く
	fp = fopen(album, "rb");
	if(fp == NULL){
		printf("err file open\n");
		return command=NULL;
	}
	int dataSize, albumSize, CRC;
	fscanf(fp, "%d,%d,%d",&dataSize, &albumSize, &CRC);
	printf("dataSize:%d\n", dataSize);
	printf("albumSize:%d\n", albumSize);
	printf("CRC:%d\n",CRC);
	unsigned char lsb0, lsb1, msb0, msb1;
	unsigned char microBit[4];
  *sendCommandBytes = dataSize;
  command = (unsigned char*)malloc(sizeof(unsigned char)*(*sendCommandBytes));
  command[0] = 0xFE;
  command[1] = 0x21;
  command[2] = 0x04;
  command[3] = 0x00;
	for(int i=0; i<4; i++){
		microBit[i] = getBitFromN(dataSize, i*8);
		command[i+4]=microBit[i];
	}
	int dataSizeTemp = microBit[0];
	dataSizeTemp = dataSizeTemp | (microBit[1]<<8) | (microBit[2]<<16) | (microBit[3]<<24);
	printf("datasizetemp:%d\n",dataSizeTemp);

	//albumsize
	for(int i=0; i<4; i++){
		microBit[i] = getBitFromN(albumSize, i*8);
		command[i+8]=microBit[i];
	}
	//CRC
	for(int i=0; i<4; i++){
		microBit[i] = getBitFromN(CRC, i*8);
		command[i+12]=microBit[i];
	}
	
  serialFlush(fd);
  for(int i=0; i<16; i++){
		serialPutchar(fd, command[i]);
	}
  printf("\n");
	//アルバムデータを1行ずつ取り出して格納
	for(int i=0; i<albumSize; i++){
		unsigned char microAlbumData;
		int albumData;
		fscanf(fp, "%d", &albumData);
		microAlbumData = getBitFromN(albumData, 0);
		command[i+16] = microAlbumData;
		serialPutchar(fd, command[i+16]);
	}
	fclose(fp);
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

//ヘッダー部が正しければ1を返す
int responseIsErr(int fd){
	int err;
  if(serialGetchar(fd) != 0xFE){
    return 1;
  }
  if( (err = serialGetchar(fd)) != 0x00){
		printf("response code %d\n",err);
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

