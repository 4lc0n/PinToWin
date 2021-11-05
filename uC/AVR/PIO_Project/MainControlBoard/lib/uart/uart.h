/**
 *  Library to use the UART interface of the ATMega2560 microcontroller
 *  use fprintf to cast any variable into a string
 * 
 *  runs all transmit and receive function in interrupt mode, non blocking
 *  transmit and receive via a buffer, allocated on uart_init for the specific uart interface
 * 
 * */


#ifndef UART_H
#define UART_H

#include <stdint.h>

#define BUFFER_SIZE 64



/**
 *  @brief init function for uart driver, allocates memory for transmit and receive function
 * 
 *  @param uart_if: select uart interface (uart 1 - 4)
 *  @param baudrate: select desired baudrate
 * 
 * */
void uart_init(uint8_t uart_if, uint32_t baudrate);



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







/**
 * 
 *  UART transmit and receive interrupts are declared here
 *  whith this, the driver runs in interrupt mode to always get called on a transmit or receive interrupt
 *  
 * 
 * 
 * */

#endif