#ifndef TONE_H
#define TONE_H

#include <avr/io.h>

#include "misc.h"
#include <stdio.h>



// melody from https://github.com/lukecyca/TetrisThemeArduino/blob/master/TetrisThemeArduino.ino
#define _R     (0)      // rest
#define _C3    (130.81)
#define _CS3   (138.59)
#define _D3    (146.83)
#define _DS3   (155.56)
#define _E3    (164.81)
#define _F3    (174.61)
#define _FS3   (185.00)
#define _G3    (196.00)
#define _GS3   (207.65)
#define _A3    (220.00)
#define _AS3   (233.08)
#define _B3    (246.94)
#define _C4    (261.63)
#define _CS4   (277.18)
#define _D4    (293.66)
#define _DS4   (311.13)
#define _E4    (329.63)
#define _F4    (349.23)
#define _FS4   (369.99)
#define _G4    (392.00)
#define _GS4   (415.30)
#define _A4    (440.00)
#define _AS4   (466.16)
#define _B4    (493.88)
#define _C5    (523.25)
#define _CS5   (554.37)
#define _D5    (587.33)
#define _DS5   (622.25)
#define _E5    (659.25)
#define _F5    (698.46)
#define _FS5   (739.99)
#define _G5    (783.99)
#define _GS5   (830.61)
#define _A5    (880.00)
#define _AS5   (932.33)
#define _B5    (987.77)

#define BPM 120

float lead_notes[] = {
  // part 1
  _E5, _B4, _C5, _D5, _C5, _B4, _A4, _A4, _C5, _E5, _D5, _C5, _B4, _B4, _C5, _D5, _E5, _C5, _A4, _A4, _R,
  _D5, _F5, _A5, _G5, _F5, _E5, _C5, _E5, _D5, _C5, _B4, _B4, _C5, _D5, _E5, _C5, _A4, _A4, _R,

  // part 2
  _E4, _C4, _D4, _B3, _C4, _A3, _GS3, _B3,
  _E4, _C4, _D4, _B3, _C4, _E4, _A4, _A4, _GS4, _R

};
float lead_times[] = {
  // part 1
  1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
  1.5, 0.5, 1.0, 0.5, 0.5, 1.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,

  // part 2
  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
  2.0, 2.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 3.0, 1.0
};


/**
 * @brief initialisation function for tone generation using TIMER 3, output B (PE4)
 * 
 * 
 * 
 */
void tone_init()
{

    OCR3A = 50; 
    OCR3B = 20;

    // clear output B on match, wavegen 4 (not set here)
    TCCR3A = (2 << COM3B0) | (3 << WGM30);

    // prescaler 64 gives enough range and resolution
    TCCR3B = (3 << WGM32) | (3 << CS30);

    // deactivate output
    DDRE &= ~(1 << PE4);


}   

/**
 * @brief plays a certain frequency with half duty cycle
 * 
 * @param frequency
 */
void tone_play(uint16_t frequency)
{
    if(frequency == 0){
        DDRE &= ~(1 << PE4);
    }
    else{
    uint16_t timer_value = F_CPU / 64 / frequency;
    OCR3A = timer_value;            // set timer value for frequency

    OCR3B = (timer_value /2);       // half time
    
    
    DDRE |= (1 << PE4);
    }
    
}


void tone_stop()
{

    DDRE &= ~(1 << PE4);
}



#endif