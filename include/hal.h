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

typedef enum { AP_STATE_DYNAMIC = 0, AP_STATE_STATIC = 1 } ap_state_t;

typedef enum { AP_TARGET_C = 0, AP_TARGET_A = 1 } ap_target_t;

//===------------------------------------------------------------------------===
// Operations
//===------------------------------------------------------------------------===

AP_NO_OPT void ap_bootstrap();
AP_NO_OPT void ap_reset_engine();
AP_NO_OPT void ap_flush(ap_bank_t bank, ap_column_t col);

void ap_write_col(ap_bank_t bank, ap_column_t col, uint8_t *dest, size_t size);
void ap_read_col(ap_bank_t bank, ap_column_t col, uint8_t *dest, size_t size);
void ap_launch(ap_cmd_t cmd, ap_bank_t bank, ap_column_t col, ap_axis_t axis,
               ap_target_t target);
void ap_wait();
bool ap_poll();
void ap_execute(ap_cmd_t cmd, ap_bank_t bank, ap_column_t col, ap_axis_t axis,
                ap_target_t target);
void ap_search(ap_bank_t bank, ap_column_t col, uint8_t key,
               ap_target_t target);

#endif // HAL_H