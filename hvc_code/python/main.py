import serial
import time
import sys
import command_solver

def main():
	serial_path = '/dev/ttyACM0'
	baudrate = 9600
	cs = command_solver.CommandSolver()
	ser = serial.Serial(serial_path, baudrate)
	time.sleep(2)
	
	while True:
		#コマンドを受け付ける
		print(u'コマンドを受付中')
		inputtext = input()
		
		#どのコマンドかを分岐条件から選択して実行
		commandNum = -1 #commandNum には実行するコマンド番号
		resnponseBytes = -1 #responseBytesには応答のバイト数
		commandNum, responseBytes = cs.solveCommand(inputtext)
		if commandNum == -1:
			#終了処理
			ser.close()
			sys.exit()
		else:
			#何かしらのコマンドを実行
			if commandNum == 0:
				print(u'バージョン情報の所得')
				ser.write(bytes([0xFE, 0x00, 0x00, 0x00]))
			elif commandNum == 1:
				print(u'カメラの角度情報取得')
				ser.write(bytes([0xFE, 0x02, 0x00, 0x00]))
			
			#応答の読み取り
			if responseBytes > 0:
				line = ser.read(6+responseBytes)
				print(line)
				print("done") 


if __name__ == "__main__":
	main()
