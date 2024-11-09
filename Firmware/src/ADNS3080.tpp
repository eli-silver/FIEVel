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


//------------ Read and write registers --------------
#ifndef ADNS3080_TPP
#define ADNS3080_TPP
#include "ADNS3080.h"

template<TEMPLATE_TYPE>
void ADNS3080<TEMPLATE_INPUTS>
::writeRegister( const uint8_t reg, uint8_t output ) {
  // Enable communication
  digitalWrite( PIN_NCS, LOW );
  SPI.beginTransaction(SPISettings(SPI_RATE, MSBFIRST, SPI_MODE3));
  SPI.transfer( reg | B10000000 );
  
  // Send value
  SPI.transfer( output );
  SPI.endTransaction();
  // Disable communcation
  digitalWrite( PIN_NCS, HIGH );
  delayMicroseconds( ADNS3080_T_SWW );
}

template<TEMPLATE_TYPE>
uint8_t ADNS3080<TEMPLATE_INPUTS>
::readRegister( const uint8_t reg ) {
  // Enable communication
  digitalWrite( PIN_NCS, LOW );
  SPI.beginTransaction(SPISettings(SPI_RATE, MSBFIRST, SPI_MODE3));
  SPI.transfer( reg );
  delayMicroseconds( ADNS3080_T_SRAD_MOT );
  
  // Receive value
  uint8_t output = SPI.transfer(0x00);
  
  // Dissable communication
  SPI.endTransaction();
  digitalWrite( PIN_NCS, HIGH ); 
  return output;
}

//------------ Miscellaneous functions ---------------

// Cycle reset pin
template<TEMPLATE_TYPE>
void ADNS3080<TEMPLATE_INPUTS>
::reset() {
  digitalWrite( PIN_RESET, HIGH );
  delayMicroseconds( ADNS3080_T_PW_RESET );                  
  digitalWrite( PIN_RESET, LOW );
  delayMicroseconds( ADNS3080_T_IN_RST );              
}

// Initialize and configure sensor
template<TEMPLATE_TYPE>
bool ADNS3080<TEMPLATE_INPUTS>
::setup( const bool led_mode, const bool resolution ) {
  
  // Configure SPI
  SPI.begin();
  //SPI.setClockDivider( SPI_CLOCK_DIV32 );
  //SPI.setDataMode( SPI_MODE3 );
  //SPI.setBitOrder( MSBFIRST );  
  
  // Set sensor pins
  pinMode( PIN_RESET, OUTPUT );
  pinMode( PIN_NCS,   OUTPUT );

  // Disable communication and reset 
  digitalWrite( PIN_NCS, HIGH );
  reset();
  delay(5);
  // Configure sensor:
  Serial.print(led_mode);
  Serial.println(resolution);
  delay(1);
  //                          LED Shutter    High resolution
  byte current_config = readRegister(ADNS3080_CONFIGURATION_BITS);
  uint8_t mask = current_config | led_mode << 6 | resolution << 4;  
  writeRegister( ADNS3080_CONFIGURATION_BITS, mask );
  delayMicroseconds(50);
  byte current_ext_config = readRegister(Extended_Config);
  writeRegister(Extended_Config, current_ext_config | B00000011);
  //writeRegister(Extended_Config, current_ext_config & B11111100);

  // We set frame_period_max to 3710 counts / 24MHz clock giving 6469fps
  // max shutter must be shorter than frame period, or less than 3710
  // Arbitrary choice is 3000, which is 0x0BB8 or 1/8000 sec shutter speed

  delay(5);
////////////// SHUTTER SETTING ///////////////////
  uint16_t shutterCounts = 1500;
  uint8_t shutterMask = (byte) shutterCounts & 255;
  writeRegister(Shutter_Max_Bound_Lower, shutterMask);
  delayMicroseconds(75);
  shutterMask = (byte) (shutterCounts >> 8) & 255;
  writeRegister(Shutter_Max_Bound_Upper, shutterMask);
  
  delay(5);

  current_ext_config = readRegister(Extended_Config);
  delayMicroseconds(75);

  //if(current_ext_config > 127){
  //  delay(5);
  //}

  // 2 frames must elapse after writing Shutter_Max_Bound before
  // writing Frame_Period_Max. Extended config MSB is a busy indicator:
  //   1: register busy, do not write.   0: register ready.

  
  ////////////// FRAME SETTING ///////////////////
  float fps = 6400;
  uint16_t fpsCounts = round(24000000/fps);
  uint8_t fpsmask = (byte) fpsCounts & 255;
  writeRegister(Frame_Period_Max_Bound_Lower, fpsmask);
  delayMicroseconds(75);
  fpsmask = (byte) (fpsCounts >> 8) & 255;
  writeRegister(Frame_Period_Max_Bound_Upper, fpsmask);
  delayMicroseconds(75);
  writeRegister(Frame_Period_Min_Bound_Lower, 0x00);
  delayMicroseconds(75);
  writeRegister(Frame_Period_Min_Bound_Upper, 0x10);
  delayMicroseconds(75);


  byte product_id = readRegister(0x00);
  Serial.printf("Product Id:            0x%02x  :  ", product_id);
  Serial.println(product_id);
  delayMicroseconds(75);

  byte inv_product_id = readRegister(0x3f);
  Serial.printf("Inverse Product Id:    0x%02x  :  ", inv_product_id);
  Serial.println(inv_product_id);
  delayMicroseconds(75);

  byte config_b = readRegister(0x0a);  
  Serial.printf("Configuration Bits:    0x%02x  :  ", config_b);
  Serial.println(config_b);
  delayMicroseconds(75);

  // Check Connection

  if( readRegister(ADNS3080_PRODUCT_ID) == ADNS3080_PRODUCT_ID_VALUE ) {
    return true;
  } else {
    return false;
  } 

}

// Clear DELTA_X, DELTA_Y and motion registers
template<TEMPLATE_TYPE>
void ADNS3080<TEMPLATE_INPUTS>
::motionClear() {
  writeRegister( ADNS3080_MOTION_CLEAR, 0x00 );
}

//----------------- Major outputs --------------------

// Get motion burst data
template<TEMPLATE_TYPE>
void ADNS3080<TEMPLATE_INPUTS>
::motionBurst( uint8_t *motion, uint8_t *over_flow, int8_t *dx, int8_t *dy, uint8_t *squal, uint16_t *shutter, uint8_t *max_pix ) {
  
  // Enable communication
  digitalWrite( PIN_NCS, LOW );
  SPI.beginTransaction(SPISettings(SPI_RATE, MSBFIRST, SPI_MODE3));

  SPI.transfer( ADNS3080_MOTION_BURST );
  delayMicroseconds( ADNS3080_T_SRAD_MOT );

  // Receieve data:
  uint8_t motion_byte = SPI.transfer(0x00);
  *motion     = (motion_byte & B10000000) >> 7;
  *over_flow  = (motion_byte & B00010000) >> 4;
  //
  *dx      = SPI.transfer(0x00);
  *dy      = SPI.transfer(0x00);
  *squal   = SPI.transfer(0x00);
  *shutter = SPI.transfer16(0x00);
  *max_pix = SPI.transfer(0x00);

  // Disable communication
  SPI.endTransaction();
  digitalWrite( PIN_NCS, HIGH ); 

  // Zero displacement if motion did not occur
  if( *motion == false ) {
    *dx = 0;
    *dy = 0;
  }
}

// Get displacement data from motion burst
template<TEMPLATE_TYPE>
void ADNS3080<TEMPLATE_INPUTS>
::displacement( int8_t *dx, int8_t *dy, int8_t *squal, int8_t *max_pix ) {
  
  // Enable communication
  digitalWrite( PIN_NCS, LOW );
  SPI.beginTransaction(SPISettings(SPI_RATE, MSBFIRST, SPI_MODE3));
  SPI.transfer( ADNS3080_MOTION_BURST );
  delayMicroseconds( ADNS3080_T_SRAD_MOT );

  // Check if motion occured
  uint8_t motion  = (SPI.transfer(0x00) & B10000000) >> 7;

  if( motion == true ) {
    *dx = SPI.transfer(0x00);
    *dy = SPI.transfer(0x00);
    *squal = SPI.transfer(ADNS3080_SQUAL);
    *max_pix = SPI.transfer(ADNS3080_MAXIMUM_PIXEL);
  } else {
    *dx = 0;
    *dy = 0;
    *squal=0;
    *max_pix=0;
  }

  // Disable communication
  SPI.endTransaction();
  digitalWrite( PIN_NCS, HIGH ); 
}  

// Retreive pixels of next frame:
template<TEMPLATE_TYPE>
void ADNS3080<TEMPLATE_INPUTS>
::frameCapture( uint8_t output[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y] ) {  
  
  // Store pixel values
  writeRegister( ADNS3080_FRAME_CAPTURE, 0x83 );
  delayMicroseconds(75);

  // Enable communication
  digitalWrite( PIN_NCS, LOW );
  delayMicroseconds(50);
  SPI.beginTransaction(SPISettings(SPI_RATE, MSBFIRST, SPI_MODE3));
  SPI.transfer( ADNS3080_PIXEL_BURST );
  delayMicroseconds( ADNS3080_T_SRAD );

  //-- First pixel:
  uint8_t pixel = 0;

  // Recieve pixels until first is found 
  while( (pixel & B01000000) == 0 ) {
      pixel = SPI.transfer(0x00);
      delayMicroseconds( ADNS3080_T_LOAD );  
  }
  
  //-- Scan first frame:
  for( int y = 0; y < ADNS3080_PIXELS_Y; y += 1 ) {
    for( int x = 0; x < ADNS3080_PIXELS_X; x += 1 ) {  
      
      // Store and scale past pixel
      output[x][y] = pixel << 2; 
      
      // Receive new pixel
      pixel = SPI.transfer(0x00);
      delayMicroseconds( ADNS3080_T_LOAD );  
    }
  }
  // Disable communication
  SPI.endTransaction();
  digitalWrite( PIN_NCS, HIGH ); 
  delayMicroseconds( ADNS3080_T_LOAD + ADNS3080_T_BEXIT );
  delayMicroseconds(75);

}    

#endif // ADNS3080_TPP

