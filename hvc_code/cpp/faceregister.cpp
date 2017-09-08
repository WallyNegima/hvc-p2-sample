#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/ioctl.h>
#include <fluent.hpp>
#include <sqlite3.h>

//検出結果を格納する構造体
typedef struct{
	long   posX;       /* Center x-coordinate */
	long   posY;       /* Center y-coordinate */
	long   size;       /* Size */
	long   confidence; /* Degree of confidence */
}RESULT;

const char* serialPath = "/dev/ttyACM0";
const int baudrate = 9600;
int commandBytes = 4+3;
int fd;
int lsb, msb;
sqlite3 *db;
char *zErrMsg;

//検出した座標，その大きさ，信頼度を求めてRESULT型の構造体に格納
void getObjectResult(RESULT* result){
	int receiveData[8];
	int i;
	for(i=0; i<8; i++){
		receiveData[i] = (int)serialGetchar(fd);
	}
	result->posX = (long)(receiveData[0] | receiveData[1]<<8);
	result->posY = (long)(receiveData[2] | receiveData[3]<<8);
	result->size = (long)(receiveData[4] | receiveData[5]<<8);
	result->confidence = (long)(receiveData[6] | receiveData[7]<<8);
}

//ビットを反転させる
long bitswap(long data){
	long swaped = 0;
	int i = 0;
	swaped =  data;
	for(i=0; i<16; i++){
		swaped <<= 1;
		data >>= 1;
		swaped |= (data & 1);
	}
	return swaped;
}

int main(){
  /*シリアルオープン*/
  fd = serialOpen(serialPath, baudrate);
  if(fd<0){
    printf("can not open serialPort");
  }else{
  //  printf("connected! fd:%d\n", fd);
  }
	unsigned char *command;
	command = (unsigned char*)malloc(sizeof(unsigned char)*commandBytes);
	command[0] = 0xFE;
	command[1] = 0x10;
	command[2] = 0x03;
	command[3] = 0x00;
	command[4] = 0x01;
	command[5] = 0x00;
	command[6] = 0x00;

	while(1){
		int i;
		serialFlush(fd); //コマンド送る前に送信途中のデータは破棄
		for(i=0; i<commandBytes; i++){
			serialPutchar(fd, command[i]);
		}
		delay(500);
		if(!serialDataAvail(fd)){
		//	printf("kitenai\n");
		}
		if(serialDataAvail(fd)){
			//結果を受け取った後の処理
			//fluentdに流すやつを用意
			fluent::Logger *logger  = new fluent::Logger();
			logger->new_forward("localhost", 24224);

			//ヘッダー部を解析
			serialGetchar(fd);
			int responseCode;
			responseCode = serialGetchar(fd);
			if( responseCode == 0x00){
				//人の顔を認識したら登録してbreak
				//データ長をもとめる
				unsigned char tmp_datasize[4];
				unsigned char l_lsb, l_msb, h_lsb, h_msb;
				unsigned long datasize;
				//printf("datasize:");
				for(i=0; i<4; i++){
					tmp_datasize[i] = serialGetchar(fd);
					//printf("%x ", tmp_datasize[i]);
				}
				h_msb = tmp_datasize[3];
				h_lsb = tmp_datasize[2];
				l_msb = tmp_datasize[1];
				l_lsb = tmp_datasize[0];

				datasize = (long)l_lsb;
				datasize = datasize | (l_msb << 8) | (h_lsb << 16) | (h_msb << 24);
				printf("total = %d byte\n", datasize);

				//画像データを取得
				unsigned char imageHead[4];
				int imageWidth, imageHeight;
				int pixelWidth, pixelHeight;
				int pixel;
				imageWidth = 64;
				imageHeight = 64;
				serialGetchar(fd);
				serialGetchar(fd);
				serialGetchar(fd);
				serialGetchar(fd);
				for(pixelHeight=0; i<imageHeight; pixelHeight++){
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
				
				//sqlite
				int rc = sqlite3_open("/var/db/sqlite/user.db", &db);
				printf("open database user.db\n");
				//create table
				const char *create_table = "create table user(id integer, name text)";
				rc = sqlite3_exec(db, create_table, 0, 0, &zErrMsg);
				printf("create table\n");
				
				//現在登録している人数を確認する
				const char *sql = "select id, name from user order by id";
				sqlite3_stmt *stmt = NULL;
				int ret = sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
				int userNum = -1;
				if(ret == SQLITE_OK && stmt){
					userNum = sqlite3_column_count(stmt);
					for(int col=0; col<userNum; col++){
						printf("%d %s\n", sqlite3_column_int(stmt, col), sqlite3_column_text(stmt, col));
					}
					printf("user num = %d \n", userNum);
				}else{
					printf("cannot get userNum\n");
				}

				sql = "insert into user(id, name) values(?, ?)";
				stmt = NULL;
				sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
				sqlite3_reset(stmt);
				//名前を入力してもらう
				printf("名前を入力してね\n");
				char username[32];
				fgets(username, sizeof(username), stdin);
				username[strlen(username)-1] = '\0';
				printf("uour name is %s \n", &username);

				sqlite3_bind_int(stmt, 1, userNum);
				sqlite3_bind_text(stmt, 2, username, strlen(username), SQLITE_TRANSIENT);
				int loop = 0;
				while(SQLITE_DONE != sqlite3_step(stmt)){
					if(loop++ > 1000){
						//error
						printf("error\n");
					}
				}
				printf("登録したよ");
				sqlite3_exec(db, "commit;",0,0,&zErrMsg);
				sqlite3_finalize(stmt);


				delete logger;
				//ROMに書き込む
				command[0] = 0xFE;
				command[1] = 0x22;
				command[2] = 0x00;
				command[3] = 0x00;
				int i=0;
				serialFlush(fd);
				for(i=0; i<4; i++){
					serialPutchar(fd, command[i]);
				}
				
				if(serialDataAvail){
					for(i=0; i<6; i++){
						serialGetchar(fd);
					}
				}

				printf("registed");
				break;
			}
		}
	}
	serialClose(fd);
	free(command);
}
