/**
 *  Library to use the UART interface of the ATMega2560 microcontroller
 *  use fprintf to cast any variable into a string
 * 
 *  runs all transmit and receive function in interrupt mode, non blocking
 *  transmit and receive via a buffer, allocated on uart_init for the specific uart interface
 * 
 * 
 *  IMPLEMENTED YET: TRANSMIT
 * 
 * */


#include "uart.h"

#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <avr/interrupt.h>
#include <task.h>

#include "ringbuffer.h"

// ------------------------------------
//          private variables
// ------------------------------------

// buffer implementation: head will point at 0 at start, tail will point to last variable. actual buffer size = BUFFER_SIZE - 1
// on write: check if ( head + 1 )==tail --> full, else write
// on read: if tail + 1 == head --> empty, else read


rbuff uarttx[4] = {{0, 0, 0, 0, 0},{0, 0, 0, 0, 0},{0, 0, 0, 0, 0},{0, 0, 0, 0, 0}}, uartrx[4] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};




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

    if(uart_if > 3){
        return 0;                         // uart > 3 doesn't exist!
    }
    
    // check if already initialized, otherwise memory will be allocated multible times
    if((uarttx[uart_if].initialized == 1) | (uartrx[uart_if].initialized == 1)){
        return 1;           // already initialized
    }

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
        rbuffer_init(&uarttx[uart_if], c);
    }
    else{
        return 0;       // failed to allocate space for buffer
    }
    c = malloc(BUFFER_SIZE);
    if(c != NULL){
        rbuffer_init(&uartrx[uart_if], c);
        
    }
    else{
        free(uarttx[uart_if].buf);
        return 0;       // failed to allocate space for buffer
    }   

    
    return 1;

}


/**
 *  @brief function to return available space in transmit buffer
 * 
 *  @param uart_if: select uart interface (0 - 3)
 *  
 *  @return   size in byte
 * */
volatile uint8_t uart_tx_buffer_state(uint8_t uart_if)
{
    if(uart_if > 3){
        return 0;                         // uart > 3 doesn't exist!
    }
    

    // buffer size - ( head - (tail + 1) ), as if tail + 1 == head --> empty            // -1, as one item always need to be in between head and tail
    return rbuffer_free(&(uarttx[uart_if]));
}

/**
 *  @brief transmit a byte string via a selected uart port in interrupt mode
 *  
 *  @param uart_if: select uart interface (1 - 4)
 *  @param s: char array with zero termination
 * 
 * */
void uart_puts(uint8_t uart_if, char* s)
{

    if(uart_if > 3){
        return;                         // uart > 3 doesn't exist!
    }
    
    uint8_t n = 0;          // variable to keep track on bytes, so it desn't exceed BUFFER_SIZE (safety feature)
    
    while(*s != '\0'){
        uart_putc(uart_if, *s);
        n++; 
        s++;

        if(n > BUFFER_SIZE)
            return;
    }

}


/**
 *  @brief transmit a byte string via a selected uart port in interrupt mode. Toes not check if it fits in tx buffer. if not, will trunctated.
 *  
 *  @param uart_if: select uart interface (0 - 3)
 *  @param c: char
 *  
 * */
void uart_putc(uint8_t uart_if, char c)
{
    if(uart_if > 3){
        return;                         // uart > 3 doesn't exist!
    }
    if(uarttx[uart_if].initialized == 0){    // check if already initialized
        uart_init(uart_if, DEFAULT_BAUD);
    }

    else{                                    // check if already initialized
        if(uarttx[uart_if].transmitting == 0)   // if not transmitting
        {
            uarttx[uart_if].transmitting = 1;
            switch(uart_if){                // feed directly into data register
                case 0:
                    UDR0 = c;
                    break;
                case 1:
                    UDR1 = c;
                    break;
                case 2: 
                    UDR2 = c;
                    break;
                case 3: 
                    UDR3 = c;
                    break;
                default:
                    return;         
            }
        }                                   // if not transmitting

        else{
            rbuffer_write(&uarttx[uart_if], c);
        }

    }   // check if already initialized
}





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
    char c = rbuffer_read(&uarttx[0]);      // read from buffer ( is approx. same speed as checking size + on not empty is faster than first checking )
    if(c == 0){                             // if returns 0 (empty)
        uarttx[0].transmitting = 0;         // stop transmitting flag
        return; 
    }

    UDR0 = c;                               // else add new data


}


ISR(USART0_RX_vect){
    ;

}
ISR(USART1_TX_vect){
    char c = rbuffer_read(&uarttx[1]);      // read from buffer ( is approx. same speed as checking size + on not empty is faster than first checking )
    if(c == 0){                             // if returns 0 (empty)
        uarttx[1].transmitting = 0;         // stop transmitting flag
        return; 
    }

    UDR1 = c;                               // else add new data


}


ISR(USART1_RX_vect){
    ;

}

ISR(USART2_TX_vect){
    char c = rbuffer_read(&uarttx[2]);      // read from buffer ( is approx. same speed as checking size + on not empty is faster than first checking )
    if(c == 0){                             // if returns 0 (empty)
        uarttx[2].transmitting = 0;         // stop transmitting flag
        return; 
    }

    UDR2 = c;                               // else add new data


}


ISR(USART2_RX_vect){
    ;

}

ISR(USART3_TX_vect){
    char c = rbuffer_read(&uarttx[3]);      // read from buffer ( is approx. same speed as checking size + on not empty is faster than first checking )
    if(c == 0){                             // if returns 0 (empty)
        uarttx[3].transmitting = 0;         // stop transmitting flag
        return; 
    }

    UDR3 = c;                               // else add new data


}


ISR(USART3_RX_vect){
    ;

}