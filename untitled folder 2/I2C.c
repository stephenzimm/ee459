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
#include <avr/interrupt.h>

int main(void)
{



  ///interuppt code
    DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input
    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    // PD2 is now an input with pull-up enabled
    EICRA	=	0x02; //TRIGGER ON FALLING EDGE
    //EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT0);     // Turns on INT1
    sei();

		# define FOSC 9830400 // Clock frequency = Oscillator freq .
		# define BDIV ( FOSC / 100000 - 16) / 2 + 1
		TWSR = 0; // Set prescalar for 1
		TWBR = BDIV ; // Set bit rate register

		i2c_init ( BDIV );
  //////
}
