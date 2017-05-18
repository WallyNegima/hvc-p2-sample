#include <stdio.h>
#include <string.g>
#include <wiringPi.h>
#include <wiringSerial.h>

int main(){
  //シリアルポートをオープン
  int fd = serialOpen("");
  if(fd<0){
    printf("can not ipen serialport");
  }

  while(1){
    //受信処理
    while(serialDataAvail(df){
      printf("recieve %c", serialGetchar(fd));
      fflush(stdout);
    }

    //送信処理
    serialPuts(fd, "hello world¥n");

    delay(100);
  }

  return 0;
}
