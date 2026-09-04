cmake_minimum_required(VERSION 4.2)
project("Rocket AP" LANGUAGES C CXX ASM)

# Enable generation of compile_commands.json for clangd / language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# Hardware Abstraction Layer (HAL) Library Target
# -----------------------------------------------------------------------------
add_library(hal STATIC
    src/ap_hal.c
    src/ap_funcs.S
    src/cycle_counter.S
)

# Include directories: include/ (public) and src/
target_include_directories(hal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(hal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# If sysroot include directory exists, explicitly add it as system include directory
if(EXISTS "${RISCV_SYSROOT}/include")
    target_include_directories(hal SYSTEM PUBLIC "${RISCV_SYSROOT}/include")
endif()