#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include "STBAPI.h"
#include "STBCommonDef.h"
#include "STBTypedef.h"

int main(){
  //シリアルポートをオープン
  int fd = serialOpen("/dev/ttyACM0", 9600);
  if(fd<0){
    printf("can not ipen serialport");
  }else{
		printf("%d\n",fd);
	}
  while(1){
    //送信処理
		int input = 0x000000FE;
		char command[] = {0xFE, 0x00, 0x00, 0x00};
		printf("%s",command);
		serialPuts(fd,command);


		//受信処理
    while(serialDataAvail(fd)){
      printf("%c", serialGetchar(fd));
			printf("---\n");
      fflush(stdout);
    }
		printf("loop finish\n");

    delay(100);
  }

  return 0;
}

/*
int com_send(int fd, unsigned char *buf, int len)
{
    DWORD dwSize = 0;
    serialPuts(fd, buf,len);
    return (int)dwSize;
}
*/
