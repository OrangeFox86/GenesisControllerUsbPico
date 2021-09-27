cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(GenesisControllerPico)
pico_sdk_init()
add_executable(GenesisControllerPico
  main.cpp
  UsbKeyboard.cpp
  UsbGamepad.cpp
  usb_descriptors.c
  GenesisController.cpp
  timers.cpp
  UsbKeyboardGenesisControllerObserver.cpp
  UsbGamepadGenesisControllerObserver.cpp
)
pico_add_extra_outputs(GenesisControllerPico)
target_link_libraries(GenesisControllerPico PRIVATE pico_stdlib tinyusb_device tinyusb_board pico_multicore)

target_include_directories(GenesisControllerPico PRIVATE ${CMAKE_CURRENT_LIST_DIR})


