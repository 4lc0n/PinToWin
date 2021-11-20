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



#define MATRIX_ROW0_P   PF0
#define MATRIX_ROW0_DDR DDRF
#define MATRIX_ROW0_PORT    PORTF

#define MATRIX_ROW1_P   PF1
#define MATRIX_ROW1_DDR DDRF
#define MATRIX_ROW1_PORT    PORTF

#define MATRIX_ROW2_P   PF2
#define MATRIX_ROW2_DDR DDRF
#define MATRIX_ROW2_PORT    PORTF







// ----------------------------------
//          Timing Definitions
// ----------------------------------

#define BUTTON_DEBOUNCE_MS  50


#define DUTYCYCLE_TARGET 0.2        // target for solenoid duty cycle




#endif