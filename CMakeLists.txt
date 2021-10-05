cmake_minimum_required(VERSION 3.12)

# Have pico_sdk_import.cmake use the local SDK if it was pulled down; otherwise, use path at
# PICO_SDK_PATH environment variable as specified within pico_sdk_import.cmake
if (EXISTS "${CMAKE_SOURCE_DIR}/lib/pico-sdk/CMakeLists.txt")
  set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/lib/pico-sdk/")
  message("Using local pico SDK at ('${PICO_SDK_PATH}')")
endif ()
include(pico_sdk_import.cmake)

project(GenesisControllerPico)
pico_sdk_init()
add_executable(GenesisControllerPico
  main.cpp
  UsbControllerDevice.cpp
  UsbKeyboard.cpp
  UsbGamepad.cpp
  usb_descriptors.c
  usb_execution.cpp
  GenesisController.cpp
  timers.cpp
  UsbKeyboardGenesisControllerObserver.cpp
  UsbGamepadGenesisControllerObserver.cpp
)
pico_add_extra_outputs(GenesisControllerPico)
target_link_libraries(GenesisControllerPico PRIVATE pico_stdlib pico_unique_id tinyusb_device tinyusb_board tinyusb_device_base pico_multicore)

target_include_directories(GenesisControllerPico PRIVATE ${CMAKE_CURRENT_LIST_DIR})


