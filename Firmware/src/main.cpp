
#include "ADNS3080.h"
#include "ADNS3080.tpp"
#include "Sensor.h"


Sensor new_sensor = Sensor(); 

// Sync with python script
#define BEGIN_CHAR    'A'

// Initial position
int x = 0;
int y = 0;

uint32_t prev_micros = 0;
bool frame_capture = true;
bool poll_motion = false;
bool return_zeros = true;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

bool recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
            return true;
        }
    }
    return false;
}

void handleSerialInput(void){
  if(!newData) return;

  char *tokens[10]; // max number of tokens in message is 10
  char *ptr = NULL; // pointer to next token

  uint8_t i = 0;
  ptr = strtok(receivedChars, " ");
  if(ptr == NULL) return;
  while( ptr != NULL ){
    tokens[i] = ptr;
    i ++;
    ptr = strtok(NULL, " ");
  }

  uint8_t n=0;
  while(n < i){
    Serial.println(tokens[n]);
    n++;
  }

  // if(tokens[0] == "sensor" ){
  //   Serial.println("command: sensor");

  // }else if(tokens[0] == "command: mode"){
  //   Serial.println("mode");
  // }else{
  //   Serial.println("command: invalid");
  // }


  newData = false;
}


void setup() {
  Serial.begin(250000);
  float curr_framerate = new_sensor.get_frame_rate();
  Serial.printf("Frame rate: %f\n", curr_framerate);
  new_sensor.set_frame_rate(5000);
  curr_framerate = new_sensor.get_frame_rate();
  Serial.printf("New rame rate: %f\n", curr_framerate);
}


void loop() {
//   if(frame_capture){
//       // Get frame data
//     uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y];
//     //sensor.frameCapture( frame );

//     // Scan grid and send pixel color
//     for( int y = 0; y < ADNS3080_PIXELS_Y; y += 1 ) {
//       for( int x = 0; x < ADNS3080_PIXELS_X; x += 1 ) {
//         uint8_t val = frame[x][y];
//         if (val == 41 ){
//           val = 42;
//         }
//         Serial.println( val );
//       }
//     }
//     delay(100); // limiting framerate to help frame shift artifacts
//     // Indicate new frame
//     Serial.println( BEGIN_CHAR );  
//   }
  if ( recvWithEndMarker() ) {
    Serial.println(receivedChars);
    handleSerialInput();
  }


}
