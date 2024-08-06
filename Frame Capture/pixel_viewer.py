# MIT License

# Copyright (c) 2022 RCmags

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.



import serial
import tkinter

#--------------- Constants --------------------------

# Pixel grid
WINDOW_SIZE = 400
NUM_PIXEL   = 30

# Serial communication [Must match arduino sketch]
BAUD_RATE   = 2000000
SERIAL_PORT = '/dev/cu.usbmodem152838801'
BEGIN_CHAR  = 'A'

# Depedent constants
PIXEL_SIZE  = WINDOW_SIZE/NUM_PIXEL
DELAY_MSEC  = int( 1000*( 4*NUM_PIXEL*NUM_PIXEL )/BAUD_RATE )

#--------------- Variable initialization ------------

# Display window:
window = tkinter.Tk()
canvas = tkinter.Canvas( window, height = WINDOW_SIZE, width = WINDOW_SIZE )

# Pixel elements:
pixel = []
color = []

    # Scan grid
for y in range( 0, NUM_PIXEL ):
    pixel.append( [] )
    color.append( [] )
    for x in range( 0, NUM_PIXEL ):
        color[y].append(0)
        pixel[y].append( canvas.create_rectangle(  PIXEL_SIZE*x,                PIXEL_SIZE*y,
                                                   PIXEL_SIZE*x + PIXEL_SIZE,   PIXEL_SIZE*y + PIXEL_SIZE,
                                                   fill = "black",              width = 0                   ) )                                 

#--------------- Functions --------------------------

def getData():
    try:
        line = arduino.readline().decode("utf-8")
        #print( line )
        return line
    except:
        return ""

def hexColor( byte ):
    return "#%02x%02x%02x" % (byte, byte, byte)

def setPixelColor():    
    # Find start character:
    data = getData()
    
    while BEGIN_CHAR not in data:   
        data = getData()

    # Scan frame:
    for y in range( 0, NUM_PIXEL ):
        for x in range( 0, NUM_PIXEL ):
            try:
                color[x][y] = int( getData() )
            except:
                continue
            
    # Set pixel color:    
    for y in range( 0, NUM_PIXEL ): 
        for x in range( 0, NUM_PIXEL ):
            try:
                canvas.itemconfig( pixel[x][y], fill = hexColor( color[x][y] ) )
            except:
                continue
            
def updateImage(): 
    setPixelColor()
    canvas.after( DELAY_MSEC, updateImage )

#--------------- Program execution ------------------

try:
    # Make connection
    arduino = serial.Serial( SERIAL_PORT, BAUD_RATE, timeout = float(DELAY_MSEC)/1000 )
    print( "Successful connection" )

    # Update image
    canvas.pack()
    canvas.after( DELAY_MSEC, updateImage )
    window.mainloop()
    arduino.close()
except:
    print( "Connection failure" )

