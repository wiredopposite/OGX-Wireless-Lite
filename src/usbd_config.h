#ifndef USBD_CONFIG_H_
#define USBD_CONFIG_H_

#include "usbd/usbd_boards.h"

// Boards
// OGXM_PI_PICO
// OGXM_ADA_FEATHER_USBH 
// OGXM_RPZERO_INTERPOSER
// OGXW_RETAIL_1CH
// OGXW_RPZERO_1CH
// OGXW_RPZERO_2CH
// OGXW_LITE

#define USBD_BOARD        OGXW_LITE
#define USBD_MAX_GAMEPADS 1 // This is set by idf.py menuconfig for the ESP32S3, number here is ignored in that case.
#define CDC_DEBUG         0 // Set to 1 for CDC device, helpful for debugging USB host. Include utilities/log.h and use log() as you would printf()

#endif // USBD_CONFIG_H_