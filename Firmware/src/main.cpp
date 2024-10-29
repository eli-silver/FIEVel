
#include "ADNS3080.h"
#include "ADNS3080.tpp"


// SPI pins:
#define PIN_RESET     9        
#define PIN_CS        10

// LED Pin to trigger motion burst read:
#define PIN_LED_TRIG 6

ADNS3080 <PIN_RESET, PIN_CS> sensor;

// Sync with python script
#define BEGIN_CHAR    'A'

// Initial position
int x = 0;
int y = 0;

uint32_t prev_micros = 0;
bool frame_capture = false;
bool poll_motion = false;
bool return_zeros = true;

// for polling:
IntervalTimer poll_timer;

void frame_interrupt(void){
  uint8_t motion;
  uint8_t over_flow;
  int8_t dx,dy;
  uint8_t squal;
  uint16_t shutter;
  uint8_t max_pix;

  sensor.motionBurst(&motion, &over_flow, &dx, &dy, &squal, &shutter, &max_pix);
  uint32_t microseconds = micros();
  if((motion == true) | return_zeros) {
    uint32_t frame_time = (microseconds - prev_micros);
    prev_micros = microseconds;
    Serial.printf("%lu %.3lu %u %.3d %u %.3d %.3d \n",microseconds,frame_time,shutter,squal,motion,dx,dy); 
  }
}


void setup() {
  if (frame_capture){
    Serial.begin(200000); // must match python script
  }else{
    Serial.begin(2500000);
  }
  bool is_setup =  sensor.setup( true, true );  

  Serial.print("Sensor Setup Response: ");
  Serial.println(is_setup);


  uint8_t FRupper = sensor.readRegister(Frame_Period_Max_Bound_Upper);
  delayMicroseconds( 100 );
  uint8_t FRlower = sensor.readRegister(Frame_Period_Max_Bound_Lower);
  uint16_t FRcounts = FRupper << 8 | FRlower;
  Serial.printf("%lu \n",FRcounts);
  
  if(frame_capture){
    Serial.flush();
    Serial.println( BEGIN_CHAR );
  }else if(poll_motion){
    poll_timer.begin(frame_interrupt, 157);
  }
  else{
    prev_micros = micros();
    attachInterrupt(digitalPinToInterrupt(PIN_LED_TRIG), frame_interrupt, FALLING);
  } 
}

void loop() {
  if(frame_capture){
      // Get frame data
    uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y];
    sensor.frameCapture( frame );

    // Scan grid and send pixel color
    for( int y = 0; y < ADNS3080_PIXELS_Y; y += 1 ) {
      for( int x = 0; x < ADNS3080_PIXELS_X; x += 1 ) {
        uint8_t val = frame[x][y];
        if (val == 41 ){
          val = 42;
        }
        Serial.println( val );
      }
    }
    delay(100); // limiting framerate to help frame shift artifacts
    // Indicate new frame
    Serial.println( BEGIN_CHAR );  
  }
  else{
    // Data is 
    delay(10);
  }
}
