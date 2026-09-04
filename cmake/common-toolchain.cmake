# -----------------------------------------------------------------------------
# RISC-V Bare-Metal Cross-Compilation Setup
# -----------------------------------------------------------------------------

# Bare-Metal RISC-V Setup
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

# Prevent CMake from attempting to link a test executable during compiler detection
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Path to RISC-V toolchain root directory
if(DEFINED ENV{RISCV_PATH})
    set(RISCV_TOOLCHAIN_ROOT "$ENV{RISCV_PATH}" CACHE PATH "Path to RISC-V toolchain root directory")
else()
    set(RISCV_TOOLCHAIN_ROOT "/opt/riscv" CACHE PATH "Path to RISC-V toolchain root directory")
endif()

# Target triple configuration
set(RISCV_TARGET_TRIPLE "riscv64-unknown-elf" CACHE STRING "RISC-V target triple")
set(RISCV_SYSROOT "${RISCV_TOOLCHAIN_ROOT}/${RISCV_TARGET_TRIPLE}" CACHE PATH "Path to RISC-V sysroot")
set(CMAKE_SYSROOT "${RISCV_SYSROOT}")

set(RISCV_ARCH "rv32imac_zicsr" CACHE STRING "RISC-V architecture string")
set(RISCV_ABI "ilp32" CACHE STRING "RISC-V ABI string")
set(RISCV_CMODEL "medany" CACHE STRING "RISC-V code model")

# Common architecture, ABI, and optimization flags
add_compile_options(
    -march=${RISCV_ARCH}
    -mabi=${RISCV_ABI}
    -mcmodel=${RISCV_CMODEL}
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
)

