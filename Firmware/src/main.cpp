
#include "ADNS3080.h"
#include "ADNS3080.tpp"


// SPI pins:
#define PIN_RESET     9        
#define PIN_CS        10

const int PIN_STP = 33;
const int PIN_DIR = 34;

// LED Pin to trigger motion burst read:
#define PIN_LED_TRIG 6

ADNS3080 <PIN_RESET, PIN_CS> sensor;

IntervalTimer step_timer;
int step_state = 0;
bool motor_running = 0;

// Sync with python script
#define BEGIN_CHAR    'A'

// Initial position
int x = 0;
int y = 0;

uint32_t prev_micros = 0;

//bool frame_capture = true;
bool frame_capture = false;

//bool poll_motion = true;
bool poll_motion = false;
bool return_zeros = true;
//bool return_zeros = false;

// for polling:
IntervalTimer poll_timer;

// moving average parameters:
const int nvalues = 100;             // Moving average window size

static int current = 0;            // Index for current value
static int cvalues = 0;            // Count of values read (<= nvalues)
static float sum = 0;               // Rolling sum
static float values[nvalues];

void step_isr(void){
  step_state = !step_state;
  digitalWriteFast(PIN_STP, step_state);
}

void set_motor(int32_t speed){
  // we need microseconds per half step period. Interrupt with toggle
  // step pin state at this frequency
  // forgot the 1:3 gear ratio at first! rpm of motor is 3x rpm of disk

    // NEW METHOD:
  // speed(mm/s) * 1600(pulses / rev) * 1/400 (rev/mm) = pulses / second 
  // period = 1/(pulses/second) = seconds/pulse => * 1_000_000(us/s) * 1/2 (half period / period)
  // 400(mm/rev)/1600(pulses/rev) * 1_000_000(us/s) /2(half period/period) = 125_000

  if(speed==0){
    step_timer.end();
    motor_running = false;
    return;
  }

  if(speed > 0){
    digitalWrite(PIN_DIR, 1);
  }else{
    digitalWrite(PIN_DIR, 0);
  }
  float half_period = 125000.0 / abs(speed * 3);

  if(!motor_running){
    step_timer.begin(step_isr, half_period);
    step_timer.priority(1);
    motor_running = true;
    return;
  } 
  step_timer.update(half_period);
}



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

    //float speed_x = 1000 * float(dx)/frame_time; // units are in counts / ms
    //float speed_y = 1000 * float(dy)/frame_time;

    //sum += speed_x;

  // If the window is full, adjust the sum by deleting the oldest value
  //if (cvalues == nvalues)
  //  sum -= values[current];
  //values[current] = speed_x;          // Replace the oldest with the latest
  //if (++current >= nvalues)
  //  current = 0;
  if (cvalues < nvalues)
    cvalues += 1; 
  //float speed_x_avg = sum/cvalues;
  
  //float speed_x_mm_s = speed_x_avg  * 0.007142 * 1000; // units are (counts/ms) * (mm/count) * (ms/s) = mm/s 


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

  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_STP, OUTPUT);

  Serial.print("Setup Response: ");
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
    set_motor(2000);
    attachInterrupt(digitalPinToInterrupt(PIN_LED_TRIG), frame_interrupt, FALLING);
  } 
}

void loop() {
  if(Serial.available()){
    Serial.println("serial available!");
    int new_speed = Serial.parseInt();
    Serial.print(" New Speed: ");
    Serial.println(new_speed);
    set_motor(new_speed);
  }

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
    delay(100); // limiting framerate seemed to help strange frame artifacts 
    // Indicate new frame
    Serial.println( BEGIN_CHAR );  
  }
  else{
    delay(10);
  }
}
