#include "utilities/scaling.h"

#define BP32_AXIS_MIN (-512)

uint8_t deadzone_percent_from_threshold(int16_t deadzone_threshold, int16_t max_value) 
{
    int32_t percent = (int32_t)deadzone_threshold * 100 / max_value;
    return (uint8_t)percent;
}

int16_t deadzone_threshold_from_percent(uint8_t deadzone_percent, int16_t max_value) 
{
    int32_t threshold = (max_value * deadzone_percent) / 100;
    return (int16_t)threshold;
}

double scale_value(double value, double og_min, double og_max, double new_min, double new_max) 
{
    if (value > og_max) 
    {
        value = og_max;
    }
    else if (value < og_min) 
    {
        value = og_min;
    }

    return ((value - og_min) / (og_max - og_min)) * (new_max - new_min) + new_min;
}

uint8_t scale_bp_trigger(int32_t value, int16_t deadzone_threshold) 
{
    if (value <= deadzone_threshold) return 0;

    return (uint8_t)scale_value(value, 0, BP32_TRIGGER_MAX, 0, UINT8_MAX);
}

int16_t scale_bp_axis(int16_t value, int16_t deadzone_threshold, bool invert) 
{
    int16_t new_value = 0;

    if (value > deadzone_threshold) 
    {
        new_value = (int16_t)scale_value(value, 0, BP32_AXIS_MAX, 0, INT16_MAX);
    } 
    else if (value < -deadzone_threshold) 
    {
        new_value = (int16_t)-scale_value(-value, 0, BP32_AXIS_MAX, 0, INT16_MAX); // don't have to clamp again if inverting, will only ever be off by 1
    } 
    else 
    {
        return 0;
    }

    if (invert)
    {
        new_value = -new_value;
    }

    return new_value;
}