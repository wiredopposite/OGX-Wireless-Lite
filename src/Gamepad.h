#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include <cstdint>
#include "user_settings/user_settings.h"

struct GamepadButtons
{
	bool up 		{false};
	bool down 		{false};
	bool left 		{false};
	bool right 		{false};
	bool a			{false};
	bool b 			{false};
	bool x 			{false};
	bool y 			{false};
	bool l3			{false};
	bool r3 		{false};
	bool back 		{false};
	bool start 		{false};
	bool rb 		{false};
	bool lb 		{false};
	bool sys 		{false};
	bool misc		{false};
};

struct GamepadTriggers
{
	uint8_t l 		{0};
	uint8_t r 		{0};
};

struct GamepadJoysticks
{
	int16_t lx 		{0};
	int16_t ly 		{0};
	int16_t rx 		{0};
	int16_t ry 		{0};
};

struct GamepadRumble
{
	uint8_t l	{0};
	uint8_t r	{0};
};

struct GamepadMappings
{
    uint16_t up;
	uint16_t down;
	uint16_t left;
	uint16_t right;
    uint16_t a;
	uint16_t b;
	uint16_t x;
	uint16_t y;
	uint16_t l3;
	uint16_t r3;
	uint16_t lb;
	uint16_t rb;
	uint16_t back;
	uint16_t start;
	uint16_t sys;
	uint16_t misc;
};

class Gamepad 
{
	public:
		UserProfile      profile;
		GamepadMappings  mappings;

		GamepadButtons 	 buttons;
		GamepadTriggers  triggers;
		GamepadJoysticks joysticks;
		GamepadRumble 	 rumble;

		void reset_pad();
		void reset_rumble();
};

Gamepad * gamepad(int idx);

#endif // _GAMEPAD_H_
