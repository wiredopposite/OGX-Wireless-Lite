cmake_minimum_required(VERSION 3.17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(FAMILY espressif)
set(BOARD espressif_s3_devkitc)
include(${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/hw/bsp/family_support.cmake)

set(BLUEPAD32_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/components/bluepad32)
set(BTSTACK_ROOT ${BLUEPAD32_ROOT}/components/btstack)

# gets PROJECT name for the example (e.g. <BOARD>-<DIR_NAME>)
family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})

project(${PROJECT} C CXX ASM)

# Checks this example is valid for the family and initializes the project
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})

# Espressif has its own cmake build system
if(FAMILY STREQUAL "espressif")
  return()
endif()

# add_executable(${PROJECT})

# # Example source
# target_sources(${PROJECT} PUBLIC
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/freertos_hook.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/gamepadsetup.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/bp32/my_platform.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/inputmode.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/usbdriver.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/drivermanager.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/hid/HIDDriver.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xinput/XInputDriver.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xboxog/XboxOriginalDriver.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xboxog/xid/xid.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xboxog/xid/xid_driver.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xboxog/xid/xid_gamepad.C
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xboxog/xid/xid_remote.C
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usbd/xboxog/xid/xid_steelbattalion.c
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/scaling.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
#   )

# # Example include
# target_include_directories(${PROJECT} PUBLIC
#   ${CMAKE_CURRENT_SOURCE_DIR}/src
#   )

# # Configure compilation flags and libraries for the example with FreeRTOS.
# # See the corresponding function in hw/bsp/FAMILY/family.cmake for details.
# family_configure_device_example(${PROJECT} freertos)
