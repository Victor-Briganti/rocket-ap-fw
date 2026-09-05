//===----------- hal.h - Hardware Abstraction Layer -----------*--- H ---*-===//
//
//  See https://github.com/Victor-Briganti/rocket-ap-fw/blob/main/LICENSE for
//  license information.
//
//===----------------------------------------------------------------------===//
//
// This file provides a interface to interact with the Rocket AP hardware.
//
//===----------------------------------------------------------------------===//

#ifndef HAL_H
#define HAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__clang__)
#define AP_NO_OPT __attribute__((optnone))
#elif defined(__GNUC__) || defined(__GNUG__)
#define AP_NO_OPT __attribute__((optimize("O0")))
#else
#define AP_NO_OPT
#endif

//===------------------------------------------------------------------------===
// Memory Banks
//===------------------------------------------------------------------------===

// Physical starting address of the AP accelerator.
// All memory banks and control registrs are calculated as offsets from this
// base address.
#define AP_BASE_ADDR 0x80004000

// Size of one single column per memory bank.
#define AP_COL_SIZE 512

// Number of columns in each memory bank.
#define AP_COL_COUNT 6

//===------------------------------------------------------------------------===
// Memory Banks
//===------------------------------------------------------------------------===

#define AP_BANK_A_BASE_ADDR AP_BASE_ADDR
#define AP_BANK_B_BASE_ADDR (AP_BASE_ADDR + (AP_COL_SIZE * 2))
#define AP_BANK_C_BASE_ADDR (AP_BASE_ADDR + (AP_COL_SIZE * 4))

#define AP_CAM_A0 ((volatile uint32_t *)AP_BANK_A_BASE_ADDR)
#define AP_CAM_A1 ((volatile uint32_t *)(AP_BANK_A_BASE_ADDR + AP_COL_SIZE))
#define AP_CAM_B0 ((volatile uint32_t *)AP_BANK_B_BASE_ADDR)
#define AP_CAM_B1 ((volatile uint32_t *)(AP_BANK_B_BASE_ADDR + AP_COL_SIZE))
#define AP_CAM_C0 ((volatile uint32_t *)AP_BANK_C_BASE_ADDR)
#define AP_CAM_C1 ((volatile uint32_t *)(AP_BANK_C_BASE_ADDR + AP_COL_SIZE))

//===------------------------------------------------------------------------===
// Control Registers
//===------------------------------------------------------------------------===

// Start address of the control registers
#define AP_CTRL_REG_BASE_ADDR (AP_BASE_ADDR + (AP_COL_SIZE * AP_COL_COUNT))

// Controls system-level states, issues the AP reset command, and holds
// the trigger bit that actually tells the hardware to start computing.
#define AP_CONTROL ((volatile uint32_t *)(AP_CTRL_REG_BASE_ADDR))

// Configures the specifics of the next operation, defines which command to run,
// the direction, and the target bank.
#define AP_MODE ((volatile uint32_t *)(AP_CTRL_REG_BASE_ADDR + 4))

// The main CPU reads this register to check the hardware interrupt status.
// It acts as a flag to know when the AP has finished its computation.
#define AP_IRQ ((volatile uint32_t *)(AP_CTRL_REG_BASE_ADDR + 8))

//===------------------------------------------------------------------------===
// Enums
//===------------------------------------------------------------------------===

typedef enum {
  AP_CMD_OR = 0,
  AP_CMD_XOR = 1,
  AP_CMD_AND = 2,
  AP_CMD_NOT = 3,
  AP_CMD_ADD = 4,
  AP_CMD_SUB = 5,
  AP_CMD_MULT = 6,
  AP_CMD_SET = 7,
  AP_CMD_SEARCH = 8,
  AP_CMD_ADD_D = 9
} ap_cmd_t;

typedef enum {
  AP_BANK_A = 0,
  AP_BANK_B = 1,
  AP_BANK_C = 2,
} ap_bank_t;

typedef enum {
  AP_COL_LEFT = 0,
  AP_COL_RIGHT = 0b111, // 7
} ap_column_t;

typedef enum { AP_MODE_MEM = 0, AP_MODE_PROC = 1 } ap_mode_t;

typedef enum { AP_AXIS_HOR = 0, AP_AXIS_VERT = 1 } ap_axis_t;

typedef enum { AP_STATE_DYN = 0, AP_STATE_STAT = 1 } ap_state_t;

typedef enum { AP_DEST_C = 0, AP_DEST_A = 1 } ap_dest_t;

//===------------------------------------------------------------------------===
// Operations
//===------------------------------------------------------------------------===

AP_NO_OPT void ap_reset();
void ap_search(uint8_t key, ap_column_t col, ap_dest_t target);

void ap_flush_col(ap_bank_t bank, ap_column_t col);

void ap_write_col(ap_bank_t bank, ap_column_t col, uint8_t *dest, size_t size);
void ap_read_col(ap_bank_t bank, ap_column_t col, uint8_t *dest, size_t size);

void ap_memcpy(void *dest, void *src, size_t bytes);

#endif // HAL_H