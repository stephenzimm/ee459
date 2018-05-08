/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>

// Serial init
void serial_init ( unsigned short ubrr ) {
	UBRR0 = ubrr ; // Set baud rate
	UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
	UCSR0B |= (1 << RXEN0 ); // Turn on receiver
	UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
	// one stop bit , 8 data bits
}

//serial_out - Output a byte to the USART0 port
void serial_out ( char ch )
{
	while (( UCSR0A & (1 << UDRE0 )) == 0);
	UDR0 = ch ;
}

//serial_in - Read a byte from the USART0 and return it
char serial_in ()
{
while ( !( UCSR0A & (1 << RXC0 )) );
PORTC |= 1 << PC0;
return UDR0 ;
}



int main(void)
{

	DDRC |= 1 << DDC1;
	DDRC |= 1 << DDC2;
	PORTC &= ~(1 << PC1);//Set the mux to 0 for screen
	PORTC &= ~(1 << PC2);
	//PORTC |= 1 << PC1;
	// Call init sequence
	serial_init(47);
	DDRC |= 1 << DDC0;
	// Forever loop

	serial_out(254);
	serial_out(81);
	_delay_ms(10);
	while(1)
	{


		int i = 0; //just to prevent error of Declaration after Statement, not allowed in C
		char word[] = "123456789";
		for(i = 0; word[i] != 0; i++)
		{
			serial_out(word[i]);
			_delay_ms(100);
		}

		_delay_ms(1000);
	}

    return 0;   /* never reached */
}
