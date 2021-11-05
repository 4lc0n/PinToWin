/**
 *  Library to use the UART interface of the ATMega2560 microcontroller
 *  use fprintf to cast any variable into a string
 * 
 *  runs all transmit and receive function in interrupt mode, non blocking
 *  transmit and receive via a buffer, allocated on uart_init for the specific uart interface
 * 
 * */


#include "uart.h"

#include <stdint.h>
#include <avr/io.h>

// ------------------------------------
//          private variables
// ------------------------------------

typedef struct ring_buffer{
    char* buf;
    uint8_t head;
    uint8_t tail;
}rbuff;

rbuff uart[4];




/**
 *  @brief init function for uart driver, allocates memory for transmit and receive function
 * 
 *  @param uart_if: select uart interface (uart 1 - 4)
 *  @param baudrate: select desired baudrate
 * 
 * */
void uart_init(uint8_t uart_if, uint32_t baudrate)
{
    uint16_t ubrr;

    switch(uart_if){

        case 0:

            
            ubrr = F_CPU / 16 / baudrate - 1;               // calcualte baud value
            
            UBRR0H = (uint8_t) (ubrr >> 8);
            UBRR0L = (uint8_t) (ubrr & 0xFF);               // set baud rate control register (high and low)

            UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);           // activate tx and rx, receive interrupt and transmit complete interrupt

            UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);         // 1 stop bit, 8 data bit, no parity

            break;


        case 1:

            break;

        case 2: 

            break;
        
        case 3: 

            break;



    }


}



/**
 *  @brief transmit a byte string via a selected uart port in interrupt mode
 *  
 *  @param uart_if: select uart interface (1 - 4)
 *  @param s: char array with zero termination
 * 
 * */
void uart_transmit(uint8_t uart_if, char* s);


/**
 * @brief read a char array until a newline sign, returns NULL on no data
 * 
 * @param uart_if: selects uart interface 
 * 
 * */
char* uart_getLine(uint8_t uart_if);


/**
 * @brief reads a char from uart, returns NULL on no data
 * 
 * @param uart_if: selects uart interface
 * 
 * */
char uart_getChar(uint8_t uart_if);


ISR(USART0_TX_vect){


}


ISR(USART0_RX_vect){


}