#include <Arduino.h>
#include <Arduino_FreeRTOS.h>


#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "uart.h"

#include "misc.h"



// ##############################################
// #            global variables                #
// ##############################################

char b[100];
volatile uint8_t buttonl;
volatile uint8_t buttonr;        // volatile variable for buttonl and buttonr input, 1: active, 0: inactive

uint16_t PWM_RANGE;


// ##############################################
// #            global tasks                    #
// ##############################################

void blink(void* param);
void init_task(void *param);
void solenoid_task(void *param);


// ##############################################
// #            global functions                #
// ##############################################

void draw_welcome(void);
void setup_button_inputs(void);
void setup_pwm_outputs(void);

// ##############################################
// #                  setup                     #
// ##############################################
void setup() {
  DDRB |= (1 << PB7);
  PORTB &= ~(1 << PB7);

  xTaskCreate(
    init_task
    ,  "INIT" // A name just for humans
    ,  256  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  
    
}
void loop() {}  // nothing goes here, as all will be handeled in tasks.


void blink(void* param){

  DDRB |= (1 << 6);
  // char s[] = "blink\n";

  TickType_t last = xTaskGetTickCount();    // needed or xTaskDelayUntil
  
  while(1)
  {

    
    // uart_puts(0, s);
    // uart_puts(1, b);
    // if(br){
      PORTB |= (1 << PB6);
    
    // else
    xTaskDelayUntil(&last, 200 / portTICK_PERIOD_MS);
    
      PORTB &= ~(1 << PB6);
    
    xTaskDelayUntil(&last, 200 / portTICK_PERIOD_MS);
  }
}

/** 
 *  @brief task to initialize all other tasks
 * 
 *  @param param: parameter pointer (unused)
 * 
 * */
void init_task(void *param){

  // initialize uart interface: 
  // uart0: USB connection
  // uart1: debug connection (use FTDI Serial to USB converter)
  uart_init(0, 57600);
  // uart_init(1, 57600);  

  draw_welcome(); 
  DDRB |= (1 << 7);         // set led pin as output



  
  print_debug("create blink task... ");
  xTaskCreate(
    blink
    ,  "Blink" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  print_debug("create solenoid task... ");

  xTaskCreate(
    solenoid_task
    ,  "Solenoid" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");


  vTaskDelete(NULL);
}


/**
 *  @brief task to control solenoids with PWM
 * 
 *  @param param: parameter pointer (unused)
 * */
void solenoid_task(void *param){

  // TODO: this is just a test function to verify the workings of the ISR function
  setup_button_inputs();

  while(1)
  {
    if(buttonl){
      PORTB |= (1 << 7);
    
    }
    else{
      PORTB &= ~(1 << 7);
      
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}










// welcome screen:
// ========================================
// ||            Main Control            ||
// ||               Board                ||
// ||------------------------------------||
// ||        Firmware: 2021-11-07        ||
// ========================================
// UART0: initialized
// UART1: initialized ...
void draw_welcome(void){
  
  // char temp_buff[64];
  // sprintf(temp_buff, "\033[2J");    // clear serial screen
  // uart_puts(DEBUG_UART, temp_buff);

  print_debug("========================================\n"); 
  print_debug("||            Main Control            ||\n");
  print_debug("||               Board                ||\n");
  print_debug("||------------------------------------||\n");
  print_debug("||        Firmware: 2021-11-07        ||\n");
  print_debug("========================================\n");
 
  print_debug("UART0: initialized\n");_delay_ms(10);

  print_debug("UART1: initialized\n");_delay_ms(10);
}


/** 
 *  @brief: function to initialize inputs for button with ISR pin change * 
 * */
void setup_button_inputs(void){

  // set to input
  BUTTONL_DDR &= ~(1 << BUTTONL_P);
  BUTTONR_DDR &= ~(1 << BUTTONR_P);

  // enable internal pullup (around 35k)
  BUTTONL_PORT |= (1 << BUTTONL_P);
  BUTTONR_PORT |= (1 << BUTTONR_P);


  // PCINT9 and PCINT10 on 
  // enable corresponding PCINT 
  // ! this has to be changed manually on pin change
  PCICR |= (1 << PCIE1);

  // enable interrupt for PCIE1 pins
  PCMSK1 |= (1 << BUTTONL_INT) | (1 << BUTTONR_INT);
}



/**
 *  @brief setup function to setup timers for pwm operation on solenoids
 *  MAX value for PWM in OCR4x is set in PWM_RANGE
 *  TIMER 4, pins PH3, PH4, PH5 
 * 
 * */
void setup_pwm_outputs(void)
{

  // using timer 4 for pins
  // up counting, with max accordingly set to match pwm frequency in ICRn
  // fast pwm  mode with set on compare, so a value of 0 means always on, as input of 
  // solenoid drivers is inverted

  
  ICR4 = (F_CPU / (1 * PWM_FREQ)) - 1;                                              // calculate max. value for set PWM frequency, MUST NOT EXCEED 16 bit!

  // no interrupt needed, so none is ste

  OCR4A = 0;
  OCR4B = 0;
  OCR4C = 0;

  TCCR4A = (0x3 << COM4A0) | (0x3 << COM4B0) | (0x3 << COM4C0) | (0x2 << WGM40);    // set to set on compare, fast pwm

  TCCR4B = (0x1 << CS40) | (0x3 << WGM42);                                          // clock prescaler = 1, fast pwms


  // set PWM_RANGE to ICR4 value, to have a reference frame for pwm signal
  PWM_RANGE = ICR4;


}




/**
 * @brief Interrupt Service Routine for Pin Change Interrupt 1
 *  will handle the interrupts on buttonl and buttonr
 * 
 * */
ISR(PCINT1_vect){
  static uint8_t buttonr_prev = (1 << BUTTONR_P), buttonl_prev = (1 << BUTTONL_P);    // private variable of previous state of buttons
  static TickType_t last_buttonr_tick = 0, last_buttonl_tick = 0;

  
  TickType_t now = xTaskGetTickCount();
  uint8_t bl = BUTTONL_PIN & (1 << BUTTONL_P);  // buffer input so it won't change during ISR
  uint8_t br = BUTTONR_PIN & (1 << BUTTONR_P);  // buffer input so it won't change during ISR
  

  // // check buttonl if changed
  if((buttonl_prev != bl) && ((now - last_buttonl_tick) > (BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS))){
    
  //   // update last_tick
    last_buttonl_tick = now;

    
  //   // update last button state
    buttonl_prev = bl;

  //   // set new button state
    buttonl = !(bl & (1 << BUTTONL_P));    // inverse signal, as pullup resistor: active low
    
  }

  // check buttonr if changed
  if((buttonr_prev != br) && ((now - last_buttonr_tick) > (BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS))){
    
    // update last_tick
    last_buttonr_tick = now;

    // update last button state
    buttonr_prev = br;

    // set new button state
    buttonr = !(br & (1 << BUTTONR_P));    // inverse signal, as pullup resistor: active low
    
 
    
  }

  
}