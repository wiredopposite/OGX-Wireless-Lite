#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <btstack_stdio_esp32.h>
#include <uni.h>

#include "ble_server/ble_server.h"
#include "utilities/scaling.h"
#include "usbd/board_config.h"
#include "bp32_task.h"
#include "bp32_rumble.h"
#include "Gamepad.h"

uni_hid_device_t* bp32_gamepad[MAX_GAMEPADS] = {nullptr};

// This is here so I don't have to change the BLE app or user profile structure, will change later
GamepadMappings map_gamepad(const UserProfile &profile) 
{
    uint16_t button_mask_values[] = 
    {
        MASK_DPAD_UP,
        MASK_DPAD_DOWN,
        MASK_DPAD_LEFT,
        MASK_DPAD_RIGHT,
        MASK_BUTTON_A,
        MASK_BUTTON_B,
        MASK_BUTTON_X,
        MASK_BUTTON_Y,
        MASK_BUTTON_L3,
        MASK_BUTTON_R3,
        MASK_BUTTON_LB,
        MASK_BUTTON_RB,
        MASK_BUTTON_BACK,
        MASK_BUTTON_START,
        MASK_BUTTON_SYS,
        MASK_BUTTON_MISC,
    };

    int bp32_buttons[] =
    {
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT,
        BUTTON_A,
        BUTTON_B,
        BUTTON_X,
        BUTTON_Y,
        BUTTON_THUMB_L,
        BUTTON_THUMB_R,
        BUTTON_SHOULDER_L,
        BUTTON_SHOULDER_R,
        MISC_BUTTON_BACK,
        MISC_BUTTON_START,
        MISC_BUTTON_SYSTEM,
        MISC_BUTTON_CAPTURE,
    };

    GamepadMappings mapped = {};

    for (int i = 0; i < (sizeof(button_mask_values) / sizeof(button_mask_values[0])); i++)
    {
        if (profile.up      == button_mask_values[i]) mapped.up     |= bp32_buttons[i];
        if (profile.down    == button_mask_values[i]) mapped.down   |= bp32_buttons[i];
        if (profile.left    == button_mask_values[i]) mapped.left   |= bp32_buttons[i];
        if (profile.right   == button_mask_values[i]) mapped.right  |= bp32_buttons[i];
        if (profile.a       == button_mask_values[i]) mapped.a      |= bp32_buttons[i];
        if (profile.b       == button_mask_values[i]) mapped.b      |= bp32_buttons[i];
        if (profile.x       == button_mask_values[i]) mapped.x      |= bp32_buttons[i];
        if (profile.y       == button_mask_values[i]) mapped.y      |= bp32_buttons[i];
        if (profile.l3      == button_mask_values[i]) mapped.l3     |= bp32_buttons[i];
        if (profile.r3      == button_mask_values[i]) mapped.r3     |= bp32_buttons[i];
        if (profile.lb      == button_mask_values[i]) mapped.lb     |= bp32_buttons[i];
        if (profile.rb      == button_mask_values[i]) mapped.rb     |= bp32_buttons[i];
        if (profile.back    == button_mask_values[i]) mapped.back   |= bp32_buttons[i];
        if (profile.start   == button_mask_values[i]) mapped.start  |= bp32_buttons[i];
        if (profile.sys     == button_mask_values[i]) mapped.sys    |= bp32_buttons[i];
        if (profile.misc    == button_mask_values[i]) mapped.misc   |= bp32_buttons[i];
    }

    return mapped;
}

//
// Platform Overrides/Callbacks
//
void my_platform_init(int argc, const char** argv) 
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

#if 0
    uni_gamepad_mappings_t mappings = GAMEPAD_DEFAULT_MAPPINGS;

    // Inverted axis with inverted Y in RY.
    mappings.axis_x = UNI_GAMEPAD_MAPPINGS_AXIS_RX;
    mappings.axis_y = UNI_GAMEPAD_MAPPINGS_AXIS_RY;
    mappings.axis_ry_inverted = true;
    mappings.axis_rx = UNI_GAMEPAD_MAPPINGS_AXIS_X;
    mappings.axis_ry = UNI_GAMEPAD_MAPPINGS_AXIS_Y;

    // Invert A & B
    mappings.button_a = UNI_GAMEPAD_MAPPINGS_BUTTON_B;
    mappings.button_b = UNI_GAMEPAD_MAPPINGS_BUTTON_A;

    uni_gamepad_set_mappings(&mappings);
#endif
    //    uni_bt_service_set_enabled(true);
}

void my_platform_on_init_complete(void) 
{
    logi("Bluepad32 init complete.\n");

    uni_bt_enable_new_connections_unsafe(true);

    if (1) uni_bt_del_keys_unsafe();
}

void my_platform_on_device_connected(uni_hid_device_t* d) 
{
    logi("Device connected: %p\n", d);
}

void my_platform_on_device_disconnected(uni_hid_device_t* d) 
{
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (bp32_gamepad[i] == d)
        {
            bp32_gamepad[i] = nullptr;
            break;
        }
    }

    logi("Device disconnected: %p\n", d);
}

uni_error_t my_platform_on_device_ready(uni_hid_device_t* d)
{
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (bp32_gamepad[i] == NULL)
        {
            bp32_gamepad[i] = d;
            break;
        }
    }

    return UNI_ERROR_SUCCESS;
}

void my_platform_on_controller_data(uni_hid_device_t* d, uni_controller_t* ctl) 
{
    static uni_gamepad_t* prev_bp32_gamepad[MAX_GAMEPADS] = {};
    int idx = 0;

    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (bp32_gamepad[i] == d)
        {
            idx = i;
            break;
        }
    }

    if (memcmp(&prev_bp32_gamepad[idx], &ctl->gamepad, sizeof(uni_gamepad_t)) == 0) 
    {
        return;
    }

    uni_gamepad_t*   bp32_gamepad;
    GamepadButtons   buttons;
    GamepadTriggers  triggers;
    GamepadJoysticks joysticks;
    GamepadMappings  mappings = gamepad(idx)->mappings;

    switch (ctl->klass) 
    {
        case UNI_CONTROLLER_CLASS_GAMEPAD:
            bp32_gamepad = &ctl->gamepad;
            prev_bp32_gamepad[idx] = bp32_gamepad;

            buttons.down    = (bp32_gamepad->dpad & mappings.down)  != 0;
            buttons.left    = (bp32_gamepad->dpad & mappings.left)  != 0;
            buttons.right   = (bp32_gamepad->dpad & mappings.right) != 0;
            buttons.up      = (bp32_gamepad->dpad & mappings.up)    != 0;

            buttons.a       = (bp32_gamepad->buttons & mappings.a) != 0;
            buttons.b       = (bp32_gamepad->buttons & mappings.b) != 0;
            buttons.x       = (bp32_gamepad->buttons & mappings.x) != 0;
            buttons.y       = (bp32_gamepad->buttons & mappings.y) != 0;

            buttons.l3      = (bp32_gamepad->buttons & mappings.l3) != 0;
            buttons.r3      = (bp32_gamepad->buttons & mappings.r3) != 0;
            buttons.lb      = (bp32_gamepad->buttons & mappings.lb) != 0;
            buttons.rb      = (bp32_gamepad->buttons & mappings.rb) != 0;

            buttons.back    = (bp32_gamepad->misc_buttons & mappings.back)  != 0;
            buttons.start   = (bp32_gamepad->misc_buttons & mappings.start) != 0;
            buttons.sys     = (bp32_gamepad->misc_buttons & mappings.sys)   != 0;
            buttons.misc    = (bp32_gamepad->misc_buttons & mappings.misc)  != 0;

            triggers.l = scale_bp_trigger(bp32_gamepad->brake,    gamepad(idx)->profile.deadzone.lt);
            triggers.r = scale_bp_trigger(bp32_gamepad->throttle, gamepad(idx)->profile.deadzone.rt);

            if (triggers.l < 1) triggers.l = (bp32_gamepad->buttons & BUTTON_TRIGGER_L) ? UINT8_MAX : 0;
            if (triggers.r < 1) triggers.r = (bp32_gamepad->buttons & BUTTON_TRIGGER_R) ? UINT8_MAX : 0;

            joysticks.ly = scale_bp_axis(bp32_gamepad->axis_y,  gamepad(idx)->profile.deadzone.ly, !gamepad(idx)->profile.ly_invert);
            joysticks.lx = scale_bp_axis(bp32_gamepad->axis_x,  gamepad(idx)->profile.deadzone.ly, false);
            joysticks.ry = scale_bp_axis(bp32_gamepad->axis_ry, gamepad(idx)->profile.deadzone.ry, !gamepad(idx)->profile.ry_invert);
            joysticks.rx = scale_bp_axis(bp32_gamepad->axis_rx, gamepad(idx)->profile.deadzone.ry, false);

            gamepad(idx)->buttons   = buttons;
            gamepad(idx)->triggers  = triggers;
            gamepad(idx)->joysticks = joysticks;

            break;
        default:
            break;
    }
}

const uni_property_t* my_platform_get_property(uni_property_idx_t idx) 
{
    ARG_UNUSED(idx);
    return NULL;
}

void my_platform_on_oob_event(uni_platform_oob_event_t event, void* data) 
{
    switch (event) 
    {
        case UNI_PLATFORM_OOB_GAMEPAD_SYSTEM_BUTTON: 
        {
            uni_hid_device_t* d = reinterpret_cast<uni_hid_device_t*>(data);

            if (d == NULL) 
            {
                loge("ERROR: OOB Event: Invalid NULL device\n");
                return;
            }

            break;
        }

        case UNI_PLATFORM_OOB_BLUETOOTH_ENABLED:
            logi("OOB Event: Bluetooth enabled: %d\n", (bool)(data));
            break;

        default:
            logi("OOB Event: unsupported event: 0x%04x\n", event);
            break;
    }
}

void process_rumble(const int idx, const GamepadRumble rumble)
{
    static unsigned long last_rumble_ms[MAX_GAMEPADS] = {};
    static const int rumble_length_ms = 200;
    static const int rumble_interval = rumble_length_ms / 2;
    
    if (bp32_gamepad[idx] != nullptr)
    {
        unsigned long current_ms = esp_timer_get_time() / 1000;

        if (((current_ms - last_rumble_ms[idx]) > rumble_interval) && 
            bp32_gamepad[idx]->vendor_id != 0x057E)
        {
            #if (OGXW_DEBUG > 0)
            printf("IDX: %d | rumble right: %d | rumble left: %d\n", idx, rumble.r, rumble.l);
            #endif

            bp32_gamepad[idx]->report_parser.play_dual_rumble( bp32_gamepad[idx], 
                                                               0, 
                                                               rumble_length_ms, 
                                                               rumble.r, 
                                                               rumble.l);

            last_rumble_ms[idx] = esp_timer_get_time() / 1000;
        }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}

//
// Entry Point
//
struct uni_platform* get_my_platform(void) 
{
    static struct uni_platform plat = 
    {
        .name = "custom",
        .init = my_platform_init,
        .on_init_complete = my_platform_on_init_complete,
        .on_device_connected = my_platform_on_device_connected,
        .on_device_disconnected = my_platform_on_device_disconnected,
        .on_device_ready = my_platform_on_device_ready,
        .on_controller_data = my_platform_on_controller_data,
        .get_property = my_platform_get_property,
        .on_oob_event = my_platform_on_oob_event,
    };

    return &plat;
}

void bp32_task(void* param)
{
    (void) param;

    init_user_settings();
    
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        gamepad(i)->mappings = map_gamepad(gamepad(i)->profile);
    }

    // Don't use BTstack buffered UART. It conflicts with the console.
// #ifdef CONFIG_ESP_CONSOLE_UART
// #ifndef CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
//     btstack_stdio_init();
// #endif  // CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
// #endif  // CONFIG_ESP_CONSOLE_UART

    btstack_init();

    uni_platform_set_custom(get_my_platform()); // Must be called before uni_init()

    uni_init(0 /* argc */, NULL /* argv */); // Init Bluepad32

    init_ble_server();

    btstack_run_loop_execute();
}