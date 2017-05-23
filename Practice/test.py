import serial
import time
#import usb.core
#import usb.utill

ser = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(2)
ser.write(bytes([0xFE, 0x00, 0x00, 0x00]))
print(u'send')
line = ser.read(20)

print(line)
ser.close()

