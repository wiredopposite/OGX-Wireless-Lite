#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "user_settings/input_mode.h"
#include "user_settings/nvs_helpers.h"

#define NVS_INPUT_MODE_KEY "input_mode"

static InputMode current_input_mode;

bool update_input_mode(GamepadButtons buttons)
{
    if (!buttons.start)
    {
        return false;
    }

    InputMode new_mode = current_input_mode;

    if (buttons.up)
    {
        new_mode = INPUT_MODE_XINPUT;
    }
    else if (buttons.left)
    {
        new_mode = INPUT_MODE_HID;
    }
    else if (buttons.right)
    {
        new_mode = INPUT_MODE_XBOXORIGINAL;
    }
    else if (buttons.down)
    {
        new_mode = INPUT_MODE_SWITCH;
    }
    else if (buttons.a)
    {
        new_mode = INPUT_MODE_PSCLASSIC;
    }

    if (new_mode != current_input_mode)
    {
        if (write_uint8_to_nvs((uint8_t)new_mode, NVS_INPUT_MODE_KEY))
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            esp_restart();
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

enum InputMode get_input_mode()
{
    #if (CDC_DEGUB > 0)
        return INPUT_MODE_USBSERIAL;
    #endif

    uint8_t stored_value;

    nvs_flash_init();

    if (!retrieve_uint8_from_nvs(&stored_value, NVS_INPUT_MODE_KEY))
    {
        current_input_mode = INPUT_MODE_XBOXORIGINAL;
        return current_input_mode;
    }

    if ((InputMode)stored_value >= INPUT_MODE_XINPUT && (InputMode)stored_value <= INPUT_MODE_XBOXORIGINAL)
    {
        current_input_mode = (InputMode)stored_value;
    } 
    else 
    {
        current_input_mode = INPUT_MODE_XBOXORIGINAL;
    }

    return current_input_mode;
}