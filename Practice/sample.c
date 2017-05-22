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
  }

  while(1){
    //送信処理
		int input = 0x00;
		char command[1];
		itoa(input, command, 16);
    serialPuts(fd,command);
		

		//受信処理
    while(serialDataAvail(fd)){
      printf("recieve %c", serialGetchar(fd));
      fflush(stdout);
    }

    delay(100);
  }

  return 0;
}
