import serial
import time

ser = serial.Serial('/dev/tty.usbmodem1', 9600)
time.sleep(2)
ser.write(0xFE000)
print(u'send')
line = ser.read(10)

print(line)
ser.close()

