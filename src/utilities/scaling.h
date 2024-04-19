#ifndef _SCALING_H_
#define _SCALING_H_

#include <stdint.h>

#define BP32_TRIGGER_MAX 1023
#define BP32_AXIS_MAX 511

#ifdef __cplusplus
extern "C" {
#endif

double scale_value(double value, double og_min, double og_max, double new_min, double new_max);
uint8_t deadzone_percent_from_threshold(int16_t deadzone_threshold, int16_t max_value);
int16_t deadzone_threshold_from_percent(uint8_t deadzone_percent, int16_t max_value);
uint8_t scale_bp_trigger(int32_t value, int16_t deadzone_threshold);
int16_t scale_bp_axis(int16_t value, int16_t deadzone_threshold, bool invert);

#ifdef __cplusplus
}
#endif

#endif // _SCALING_H_