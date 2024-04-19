#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "bsp/board_api.h"
#include "tusb.h"

#include "usbd_task.h"
#include "bp32_task.h"

#define USBD_STACK_SIZE 4096

void usb_task(void* param)
{
    (void) param;

    if (board_init_after_tusb) 
    {
        board_init_after_tusb();
    }

    while (1)
    {
        tud_task(); // put this thread to waiting state until there is new events, doesn't return
    }
}

void app_main(void)
{
    board_init();

    xTaskCreatePinnedToCore(
        usbd_task,            
        "usbd",               
        USBD_STACK_SIZE * 8, 
        NULL,                 
        configMAX_PRIORITIES - 2, 
        NULL,                     
        0                         
    );

    xTaskCreatePinnedToCore(
        usb_task,           
        "usb",              
        USBD_STACK_SIZE,    
        NULL,               
        configMAX_PRIORITIES - 3,
        NULL,     
        0         
    );

    xTaskCreatePinnedToCore(
        bp32_task,            
        "bp32",               
        USBD_STACK_SIZE * 12, 
        NULL,                 
        configMAX_PRIORITIES - 1, 
        NULL,    
        1        
    );
}