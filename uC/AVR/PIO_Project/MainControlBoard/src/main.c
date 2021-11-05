#include <Arduino.h>
#include <Arduino_FreeRTOS.h>


#include <avr/io.h>
#include <util/delay.h>



#include "uart.h"





void blink(void* param);

void setup() {


  uart_init(0, 9600);

  
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

  while(1)
  {

    PORTB ^= (1 << 7);

    vTaskDelay(200 / portTICK_PERIOD_MS);



  }



}