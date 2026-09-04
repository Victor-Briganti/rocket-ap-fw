#ifndef AP_HAL
#define AP_HAL

#include <stddef.h>
#include <stdint.h>

// Description	addr	array index	byte 3	byte 2	byte 1	byte 0
// Mode configuration	0x80004300	0	sel_internal_col	sel_col
// op_direction	cmd Control settings	0x80004304	1	reserved
// ap_if_state	ap_trigger_ap	ap_rst AP irq	0x80004308	2	reserved
// reserved	reserved	state_irq*

/* Defines */
#define AP_COL_SIZE 512
#define COL_QUANT 6
#define AP_BASE_ADDR 0x80004000

#define CAM_A_BASE_ADDR AP_BASE_ADDR
#define CAM_A_0_BASE_ADDR CAM_A_BASE_ADDR
#define CAM_A_1_BASE_ADDR (CAM_A_BASE_ADDR + AP_COL_SIZE)

#define CAM_B_BASE_ADDR (AP_BASE_ADDR + (AP_COL_SIZE * 2))
#define CAM_B_0_BASE_ADDR CAM_B_BASE_ADDR
#define CAM_B_1_BASE_ADDR (CAM_B_BASE_ADDR + AP_COL_SIZE)

#define CAM_C_BASE_ADDR (AP_BASE_ADDR + (AP_COL_SIZE * 4))
#define CAM_C_0_BASE_ADDR CAM_C_BASE_ADDR
#define CAM_C_1_BASE_ADDR (CAM_C_BASE_ADDR + AP_COL_SIZE)

#define AP_CONTROL_REGISTERS_BASE_ADDR                                         \
  (AP_BASE_ADDR + (AP_COL_SIZE * COL_QUANT))
#define AP_MODE_CONFIGURATION_ADDR AP_CONTROL_REGISTERS_BASE_ADDR

#define AP_CONTROL_SETTINGS_ADDR (AP_CONTROL_REGISTERS_BASE_ADDR + 4)

#define AP_IRQ_ADDR (AP_CONTROL_REGISTERS_BASE_ADDR + 8)

#define AP_CONTROL ((volatile uint32_t *)AP_CONTROL_SETTINGS_ADDR)
#define AP_MODE ((volatile uint32_t *)AP_MODE_CONFIGURATION_ADDR)
#define AP_IRQ ((volatile uint32_t *)AP_IRQ_ADDR)

#define AP_CAM_A ((volatile uint8_t *)CAM_A_0_BASE_ADDR)
#define AP_CAM_A_1 ((volatile uint8_t *)CAM_A_1_BASE_ADDR)
#define AP_CAM_B ((volatile uint8_t *)CAM_B_0_BASE_ADDR)
#define AP_CAM_B_1 ((volatile uint8_t *)CAM_B_1_BASE_ADDR)
#define AP_CAM_C ((volatile uint8_t *)CAM_C_0_BASE_ADDR)
#define AP_CAM_C_1 ((volatile uint8_t *)CAM_C_1_BASE_ADDR)

/* Logical operations */
/* Support for 8 bits */
/* Arithmetic operations */
/* Support for 4 bits */

/* Enums */
typedef enum {
  OR = 0,
  XOR = 1,
  AND = 2,
  NOT = 3,
  ADD = 4,
  SUB = 5,
  MULT = 6,
  SET = 7,
  SEARCH = 8,
  ADD_D = 9
} APOperations;

typedef enum {
  CAM_A = 0,
  CAM_B = 1,
  CAM_C = 2,
} APCollunm;

typedef enum {
  LEFT = 0,
  RIGHT = 7, // 0b111
} APInternalCollunm;

typedef enum { MEMORY = 0, ASSOCIATIVE_PROCESSOR = 1 } APMode;

typedef enum { HORIZONTAL = 0, VERTICAL = 1 } APOpDirection;

typedef enum { DYNAMIC = 0, STATIC = 1 } APIfState;

typedef enum { TRUE = 1, FALSE = 0 } BitState;

typedef enum { TARGET_C = 0, TARGET_A = 1 } OpTarget;

/* Macro functions */
#define set_mode_reg(sel_internal_col, sel_col, op_direction, cmd)             \
  {                                                                            \
    *AP_MODE = (sel_internal_col << 24) | (sel_col << 16) |                    \
               (op_direction << 8) | (cmd);                                    \
  }

#define set_control_reg(ap_if_state, ap_op_target, ap_trigger_ap, ap_rst)      \
  {                                                                            \
    *AP_CONTROL = (ap_if_state << 24) | (ap_op_target << 16) |                 \
                  (ap_trigger_ap << 8) | (ap_rst);                             \
  }

/* Prototypes */
// Utilities
void tiny_delay(uint32_t delay_in_nops);

// Reset both internal collumns
// Base startup
#pragma GCC push_options
#pragma GCC optimize("O0")
void warmup_ap();
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize("O0")
void reset_ap_engine();
#pragma GCC pop_options

// Extra
void ap_search(uint8_t key, APInternalCollunm internal_col, OpTarget target);

void ap_flush_col(APCollunm col, APInternalCollunm internal_col);

void ap_flush_a_1();

void ap_flush_b_0();

void ap_flush_c_0();

void ap_flush_c_1();

void ap_memcpy(void *target, void *src, size_t bytes);

// R/W functions
void ap_write_vector(APCollunm col, APInternalCollunm internal_col, uint8_t *V,
                     size_t size);

void ap_read_vector(APCollunm col, APInternalCollunm internal_col, uint8_t *V,
                    size_t size);

void ap_read_result_vector(APInternalCollunm internal_col, uint8_t *V,
                           size_t size);

void ap_set_value(APCollunm col, APInternalCollunm internal_col, uint8_t value);

void ap_set_value_cam_a_left(uint8_t factor);

// Assembly
extern void ap_store_data(unsigned int addr, uint8_t data);
extern unsigned char ap_get_data(unsigned int addr);
extern void ap_get_data_to(unsigned int dest, unsigned int src);

// AP computing
void ap_trigger_computing_w_wait(APOperations op, APOpDirection op_direction,
                                 APInternalCollunm internal_col);

void ap_trigger_computing_w_wait_target_a(APOperations op,
                                          APOpDirection op_direction,
                                          uint8_t internal_col_bits);

void ap_trigger_vertical_computing_target(APOperations op, APCollunm col,
                                          APInternalCollunm internal_col,
                                          OpTarget target);

void ap_computing(APOperations op, APInternalCollunm internal_col,
                  APOpDirection op_direction, uint8_t *A, uint8_t *B,
                  size_t size);

void ap_vertical_computing(APOperations op, APCollunm col,
                           APInternalCollunm internal_col, uint8_t *V,
                           size_t size);

// void ap_trigger_computing(APOperations op, APInternalCollunm internal_col,
// APOpDirection op_direction, OpTarget op_target);
void ap_trigger_computing(APOperations op, APCollunm col,
                          APInternalCollunm internal_col,
                          APOpDirection op_direction, OpTarget op_target);
void ap_trigger_vertical_computing_w_wait(APOperations op, APCollunm col,
                                          APInternalCollunm internal_col);

void release_ap_if();

BitState ap_waiting_for_computing(uint32_t loops);

// AP IRQ check
volatile uint8_t ap_irq_check();

#endif
