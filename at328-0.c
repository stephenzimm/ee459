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

int main(void)
{
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output

    while (1) {
	PORTC |= 1 << PC0;      // Set PC0 to a 1
	_delay_ms(250);
	PORTC &= ~(1 << PC0);   // Set PC0 to a 0
	_delay_ms(250);
    }

    return 0;   /* never reached */
}




/*
//turn on Row 2, Row 3, turn off row 1
PORTD |= 1 << PD7;
PORTD |= 1 << PD6;
PORTB &= ~(1 << PB0);//PORTB |= 1 << PB0;// Hold Column 1,4,7,* at 1
if ((PIND & (1 << PD5)) == 0) //1 is pressed, voltage is shorted and goes low
    {
      passcode[i] = 1;
      _delay_ms(5);
      PORTC |= 1 << PC0;      // Set PC0 to a 1
      //_delay_ms(250);

    }
    else
    {
      PORTC &= ~(1 << PC0);   // Set PC0 to a 0
      //_delay_ms(250);

    } //record 1

  if ((PINB & (1 << PB7)) == 0) //4 is pressed
       {
        passcode[i] = 4;
        _delay_ms(5);
        } //record 1
*/
