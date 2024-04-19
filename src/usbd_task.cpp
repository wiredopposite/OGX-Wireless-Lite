#include <stdint.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "tusb.h"
#include "bsp/board_api.h"

#include "usbd/board_config.h"
#include "usbd/drivermanager.h"
#include "usbd/drivers/gpdriver.h"
#include "bp32_rumble.h"
#include "usbd_task.h"
#include "user_settings/input_mode.h"
#include "Gamepad.h"

void usbd_task(void* param)
{
    (void) param;

    InputMode mode = get_input_mode();
    DriverManager& driverManager = DriverManager::getInstance();
    driverManager.setup(mode); // tud_init gets called here

    Gamepad local_gamepad;
    GamepadRumble local_rumble;
    GamepadButtons prev_buttons = gamepad(0)->buttons;

    unsigned long last_time_gamepad_changed = esp_timer_get_time() / 1000;
    unsigned long last_time_gamepad_checked = esp_timer_get_time() / 1000;

    while (1)
    {
        for (int i = 0; i < MAX_GAMEPADS; i++)
        {
            uint8_t outBuffer[64];
            GPDriver* driver = driverManager.getDriver();

            local_gamepad = *gamepad(i);

            driver->process(i, &local_gamepad, outBuffer);
            driver->update_rumble(i, gamepad(i));

            local_rumble = gamepad(i)->rumble;
            process_rumble(i, local_rumble);

            vTaskDelay(1);
        }

        if (((esp_timer_get_time() / 1000) - last_time_gamepad_checked) >= 500) 
        {
            // check if digital buttons have changed
            if (memcmp(&gamepad(0)->buttons, &prev_buttons, sizeof(GamepadButtons)) != 0) 
            {
                prev_buttons = gamepad(0)->buttons;
                last_time_gamepad_changed = esp_timer_get_time() / 1000;
            }

            // haven't changed for 3 seconds
            else if (((esp_timer_get_time() / 1000) - last_time_gamepad_changed) >= 3000) 
            {
                if (!update_input_mode(prev_buttons))
                {
                    last_time_gamepad_changed = esp_timer_get_time() / 1000;
                }
            }

            last_time_gamepad_checked = esp_timer_get_time() / 1000;
        }

        vTaskDelay(1);
    }
}