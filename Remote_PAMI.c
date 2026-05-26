#include <stdio.h>
#include "pico/stdlib.h"

#include "Remote_PAMI.h"

// Global variable definitions
PIO pio_motor = pio0;
StepperMotor motor0;
StepperMotor motor1;
StepperMotor motor2;

void pio0_irq_handler() {
    // Check which state machine triggered the IRQ by looking at the IRQ flags
    // irq_status tells us which of the 4 SMs hit their 'irq wait 0' instruction
    uint32_t irq_status = pio_motor->ints0;

    if(irq_status & PIO_INTR_SM0_BITS)
    {
        pio_interrupt_clear(motor0.pio, motor0.sm); // Clear the interrupt flag for SM0
        motor0.moving = false; // Mark motor0 as done moving
        printf("Motor 0 done moving!\n");
    }
    if(irq_status & PIO_INTR_SM1_BITS)
    {
        pio_interrupt_clear(motor1.pio, motor1.sm); // Clear the interrupt flag for SM1
        motor1.moving = false; // Mark motor1 as done moving
        printf("Motor 1 done moving!\n");
    }
    if(irq_status & PIO_INTR_SM2_BITS)
    {
        pio_interrupt_clear(motor2.pio, motor2.sm); // Clear the interrupt flag for SM2
        motor2.moving = false; // Mark motor2 as done moving
        printf("Motor 2 done moving!\n");
    }

}

void move_motor(PIO pio, uint sm, uint32_t delay_cycles, uint32_t steps)
{
    // Push speed (delay cyles) then distance (step count) into FIFO
    pio_sm_put_blocking(pio, sm, delay_cycles);
    pio_sm_put_blocking(pio, sm, steps);
}

int main()
{
    stdio_init_all();
    sleep_ms(2000); // Wait for the USB serial connection to be established
    printf("Initializing 3-axis PIO stepper control...\n");

    // 1. Associate your custom handler function with the hardware vector table
    irq_set_exclusive_handler(PIO0_IRQ_0, pio0_irq_handler);

    // 2. Clear any lingering stale flags on this channel before turning it on
    pio_set_irq0_source_enabled(pio_motor, pis_interrupt0, true); // For SM0 (rel 0)
    pio_set_irq0_source_enabled(pio_motor, pis_interrupt1, true); // For SM1 (rel 1)
    pio_set_irq0_source_enabled(pio_motor, pis_interrupt2, true); // For SM2 (rel 2)

    // 3. Enable the actual interrupt channel on the ARM CPU Core NVIC
    irq_set_enabled(PIO0_IRQ_0, true);

    // Load the shared program memory once into PIO0
    uint pio0_offset = pio_add_program(pio_motor, &stepper_program);

    // Initialize Left Wheel on SM0 (Step Pin 2, Dir Pin 3)
    stepper_init(&motor0, pio_motor, 0, M0_STEP_PIN, M0_DIR_PIN, pio0_offset);
    
    // Initialize Right Wheel on SM1 (Step Pin 4, Dir Pin 5)
    stepper_init(&motor1, pio_motor, 1, M1_STEP_PIN, M1_DIR_PIN, pio0_offset);
    
    // Initialize Back Wheel on SM2 (Step Pin 6, Dir Pin 7)
    stepper_init(&motor2, pio_motor, 2, M2_STEP_PIN, M2_DIR_PIN, pio0_offset);

    printf("Going to move!\n");

    float speed1 = 1.6f; // 2.0 steps/ms
    float speed2 = 3.2f; // 1.5 steps/ms

    int state = 0;
    int next_movement_state = 1; // Explicitly track where to go next

    while (true) {
        switch(state) {
            case 0:
                printf("Movement 1\n");
                stepper_move(&motor0, 3200, speed1);
                stepper_move(&motor1, 3200, speed1);
                stepper_move(&motor2, 3200, speed1);
                state = 2; // Set to 2 to wait for completion before next move
                next_movement_state = 1; // After we wait, execute Movement 2
                break;
            case 1:
                printf("Movement 2\n");
                stepper_move(&motor0, -3200, speed2);
                stepper_move(&motor1, -3200, speed2);
                stepper_move(&motor2, -3200, speed2);
                state = 2; // Set to 2 to wait for completion before next move
                next_movement_state = 0; // After we wait, go back to Movement 1
                break;
            case 2:
                // Wait for all motors to finish moving
                printf("Waiting for motors to finish...\n");
                if(!(motor0.moving || motor1.moving || motor2.moving)) {
                    state = next_movement_state;
                }
                break;
            default:
                break;
        }
        

        sleep_ms(100);


    }
}
