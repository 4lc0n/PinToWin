/**
 *  Header for Ring Buffer implementation as a struct 
 * 
 *  available size = BUFFER_SIZE - 1
 * 
 * 
 * 
 * */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>


#define BUFFER_SIZE 64


typedef struct ring_buffer{
    char* buf;
    uint8_t head;
    uint8_t tail;
    uint8_t initialized;
    uint8_t transmitting;
}rbuff;



/** 
 * 
 * 
 * 
 * */
void rbuffer_init(rbuff *b, char* mem)
{
    b->buf = mem;
    b->head = 0;
    b->tail = BUFFER_SIZE - 1;
    b->initialized = 1;
    b->transmitting = 0;
}


/**
 *  @brief returns free buffer size
 *  
 *  @param b ringbuffer structure pointer
 * 
 *  @return free space in bytes
 * 
 * */
uint8_t rbuffer_free(rbuff *b){
    uint8_t state;
    
    cli();      // disable interrupt, as uart may interfere
    state = (b->tail < b->head) ? (b->tail + BUFFER_SIZE - b->head) : (b->tail - b->head);
    sei();      // reactivating

    return state;
}

/**
 *  @brief writes, to head pointer, increases afterwards to empty space
 * 
 *  @param b: ring buffer structure pointer
 *  @param c: char to write to
 * 
 *  @return 1 on success, 0 on fail
 * */
uint8_t rbuffer_write(rbuff *b, char c){
    // check if not full
    cli();      // disable interrupt, as uart may interfere
    if(b->head  != b->tail)        // check if buffer is full
    {
        
        b->buf[b->head] = c;      // put into buffer                       // store to buffer
        b->head = (b->head + 1) % BUFFER_SIZE;        // increase head pointer  // increase head index
        sei();      // reactivating
    }
    else{
        sei();      // reactivating
        return 0;
    }
    return 1;

}
/**
 *  @brief reads from tail+1, returns 0 if tail+1 == head
 *  
 *  @param b buffer structure
 * 
 *  @return char from buffer
 * */
char rbuffer_read(rbuff *b){
    char c = 0;
    uint8_t mSREG = SREG;       // store SREG for interrupt restoration, if disabled (call from ISR, do not reactivate)

    cli();      // disable interrupt, as uart may interfere
    if(((b->tail + 1) % BUFFER_SIZE) != b->head){        // check if buffer is not empty
        b->tail = (b->tail + 1) % BUFFER_SIZE;
        c = b->buf[b->tail];
    }
    // restore interrupt flag (nested interrupt is not wanted here)
    SREG = mSREG;
    return c;
}




#endif