#ifndef __REMOTE_PAMI_H__
#define __REMOTE_PAMI_H__

#include "hardware/pio.h"
#include "hardware/irq.h"

#include "stepper.h"

#define M0_STEP_PIN     7
#define M0_DIR_PIN      8
#define M1_STEP_PIN     17
#define M1_DIR_PIN      16
#define M2_STEP_PIN     2
#define M2_DIR_PIN      3

extern PIO pio_motor;


#endif /* __REMOTE_PAMI_H__ */