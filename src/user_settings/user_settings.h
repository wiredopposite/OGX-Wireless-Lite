#ifndef USER_SETTINGS_H_
#define USER_SETTINGS_H_

#include <stdint.h>
#include <stdlib.h>

#define MASK_DPAD_UP      0x0001
#define MASK_DPAD_DOWN    0x0002
#define MASK_DPAD_LEFT    0x0004
#define MASK_DPAD_RIGHT   0x0008
#define MASK_BUTTON_A     0x0010
#define MASK_BUTTON_B     0x0020
#define MASK_BUTTON_X     0x0040
#define MASK_BUTTON_Y     0x0080
#define MASK_BUTTON_L3    0x0100
#define MASK_BUTTON_R3    0x0200
#define MASK_BUTTON_BACK  0x0400
#define MASK_BUTTON_START 0x0800
#define MASK_BUTTON_LB    0x1000
#define MASK_BUTTON_RB    0x2000
#define MASK_BUTTON_SYS   0x4000
#define MASK_BUTTON_MISC  0x8000

struct DeadzoneThresholds // Deadzone values based on bp32 min/max
{
    int16_t ly;
    int16_t ry;
    int16_t lt;
    int16_t rt;
};

struct UserProfile 
{
    uint8_t  profile_id;
    char     profile_name[17];
    bool     ly_invert;
    bool     ry_invert;
    DeadzoneThresholds deadzone;
    uint16_t up   ;
    uint16_t down ;
    uint16_t left ;
    uint16_t right;
    uint16_t a    ;
    uint16_t b    ;
    uint16_t x    ;
    uint16_t y    ;
    uint16_t l3   ;
    uint16_t r3   ;
    uint16_t back ;
    uint16_t start;
    uint16_t lb   ;
    uint16_t rb   ;
    uint16_t sys  ;
    uint16_t misc ;
} __attribute__((__packed__));

void init_user_settings();
bool store_user_profile(int idx, uint8_t profile_id, const UserProfile& profile);
bool store_active_profile_id(int idx, uint8_t profile_id);
bool retrieve_active_profile_id(int idx, uint8_t* profile_id_buffer);
bool retrieve_user_profile(uint8_t profile_id, UserProfile* profile_buffer);

#endif // USER_SETTINGS_H_