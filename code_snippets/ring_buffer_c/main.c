#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define BUFFER_SIZE 64

typedef struct ring_buffer{
    char* buf;
    uint8_t head;
    uint8_t tail;
    uint8_t initialized;
    uint8_t transmitting;
}rbuff;


rbuff uarttx[1];
uint8_t uart_if = 0;

// returns free buffer size
// free = tail - head
// if tail < head: tail + BUFFER_SIZE - head
uint8_t rbuffer_free(uint8_t uart_if){
    return (uarttx[uart_if].tail < uarttx[uart_if].head) ? (uarttx[uart_if].tail + BUFFER_SIZE - uarttx[uart_if].head) : (uarttx[uart_if].tail - uarttx[uart_if].head);

}

// writes, to head pointer, increases afterwards to empty space
uint8_t rbuffer_write(uint8_t uart_if, char c){
    // check if not full
    if(uarttx[uart_if].head  != uarttx[uart_if].tail)        // check if buffer is full
    {
        uarttx[uart_if].buf[uarttx[uart_if].head] = c;      // put into buffer                       // store to buffer
        uarttx[uart_if].head = (uarttx[uart_if].head + 1) % BUFFER_SIZE;        // increase head pointer  // increase head index
    }
    else{
        return 0;
    }
    return 1;

}
// reads from tail+1, returns 0 if tail+1 == head
char rbuffer_read(uint8_t uart_if){
    if((uarttx[uart_if].tail + 1) % BUFFER_SIZE != uarttx[uart_if].head){        // check if buffer is not empty
        char c = uarttx[uart_if].buf[(uarttx[uart_if].tail + 1) % BUFFER_SIZE];
        uarttx[uart_if].tail = (uarttx[uart_if].tail + 1) % BUFFER_SIZE;
        return c;
    }

    return 0;


}

int main()
{


    // initialize ring buffer
    char* c = (char*)malloc(BUFFER_SIZE);  // allocate memory for buffer
    if(c != NULL){
        uarttx[uart_if].buf = c;    // store to pointer
        uarttx[uart_if].head = 0;   // set head to 0
        uarttx[uart_if].tail = BUFFER_SIZE - 1; // set tail to end of buffer

    }
    else{
        printf("failed to allocate buffer\n");
        return 0;       // failed to allocate space for buffer
    }


    printf("free: %d\n", rbuffer_free(uart_if));
    printf("putting 'a' into buffer\n");
    rbuffer_write(uart_if, 'a');

    printf("free: %d\n", rbuffer_free(uart_if));
    printf("zero_terminating...\n");
    rbuffer_write(uart_if, '\0');

    printf("free: %d\n", rbuffer_free(uart_if));
    printf("Buffer data: %s\n", uarttx[uart_if].buf);

    printf("Read buffer: %c\n", rbuffer_read(uart_if));
    printf("Buffer size: %d\n", rbuffer_free(uart_if));

    int ret = 0;
    for(int i = 0; i < 5; i++){
        
        // printf("putting 'a' into buffer\n");
        ret = rbuffer_write(uart_if, 'a');
        printf("ret: %d\tfree: %d\n", ret, rbuffer_free(uart_if));
    }
    for(int i = 0; i < 5; i++){
        
        // printf("putting 'a' into buffer\n");
        ret = rbuffer_read(uart_if);
        printf("ret: %d\tfree: %d\n", ret, rbuffer_free(uart_if));
    }

    for(int i = 0; i < 64; i++){
        
        // printf("putting 'a' into buffer\n");
        ret = rbuffer_write(uart_if, 'a');
        printf("ret: %d\tfree: %d\n", ret, rbuffer_free(uart_if));
    }
    for(int i = 0; i < 64; i++){
        
        // printf("putting 'a' into buffer\n");
        ret = rbuffer_read(uart_if);
        printf("ret: %d\tfree: %d\n", ret, rbuffer_free(uart_if));
    }
    printf("head: %d, tail: %d\n", uarttx[uart_if].head, uarttx[uart_if].tail);

    free(uarttx[uart_if].buf);
    return 0;
}