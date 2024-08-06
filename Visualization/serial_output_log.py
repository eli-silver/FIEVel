import serial
from datetime import datetime

### EDIT THESE VALUES ###
SERIAL_PORT = '/dev/cu.usbmodem000000000'   # Name of serial port here
FILE_NAME = 'my_flie'     # Descriptive name for log file
NUM_LINES = 500000                          # Number of datapoints to log
###                   ###

BAUD_RATE   = 250000


def getData():
    try:
        data = arduino.readline().decode( "utf-8" )
        print(data)
        return data
    
    except:
        print("readline failed")
        return ""


### Program Execution ###
f=open( FILE_NAME + datetime.now().strftime("%y%m%d-%H-%M-%S")+'.csv' ,'a')

try:
# Make connection
    arduino = serial.Serial( SERIAL_PORT, BAUD_RATE, timeout = 1000 )
    print( "Successful connection" )
    arduino.reset_input_buffer()

except:
    print("serial connection failure")

curr_line = 0
while(curr_line < NUM_LINES):
    try:
        if arduino.in_waiting:
         new_data = getData()
         f.write(new_data)
         curr_line += 1
    except:
        print("Readline Error!")

f.close()




