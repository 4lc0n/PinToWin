#include <stdlib.h>
#include <avr/io.h>


int main(){

	DDRB |= (0x01 << 5);

	while(1)
	{
		PORTB ^= (1 << 5);
		for(int i = 0; i < 100000; i++)
			__asm("nop");

	}
}
