#include "att_delayed_response.h"
#include "btstack.h"
#include "esp_ota_ops.h"
#include "esp_system.h"

#include "ble_server/ble_server.h"
#include "user_settings/user_settings.h"
#include "utilities/scaling.h"
#include "usbd/board_config.h"
#include "version.h"

#define CHAR_UUID_TEST_STRING       ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789013_01_VALUE_HANDLE

#define CHAR_UUID_ACTIVE_PROFILE    ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789040_01_VALUE_HANDLE

#define CHAR_UUID_START_UPDATE      ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789020_01_VALUE_HANDLE
#define CHAR_UUID_FIRMWARE_VER      ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789021_01_VALUE_HANDLE
#define CHAR_UUID_ID_AND_NAME       ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789022_01_VALUE_HANDLE
#define CHAR_UUID_MISC_SETTINGS     ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789023_01_VALUE_HANDLE
#define CHAR_UUID_MAP_BUTTONS       ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789025_01_VALUE_HANDLE
#define CHAR_UUID_MAP_MISC_BUTTONS  ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789026_01_VALUE_HANDLE
#define CHAR_UUID_COMMIT_UPDATE     ATT_CHARACTERISTIC_12345678_1234_1234_1234_123456789030_01_VALUE_HANDLE

const char *test_string = "XPad";
const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
    // Name
    0x12, // Length of the name including the type byte
    0x09, // Type: Complete Local Name
    'O', 'G', 'X', '-', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's', '-', 'L', 'i', 't', 'e',
};
const uint8_t adv_data_len = sizeof(adv_data);

esp_ota_handle_t otaHandler = 0;

static uint16_t att_read_callback(hci_con_handle_t con_handle,
                                  uint16_t att_handle,
                                  uint16_t offset,
                                  uint8_t *buffer,
                                  uint16_t buffer_size);

static int att_write_callback(hci_con_handle_t connection_handle,
                              uint16_t att_handle,
                              uint16_t transaction_mode,
                              uint16_t offset,
                              uint8_t *buffer,
                              uint16_t buffer_size);

void init_ble_server()
{
    // setup ATT server
    att_server_init(profile_data, att_read_callback, att_write_callback);

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t *)adv_data);
    gap_advertisements_enable(1);
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle,
                                  uint16_t att_handle,
                                  uint16_t offset,
                                  uint8_t *buffer,
                                  uint16_t buffer_size)
{
    UNUSED(connection_handle);

    static UserProfile profile;
    static uint8_t active_profile_id;

    
    
    // return firmware version
    if (att_handle == CHAR_UUID_FIRMWARE_VER) 
    {
        if (buffer != NULL) 
        {
            printf("Responding with firmware version");
            return att_read_callback_handle_blob((const uint8_t *)FIRMWARE_VERSION, 
                                                 (uint16_t)strlen(FIRMWARE_VERSION), 
                                                 offset, 
                                                 buffer,
                                                 buffer_size);
        } 
        else 
        {
            return (uint16_t)strlen(FIRMWARE_VERSION);
        }
    }

    // return active profile
    if (att_handle == CHAR_UUID_ACTIVE_PROFILE) 
    {
        retrieve_active_profile_id(0, &active_profile_id);

        if (buffer != NULL) 
        {
            memcpy(buffer, &active_profile_id, sizeof(active_profile_id));
            return sizeof(active_profile_id);
        } 
        else 
        {
            return sizeof(active_profile_id);
        }
    }

    // return profile id and name
    if (att_handle == CHAR_UUID_ID_AND_NAME) 
    {   
        retrieve_active_profile_id(0, &active_profile_id);
        retrieve_user_profile(active_profile_id, &profile);
        
        uint16_t data_size = 1 + strlen(profile.profile_name) + 1; // +1 for profile_id and +1 for null terminator

        if (buffer != NULL) 
        {
            buffer[0] = profile.profile_id; // Copy profile ID
            strncpy((char*)&buffer[1], profile.profile_name, buffer_size - 1); // Copy profile name
            buffer[data_size - 1] = '\0'; // Ensure null termination
            return data_size; // Return the number of bytes written to the buffer
        } 
        else 
        {
            return data_size;
        }
    }

    // return misc settings
    if (att_handle == CHAR_UUID_MISC_SETTINGS) 
    {
        if (buffer != NULL) 
        {
            buffer[0] = profile.ly_invert ? 1 : 0;
            buffer[1] = profile.ry_invert ? 1 : 0;
            buffer[2] = deadzone_percent_from_threshold(profile.deadzone.ly, (int16_t)BP32_AXIS_MAX);
            buffer[3] = deadzone_percent_from_threshold(profile.deadzone.ry, (int16_t)BP32_AXIS_MAX);
            buffer[4] = deadzone_percent_from_threshold(profile.deadzone.lt, (int16_t)BP32_TRIGGER_MAX);
            buffer[5] = deadzone_percent_from_threshold(profile.deadzone.rt, (int16_t)BP32_TRIGGER_MAX);
            return 6;
        } 
        else 
        {
            return 6;
        }
    }

    // return abxy buttons
    if (att_handle == CHAR_UUID_MAP_BUTTONS) 
    {
        if (buffer != NULL) 
        {
            // up
            buffer[0]  =  profile.up & 0xFF; 
            buffer[1]  = (profile.up >> 8) & 0xFF; 
            // down
            buffer[2]  =  profile.down & 0xFF; 
            buffer[3]  = (profile.down >> 8) & 0xFF; 
            // left
            buffer[4]  =  profile.left & 0xFF;
            buffer[5]  = (profile.left >> 8) & 0xFF;
            // right
            buffer[6]  =  profile.right & 0xFF;
            buffer[7]  = (profile.right >> 8) & 0xFF;
            // A 
            buffer[8]  =  profile.a & 0xFF;
            buffer[9]  = (profile.a >> 8) & 0xFF; 
            // B
            buffer[10] =  profile.b & 0xFF; 
            buffer[11] = (profile.b >> 8) & 0xFF; 
            // X
            buffer[12] =  profile.x & 0xFF;
            buffer[13] = (profile.x >> 8) & 0xFF;
            // Y
            buffer[14] =  profile.y & 0xFF;
            buffer[15] = (profile.y >> 8) & 0xFF;

            return 16;
        } 
        else 
        {
            return 16;
        }
    }

    // return misc buttons
    if (att_handle == CHAR_UUID_MAP_MISC_BUTTONS) 
    {
        if (buffer != NULL) 
        {
            // L3
            buffer[0] =   profile.l3    & 0xFF;
            buffer[1] =  (profile.l3    >> 8) & 0xFF; 
            // R3   
            buffer[2] =   profile.r3    & 0xFF;
            buffer[3] =  (profile.r3    >> 8) & 0xFF;
            // Back 
            buffer[4] =   profile.back  & 0xFF;
            buffer[5] =  (profile.back  >> 8) & 0xFF;
            // Start 
            buffer[6] =   profile.start & 0xFF;
            buffer[7] =  (profile.start >> 8) & 0xFF;
            // LB 
            buffer[8] =   profile.lb    & 0xFF;
            buffer[9] =  (profile.lb    >> 8) & 0xFF;
            // RB   
            buffer[10] =  profile.rb    & 0xFF;
            buffer[11] = (profile.rb    >> 8) & 0xFF;
            // Sys
            buffer[12] =  profile.sys   & 0xFF;
            buffer[13] = (profile.sys   >> 8) & 0xFF;
            // Capture
            buffer[14] =  profile.misc  & 0xFF;
            buffer[15] = (profile.misc  >> 8) & 0xFF;

            return 16;
        } 
        else 
        {
            return 16;
        }
    }

    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle,
                              uint16_t att_handle,
                              uint16_t transaction_mode,
                              uint16_t offset,
                              uint8_t *buffer,
                              uint16_t buffer_size)
{
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(connection_handle);

    static UserProfile temp_profile = {};
    static bool pending_write = false;
    static uint8_t temp_profile_id = 0;

    if (att_handle == CHAR_UUID_ACTIVE_PROFILE) // changes active profile
    {
        temp_profile_id = buffer[0];
        store_active_profile_id(0, temp_profile_id);
        return 0;
    }

    switch (att_handle) 
    {
        case CHAR_UUID_START_UPDATE:
            retrieve_user_profile(temp_profile_id, &temp_profile);
            pending_write = true; // Start of update sequence
            break;

        case CHAR_UUID_ID_AND_NAME:
            if (pending_write) 
            {
                if (buffer_size == 18) 
                {
                    temp_profile.profile_id = buffer[0]; // First byte is profile ID
                    temp_profile_id = temp_profile.profile_id;

                    // TODO: enable custom naming and trucated name length
                    memcpy(temp_profile.profile_name, &buffer[1], buffer_size - 1);
                    temp_profile.profile_name[16] = '\0'; // Ensure null-termination
                } 
                else 
                {
                    return ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
                }        
            }
            break;

        case CHAR_UUID_MISC_SETTINGS:
            if (pending_write) 
            {
                if (buffer_size == 6) 
                {
                    temp_profile.ly_invert  = buffer[0];
                    temp_profile.ry_invert  = buffer[1];
                    temp_profile.deadzone.ly = deadzone_threshold_from_percent(buffer[2], (int16_t)BP32_AXIS_MAX);
                    temp_profile.deadzone.ry = deadzone_threshold_from_percent(buffer[3], (int16_t)BP32_AXIS_MAX);
                    temp_profile.deadzone.lt = deadzone_threshold_from_percent(buffer[4], (int16_t)BP32_TRIGGER_MAX);
                    temp_profile.deadzone.rt = deadzone_threshold_from_percent(buffer[5], (int16_t)BP32_TRIGGER_MAX);       
                } 
                else 
                {
                    return ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
                }  
            }
            break;

        case CHAR_UUID_MAP_BUTTONS:
            if (pending_write) 
            {
                if (buffer_size == 16) 
                {
                    temp_profile.up     = (uint16_t)buffer[0]  | ((uint16_t)buffer[1]  << 8);
                    temp_profile.down   = (uint16_t)buffer[2]  | ((uint16_t)buffer[3]  << 8);
                    temp_profile.left   = (uint16_t)buffer[4]  | ((uint16_t)buffer[5]  << 8);
                    temp_profile.right  = (uint16_t)buffer[6]  | ((uint16_t)buffer[7]  << 8);
                    temp_profile.a      = (uint16_t)buffer[8]  | ((uint16_t)buffer[9]  << 8);
                    temp_profile.b      = (uint16_t)buffer[10] | ((uint16_t)buffer[11] << 8);
                    temp_profile.x      = (uint16_t)buffer[12] | ((uint16_t)buffer[13] << 8);
                    temp_profile.y      = (uint16_t)buffer[14] | ((uint16_t)buffer[15] << 8);
                } 
                else 
                {
                    return ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
                }                
            }
            break;

        case CHAR_UUID_MAP_MISC_BUTTONS:
            if (pending_write) 
            {
                if (buffer_size == 16) 
                {
                    temp_profile.l3     = (uint16_t)buffer[0]  | ((uint16_t)buffer[1]  << 8);
                    temp_profile.r3     = (uint16_t)buffer[2]  | ((uint16_t)buffer[3]  << 8);
                    temp_profile.back   = (uint16_t)buffer[4]  | ((uint16_t)buffer[5]  << 8);
                    temp_profile.start  = (uint16_t)buffer[6]  | ((uint16_t)buffer[7]  << 8);
                    temp_profile.lb     = (uint16_t)buffer[8]  | ((uint16_t)buffer[9]  << 8);
                    temp_profile.rb     = (uint16_t)buffer[10] | ((uint16_t)buffer[11] << 8);
                    temp_profile.sys    = (uint16_t)buffer[12] | ((uint16_t)buffer[13] << 8);
                    temp_profile.misc   = (uint16_t)buffer[14] | ((uint16_t)buffer[15] << 8);
                } 
                else 
                {
                    return ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
                }                
            }
            break;

        case CHAR_UUID_COMMIT_UPDATE:
            if (pending_write) 
            {
                store_user_profile(0, temp_profile.profile_id, temp_profile);
                pending_write = false; // End of update sequence
                printf("Changes to Profile %d have been written to NVS.\n", temp_profile.profile_id);
            }
            break;

        default:
            //
            break;
    }
    
    return 0;
}