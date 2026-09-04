# ==============================================================================
# RISC-V Bare-Metal LLVM / Clang Toolchain File
# ==============================================================================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

# Prevent CMake from attempting to link a test executable during compiler detection
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Path to RISC-V GCC toolchain root for sysroot / newlib headers and libraries
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

# Compilers: LLVM Clang
find_program(CMAKE_C_COMPILER NAMES clang DOC "LLVM Clang C Compiler")
find_program(CMAKE_CXX_COMPILER NAMES clang++ DOC "LLVM Clang C++ Compiler")
find_program(CMAKE_ASM_COMPILER NAMES clang DOC "LLVM Clang ASM Compiler")

find_program(CMAKE_OBJCOPY NAMES llvm-objcopy ${CROSS_COMPILE}-objcopy HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_OBJDUMP NAMES llvm-objdump ${CROSS_COMPILE}-objdump HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_SIZE    NAMES llvm-size    ${CROSS_COMPILE}-size    HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_AR      NAMES llvm-ar      ${CROSS_COMPILE}-ar      HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")

# Flags to pass to Clang for RISC-V target, GCC toolchain, sysroot, and newlib headers (<string.h>)
set(CMAKE_C_FLAGS_INIT   "--target=${RISCV_TARGET_TRIPLE} --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT} --sysroot=${RISCV_SYSROOT} -isystem ${RISCV_SYSROOT}/include")
set(CMAKE_CXX_FLAGS_INIT "--target=${RISCV_TARGET_TRIPLE} --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT} --sysroot=${RISCV_SYSROOT} -isystem ${RISCV_SYSROOT}/include")
set(CMAKE_ASM_FLAGS_INIT "--target=${RISCV_TARGET_TRIPLE} --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT} --sysroot=${RISCV_SYSROOT}")

