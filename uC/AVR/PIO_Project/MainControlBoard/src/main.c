#include <Arduino.h>
#include <Arduino_FreeRTOS.h>


#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "uart.h"




// -------------------------------
//      global variables
// -------------------------------

char b[100];


// -------------------------------
//      global functions
// -------------------------------

void blink(void* param);
void init_task(void *param);
void draw_welcome(void);


// -------------------------------
//            setup
// -------------------------------
void setup() {
  DDRB |= (1 << PB7);
  PORTB &= ~(1 << PB7);

  xTaskCreate(
    init_task
    ,  "INIT" // A name just for humans
    ,  1024  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  
    
}
void loop() {;}  // nothing goes here, as all will be handeled in tasks.


void blink(void* param){

  DDRB |= (1 << 7);         // set led pin as output
  char s[] = "blink\n";

  TickType_t last = xTaskGetTickCount();    // needed or xTaskDelayUntil
  
  while(1)
  {

    PORTB ^= (1 << 7);
    uart_puts(0, s);
    uart_puts(1, b);

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
  uart_init(1, 57600);  

  draw_welcome(); 

  
  print_debug("create blink task... ");
  xTaskCreate(
    blink
    ,  "Blink" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle
  print_debug("OK\n");




  vTaskDelete(NULL);
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

  print_debug("========================================\n"); //_delay_ms(10);
//   sprintf(temp_buff, "========================================\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("||            Main Control            ||\n");//_delay_ms(10);
//   sprintf(temp_buff, "||            Main Control            ||\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("||               Board                ||\n");_delay_ms(10);
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("||------------------------------------||\n");_delay_ms(10);
//   sprintf(temp_buff, "||------------------------------------||\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("||        Firmware: 2021-11-07        ||\n");_delay_ms(10);
//   sprintf(temp_buff, "||        Firmware: 2021-11-07        ||\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("========================================\n");_delay_ms(10);
//   sprintf(temp_buff, "========================================\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("UART0: initialized\n");_delay_ms(10);
//   sprintf(temp_buff, "UART0: initialized\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
  print_debug("UART1: initialized\n");_delay_ms(10);
//   sprintf(temp_buff, "UART1: initialized\n");
//   while(uart_tx_buffer_state(DEBUG_UART) < strlen(temp_buff)) _delay_ms(10);
//   uart_puts(DEBUG_UART, temp_buff);
}