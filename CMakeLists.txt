# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(project_display_SSD1306 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(project_display_SSD1306 project_display_SSD1306.c  inc/ssd1306.c)

pico_set_program_name(project_display_SSD1306 "project_display_SSD1306")
pico_set_program_version(project_display_SSD1306 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(project_display_SSD1306 1)
pico_enable_stdio_usb(project_display_SSD1306 0)

pico_generate_pio_header(project_display_SSD1306 ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Add the standard library to the build
target_link_libraries(project_display_SSD1306
        pico_stdlib
        hardware_pio
        )

# Add the standard include files to the build
target_include_directories(project_display_SSD1306 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(project_display_SSD1306 
    hardware_i2c
    
        )

pico_add_extra_outputs(project_display_SSD1306)

