#ifndef MAIN_H
#define MAIN_H



// ----------------------------------
//          Pin Definitions
// ----------------------------------

#define BUTTONL_DDR     DDRJ
#define BUTTONL_PORT    PORTJ
#define BUTTONL_PIN     PINJ
#define BUTTONL_P       PJ0
#define BUTTONL_INT     PCINT9

#define BUTTONR_DDR     DDRJ
#define BUTTONR_PORT    PORTJ
#define BUTTONR_PIN     PINJ
#define BUTTONR_P       PJ1
#define BUTTONR_INT     PCINT10

#define BUTTON_GND_DDR  DDRH
#define BUTTON_GND_PORT PORTH
#define BUTTON_GND_P    PH1



#define RELAY_DDR       DDRF
#define RELAY_PORT      PORTF
#define RELAY_P         PF3



#define SOL1_DDR        DDRH
#define SOL1_P          PH3
#define SOL1_TIM_OCR    OCR4A

#define SOL2_DDR        DDRH
#define SOL2_P          PH4
#define SOL2_TIM_OCR    OCR4B

#define SOL3_DDR        DDRH
#define SOL3_P          PH5
#define SOL3_TIM_OCR    OCR4C

#define FLIPPER_L_OCR   SOL1_TIM_OCR        // TODO: assign these
#define FLIPPER_R_OCR   SOL3_TIM_OCR        // TODO: assign these
#define FLIPPER_START_OCR   SOL2_TIM_OCR    // TODO: assign these

#define PWM_FREQ        10e3



#define MATRIX_ROW0_P       PF0
#define MATRIX_ROW0_DDR     DDRF
#define MATRIX_ROW0_PORT    PORTF

#define MATRIX_ROW1_P       PF1
#define MATRIX_ROW1_DDR     DDRF
#define MATRIX_ROW1_PORT    PORTF

#define MATRIX_ROW2_P       PF2
#define MATRIX_ROW2_DDR     DDRF
#define MATRIX_ROW2_PORT    PORTF

// TODO: check this
// most leftern one
#define MATRIX_COL0_P       PB7
#define MATRIX_COL0_DDR     DDRB
#define MATRIX_COL0_PORT    PORTB

#define MATRIX_COL1_P       PB6
#define MATRIX_COL1_DDR     DDRB
#define MATRIX_COL1_PORT    PORTB  

#define MATRIX_COL2_P       PB5
#define MATRIX_COL2_DDR     DDRB
#define MATRIX_COL2_PORT    PORTB
//  most rightern one
#define MATRIX_COL3_P       PB4
#define MATRIX_COL3_DDR     DDRB
#define MATRIX_COL3_PORT    PORTB


// ----------------------------------
//          Timing Definitions
// ----------------------------------

#define BUTTON_DEBOUNCE_MS  50


#define DUTYCYCLE_TARGET 0.2        // target for solenoid duty cycle
#define DUTYCYCLE_REDUCTION 0.5
#define FULL_POWER_DURATION 6       // full power duration in multible of 50 ms


#define TEMPERATURE_THRESHOLD   70
#define CURRENT_THRESHOLD       2.8


// ----------------------------------
//          DEBUG Definitions
// ----------------------------------

#define DEBUG_DDR       DDRA
#define DEBUG_PORT      PORTA

#define DEBUG_SAFETY    PA0
#define DEBUG_SOLENOID  PA1
#define DEBUG_ADC       PA2
#define DEBUG_BLINK     PA3
#define DEBUG_SCORE     PA4
#define DEBUG_ADC_ISR   PA5
#define DEBUG_UART_ISR  PA6


// ----------------------------------
//          Variable states
// ----------------------------------

enum starter_conditions{Empty = 0, At_Starter = 1, Fired = 2};


// ----------------------------------
//          score points
// ----------------------------------
#define TARGET_POINTS 10
#define BUTTON_POINTS 20
#define SLINGHSHOT_POINTS 25

#endif