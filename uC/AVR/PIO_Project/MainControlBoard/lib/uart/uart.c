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
#include <stdlib.h>

// ------------------------------------
//          private variables
// ------------------------------------

// buffer implementation: head will point at 0 at start, tail will point to last variable
// on write: check if head==tail --> full, else write
// on read: if tail + 1 == head --> empty, else read
typedef struct ring_buffer{
    char* buf;
    uint8_t head;
    uint8_t tail;
}rbuff;

rbuff uarttx[4], uartrx[4];




/**
 *  @brief init function for uart driver, allocates memory for transmit and receive function
 * 
 *  @param uart_if: select uart interface (uart 1 - 4)
 *  @param baudrate: select desired baudrate
 * 
 *  @return int: 0 on fail, 1 on success
 * 
 * */
uint8_t uart_init(uint8_t uart_if, uint32_t baudrate)
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
            ubrr = F_CPU / 16 / baudrate - 1;               // calcualte baud value
            
            UBRR1H = (uint8_t) (ubrr >> 8);
            UBRR1L = (uint8_t) (ubrr & 0xFF);               // set baud rate control register (high and low)

            UCSR1B = (1 << RXCIE1) | (1 << TXCIE1) | (1 << RXEN1) | (1 << TXEN1);           // activate tx and rx, receive interrupt and transmit complete interrupt

            UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);         // 1 stop bit, 8 data bit, no parity
            break;

        case 2: 
            ubrr = F_CPU / 16 / baudrate - 1;               // calcualte baud value
            
            UBRR2H = (uint8_t) (ubrr >> 8);
            UBRR2L = (uint8_t) (ubrr & 0xFF);               // set baud rate control register (high and low)

            UCSR2B = (1 << RXCIE2) | (1 << TXCIE2) | (1 << RXEN2) | (1 << TXEN2);           // activate tx and rx, receive interrupt and transmit complete interrupt

            UCSR2C = (1 << UCSZ21) | (1 << UCSZ20);         // 1 stop bit, 8 data bit, no parity
            break;
        
        case 3: 
            ubrr = F_CPU / 16 / baudrate - 1;               // calcualte baud value
            
            UBRR3H = (uint8_t) (ubrr >> 8);
            UBRR3L = (uint8_t) (ubrr & 0xFF);               // set baud rate control register (high and low)

            UCSR3B = (1 << RXCIE3) | (1 << TXCIE3) | (1 << RXEN3) | (1 << TXEN3);           // activate tx and rx, receive interrupt and transmit complete interrupt

            UCSR3C = (1 << UCSZ31) | (1 << UCSZ30);         // 1 stop bit, 8 data bit, no parity
            break;

        default:
            return 0;           // specified uart doesn_t exist: will fail in memory allocation
            break;
    }


    char* c = malloc(BUFFER_SIZE);  // allocate memory for buffer
    if(c != NULL){
        uarttx[uart_if].buf = c;    // store to pointer
        uarttx[uart_if].head = 0;   // set head to 0
        uarttx[uart_if].tail = BUFFER_SIZE - 1; // set tail to end of buffer

    }
    else{
        return 0;       // failed to allocate space for buffer
    }
    c = malloc(BUFFER_SIZE);
    if(c != NULL){
        uartrx[uart_if].buf = c;
        uartrx[uart_if].head = 0;
        uartrx[uart_if].tail = BUFFER_SIZE - 1;
        
    }
    else{
        return 0;       // failed to allocate space for buffer
    }   

    return 1;

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