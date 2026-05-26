#include "stepper.h"

void stepper_init(StepperMotor* motor, PIO pio, uint sm, uint step_pin, uint dir_pin, uint pio_offset) {
    // 1. Populate the instance struct data
    motor->pio = pio;
    motor->sm = sm;
    motor->step_pin = step_pin;
    motor->dir_pin = dir_pin;
    motor->pio_offset = pio_offset;
    motor->moving = false;

    // 2. Initialize the physical DIRECTION pin as a standard CPU GPIO
    gpio_init(motor->dir_pin);
    gpio_set_dir(motor->dir_pin, GPIO_OUT);
    gpio_put(motor->dir_pin, 0);

    // 3. Get the default configuration for our compiled PIO program
    pio_sm_config c = stepper_program_get_default_config(motor->pio_offset);
    
    // Map the 'set' pins of the PIO program to our designated STEP GPIO pin
    sm_config_set_set_pins(&c, motor->step_pin, 1);
    
    // Initialize the STEP GPIO pin for PIO usage
    pio_gpio_init(motor->pio, motor->step_pin);
    pio_sm_set_consecutive_pindirs(motor->pio, motor->sm, motor->step_pin, 1, true);

    // 4. Set the Clock Divider to run the State Machine at exactly 1 MHz
    // RP2350 SysClock (usually 150MHz) / 150.0f = 1,000,000Hz (1 MHz)
    float div = (float)clock_get_hz(clk_sys) / 1000000.0f;
    sm_config_set_clkdiv(&c, div);

    // 5. Load configuration, clear residual FIFO data, and enable the state machine
    pio_sm_init(motor->pio, motor->sm, motor->pio_offset, &c);
    pio_sm_set_enabled(motor->pio, motor->sm, true);
}

void stepper_move(StepperMotor* motor, int32_t steps, float steps_ms) {
    if (steps_ms <= 0.0f) return; 

    // 1. Calculate the half-period delay in microseconds
    // Formula: 500 / steps_per_ms
    uint32_t speed_us = (uint32_t)(500.0f / steps_ms);

    // 2. Prevent underflow or values larger than a 16-bit threshold for stability
    if (speed_us < 2) speed_us = 2;

    bool direction = (steps > 0);
    uint32_t absolute_steps = (steps > 0) ? steps : -steps;


    gpio_put(motor->dir_pin, direction);
    motor->moving = true;
    // Push values to FIFO
    pio_sm_put_blocking(motor->pio, motor->sm, speed_us);
    pio_sm_put_blocking(motor->pio, motor->sm, absolute_steps);
}