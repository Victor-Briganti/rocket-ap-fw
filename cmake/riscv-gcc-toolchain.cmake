# ==============================================================================
# RISC-V Bare-Metal GCC Toolchain File
# ==============================================================================


# Prevent CMake from attempting to link a test executable during compiler detection
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Path to RISC-V toolchain root directory (/opt/riscv by default or from environment)
if(DEFINED ENV{RISCV})
    set(RISCV_TOOLCHAIN_ROOT "$ENV{RISCV}" CACHE PATH "Path to RISC-V toolchain root directory")
elseif(DEFINED ENV{RISCV_PATH})
    set(RISCV_TOOLCHAIN_ROOT "$ENV{RISCV_PATH}" CACHE PATH "Path to RISC-V toolchain root directory")
else()
    set(RISCV_TOOLCHAIN_ROOT "/opt/riscv" CACHE PATH "Path to RISC-V toolchain root directory")
endif()

# Target triple configuration (referencing CROSS_COMPILE from Makefile)
set(CROSS_COMPILE "riscv64-unknown-elf" CACHE STRING "RISC-V cross-compiler prefix")
set(RISCV_TARGET_TRIPLE "${CROSS_COMPILE}" CACHE STRING "RISC-V target triple")
set(RISCV_SYSROOT "${RISCV_TOOLCHAIN_ROOT}/${RISCV_TARGET_TRIPLE}" CACHE PATH "Path to RISC-V sysroot")

set(CMAKE_SYSROOT "${RISCV_SYSROOT}")

# Search for GCC / G++ / C++ cross-compiler tools
find_program(RISCV_GCC_BIN
    NAMES
        ${CROSS_COMPILE}-gcc
        ${CROSS_COMPILE}-c++
        riscv32-unknown-elf-gcc
        riscv32-unknown-elf-c++
    HINTS
        "${RISCV_TOOLCHAIN_ROOT}/bin"
    DOC "RISC-V GCC C Compiler"
)
if(RISCV_GCC_BIN)
    set(CMAKE_C_COMPILER "${RISCV_GCC_BIN}" CACHE FILEPATH "C Compiler")
else()
    # Explicitly set the cross-compiler name to prevent CMake from falling back to host cc
    set(CMAKE_C_COMPILER "${CROSS_COMPILE}-gcc" CACHE FILEPATH "C Compiler")
endif()

find_program(RISCV_GXX_BIN
    NAMES
        ${CROSS_COMPILE}-c++
        ${CROSS_COMPILE}-g++
        riscv32-unknown-elf-c++
        riscv32-unknown-elf-g++
    HINTS
        "${RISCV_TOOLCHAIN_ROOT}/bin"
    DOC "RISC-V GCC C++ Compiler"
)
if(RISCV_GXX_BIN)
    set(CMAKE_CXX_COMPILER "${RISCV_GXX_BIN}" CACHE FILEPATH "C++ Compiler")
else()
    set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}-c++" CACHE FILEPATH "C++ Compiler")
endif()

find_program(RISCV_ASM_BIN
    NAMES
        ${CROSS_COMPILE}-gcc
        riscv32-unknown-elf-gcc
    HINTS
        "${RISCV_TOOLCHAIN_ROOT}/bin"
    DOC "RISC-V GCC ASM Compiler"
)
if(RISCV_ASM_BIN)
    set(CMAKE_ASM_COMPILER "${RISCV_ASM_BIN}" CACHE FILEPATH "ASM Compiler")
else()
    set(CMAKE_ASM_COMPILER "${CROSS_COMPILE}-gcc" CACHE FILEPATH "ASM Compiler")
endif()

find_program(CMAKE_OBJCOPY NAMES ${CROSS_COMPILE}-objcopy riscv32-unknown-elf-objcopy HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_OBJDUMP NAMES ${CROSS_COMPILE}-objdump riscv32-unknown-elf-objdump HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_SIZE    NAMES ${CROSS_COMPILE}-size    riscv32-unknown-elf-size    HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_AR      NAMES ${CROSS_COMPILE}-ar      riscv32-unknown-elf-ar      HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")

