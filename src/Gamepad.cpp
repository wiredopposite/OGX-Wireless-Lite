#include "Gamepad.h"
#include "usbd/board_config.h"

void Gamepad::reset_pad() 
{
    buttons.up = buttons.down = buttons.left = buttons.right = false;
    buttons.a  = buttons.b    = buttons.x    = buttons.y     = false;
    buttons.l3 = buttons.r3   = buttons.back = buttons.start = false;
    buttons.rb = buttons.lb   = buttons.sys  = buttons.misc  = false;

    triggers.l = triggers.r = 0;

    joysticks.lx = joysticks.ly = joysticks.rx = joysticks.ry = 0;
}

void Gamepad::reset_rumble() 
{
    rumble.r = rumble.l = 0;
}

Gamepad * gamepad(int idx)
{
    static Gamepad gamepad[MAX_GAMEPADS];

    if ((idx < MAX_GAMEPADS) && (idx >= 0))
    {
        return &gamepad[idx];
    }
    
    return nullptr;
}
