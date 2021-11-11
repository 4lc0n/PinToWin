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

#define RELAY_DDR       DDRF
#define RELAY_PORT      PORTF
#define RELAY_P         PF3



#define MATRIX_ROW0     PF0
#define MATRIX_ROW1     PF1
#define MATRIX_ROW2     PF2




// ----------------------------------
//          Timing Definitions
// ----------------------------------

#define BUTTON_DEBOUNCE_MS  50




#endif