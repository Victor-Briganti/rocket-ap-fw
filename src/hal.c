//===----------- hal.c - Hardware Abstraction Layer -----------*--- C ---*-===//
//
//  See https://github.com/Victor-Briganti/rocket-ap-fw/blob/main/LICENSE for
//  license information.
//
//===----------------------------------------------------------------------===//
//
// This file provides the implementation to interact with the Rocket AP
// hardware.
//
//===----------------------------------------------------------------------===//

#include "hal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

#define ap_irq_check()                                                         \
  while (*((volatile uint8_t *)AP_IRQ) == 0) {                                 \
  }

static void set_mode(ap_bank_t bank, ap_column_t col, ap_axis_t axis,
                     ap_cmd_t cmd) {
  *AP_MODE = (((uint32_t)col) << 24) | (((uint32_t)bank) << 16) |
             (((uint32_t)axis) << 8) | ((uint32_t)cmd);
}

static void set_control(ap_state_t state, ap_target_t target, bool trigger_ap,
                        bool reset) {
  *AP_CONTROL = (((uint32_t)state) << 24) | (((uint32_t)target) << 16) |
                (((uint32_t)trigger_ap) << 8) | ((uint32_t)reset);
}

static void tiny_delay(int delay) {
  for (int i = 0; i < delay; i++) {
    asm volatile("nop");
  }
}

//===----------------------------------------------------------------------===//
// Public
//===----------------------------------------------------------------------===//

void ap_reset_engine() {
  *AP_CONTROL = 0x1000008;
  *AP_CONTROL = 0x1000000;
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;
}

AP_NO_OPT void ap_flush(ap_bank_t bank, ap_column_t col) {
  *AP_CONTROL = 0x1000000;

  uint32_t mode = 0x0;
  uint32_t ctrl1 = 0x0;

  switch (bank) {
  case AP_BANK_A:
    assert(col == AP_COL_RIGHT &&
           "Bank A supports flush only on the right side");
    mode = 0x7000000;
    ctrl1 = 0x1000001;
    break;
  case AP_BANK_B:
    mode = col == AP_COL_RIGHT ? 0x7000000 : 0x0;
    ctrl1 = 0x1000002;
    break;
  case AP_BANK_C:
    mode = col == AP_COL_RIGHT ? 0x7000000 : 0x0;
    ctrl1 = 0x1000004;
    break;
  default:
    assert(false && "Invalid bank type");
  }

  *AP_MODE = mode;
  *AP_CONTROL = ctrl1;
  *AP_CONTROL = 0x0;

  *AP_MODE = 0x0;
}

AP_NO_OPT void ap_bootstrap() {
  *AP_CONTROL = 0;
  *AP_MODE = 0;

  // Lock ap_if_state
  *AP_CONTROL = 0x1000000;

  // Reset three CAMs in parallel
  *AP_CONTROL = 0x1000007;
  *AP_CONTROL = 0x1000000;

  // Reset AP engine
  *AP_CONTROL = 0x1000008;
  *AP_CONTROL = 0x1000000;

  // Select the other internal col
  *AP_MODE = 0x7000000;

  // Reset three CAMs in parallel
  *AP_CONTROL = 0x1000007;
  *AP_CONTROL = 0x1000000;

  // Release ap_if_state
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;
}

void ap_write_col(ap_bank_t bank, ap_column_t col, uint8_t *source,
                  size_t size) {
  assert(size <= AP_COL_SIZE && "Size is bigger than the AP column size");

  uint32_t bankIdx = (AP_COL_SIZE * 2 * (uint8_t)bank);
  uint32_t colIdx = ((uint8_t)col * AP_COL_SIZE);
  uint32_t addr = AP_BANK_A_BASE_ADDR + bankIdx + colIdx;

  volatile uint8_t *cam = (volatile uint8_t *)(addr);

  for (size_t i = 0; i < size; i++) {
    cam[i] = source[i];
  }
}

void ap_read_col(ap_bank_t bank, ap_column_t col, uint8_t *dest, size_t size) {
  assert(size < AP_COL_SIZE && "Size is bigger than the AP column size");

  uint32_t bankIdx = (AP_COL_SIZE * 2 * (uint8_t)bank);
  uint32_t colIdx = ((uint8_t)col * AP_COL_SIZE);
  uint32_t addr = AP_BANK_A_BASE_ADDR + bankIdx + colIdx;

  volatile uint8_t *cam = (volatile uint8_t *)(addr);

  for (size_t i = 0; i < size; i++) {
    dest[i] = cam[i];
  }
}

void ap_search(ap_bank_t bank, ap_column_t col, uint8_t key,
               ap_target_t target) {
  assert(bank == AP_BANK_B &&
         "Currently the system only supports search on bank B");

  volatile uint8_t *keyAddr = NULL;
  keyAddr = (volatile uint8_t *)(col == AP_COL_LEFT ? AP_CAM_B1 : AP_CAM_B0);
  *keyAddr = key;

  set_control(AP_STATE_STATIC, target, false, false);
  set_mode(AP_BANK_A, col, AP_AXIS_HOR, AP_CMD_SEARCH);
  set_control(AP_STATE_STATIC, target, true, false);

  ap_irq_check();
}

void ap_launch(ap_cmd_t cmd, ap_bank_t bank, ap_column_t col, ap_axis_t axis,
               ap_target_t target) {
  assert(cmd != AP_CMD_SEARCH &&
         "Use ap_search() for search operations — it handles the key write");

  set_control(AP_STATE_STATIC, target, false, false);
  set_mode(bank, col, axis, cmd);
  set_control(AP_STATE_STATIC, target, true, false);
}

void ap_wait() { ap_irq_check(); }

bool ap_poll() { return *((volatile uint8_t *)AP_IRQ) != 0; }

void ap_execute(ap_cmd_t cmd, ap_bank_t bank, ap_column_t col, ap_axis_t axis,
                ap_target_t target) {
  assert(cmd != AP_CMD_SEARCH &&
         "Use ap_search() for search operations — it handles the key write");

  set_control(AP_STATE_STATIC, target, false, false);
  set_mode(bank, col, axis, cmd);
  set_control(AP_STATE_STATIC, target, true, false);
  ap_wait();
}
