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
 * //TODO: tick rate to either 5 ms or 1 ms
 * //TODO: check for tick counter, which data format, to prevent overflow and unexpected behaviour
 * 
 */




#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"


#include "misc.h"
#include "uart.h"
#include "adc.h"


// ##############################################
// #            global variables                #
// ##############################################

char b[100];
volatile uint8_t buttonl;
volatile uint8_t buttonr;        // volatile variable for buttonl and buttonr input, 1: active, 0: inactive

uint16_t PWM_RANGE;


extern volatile uint16_t ping_buffer[], pong_buffer[];    // buffer for adc data
extern volatile uint8_t pp_select;                        // selector for ping or pong buffer: 0 = ping, 1 = pong


uint32_t score = 0;

float temperature_l = 0, temperature_r = 0;
float current_1 = 0, current_2 = 0, current_3 = 0;                    //  starter flipper - left flipper - right flipper

// ##############################################
// #            FreeRTOS specifics              #
// ##############################################
SemaphoreHandle_t xSemaphore_r_button, xSemaphore_l_button;

SemaphoreHandle_t xSemaphore_adc_complete;

SemaphoreHandle_t xSemaphore_safety;
SemaphoreHandle_t xSemaphore_targets;

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

void safety_task(void *param);

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


  // print_debug("create score update task... ");
  // xTaskCreate(
  //   update_score_task
  //   ,  "Score" // A name just for humans
  //   ,  128  // Stack size
  //   ,  NULL //Parameters for the task
  //   ,  1  // Priority
  //   ,  NULL ); //Task Handle
  // print_debug("OK\n");

  print_debug("create adc task... ");
  xTaskCreate(
    process_adc_task
    ,  "ADC" // A name just for humans
    ,  512  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  // print_debug("create safety task... ");
  // xTaskCreate(
  //   safety_task
  //   ,  "Safety" // A name just for humans
  //   ,  128  // Stack size
  //   ,  NULL //Parameters for the task
  //   ,  configMAX_PRIORITIES-1  // Priority
  //   ,  NULL ); //Task Handle
  // print_debug("OK\n");


  vTaskDelete(NULL);
}

void blink(void* param){

  DDRB |= (1 << 6);
  char s[50];
  TickType_t last = xTaskGetTickCount();    // needed or xTaskDelayUntil
  TickType_t current;
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
    current = xTaskGetTickCount();
    sprintf(s, "%d: tl: %d, tr: %d, c1: %d, c2: %d, c3: %d\n",(int)current, (int)temperature_l, (int)temperature_r, (int)ping_buffer[2], (int)ping_buffer[3], (int)ping_buffer[4]);
    print_debug(s);

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
  uint8_t full_power_l, full_power_r;


  setup_button_inputs();

  // set pins as output: 

  SOL1_DDR |= (1 << SOL1_P);
  SOL2_DDR |= (1 << SOL2_P);
  SOL3_DDR |= (1 << SOL3_P);




  buttonl_prev = buttonl;
  buttonr_prev = buttonr;

  while(1)
  {
    if(buttonl){

      // check if it is initial run: 
      if(buttonl_prev != buttonl){
        // initial run: set to full power
        FLIPPER_L_OCR = PWM_RANGE;
        
        buttonl_prev = buttonl;
        full_power_l = 10;
      }

      else{ 
        if(full_power_l)                  // if on full power
        {
          full_power_l--;                 // reduce tick counter 
        }
        else if(FLIPPER_L_OCR > PWM_RANGE * DUTYCYCLE_TARGET){    // if not on full power anymore
          // gradually reduce duty cycle
          FLIPPER_L_OCR = (uint16_t)(FLIPPER_L_OCR * DUTYCYCLE_REDUCTION);
        }
        else{
        // set solenoid left output to standard output: 
        FLIPPER_L_OCR = PWM_RANGE * DUTYCYCLE_TARGET;    // set to 20% output
        }
        
      }
      

    }
    else{
      
      FLIPPER_L_OCR = 0;
      buttonl_prev = buttonl;
      full_power_l = 0;
    }

    if(buttonr){
      

      // check if it is initial run: 
      if(buttonr_prev != buttonr){
        // initial run: set to full power
        FLIPPER_R_OCR = PWM_RANGE;
        buttonr_prev = buttonr;

        full_power_r = 10;
      }

      
      else{

        if(full_power_r)                      // if running on full power
        {
          full_power_r--;                     // reduce counter
        }
        else if(FLIPPER_R_OCR > PWM_RANGE * DUTYCYCLE_TARGET){ // else if not on full power anymore
          // gradually reduce duty cycle
          FLIPPER_R_OCR = (uint16_t)(FLIPPER_R_OCR * DUTYCYCLE_REDUCTION);
        }
        else{
        // set solenoid left output to standard output: 
        FLIPPER_R_OCR = PWM_RANGE * DUTYCYCLE_TARGET;    // set to 20% output
        }
        
      }
      

    }
    else{
      
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
  xSemaphore_safety = xSemaphoreCreateBinary();
  if(xSemaphore_safety == NULL){
    // failed to create semaphore: 
    print_debug("failed to create semaphore in safety_task\n");
    while(1);
  }
  xSemaphore_adc_complete = xSemaphoreCreateBinary();
  if( xSemaphore_adc_complete == NULL){
    // failed to create semaphore!
    print_debug("failed to create semaphore in process_adc_task\n");
    while(1);
  }

  TickType_t lastTick = xTaskGetTickCount();

#ifdef ADC_8_BIT_RESOLUTION
  uint8_t temp_datar, temp_datal, temp_curr1, temp_curr2, temp_curr3;
#else
  uint16_t temp_datar, temp_datal, temp_curr1, temp_curr2, temp_curr3;
#endif


  // start adc unit
  adc_init();           // initialize adc unit
  adc_start();          // start first conversion

  while (1)
  {
    // TODO: complete
    // wait for semaphore
    xSemaphoreTake(xSemaphore_adc_complete, portMAX_DELAY);

    // convert to temperature
    if(pp_select == 0)
    {
      // left temp sensor
      temp_datal = ping_buffer[0];
      temp_datar = ping_buffer[1];
      temp_curr1 = ping_buffer[2];
      temp_curr2 = ping_buffer[3];
      temp_curr3 = ping_buffer[4];
    }
    else{
      temp_datal = pong_buffer[0];
      temp_datar = pong_buffer[1];
      temp_curr1 = pong_buffer[2];
      temp_curr2 = pong_buffer[3];
      temp_curr3 = pong_buffer[4];
    }
    // https://learn.adafruit.com/thermistor/using-a-thermistor

    // calculate resistance of thermistor ( R2 in voltage divider)
    float r2 = (1023.0 / temp_datal)  - 1;     // (1023/ADC - 1) 
    r2 = TEMP_SENSE_R1 / r2;                 // 10K / (1023/ADC - 1)

    temperature_l = r2 / THERMISTOR_NOMINAL;              // (R/Ro)
    temperature_l = log(temperature_l);                   // ln(R/Ro)
    temperature_l /= THERMISTOR_B;                        // 1/B * ln(R/Ro)
    temperature_l += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
    temperature_l = 1.0 / temperature_l;                  // Invert
    temperature_l -= 273.15;                              // convert absolute temp to C



    // https://learn.adafruit.com/thermistor/using-a-thermistor

    // calculate resistance of thermistor ( R1 in voltage divider)
    r2 = (1023.0 / temp_datar)  - 1;     // (1023/ADC - 1) 
    r2 = TEMP_SENSE_R1 / r2;           // 10K / (1023/ADC - 1)

    temperature_r = r2 / THERMISTOR_NOMINAL;              // (R/Ro)
    temperature_r = log(temperature_r);                   // ln(R/Ro)
    temperature_r /= THERMISTOR_B;                        // 1/B * ln(R/Ro)
    temperature_r += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
    temperature_r = 1.0 / temperature_r;                  // Invert
    temperature_r -= 273.15;                              // convert absolute temp to C
 


    // check within boundaries: 
    if(temperature_r > TEMPERATURE_THRESHOLD || temperature_l > TEMPERATURE_THRESHOLD)
    {
      RELAY_PORT &= ~(1 << RELAY_P);
    }
    else{
      RELAY_PORT |= (1 << RELAY_P);
    }

    // convert to current
    // amplification factor is CURRENTSENSE_AMP_FACTOR (11)
    current_1 = (1023.0 / temp_curr1) * V_ADC_REF / CURRENTSENSE_AMP_FACTOR;
    current_2 = (1023.0 / temp_curr2) * V_ADC_REF / CURRENTSENSE_AMP_FACTOR;
    current_3 = (1023.0 / temp_curr3) * V_ADC_REF / CURRENTSENSE_AMP_FACTOR;

    if(current_1 > CURRENT_THRESHOLD || current_2 > CURRENT_THRESHOLD || current_3 > CURRENT_THRESHOLD)
    {
      RELAY_PORT &= ~(1 << RELAY_P);
    }
    else{
      RELAY_PORT |= (1 << RELAY_P);
    }



    // give semaphore for security task
    xSemaphoreGive(xSemaphore_safety);

    // evaluate matrix


    // start adc after fixed interval

    xTaskDelayUntil(&lastTick, 1000 / ADC_SCAN_RATE / portTICK_PERIOD_MS);
    adc_start();
  }
  
}


/**
 * @brief Task to manage safety features like temperature and current of solenoids
 * Will shut off the relay if solenoids get too hot
 * 
 * @param param 
 */
void safety_task(void *param){
  

  

  RELAY_DDR |= (1 << RELAY_P);        // activate port of relay
  RELAY_PORT |= (1 << RELAY_P);       // start relay

  uint8_t relay_state;
   relay_state = 0;
  char print_buf[50];
  char print_ok[] = "ok";

  vTaskDelay(50);   // delay for 50 ticks to let the system start up

  while(1)
  {
    // TODO: in here WATCHDOG should be reset (with a sufficient margin in timeout)
    // TODO: implement WATCHDOG as reset timer
    // TODO: change tick timer to e.g. TIMER0

    // wait for semaphore: if not given afetr 1sec: shutdown relay
    if(xSemaphoreTake(xSemaphore_safety, 1000 / portTICK_PERIOD_MS) == pdTRUE)
    {

       // check states of channels

      // temperature
      if((temperature_l > TEMPERATURE_THRESHOLD) || (temperature_r > TEMPERATURE_THRESHOLD))
      {
        // warning high temperature!
        // shut down output stage
        // shut off relay
        RELAY_PORT &= ~(1 << RELAY_P);

        sprintf(print_buf, "ERRT: HIGH TEMPERATURE!: %d, %d", (int)temperature_l, (int)temperature_r);
        uart_puts(DEBUG_UART, print_buf);
        vTaskDelay(10);
      }

      // current
      else {
        if((current_1 > CURRENT_THRESHOLD) || (current_2 > CURRENT_THRESHOLD) || (current_3 > CURRENT_THRESHOLD))
        {
          // warning high current
          // shut down output stage
          // shut off relay
          RELAY_PORT &= ~(1 << RELAY_P);

          sprintf(print_buf, "ERRC: HIGH CURRENT!: %d %d %d", (int)(10*current_1), (int)(10*current_2), (int)(10*current_3));
          uart_puts(DEBUG_UART, print_buf);
          vTaskDelay(10);
        }
        else
        {
          // nothing to do, everything fine

          RELAY_PORT |= (1 << RELAY_P);
          uart_puts(DEBUG_UART, print_ok);
        }
      }

    }
    else{

      // failed to get semaphore: adc sampling was not successful, something is wrong!

      // shut off relay
      RELAY_PORT &= ~(1 << RELAY_P);


      sprintf(print_buf, "WARNING: was not able to get semaphore!\n");
      uart_puts(DEBUG_UART, print_buf);
    }



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
  print_debug("||        Firmware: 2021-11-22        ||\n");
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