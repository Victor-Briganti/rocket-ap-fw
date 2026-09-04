include("${CMAKE_CURRENT_LIST_DIR}/common-toolchain.cmake")

# ==============================================================================
# RISC-V Bare-Metal LLVM / Clang Toolchain File
# ==============================================================================

# Compilers: LLVM Clang
find_program(CMAKE_C_COMPILER NAMES clang DOC "LLVM Clang C Compiler")
find_program(CMAKE_CXX_COMPILER NAMES clang++ DOC "LLVM Clang C++ Compiler")
find_program(CMAKE_ASM_COMPILER NAMES clang DOC "LLVM Clang ASM Compiler")
find_program(CMAKE_OBJCOPY NAMES llvm-objcopy ${RISCV_TARGET_TRIPLE}-objcopy HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_OBJDUMP NAMES llvm-objdump ${RISCV_TARGET_TRIPLE}-objdump HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_SIZE    NAMES llvm-size    ${RISCV_TARGET_TRIPLE}-size    HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_AR      NAMES llvm-ar      ${RISCV_TARGET_TRIPLE}-ar      HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")

# Flags to pass to Clang for RISC-V target, GCC toolchain, sysroot, and newlib headers
set(CMAKE_C_FLAGS_INIT   "--target=${RISCV_TARGET_TRIPLE} --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT} --sysroot=${RISCV_SYSROOT} -isystem ${RISCV_SYSROOT}/include")
set(CMAKE_CXX_FLAGS_INIT "--target=${RISCV_TARGET_TRIPLE} --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT} --sysroot=${RISCV_SYSROOT} -isystem ${RISCV_SYSROOT}/include")
set(CMAKE_ASM_FLAGS_INIT "--target=${RISCV_TARGET_TRIPLE} --gcc-toolchain=${RISCV_TOOLCHAIN_ROOT} --sysroot=${RISCV_SYSROOT}")
