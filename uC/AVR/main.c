#include <avr/io.h>
#include <util/delay.h>


int main(){

	DDRB |= (0x01 << 7);

	while(1)
	{
		PORTB ^= (1 << 7);
		_delay_ms(300);
	}	
}
