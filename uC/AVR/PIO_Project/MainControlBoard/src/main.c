/**
 * @file main.c
 *  
 * 
 * @author Juergen Markl
 * @brief Main file for main board controller (ATMega2560)
 * @version 0.1
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 * 
 * TODO: tick rate to either 5 ms or 1 ms
 * TODO: check for tick counter, which data format, to prevent overflow and unexpected behaviour
 * 
 */




#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "main.h"


#include "misc.h"
#include "uart.h"


// ##############################################
// #            global variables                #
// ##############################################

char b[100];
volatile uint8_t buttonl;
volatile uint8_t buttonr;        // volatile variable for buttonl and buttonr input, 1: active, 0: inactive

uint16_t PWM_RANGE;




uint32_t score = 0;


// ##############################################
// #            FreeRTOS specifics              #
// ##############################################
SemaphoreHandle_t xSemaphore_r_button, xSemaphore_l_button;

SemaphoreHandle_t xSemaphore_adc_complete;

// ##############################################
// #            global tasks                    #
// ##############################################

void blink(void* param);
void init_task(void *param);
void solenoid_task(void *param);

void check_input_l_task(void *param);
void check_input_r_task(void *param);

void update_score_task(void *param);

void process_adc_task(void *param);

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


  xTaskCreate(
    init_task
    ,  "INIT" // A name just for humans
    ,  256  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  
    
}
void loop() {}  // nothing goes here, as all will be handeled in tasks.



// ##############################################
// #               init_task                    #
// ##############################################
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
  clear_led();

  setup_pwm_outputs();
  print_debug("PWM module: initialized\n");


  
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

  print_debug("create button input handler task right... ");
  xTaskCreate(
    check_input_r_task
    ,  "Button_r" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  print_debug("create button input handler task left... ");
  xTaskCreate(
    check_input_l_task
    ,  "Button_l" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");


  print_debug("create score update task... ");
  xTaskCreate(
    update_score_task
    ,  "Score_t" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");



  vTaskDelete(NULL);
}


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
 *  @brief task to control solenoids with PWM
 * 
 *  @param param: parameter pointer (unused)
 * */
/**
 *  @brief task to control solenoids with PWM
 * 
 *  @param param: parameter pointer (unused)
 * */
void solenoid_task(void *param){

  TickType_t lastTick = xTaskGetTickCount();
  uint8_t buttonl_prev, buttonr_prev;
  setup_button_inputs();

  // set pins as output: 

  SOL1_DDR |= (1 << SOL1_P);
  SOL2_DDR |= (1 << SOL2_P);
  SOL3_DDR |= (1 << SOL3_P);


  // TODO: put this in the safety task: 
  RELAY_DDR |= (1 << RELAY_P);
  RELAY_PORT |= (1 << RELAY_P);

  buttonl_prev = buttonl;
  buttonr_prev = buttonr;

  while(1)
  {
    if(buttonl){
      set_led();

      // check if it is initial run: 
      if(buttonl_prev != buttonl){
        // initial run: set to full power
        FLIPPER_L_OCR = PWM_RANGE;
        buttonl_prev = buttonl;
      }

      else{
        if(FLIPPER_L_OCR > PWM_RANGE / 5){
          // gradually reduce duty cycle
          FLIPPER_L_OCR = (uint16_t)(FLIPPER_L_OCR * 0.75);
        }
        else{
        // set solenoid left output to standard output: 
        FLIPPER_L_OCR = PWM_RANGE / 5;    // set to 20% output
        }
        
      }
      

    }
    else{
      clear_led();
      FLIPPER_L_OCR = 0;
      buttonl_prev = buttonl;
      
    }

    if(buttonr){
      set_led();

      // check if it is initial run: 
      if(buttonr_prev != buttonr){
        // initial run: set to full power
        FLIPPER_R_OCR = PWM_RANGE;
        buttonr_prev = buttonr;
      }

      else{
        if(FLIPPER_R_OCR > PWM_RANGE / 5){
          // gradually reduce duty cycle
          FLIPPER_R_OCR = (uint16_t)(FLIPPER_R_OCR * 0.75);
        }
        else{
        // set solenoid left output to standard output: 
        FLIPPER_R_OCR = PWM_RANGE / 5;    // set to 20% output
        }
        
      }
      

    }
    else{
      clear_led();
      FLIPPER_R_OCR = 0;
      buttonr_prev = buttonr;
    }

    // TODO: implement starter flipper mechanism

    xTaskDelayUntil(&lastTick, 50 / portTICK_PERIOD_MS);       // delay for 50 ms until next check
                                                              // delays of up to 100 ms feel 'natural'
    
  }
}


/**
 *  @brief task to ckeck button state after a debounce interval
 * 
 * */
void check_input_l_task(void *param)
{
  xSemaphore_l_button = xSemaphoreCreateBinary();
  if( xSemaphore_l_button == NULL){
    // failed to create semaphore!
    print_debug("failed to create semaphore in check_input_l_task\n");
    while(1);
  }

  TickType_t lastTick = xTaskGetTickCount();

  while(1){


    xSemaphoreTake(xSemaphore_l_button, portMAX_DELAY);

    // debounce: wait until state is settled
    xTaskDelayUntil(&lastTick, BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS); 

    // read button state:
    buttonl = !(BUTTONL_PIN & (1 << BUTTONL_P));    // inverse signal, as pullup resistor: active low

  }

}
/**
 *  @brief task to ckeck button state after a debounce interval
 * 
 * */
void check_input_r_task(void *param)
{
  xSemaphore_r_button = xSemaphoreCreateBinary();
  if( xSemaphore_r_button == NULL){
    // failed to create semaphore!
    print_debug("failed to create semaphore in check_input_l_task\n");
    while(1);
  }

  TickType_t lastTick = xTaskGetTickCount();

  while(1){


    xSemaphoreTake(xSemaphore_r_button, portMAX_DELAY);

    // debounce: wait until state is settled
    xTaskDelayUntil(&lastTick, BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS); 

    // read button state:
    buttonr = !(BUTTONR_PIN & (1 << BUTTONR_P));    // inverse signal, as pullup resistor: active low

  }
}


/**
 *  @brief Task to update the score board and send via UART0
 * 
 * */
void update_score_task(void *param){

  TickType_t lastTick = xTaskGetTickCount();

  // TODO: this is just for test purpose: remove when light sensors are implemented
  uint8_t last_l_button, last_r_button;
  last_l_button = buttonl;
  last_r_button = buttonr;

  char score_s[10];


  while(1)
  {

    if(buttonl != last_l_button){
      last_l_button = buttonl;
      score += 10;

      // print to uart0
      sprintf(score_s, "%lu\n", score);
      uart_puts(0, score_s);

    }

    if(buttonr != last_r_button){
      last_r_button = buttonr;
      score += 10;

      // print to uart0
      sprintf(score_s, "%lu\n", score);
      uart_puts(0, score_s);
    }

    vTaskDelayUntil(&lastTick, 100 / portTICK_PERIOD_MS);    

  }



}

/**
 * @brief Task to process adc sampled data in ping pong buffer
 * will execute every time one scan is complete. from here the next 
 * scan should be started using adc_start^
 * 
 * @param param unused
 */
void process_adc_task(void *param)
{
  xSemaphore_adc_complete = xSemaphoreCreateBinary();
  if( xSemaphore_adc_complete == NULL){
    // failed to create semaphore!
    print_debug("failed to create semaphore in process_adc_task\n");
    while(1);
  }




  while (1)
  {
    
    // wait for semaphore

    // convert to temperature

    // convert to current

    // give semaphore for security task


    // evaluate matrix


    // start adc after fixed interval

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
  print_debug("||        Firmware: 2021-11-17        ||\n");
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

  // activate GND pin for the button input
  BUTTON_GND_DDR |= (1 << BUTTON_GND_P);
  BUTTON_GND_PORT &= ~(1 << BUTTON_GND_P);
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



  uint8_t bl = BUTTONL_PIN & (1 << BUTTONL_P);  // buffer input so it won't change during ISR
  uint8_t br = BUTTONR_PIN & (1 << BUTTONR_P);  // buffer input so it won't change during ISR
  

  // // check buttonl if changed
  if((buttonl_prev != bl)){

    // update last button state
    buttonl_prev = bl;

    // give semaphore so task to check button l can debounce it
    xSemaphoreGiveFromISR(xSemaphore_l_button, NULL);

  }

  // check buttonr if changed
  if((buttonr_prev != br) ){

    // update last button state
    buttonr_prev = br;

    // give semaphore
    xSemaphoreGiveFromISR(xSemaphore_r_button, NULL);
    
  }

  
}