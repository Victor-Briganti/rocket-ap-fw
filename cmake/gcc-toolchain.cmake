include("${CMAKE_CURRENT_LIST_DIR}/common-toolchain.cmake")

# ==============================================================================
# RISC-V Bare-Metal GCC Toolchain File
# ==============================================================================

# Search for GCC / G++ / C++ cross-compiler tools
find_program(RISCV_C_BIN
    NAMES
        ${RISCV_TARGET_TRIPLE}-gcc
        ${RISCV_TARGET_TRIPLE}-c++
        riscv32-unknown-elf-gcc
        riscv32-unknown-elf-c++
    HINTS
        "${RISCV_TOOLCHAIN_ROOT}/bin"
    DOC "RISC-V GCC C Compiler"
)
set(CMAKE_C_COMPILER "${RISCV_C_BIN}" CACHE FILEPATH "C Compiler")

find_program(RISCV_CXX_BIN
    NAMES
        ${RISCV_TARGET_TRIPLE}-c++
        ${RISCV_TARGET_TRIPLE}-g++
        riscv32-unknown-elf-c++
        riscv32-unknown-elf-g++
    HINTS
        "${RISCV_TOOLCHAIN_ROOT}/bin"
    DOC "RISC-V GCC C++ Compiler"
)
set(CMAKE_CXX_COMPILER "${RISCV_CXX_BIN}" CACHE FILEPATH "C++ Compiler")

find_program(RISCV_ASM_BIN
    NAMES
        ${RISCV_TARGET_TRIPLE}-gcc
        riscv32-unknown-elf-gcc
    HINTS
        "${RISCV_TOOLCHAIN_ROOT}/bin"
    DOC "RISC-V GCC ASM Compiler"
)
set(CMAKE_ASM_COMPILER "${RISCV_ASM_BIN}" CACHE FILEPATH "ASM Compiler")

find_program(CMAKE_OBJCOPY NAMES ${RISCV_TARGET_TRIPLE}-objcopy riscv32-unknown-elf-objcopy HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_OBJDUMP NAMES ${RISCV_TARGET_TRIPLE}-objdump riscv32-unknown-elf-objdump HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_SIZE    NAMES ${RISCV_TARGET_TRIPLE}-size    riscv32-unknown-elf-size    HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
find_program(CMAKE_AR      NAMES ${RISCV_TARGET_TRIPLE}-ar      riscv32-unknown-elf-ar      HINTS "${RISCV_TOOLCHAIN_ROOT}/bin")
