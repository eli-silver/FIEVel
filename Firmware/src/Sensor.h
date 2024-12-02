// src/Sensor.h
#ifndef SENSOR_H
#define SENSOR_H

#include "DataPoint.h"
#include "ADNS3080.h"
#define PIN_RESET     9        
#define PIN_CS        10
// LED Pin to trigger motion burst read:
#define PIN_LED_TRIG 6

class Sensor {

private:
    uint16_t framerate_counts;
    uint16_t shutter_time_counts;
    const bool high_res = true;
    const bool led_strobe = true;
    bool frame_capture;
    bool poll_motion;
    bool return_zeros;
    bool isAutoExposure;

    void frame_interrupt(void);


public: 
    Sensor(int frameRateFPS = 6000);

    float get_frame_rate();
    void set_frame_rate(float frame_rate_fps);

    float get_shutter_time();
    void set_shutter_time(float shutter_time_us);

    bool get_led_mode();
    void set_led_mode(bool isStrobeLED);

    DataPoint read_displacement();
};


#endif // SENSOR_H