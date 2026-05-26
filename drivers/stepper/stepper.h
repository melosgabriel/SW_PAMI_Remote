#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "stepper.pio.h"

// Object structure representing a single instance of a stepper motor
typedef struct {
    PIO pio;            // pio0 or pio1
    uint sm;            // State Machine index (0 to 3)
    uint step_pin;      // GPIO pin assigned to STEP
    uint dir_pin;       // GPIO pin assigned to DIR
    uint pio_offset;    // Memory location where the PIO program is loaded
    bool moving;         // Flag to indicate if the motor is currently executing a move command
} StepperMotor;

void init_stepper_sm(PIO pio, uint sm, uint offset, uint step_pin);

// Public API Functions
void stepper_init(StepperMotor* motor, PIO pio, uint sm, uint step_pin, uint dir_pin, uint pio_offset);
void stepper_move(StepperMotor* motor, int32_t steps, float steps_ms);

#endif /* __STEPPER_H__ */