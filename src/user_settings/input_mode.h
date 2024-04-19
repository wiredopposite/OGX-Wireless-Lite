#ifndef INPUT_MODE_H_
#define INPUT_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Gamepad.h"
#include "usbd/inputmodes.h"

enum InputMode get_input_mode();
bool update_input_mode(GamepadButtons buttons);

#ifdef __cplusplus
}
#endif

#endif // INPUT_MODE_H_