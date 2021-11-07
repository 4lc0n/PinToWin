#include <Arduino.h>
#include <Arduino_FreeRTOS.h>


#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "uart.h"



char b[100];

void blink(void* param);

void setup() {


  uart_init(0, 57600);
  uart_init(1, 57600);  
  uart_putc(0, 'a');

  _delay_ms(100);

  uart_putc(0, 'b');

  
  sprintf(b, "the fox jumpes over the rope\n");
  uart_puts(0, b);
  
    xTaskCreate(
    blink
    ,  "Blink" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle


}

void loop() {
  // nothing goes here, as all will be handeled in tasks.
}

void blink(void* param){

  DDRB |= (1 << 7);

  char s[] = "blink\n";

  TickType_t last = xTaskGetTickCount();
  
  while(1)
  {

    PORTB ^= (1 << 7);
    uart_puts(0, s);
    uart_puts(1, b);

    xTaskDelayUntil(&last, 200 / portTICK_PERIOD_MS);



  }



}