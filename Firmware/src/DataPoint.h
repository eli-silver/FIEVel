#ifndef DATAPOINT_H
#define DATAPOINT_H

#include <Arduino.h>

struct DataPoint {
    bool isValid;
    bool motion;
    uint8_t overflow;
    int8_t dx, dy;
    uint8_t squal;
    uint16_t shutter;
    uint8_t max_pix;
    uint32_t time;
};

#endif // DATAPOINT_H