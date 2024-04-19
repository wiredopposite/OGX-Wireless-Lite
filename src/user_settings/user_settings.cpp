#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "user_settings/user_settings.h"
#include "user_settings/nvs_helpers.h"
#include "usbd/board_config.h"
#include "Gamepad.h"

#define NVS_PROFILE_KEY_PREFIX "profile"
#define NVS_PROFILE_KEY_SIZE (sizeof(NVS_PROFILE_KEY_PREFIX) + 3 + 1)
#define NVS_ACTIVE_PROFILE_ID_KEY_PREFIX "active_id"
#define NVS_ACTIVE_PROFILE_ID_KEY_SIZE (sizeof(NVS_ACTIVE_PROFILE_ID_KEY_PREFIX) + 3 + 1)

#define DEFAULT_PROFILE_ID 0x01
#define MAX_PROFILES 8

UserProfile get_default_profile(uint8_t profile_id);
void get_variable_nvs_key(char* key_buffer, const size_t buffer_len, const char* prefix, const int suffix);

//
// Read/Write
//

bool store_active_profile_id(int idx, uint8_t profile_id) 
{
    char profile_id_key[NVS_ACTIVE_PROFILE_ID_KEY_SIZE];
    get_variable_nvs_key(profile_id_key, NVS_ACTIVE_PROFILE_ID_KEY_SIZE, NVS_ACTIVE_PROFILE_ID_KEY_PREFIX, idx);

    return write_uint8_to_nvs(profile_id, profile_id_key);
}

bool retrieve_active_profile_id(int idx, uint8_t* profile_id_buffer) 
{
    char profile_id_key[NVS_ACTIVE_PROFILE_ID_KEY_SIZE];
    get_variable_nvs_key(profile_id_key, NVS_ACTIVE_PROFILE_ID_KEY_SIZE, NVS_ACTIVE_PROFILE_ID_KEY_PREFIX, idx);

    return retrieve_uint8_from_nvs(profile_id_buffer, profile_id_key);
}

bool profile_exists_in_nvs(uint8_t profile_id)
{
    char profile_key[NVS_PROFILE_KEY_SIZE];
    get_variable_nvs_key(profile_key, NVS_PROFILE_KEY_SIZE, NVS_PROFILE_KEY_PREFIX, (int)profile_id);

    size_t size = 0;

    if (blob_exists_in_nvs(profile_key, &size))
    {
        if (size == sizeof(UserProfile))
        return true;
    }

    return false;
}

bool store_user_profile(int idx, uint8_t profile_id, const UserProfile& profile)
{   
    char profile_key[NVS_PROFILE_KEY_SIZE];
    get_variable_nvs_key(profile_key, NVS_PROFILE_KEY_SIZE, NVS_PROFILE_KEY_PREFIX, (int)profile_id);

    if (write_blob_to_nvs((const uint8_t*)&profile, sizeof(UserProfile), profile_key))
    {
        if (store_active_profile_id(idx, profile_id))
        {
            gamepad(idx)->profile = profile;
            printf("Settings stored for profile %d\n", profile_id);
            return true;
        }
    }

    printf("Failed to store profile %d settings", profile_id);

    return false;
}

bool retrieve_user_profile(uint8_t profile_id, UserProfile* profile_buffer) 
{
    char profile_key[NVS_PROFILE_KEY_SIZE]; 
    get_variable_nvs_key(profile_key, NVS_PROFILE_KEY_SIZE, NVS_PROFILE_KEY_PREFIX, (int)profile_id);

    size_t required_size = sizeof(UserProfile);
    bool success = retrieve_blob_from_nvs((uint8_t*)profile_buffer, &required_size, profile_key);

    if (!success) 
    {
        printf("Failed to retrieved Profile %d.\n", profile_id);
        return false;
    }

    printf("Successfully retrieved Profile %d\n", profile_id);
    return true;
}

void init_user_settings()
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || 
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        printf("NVS error detected, erasing.\n");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        ESP_ERROR_CHECK(err);
    }
    else if (err != ESP_OK)
    {
        printf("Error: NVS not initiated. Error: %s\n", esp_err_to_name(err));

        for (int i = 0; i < MAX_GAMEPADS; i++) 
        {
            gamepad(i)->profile = get_default_profile(DEFAULT_PROFILE_ID);
        }

        return;
    }

    bool new_profiles = false;

    for (int i = 0; i < MAX_PROFILES; i++)
    {
        uint8_t temp_profile_id = i + 1;

        if (!profile_exists_in_nvs(temp_profile_id))
        {
            new_profiles = true;

            vTaskDelay(5);

            printf("Profile ID %d not found, writing profile to NVS.\n", temp_profile_id);

            store_user_profile(i % MAX_GAMEPADS, temp_profile_id, get_default_profile(temp_profile_id));
        }

        vTaskDelay(5);
    }

    for (int i = 0; i < MAX_GAMEPADS; i++) 
    {
        if (new_profiles)
        {
            store_active_profile_id(i, DEFAULT_PROFILE_ID);
        }

        uint8_t active_profile_id;
        UserProfile profile;

        if (retrieve_active_profile_id(i, &active_profile_id) && 
            retrieve_user_profile(active_profile_id, &profile)) 
        {
            printf("Profile ID %d loaded for gamepad %d.\n", active_profile_id, i);
        } 
        else 
        {
            printf("Error loading profile for gamepad %d; using default.\n", i);
            profile = get_default_profile(DEFAULT_PROFILE_ID);
        }
        
        gamepad(i)->profile = profile;

        vTaskDelay(5);
    }
}

//
// Helpers
//

void get_variable_nvs_key(char* key_buffer, const size_t buffer_len, const char* prefix, const int suffix)
{
    snprintf(key_buffer, buffer_len, "%s%d", prefix, suffix);
}

UserProfile get_default_profile(uint8_t profile_id)
{
    UserProfile default_profile = {};

    default_profile.profile_id = profile_id;
    snprintf(default_profile.profile_name, sizeof(default_profile.profile_name), "Profile %d", profile_id);

    default_profile.ly_invert  = false;
    default_profile.ry_invert  = false;
    
    default_profile.deadzone.ly = 0;
    default_profile.deadzone.ry = 0;
    default_profile.deadzone.lt = 0;
    default_profile.deadzone.rt = 0;

    default_profile.up      = MASK_DPAD_UP;
    default_profile.down    = MASK_DPAD_DOWN;
    default_profile.left    = MASK_DPAD_LEFT;
    default_profile.right   = MASK_DPAD_RIGHT;
    default_profile.a       = MASK_BUTTON_A;
    default_profile.b       = MASK_BUTTON_B;
    default_profile.x       = MASK_BUTTON_X;
    default_profile.y       = MASK_BUTTON_Y;
    default_profile.l3      = MASK_BUTTON_L3;
    default_profile.r3      = MASK_BUTTON_R3;
    default_profile.back    = MASK_BUTTON_BACK;
    default_profile.start   = MASK_BUTTON_START;
    default_profile.lb      = MASK_BUTTON_LB;
    default_profile.rb      = MASK_BUTTON_RB;
    default_profile.sys     = MASK_BUTTON_SYS;
    default_profile.misc    = MASK_BUTTON_MISC;

    return default_profile;
}