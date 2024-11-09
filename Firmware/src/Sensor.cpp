// src/Sensor.cpp
#include "Sensor.h"


Sensor::Sensor( int frame_rate_fps ){

    bool is_setup = mouse_sensor.setup(led_strobe, high_res);
    Serial.print("Sensor Setup Response: ");
    Serial.println(is_setup);

    if (frame_rate_fps > 100 && frame_rate_fps < 6470){
        set_frame_rate(frame_rate_fps);
    }
}


float Sensor:: get_frame_rate(){
    Serial.println("in get frame rate");
    uint8_t FRupper = mouse_sensor.readRegister(Frame_Period_Max_Bound_Upper);
    delayMicroseconds( 100 );
    uint8_t FRlower = mouse_sensor.readRegister(Frame_Period_Max_Bound_Lower);
    uint16_t FRcounts = FRupper << 8 | FRlower;
    Serial.println(FRcounts);
    return 24000000.0/FRcounts; // return value in FPS

}


void Sensor::set_frame_rate(float frame_rate_fps){
    Serial.print(frame_rate_fps);
    framerate_counts = round(24000000/frame_rate_fps);
    Serial.print(framerate_counts);
    uint8_t fpsmask = (byte) framerate_counts & 255;
    mouse_sensor.writeRegister(Frame_Period_Max_Bound_Lower, fpsmask);
    delayMicroseconds(75);
    fpsmask = (byte) (framerate_counts >> 8) & 255;
    mouse_sensor.writeRegister(Frame_Period_Max_Bound_Upper, fpsmask);
    delayMicroseconds(75);
    mouse_sensor.writeRegister(Frame_Period_Min_Bound_Lower, 0x00);
    delayMicroseconds(75);
    mouse_sensor.writeRegister(Frame_Period_Min_Bound_Upper, 0x10);
    delayMicroseconds(75);
}