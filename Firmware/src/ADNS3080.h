/*
MIT License

Copyright (c) 2022 RCmags

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <Arduino.h>
#include <SPI.h>

#ifndef ADNS3080_h
#define ADNS3080_h 

//------------ Constants and registers ---------------

// Signal delay time:
#define ADNS3080_T_IN_RST             500
#define ADNS3080_T_PW_RESET           10
#define ADNS3080_T_SRAD_MOT           75
#define ADNS3080_T_SWW                50
#define ADNS3080_T_SRAD               50
#define ADNS3080_T_LOAD               10
#define ADNS3080_T_BEXIT              4

// Pixel dimensions:
#define ADNS3080_PIXELS_X             30
#define ADNS3080_PIXELS_Y             30

// Registers: 
#define ADNS3080_PRODUCT_ID           0x00
#define ADNS3080_SQUAL                0x05
#define ADNS3080_MAXIMUM_PIXEL        0x07
#define ADNS3080_CONFIGURATION_BITS   0x0a
#define ADNS3080_MOTION_CLEAR         0x12
#define ADNS3080_FRAME_CAPTURE        0x13
#define ADNS3080_PIXEL_BURST          0x40
#define ADNS3080_MOTION_BURST         0x50
#define ADNS3080_PRODUCT_ID_VALUE     0x17

const byte  Extended_Config = 0x0b;
const byte  Frame_Period_Max_Bound_Lower = 0x19;
const byte  Frame_Period_Max_Bound_Upper = 0x1a;
const byte  Frame_Period_Min_Bound_Lower = 0x1b;
const byte  Frame_Period_Min_Bound_Upper = 0x1c;
const byte  Shutter_Max_Bound_Lower      = 0x1d;
const byte  Shutter_Max_Bound_Upper      = 0x1e;

// SPI Clock Rate:
#define SPI_RATE                      2000000
//#define SPI_RATE                      1000000


//--------------- Template Parameters ---------------- [ No characters after backlash! ]

#define TEMPLATE_TYPE           \
        uint8_t PIN_RESET,      \
        uint8_t PIN_NCS         
    
#define TEMPLATE_INPUTS         \
                PIN_RESET,      \
                PIN_NCS 
  
//---------------- Class definition ------------------
  
template <TEMPLATE_TYPE>
class ADNS3080 {  
  private:
    // Read and write registers:
    //void writeRegister( const uint8_t, uint8_t );
    //uint8_t readRegister( const uint8_t );
   
  public:   
    // Miscellaneous functions:
    void reset();
    bool setup( const bool, const bool );
    void motionClear();
    void writeRegister( const uint8_t, uint8_t );
    uint8_t readRegister( const uint8_t );
    
    // Major outputs:
    void motionBurst( uint8_t*, uint8_t*, int8_t*, int8_t*, uint8_t*, uint16_t*, uint8_t* );
    void displacement( int8_t*, int8_t*, int8_t*, int8_t* );
    void frameCapture( uint8_t[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y] );
};

//#include "ADNS3080.tpp"

#endif
