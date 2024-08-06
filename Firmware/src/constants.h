#include <Arduino.h>
#include <SPI.h>

// Constants and Register Addresses:

const byte  Product_ID      = 0x00;
const byte  Revision_ID     = 0x01;
const byte  Motion          = 0x02;
const byte  Delta_X         = 0x03;
const byte  Delta_Y         = 0x04;
const byte  SQUAL           = 0x05;
const byte  Pixel_Sum       = 0x06;
const byte  Maximum_Pixel   = 0x07;
const byte  Config_Bits     = 0x0a;
const byte  Extended_Config = 0x0b;
const byte  Data_Out_Lower  = 0x0c;
const byte  Data_Out_Upper  = 0x0d;
const byte  Shutter_Lower   = 0x0e;
const byte  Shutter_Upper   = 0x0f;
const byte  Frame_Period_Lower = 0x10;
const byte  Frame_Period_Upper = 0x11;
const byte  Motion_Clear    = 0x12;
const byte  Frame_Capture   = 0x13;
const byte  SROM_Enavle     = 0x14;

const byte  Frame_Period_Max_Bound_Lower = 0x19;
const byte  Frame_Period_Max_Bound_Upper = 0x1a;
const byte  Frame_Period_Min_Bound_Lower = 0x1b;
const byte  Frame_Period_Min_Bound_Upper = 0x1c;
const byte  Shutter_Max_Bound_Lower      = 0x1d;
const byte  Shutter_Max_Bound_Upper      = 0x1e;
const byte  SROM_ID                      = 0x1f;

const byte  Observation     = 0x3d;
const byte  Inverse_Product_ID   = 0x3f;
const byte  Pixel_Burst     = 0x40;
const byte  Motion_Burst    = 0x50;
const byte  SROM_Load       = 0x60;