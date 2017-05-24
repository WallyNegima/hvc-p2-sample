#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define serialPath "/dev/ttyACM0"
#define baudrate (9600)


int main(){
  /*シリアルオープン*/
  int fd = serialOpen(serialPath, baudrate);
  if(fd<0){
    printf("can not open serialPort");
  }else{
    printf("connected!");
  }
}

  
