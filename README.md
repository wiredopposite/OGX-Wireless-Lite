# OGX-Wireless-Lite
![OGX-Wireless-Lite](images/esp32s3_devkit.jpg "OGX-Wireless-Lite")

Firmware for the ESP32-S3 that leverages its built-in USB capabilities to emulate gamepads for several game consoles, enabling you to use your Bluetooth LE capable gamepads.

### Supported platforms
- Original Xbox
- XInput
- PlayStation 3
- PlayStation Classic
- Nintnendo Switch

### Supported gamepads
- Xbox One (Bluetooth version)
- Xbox Series
- Xbox Elite 2
- Steam
- Stadia
- Generic BLE HID gamepads should work as well

## Changing input mode
By default the input mode is set to OG Xbox, you must hold a button combo for 3 seconds to change which platform you want to play on. Your chosen input mode will persist after powering off the device. 

Start = Plus (Switch) = Options (Dualsense/DS4)

### XInput
Start + Dpad Up 
### Original Xbox
Start + Dpad Right
### Switch
Start + Dpad Down
### PlayStation 3
Start + Dpad Left
### PlayStation Classic
Start + A (Cross for PlayStation and B for Switch gamepads)

After a new mode is stored, the device will reset itself so you don't need to unplug it. 

## Hardware
You can use either the official ESP32-S3 DevKit C, DevKit M, or the USB C clone that's widely available.