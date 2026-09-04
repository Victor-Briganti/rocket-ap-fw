cmake_minimum_required(VERSION 4.2)
project("Rocket AP" LANGUAGES C CXX ASM)

# Enable generation of compile_commands.json for clangd / language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Bare-Metal RISC-V Setup
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

# -----------------------------------------------------------------------------
# RISC-V Bare-Metal Cross-Compilation Setup
# -----------------------------------------------------------------------------

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_SYSTEM_NAME Generic CACHE STRING "Target System (Generic for bare-metal)")
    set(CMAKE_SYSTEM_PROCESSOR riscv CACHE STRING "Target Processor")

    # Prevent CMake compiler checks from failing due to lack of standard crt0 / linker script
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY CACHE STRING "")

    # Toolchain root directory (/opt/riscv by default or from $RISCV / $RISCV_PATH)
    if(DEFINED ENV{RISCV})
        set(RISCV_TOOLCHAIN_ROOT_DEFAULT "$ENV{RISCV}")
    elseif(DEFINED ENV{RISCV_PATH})
        set(RISCV_TOOLCHAIN_ROOT_DEFAULT "$ENV{RISCV_PATH}")
    else()
        set(RISCV_TOOLCHAIN_ROOT_DEFAULT "/opt/riscv")
    endif()

    set(RISCV_TOOLCHAIN_ROOT "${RISCV_TOOLCHAIN_ROOT_DEFAULT}" CACHE PATH "Path to RISC-V toolchain root directory")
    set(RISCV_TARGET_TRIPLE "riscv64-unknown-elf" CACHE STRING "RISC-V target triple")
    set(RISCV_SYSROOT "${RISCV_TOOLCHAIN_ROOT}/${RISCV_TARGET_TRIPLE}" CACHE PATH "Path to RISC-V sysroot")

    # Locate compilers: check ${RISCV_TOOLCHAIN_ROOT}/bin first, then system PATH
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER
            NAMES
                ${RISCV_TARGET_TRIPLE}-gcc
                ${RISCV_TARGET_TRIPLE}-c++
                riscv32-unknown-elf-gcc
                riscv32-unknown-elf-c++
                clang
            HINTS
                "${RISCV_TOOLCHAIN_ROOT}/bin"
            DOC "C Compiler"
        )
    endif()

    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER
            NAMES
                ${RISCV_TARGET_TRIPLE}-c++
                ${RISCV_TARGET_TRIPLE}-g++
                riscv32-unknown-elf-c++
                riscv32-unknown-elf-g++
                clang++
            HINTS
                "${RISCV_TOOLCHAIN_ROOT}/bin"
            DOC "C++ Compiler"
        )
    endif()

    if(NOT CMAKE_ASM_COMPILER)
        if(CMAKE_C_COMPILER)
            set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}" CACHE FILEPATH "ASM Compiler")
        endif()
    endif()
endif()

# -----------------------------------------------------------------------------
# RISC-V Architecture, ABI, and Toolchain Paths (referencing Makefile)
# -----------------------------------------------------------------------------
set(RISCV_ARCH "rv32imac" CACHE STRING "RISC-V architecture string (from Makefile: rv32imac)")
set(RISCV_ABI "ilp32" CACHE STRING "RISC-V ABI string (from Makefile: ilp32)")
set(RISCV_CMODEL "medany" CACHE STRING "RISC-V code model (from Makefile: medany)")

if(NOT DEFINED RISCV_TOOLCHAIN_ROOT)
    if(DEFINED ENV{RISCV})
        set(RISCV_TOOLCHAIN_ROOT "$ENV{RISCV}" CACHE PATH "Path to RISC-V toolchain root directory")
    elseif(DEFINED ENV{RISCV_PATH})
        set(RISCV_TOOLCHAIN_ROOT "$ENV{RISCV_PATH}" CACHE PATH "Path to RISC-V toolchain root directory")
    else()
        set(RISCV_TOOLCHAIN_ROOT "/opt/riscv" CACHE PATH "Path to RISC-V toolchain root directory")
    endif()
endif()

if(NOT DEFINED RISCV_TARGET_TRIPLE)
    set(RISCV_TARGET_TRIPLE "riscv64-unknown-elf" CACHE STRING "RISC-V target triple")
endif()

if(NOT DEFINED RISCV_SYSROOT)
    set(RISCV_SYSROOT "${RISCV_TOOLCHAIN_ROOT}/${RISCV_TARGET_TRIPLE}" CACHE PATH "Path to RISC-V sysroot")
endif()

# Common architecture, ABI, and optimization flags matching Makefile
add_compile_options(
    -march=${RISCV_ARCH}
    -mabi=${RISCV_ABI}
    -mcmodel=${RISCV_CMODEL}
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
)

# Clang-specific flags to locate system headers (like <string.h>) and GCC toolchain
if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(NOT CMAKE_C_FLAGS MATCHES "--gcc-toolchain" AND NOT CMAKE_C_FLAGS_INIT MATCHES "--gcc-toolchain")
        add_compile_options(
            --target=${RISCV_TARGET_TRIPLE}
            --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT}
            --sysroot=${RISCV_SYSROOT}
            -isystem ${RISCV_SYSROOT}/include
        )
    endif()
endif()

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