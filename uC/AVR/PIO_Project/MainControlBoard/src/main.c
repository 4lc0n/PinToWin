/**
 * @file main.c
 *  
 * 
 * @author Juergen Markl
 * @brief Main file for main board controller (ATMega2560): controlls solenoids and button inputs and
 * targets being hit, transferes score
 * @version 1.0
 * @date 2021-11-29
 * 
 * @copyright Copyright (c) 2021
 * 
 * 
 * 
 */




#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"


#include "misc.h"
#include "uart.h"
#include "adc.h"
#include "tone.h"


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

enum starter_conditions starter_state = Empty;
uint8_t starter_delay_after_fired = 0;
uint8_t game_running = 0;         


extern float lead_notes[], lead_times[];



adc_type temp_info[20];
// ##############################################
// #            FreeRTOS specifics              #
// ##############################################
SemaphoreHandle_t xSemaphore_r_button, xSemaphore_l_button, xSemaphore_rpi_button;

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

void check_input_rpi_task(void *param);

void update_score_task(void *param);

void process_adc_task(void *param);

void safety_task(void *param);

void music_task(void *param);



// ##############################################
// #            global functions                #
// ##############################################



void setup_watchdog(void);

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
  uart_init(1, 57600);  

  // activate debug port for task monitoring
  DEBUG_DDR = 0xFF;
  DEBUG_PORT = 0;

  draw_welcome(); 


  setup_pwm_outputs();
  print_debug("PWM module: initialized\n");


  
  print_debug("create blink task... ");
  xTaskCreate(
    blink
    ,  "Blink" // A name just for humans
    ,  300  // Stack size
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

  print_debug("create button rpi handler task... ");
  xTaskCreate(
    check_input_rpi_task
    ,  "RPI_input" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  // moved back after check_input_tasks
  print_debug("create solenoid task... ");
  xTaskCreate(
    solenoid_task
    ,  "Solenoid" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");


  print_debug("create score update task... ");
  xTaskCreate(
    update_score_task
    ,  "Score" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  print_debug("create adc task... ");
  xTaskCreate(
    process_adc_task
    ,  "ADC" // A name just for humans
    ,  512  // Stack size
    ,  NULL //Parameters for the task
    ,  2  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  print_debug("create safety task... ");
  xTaskCreate(
    safety_task
    ,  "Safety" // A name just for humans
    ,  256  // Stack size
    ,  NULL //Parameters for the task
    ,  configMAX_PRIORITIES-1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");
  

    print_debug("create music task... ");
  xTaskCreate(
    music_task
    ,  "Safety" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  configMAX_PRIORITIES-1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");

  vTaskDelete(NULL);
}


// ##############################################
// #               blink task                   #
// ##############################################
void blink(void* param){

  DDRB |= (1 << 6);
  char s[100];
  TickType_t last = xTaskGetTickCount();    // needed or xTaskDelayUntil
  TickType_t current;

  vTaskDelay(10);

  while(1)
  {

    DEBUG_PORT |= (1 << DEBUG_BLINK);

    

    
    current = xTaskGetTickCount();
    sprintf(s, "%ld: tl: %d, tr: %d, c1: %d, c2: %d, c3: %d\n", (uint32_t)current, (int)temperature_l, (int)temperature_r, (int)(current_1*100), (int)(current_2*100), (int)(current_3*100));
    print_debug(s);
    sprintf(s, ", starter: %d temp info: %d, th %d\n", (uint8_t)(starter_state), temp_info[5], temp_info[15]);
    print_debug(s);

    // raw readings
    sprintf(s, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", (int)temp_info[0], (int)temp_info[1], (int)temp_info[2], (int)temp_info[3], (int)temp_info[4], (int)temp_info[5], (int)temp_info[6], (int)temp_info[7], (int)temp_info[8], (int)temp_info[9], (int)temp_info[10], (int)temp_info[11], (int)temp_info[12], (int)temp_info[13], (int)temp_info[14]);
    print_debug(s);


    #
    DEBUG_PORT &= ~(1 << DEBUG_BLINK);

    xTaskDelayUntil(&last, 200 / portTICK_PERIOD_MS);


  }
}

// ##############################################
// #              solenoid task                 #
// ##############################################
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

  vTaskDelay(10);

  while(1)
  {
    // delay on begin of while loop, so on continue, the delay and yield to another task is executed
    xTaskDelayUntil(&lastTick, 50 / portTICK_PERIOD_MS);       // delay for 50 ms until next check
                                                              // delays of up to 100 ms feel 'natural'


    DEBUG_PORT |= (1 << DEBUG_SOLENOID);


    // check if ball is at starter position
    if(starter_state == At_Starter){

      // to start the game: press both buttons simultaneous
      if(buttonl && buttonr)
      {
        // fire solenoid
        FLIPPER_START_OCR = PWM_RANGE;    // at full power

        // update starter variable
        starter_state = Fired;

        // shut off other solenoids

        FLIPPER_L_OCR = 0;
        FLIPPER_R_OCR = 0;
      }
      
      // if not both buttons pressed: 
      // nothing to do
      // continue, don't fire flipper solenoids
      // because otherwise all solenoids will be fired and
      // probably the current protection or current limit of the 
      // power supply is reached
      continue;

    }
    else if(starter_state == Fired)
    {
      // this state should show up ideally around 20 times: 
      // check if starter_delay_after_fired is below 50: 500 ms have passed (adc task reduces it by 1 every 10 ms)
      if(starter_delay_after_fired < 50)
      {
        // shut down starter solenoid
        FLIPPER_START_OCR = 0;

      }
      else{
        // if starter flipper still fired: 
        // continue and don't fire other solenoids, 
        // because with both buttons pressed, turrend protection of 
        // power supply will probably trigger
        continue;
      }
    }

    if(buttonl){

      // check if it is initial run: 
      if(buttonl_prev != buttonl){
        // initial run: set to full power
        FLIPPER_L_OCR = PWM_RANGE;
        
        buttonl_prev = buttonl;
        full_power_l = FULL_POWER_DURATION;
      }

      else{ 
        if(full_power_l)                  // if on full power
        {
          full_power_l--;                 // reduce tick counter 
        }
        else {
          if(FLIPPER_L_OCR > PWM_RANGE * DUTYCYCLE_TARGET){    // if not on full power anymore
          // gradually reduce duty cycle
          FLIPPER_L_OCR = (uint16_t)(FLIPPER_L_OCR * DUTYCYCLE_REDUCTION);
          }
          else{
          // set solenoid left output to standard output: 
          FLIPPER_L_OCR = PWM_RANGE * DUTYCYCLE_TARGET;    // set to 20% output
          }
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

        full_power_r = FULL_POWER_DURATION;
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
    DEBUG_PORT &= ~(1 << DEBUG_SOLENOID);
  }
}

// ##############################################
// #               button l                     #
// ##############################################
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

  vTaskDelay(10);

  while(1){


    xSemaphoreTake(xSemaphore_l_button, portMAX_DELAY);


    // debounce: wait until state is settled
    xTaskDelayUntil(&lastTick, BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS); 

    // read button state:
    buttonl = !(BUTTONL_PIN & (1 << BUTTONL_P));    // inverse signal, as pullup resistor: active low

 
  }

}
// ##############################################
// #               button r                     #
// ##############################################
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

  vTaskDelay(10);

  while(1){


    xSemaphoreTake(xSemaphore_r_button, portMAX_DELAY);



    // debounce: wait until state is settled
    xTaskDelayUntil(&lastTick, BUTTON_DEBOUNCE_MS / portTICK_PERIOD_MS); 

    // read button state:
    buttonr = !(BUTTONR_PIN & (1 << BUTTONR_P));    // inverse signal, as pullup resistor: active low


  }
}

/**
 * @brief Task to ckeck button state on the RPi input lines
 * 
 * @param param 
 */
void check_input_rpi_task(void *param)
{

  xSemaphore_rpi_button = xSemaphoreCreateBinary();
  if( xSemaphore_rpi_button == NULL){
    // failed to create semaphore!
    print_debug("failed to create semaphore in ckeck_input_rpi_task\n");
    while(1);
  }

  while(1)
  {
    xSemaphoreTake(xSemaphore_rpi_button, portMAX_DELAY);

    buttonr = !(RPI_R_PIN & (1 << RPI_R_P));   // inverse signal, as active low!
    buttonl = !(RPI_L_PIN & (1 << RPI_L_P));   //

  }
}

// ##############################################
// #              score task                    #
// ##############################################
/**
 *  @brief Task to update the score board and send via UART0
 * 
 * */
void update_score_task(void *param){

  TickType_t lastTick = xTaskGetTickCount();


  uint8_t chars_written = 0;
  char score_s[10];
  uint32_t last_score = 0;

  vTaskDelay(10);

  while(1)
  {

    DEBUG_PORT |= (1 << DEBUG_SCORE);

    if(score != last_score && game_running){

      // either a new target was hit, or score is reset to 0
      // we will not check here, as it doesn't matter

      // just update the UART output
      chars_written = sprintf(score_s, "%lu\n", score);

      // wait for the UART transmit buffer to have enough space available
      // should never !! ever !! happen, as this is the only data transmitted!
      while(uart_tx_buffer_state(0) < chars_written)  vTaskDelay((TickType_t)1);    // wait for 1 tick if tx buffer full
      uart_puts(0, score_s);

      last_score = score;

    }
    else{
      if(!game_running)
      {
        // game not started, set score to 0 as long as this variable is false
        score = 0;
      }
    }

    DEBUG_PORT &= ~(1 << DEBUG_SCORE);

    vTaskDelayUntil(&lastTick, 100 / portTICK_PERIOD_MS);    

  }



}


// ##############################################
// #               adc task                     #
// ##############################################
/**
 * @brief Task to process adc sampled data in ping pong buffer
 * will execute every time one scan is complete. from here the next 
 * scan should be started using adc_start^
 * 
 * @param param unused
 */
void process_adc_task(void *param)
{

  setup_matrix_outputs();


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


  adc_type temp_datar, temp_datal, temp_curr1, temp_curr2, temp_curr3;
  adc_type temp_targets[10] = {0};
  adc_type base_level[10] = {0};


  // in temp_targets: 
  // 0: starter
  // 1: button r
  // 2: button m
  // 3: button l
  
  // 4: slingshot r
  // 5: target center
  // 6: slingshot l

  // 7: wheel r
  // 8: wheel m
  // 9: wheel l


  vTaskDelay(10);

  // start adc unit
  adc_init();           // initialize adc unit
  adc_start();          // start first conversion

  while (1)
  {

    // wait for semaphore
    xSemaphoreTake(xSemaphore_adc_complete, portMAX_DELAY);

    DEBUG_PORT |= (1 << DEBUG_ADC);

    // copy into local variables, in case calculation is interrupted and buffer gets
    // overwritten
    if(pp_select == 0)
    {
      // left temp sensor
      temp_datal = ping_buffer[0];
      temp_datar = ping_buffer[1];
      temp_curr1 = ping_buffer[2];
      temp_curr2 = ping_buffer[3];
      temp_curr3 = ping_buffer[4];
      for(uint8_t i = 0; i < 10; i++){
        temp_targets[i] = ping_buffer[5+i];
      }
    }
    else{
      temp_datal = pong_buffer[0];    
      temp_datar = pong_buffer[1];
      temp_curr1 = pong_buffer[2];
      temp_curr2 = pong_buffer[3];
      temp_curr3 = pong_buffer[4];
      for(uint8_t i = 0; i < 10; i++){
        temp_targets[i] = ping_buffer[5+i];
      }
    }
    temp_info[0] = temp_datal;
    temp_info[1] = temp_datar;
    temp_info[2] = temp_curr1;
    temp_info[3] = temp_curr2;
    temp_info[4] = temp_curr3;
    for(uint8_t i = 0; i < 10; i++){
      temp_info[5 + i] = temp_targets[i];
    }

    // convert to temperature
    // https://learn.adafruit.com/thermistor/using-a-thermistor

    // calculate resistance of thermistor ( R2 in voltage divider)
    float r2 = (ADC_MAX_F/ (temp_datal+1))  - 1;     // (1023/ADC - 1) 
    r2 = TEMP_SENSE_R1 / r2;                 // 10K / (1023/ADC - 1)

    temperature_l = r2 / THERMISTOR_NOMINAL;              // (R/Ro)
    temperature_l = log(temperature_l);                   // ln(R/Ro)
    temperature_l /= THERMISTOR_B;                        // 1/B * ln(R/Ro)
    temperature_l += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
    temperature_l = 1.0 / temperature_l;                  // Invert
    temperature_l -= 273.15;                              // convert absolute temp to C



    // https://learn.adafruit.com/thermistor/using-a-thermistor

    // calculate resistance of thermistor ( R1 in voltage divider)
    r2 = (ADC_MAX_F/ (temp_datar+1))  - 1;     // (1023/ADC - 1) 
    r2 = TEMP_SENSE_R1 / r2;           // 10K / (1023/ADC - 1)

    temperature_r = r2 / THERMISTOR_NOMINAL;              // (R/Ro)
    temperature_r = log(temperature_r);                   // ln(R/Ro)
    temperature_r /= THERMISTOR_B;                        // 1/B * ln(R/Ro)
    temperature_r += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
    temperature_r = 1.0 / temperature_r;                  // Invert
    temperature_r -= 273.15;                              // convert absolute temp to C
 




    // convert to current
    // amplification factor is CURRENTSENSE_AMP_FACTOR (11)
    current_1 = ( (temp_curr1+1.0) / ADC_MAX_F) * V_ADC_REF ;
    current_1 = current_1  / SHUNT_R / CURRENTSENSE_AMP_FACTOR;

    current_2 = ((temp_curr2+1.0) / ADC_MAX_F) * V_ADC_REF ;
    current_2 = current_2  / SHUNT_R / CURRENTSENSE_AMP_FACTOR;

    current_3 = ((temp_curr3+1.0) / ADC_MAX_F) * V_ADC_REF ;
    current_3 = current_3  / SHUNT_R / CURRENTSENSE_AMP_FACTOR;

    // give semaphore for security task
    xSemaphoreGive(xSemaphore_safety);



    // evaluate matrix
    
    // evaluate column by column
    // column 0: starter 
    // for starter: base level will be set as a slow moving average of the light
    //    condition, wheneveer the light reading is below a threshold value, 
    //    it is assumed the ball is there. 
    //    as of the nature of the filter, the value will decrese over time, as 
    //    the filter adapts to it, but the set variable till stays true
    
    // if light reading is significantly lower than base level and ball was not at starter beforehand
    if(temp_targets[0] < STARTER_THRESHOLD * base_level[0])
    {
      // below threshold

      if(starter_state == Empty){   // if ball was not there previously
   
        // reset score
        score = 0;

        // signal raspberry that game needs to be reset
        RPI_RES_PORT |= (1 << RPI_RES_P);
        
        // set variable for ball in starter position
        starter_state = At_Starter;
        starter_delay_after_fired = 100;      // prepare variable to count down after ball was fired
        print_debug("Game over!\n\nstarter armed\n");

      }
      else{   // if ball already registered
        // this state could also involve that the solenoid just fired
        // and the pusher is blocking the light
        // 
        // do nothing
        _NOP();
      }
    }
    else
    { // if at threshold
      if(starter_state == Fired)
      {
        // if just fired recently
        // reduce starter_delay_after_fired
        // this will count down from 100, so at a 100 hz rate for 1 sec
        // intention: if light condition stays the same for 1 sec, it is
        // assumed, that the ball left the starting position successfully
        // and pusher retracted (see starter implementation)
        starter_delay_after_fired--;

        // if starter is fired, set the game_running to true to start counting score
        game_running = 1;

        // set output to low for raspberry: game running again
        RPI_RES_PORT &= ~(1 << RPI_RES_P);

        if(starter_delay_after_fired == 0)
        {
          // reset starter state
          starter_state = Empty;
          score = 0;
        }
      }

      // else: nothing to do
      // if starter_state == At_Starter
      // we do not need to do anything, as starting will be handled in solenoid task
      
      // if starter_state == empty: everything is fine, ball is in game
    }
    // update base level to adapt to gradual light changes
    base_level[0] = (adc_type)((float)base_level[0] * COMP_FILTER_FACTOR + (float)temp_targets[0] * (1.0 - COMP_FILTER_FACTOR));

    // assign to temp_info to be printed at blink
    



    // === button targets === 
    // if light reading is significantly higher than base level 
    if(temp_targets[1] > (BUTTON_TARGET_THRESHOLD * base_level[1]))
    {
      // increase score
      score += BUTTON_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[1] = temp_targets[1];


      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[1] = (adc_type)((float)base_level[1] * COMP_FILTER_FACTOR + (float)temp_targets[1] * (1.0 - COMP_FILTER_FACTOR));

    if(temp_targets[2] > (BUTTON_TARGET_THRESHOLD * base_level[2]))
    {
      // increase score
      score += BUTTON_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[2] = temp_targets[2];


      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[2] = (adc_type)((float)base_level[2] * COMP_FILTER_FACTOR + (float)temp_targets[2] * (1.0 - COMP_FILTER_FACTOR));

    if(temp_targets[3] > (BUTTON_TARGET_THRESHOLD * base_level[3]))
    {
      // increase score
      score += BUTTON_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[3] = temp_targets[3];

      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[3] = (adc_type)((float)base_level[3] * COMP_FILTER_FACTOR + (float)temp_targets[3] * (1.0 - COMP_FILTER_FACTOR));


  

    
    // ===  target at center of board ===
    // if light reading is significantly lower than base level 
    if(temp_targets[5] < TARGET_THRESHOLD * base_level[5])
    {
      // increase score
      score += TARGET_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[5] = temp_targets[5];

      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    
    base_level[5] = (adc_type)((float)base_level[5] * COMP_FILTER_FACTOR + (float)temp_targets[5] * (1.0 - COMP_FILTER_FACTOR));

    temp_info[15] = base_level[5];



    // ===  sling shot  ===
    // if light reading is significantly lower than base level 
    if(temp_targets[4] < SLINGSHOT_THRESHOLD * base_level[4])
    {
      // increase score
      score += SLINGHSHOT_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[4] = temp_targets[4];

      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[4] = (adc_type)((float)base_level[4] * COMP_FILTER_FACTOR + (float)temp_targets[4] * (1.0 - COMP_FILTER_FACTOR));

    // if light reading is significantly lower than base level 
    if(temp_targets[6] < SLINGSHOT_THRESHOLD * base_level[6])
    {
      // increase score
      score += SLINGHSHOT_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[6] = temp_targets[6];

      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[6] = (adc_type)((float)base_level[6] * COMP_FILTER_FACTOR + (float)temp_targets[6] * (1.0 - COMP_FILTER_FACTOR));




    //  === Wheel target ===
    // if light reading is significantly higher than base level 
    if(temp_targets[7] < (WHEEL_THRESHOLD * base_level[7]))
    {
      // increase score
      score += WHEEL_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[7] = temp_targets[7];


      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[7] = (adc_type)((float)base_level[7] * COMP_FILTER_FACTOR + (float)temp_targets[7] * (1.0 - COMP_FILTER_FACTOR));

    if(temp_targets[8] < (WHEEL_THRESHOLD * base_level[8]))
    {
      // increase score
      score += WHEEL_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[8] = temp_targets[8];


      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[8] = (adc_type)((float)base_level[8] * COMP_FILTER_FACTOR + (float)temp_targets[8] * (1.0 - COMP_FILTER_FACTOR));

    if(temp_targets[9] < (WHEEL_THRESHOLD * base_level[9]))
    {
      // increase score
      score += WHEEL_POINTS;
      
      // adjust base_level to match current value, so no further trigger will be generated
      base_level[9] = temp_targets[9];

      // if not working: set a timer with last score given for this target
      // if difference greater than e.g. 300 ms: give score, set timer to current time
    }
    base_level[9] = (adc_type)((float)base_level[9] * COMP_FILTER_FACTOR + (float)temp_targets[9] * (1.0 - COMP_FILTER_FACTOR));


    DEBUG_PORT &= ~(1 << DEBUG_ADC);

    // start adc after fixed interval

    xTaskDelayUntil(&lastTick, 1000 / ADC_SCAN_RATE / portTICK_PERIOD_MS);
    
    DEBUG_PORT |= (1 << DEBUG_ADC);
    adc_start();
    DEBUG_PORT &= ~(1 << DEBUG_ADC);

  }
  
}

// ##############################################
// #              safety task                   #
// ##############################################
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
  relay_state = 1;
  const char wrn_high_c[] = "ERR: high current\n";
  const char wrn_high_t[] = "ERR: high temperature\n";
  const char wrn_timeout[] = "WRN: cannot get semaphore\n";

  vTaskDelay(600 / portTICK_PERIOD_MS);   // delay for 50 ticks to let the system start up

  while(1)
  {
    

    // wait for semaphore: if not given afetr 1sec: shutdown relay
    if(xSemaphoreTake(xSemaphore_safety, 1000 / portTICK_PERIOD_MS) == pdTRUE)
    {
      DEBUG_PORT |= (1 << DEBUG_SAFETY);

       // check states of channels

      // temperature
      if((temperature_l > TEMPERATURE_THRESHOLD) || (temperature_r > TEMPERATURE_THRESHOLD))
      {
        // warning high temperature!
        // shut down output stage
        // shut off relay
        RELAY_PORT &= ~(1 << RELAY_P);
        if(relay_state){
          
          print_debug(wrn_high_t);
          relay_state = 0;
        }
        
      }

      // current
      else {
        if((current_1 > CURRENT_THRESHOLD) || (current_2 > CURRENT_THRESHOLD) || (current_3 > CURRENT_THRESHOLD))
        {
          // warning high current
          // shut down output stage
          // shut off relay
          RELAY_PORT &= ~(1 << RELAY_P);
          if(relay_state)
          {
            print_debug(wrn_high_c);
            relay_state = 0;
          }
          
        }
        else
        {
          // nothing to do, everything fine
          relay_state = 1;
          RELAY_PORT |= (1 << RELAY_P);
          // uart_puts(DEBUG_UART, print_ok);
        }
      }

    }
    else{
      DEBUG_PORT |= (1 << DEBUG_SAFETY);

      // failed to get semaphore: adc sampling was not successful, something is wrong!

      // shut off relay
      RELAY_PORT &= ~(1 << RELAY_P);

      if(relay_state){
        print_debug(wrn_timeout);
        relay_state = 0;
      }
      
      
    }

    DEBUG_PORT &= ~(1 << DEBUG_SAFETY);
    wdt_reset();                          // reset watchdog, this was the last safe run

  }

}

// ##############################################
// #             music task                     #
// ##############################################
void music_task(void *param)
{

  uint8_t step = 0;
  TickType_t last = xTaskGetTickCount();
  tone_init();


  // as no speaker is connected currently (2021-11-29), this has no effect other than
  // generating a square wave with variable frequency :blobsad:

  while(1)
  {
    step++;

    // when reached the end of the song: play the same song again
    if(step == (sizeof(lead_notes)/(sizeof(float)))){   // sizeof didn't work, so here we go
      step = 0;
    }

    // function call to frequency generator
    tone_play((uint16_t)(lead_notes[step]));

    // calculate delay
    vTaskDelayUntil(&last, (TickType_t)((lead_times[step] / 2) * 1000 / portTICK_PERIOD_MS)); 


  }



}


// ##############################################
// #             ISR Routines                   #
// ##############################################
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


ISR(PCINT2_vect){
  // this is just a mirrow of ISR(PCINT1_vect), but no hardware debouncing 
  // is required, as none is expected from the RPI

  xSemaphoreGiveFromISR(xSemaphore_rpi_button, NULL);
}