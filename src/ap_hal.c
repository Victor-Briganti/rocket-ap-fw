#include "ap_hal.h"
#include <stdint.h>
#include <string.h>

void tiny_delay(uint32_t delay_in_nops) {
  for (int i = 0; i < delay_in_nops; i++)
    asm volatile("nop");
}

#pragma GCC push_options
#pragma GCC optmize("00")
void warmup_ap() {
#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif

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

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}
#pragma GCC pop_options

void reset_ap_engine() {
  // Reset AP engine
  *AP_CONTROL = 0x1000008;
  *AP_CONTROL = 0x1000000;
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;
}

void ap_write_vector(APCollunm col, APInternalCollunm internal_col, uint8_t *V,
                     size_t size) {
  if (size > AP_COL_SIZE)
    return;

  uint8_t i_col = (uint8_t)internal_col;
  uint32_t final_addr = CAM_A_BASE_ADDR + (AP_COL_SIZE * 2 * ((uint8_t)col)) +
                        (i_col * AP_COL_SIZE);

  volatile uint8_t *cam = (volatile uint8_t *)(final_addr);

  for (int i = 0; i < size; i++) {
    cam[i] = V[i];
  }
}

void ap_read_vector(APCollunm col, APInternalCollunm internal_col, uint8_t *V,
                    size_t size) {
  if (size > AP_COL_SIZE)
    return;

  uint8_t i_col = (uint8_t)internal_col;
  uint32_t final_addr = CAM_A_BASE_ADDR + (AP_COL_SIZE * 2 * ((uint8_t)col)) +
                        (i_col * AP_COL_SIZE);

  volatile uint8_t *cam = (volatile uint8_t *)(final_addr);

  for (int i = 0; i < size; i++) {
    V[i] = cam[i];
  }
}

void ap_read_result_vector(APInternalCollunm internal_col, uint8_t *V,
                           size_t size) {
  ap_read_vector(CAM_C, internal_col, V, size);
}

void ap_trigger_computing_w_wait(APOperations op, APOpDirection op_direction,
                                 APInternalCollunm internal_col) {

  // Lock ap_if_state
  set_control_reg(1, 0, 0, 0);
  set_mode_reg(internal_col, 0, op_direction, op);
  set_control_reg(1, 0, 1, 0);

  while (ap_irq_check() == 0) {
  }

  *AP_CONTROL = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_trigger_computing_w_wait_target_a(APOperations op,
                                          APOpDirection op_direction,
                                          uint8_t internal_col_bits) {

  // Lock ap_if_state
  set_control_reg(1, 0, 0, 0);
  set_mode_reg(internal_col_bits, 0, op_direction, op);
  set_control_reg(1, 1, 1, 0);

  while (ap_irq_check() == 0) {
  }

  *AP_CONTROL = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_flush_a_1() {
  // Lock ap_if_state

  *AP_CONTROL = 0x1000000;
  *AP_MODE = 0x7000000;    // RIGHT
  *AP_CONTROL = 0x1000001; // 001 -> Only A col
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_flush_b_0() {
  *AP_CONTROL = 0x1000000;
  *AP_MODE = 0x0;          // RIGHT
  *AP_CONTROL = 0x1000002; // 001 -> Only A col
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_flush_c_0() {
  // Lock ap_if_state
  *AP_MODE = 0x0; // LEFT
  *AP_CONTROL = 0x1000000;
  *AP_CONTROL = 0x1000004; // 100 -> Only C col
  *AP_CONTROL = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_flush_c_1() {
  // Lock ap_if_state

  *AP_CONTROL = 0x1000000;
  *AP_MODE = 0x7000000;    // RIGHT
  *AP_CONTROL = 0x1000004; // 100 -> Only C col
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_flush_b_1() {
  // Lock ap_if_state

  *AP_CONTROL = 0x1000000;
  *AP_MODE = 0x7000000;
  *AP_CONTROL = 0x1000002; // 100 -> Only C col
  *AP_CONTROL = 0x0;
  *AP_MODE = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void reset_ap_regs() {}

void ap_computing(APOperations op, APInternalCollunm internal_col,
                  APOpDirection op_direction, uint8_t A[], uint8_t B[],
                  size_t size) {
  // Load Vectors
  ap_write_vector(CAM_A, internal_col, A, size);
  ap_write_vector(CAM_B, internal_col, B, size);

  // Lock ap_if_state
  set_control_reg(1, 0, 0, 0);
  set_mode_reg(internal_col, 0, op_direction, op);
  set_control_reg(1, 0, 1, 0);

  return;
}

void ap_trigger_computing(APOperations op, APCollunm col,
                          APInternalCollunm internal_col,
                          APOpDirection op_direction, OpTarget op_target) {
  set_control_reg(TRUE, op_target, FALSE, FALSE);
  set_mode_reg(internal_col, col, op_direction, op);
  set_control_reg(TRUE, op_target, TRUE, FALSE);
  ap_waiting_for_computing(5);
}

void ap_vertical_computing(APOperations op, APCollunm col,
                           APInternalCollunm internal_col, uint8_t *V,
                           size_t size) {
  // Load Vectors
  ap_write_vector(col, internal_col, V, size);

  // Block interface
  *AP_CONTROL = 0x10000;

  // Setting internal collumn
  set_mode_reg(internal_col, col, 1, op);

  // Trigger ap computation - bug here
  *AP_CONTROL = 0x10000 | (1 << 8);
}

void ap_trigger_vertical_computing_w_wait(APOperations op, APCollunm col,
                                          APInternalCollunm internal_col) {
  // Block interface
  *AP_CONTROL = 0x10000;

  // Setting internal collumn
  set_mode_reg(internal_col, col, 1, op);

  // Trigger ap computation - bug here
  *AP_CONTROL = 0x10000 | (1 << 8);

  // Wait to finish AP computation
  while (ap_irq_check() == 0) {
  }

  // Release block interface
  *AP_CONTROL = 0x0;
  tiny_delay(1);
}

void ap_trigger_vertical_computing_target(APOperations op, APCollunm col,
                                          APInternalCollunm internal_col,
                                          OpTarget target) {
  // Block interface
  *AP_CONTROL = 0x10000;

  // Setting internal collumn
  set_mode_reg(internal_col, col, 1, op);

  // Trigger ap computation
  set_control_reg(TRUE, target, TRUE, FALSE);

  // Wait to finish AP computation
  while (ap_irq_check() == 0) {
  }

  // Release block interface
  *AP_CONTROL = 0x0;
  tiny_delay(1);
}

void release_ap_if() { set_control_reg(FALSE, FALSE, FALSE, FALSE); }

volatile uint8_t ap_irq_check() {
  volatile uint8_t ap_irq = *AP_IRQ;
  return ap_irq;
}

BitState ap_waiting_for_computing(uint32_t loops) {
  while (loops--) {
    if (ap_irq_check() == TRUE) {
      *AP_CONTROL = 0x0;
      *AP_MODE = 0x0;
      return TRUE;
    }
  }
  return FALSE;
}

void ap_flush_col(APCollunm col, APInternalCollunm internal_col) {
  set_control_reg(1, 0, 0, 0);
  set_mode_reg(internal_col, col, 0, 0);
  set_control_reg(1, 0, 0, 1 << col);
  set_control_reg(1, 0, 0, 0);
  set_control_reg(0, 0, 0, 0);
}

void ap_set_value(APCollunm col, APInternalCollunm internal_col,
                  uint8_t value) {
  volatile uint32_t addr = CAM_A_0_BASE_ADDR +
                           (AP_COL_SIZE * 2 * ((uint8_t)col)) +
                           (AP_COL_SIZE * ((uint8_t)internal_col));
  uint8_t *ap = (volatile uint8_t *)addr;
  *ap = value;
  set_control_reg(1, 0, 0, 0);
  set_mode_reg(internal_col, col, 0, SET);
  set_control_reg(1, 0, 1, 0);
  // ap_waiting_for_computing(5);
  while (ap_irq_check() == 0) {
  }
  // Release block interface
  *AP_CONTROL = 0x0;
}

void ap_set_value_cam_a_left(uint8_t factor) {
  *AP_CAM_A = factor;
  set_control_reg(1, 0, 0, 0);
  set_mode_reg(LEFT, CAM_A, 0, SET);
  set_control_reg(1, 0, 1, 0);
  while (ap_irq_check() == 0) {
  }
  *AP_CONTROL = 0x0;

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_memcpy(void *target, void *src, size_t bytes) {
  memcpy(target, src, bytes);

#ifdef AP_MONITOR
  ap_monitor(AP_MONITOR_ELEMENTS);
#endif
}

void ap_search(uint8_t key, APInternalCollunm internal_col, OpTarget target) {
  volatile uint8_t *key_addr = (volatile uint8_t *)CAM_B_0_BASE_ADDR;
  if (internal_col == 1) {
    key_addr = (volatile uint8_t *)CAM_B_1_BASE_ADDR;
  }
  *key_addr = key;
  set_control_reg(TRUE, target, 0, 0);
  set_mode_reg(internal_col, 0, 0, SEARCH);
  set_control_reg(TRUE, target, TRUE, 0);
  // ap_waiting_for_computing(5);
  while (ap_irq_check() == 0) {
  }
}
