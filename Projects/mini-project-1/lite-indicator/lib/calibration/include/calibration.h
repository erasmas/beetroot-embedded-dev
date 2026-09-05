#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdbool.h>

/**
 * @brief Start the BOOT button task and load saved thresholds
 * @arg   set_key - drives the output the calibration blinks for feedback
 */
void calibration_init(void (*set_key)(bool on));

/**
 * @brief Wait for a button press and act on it
 * @arg   timeout_ms - how long to wait before giving up
 * @return true if a press was handled, false on timeout
 */
bool calibration_handle_input(int timeout_ms);

bool calibration_active(void);

const char *calibration_state_name(void);

int calibration_threshold_dark(void);

int calibration_threshold_light(void);

#endif // CALIBRATION_H
