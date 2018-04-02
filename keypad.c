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
int key = 0;
int state = 0;
//state = 0 | ENTER PASSWORD
//state = 1 | HOME SCREEN
//state = 2 | SEND MONEY
//etc, add states as needed
int keypadScan()
  {
    int key;
    PORTD |= 1 << PD7;
    PORTD |= 1 << PD6;
    PORTB &= ~(1 << PB0);
    // Hold Column 1,4,7,* at 1
    if ((PIND & (1 << PD5)) == 0)  //1 is pressed
        {
          key = 1;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;

        }

      //record 1

    if ((PINB & (1 << PB7)) == 0)  //4 is pressed
        {
          key = 4;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;

        } //record 1
    if ((PINB & (1 << PB1)) == 0)  //7 is pressed
        {
          key = 7;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;


        } //record 7



    PORTD &= ~(1 << PD7);
    PORTD |= 1 << PD6;
    PORTB |= 1 << PB0;
  if ((PIND & (1 << PD5)) == 0)//2 is pressed
        {
          key = 2;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;


        } //record 2
    if ((PINB & (1 << PB7)) == 0) //5 is pressed
          {
          key = 5;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;

        } //record 5
    if ((PINB & (1 << PB1)) == 0)//8 is pressed
        {
          key = 8;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;

        } //record 8
    if ((PINB & (1 << PB2)) == 0) //0 is pressed
        {
          key = 0;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;


        } //record 0


    PORTD |= 1 << PD7;
    PORTD &= ~(1 << PD6);
    PORTB |= 1 << PB0;
  if ((PIND & (1 << PB5)) == 0) //3 is pressed
        {
          key = 3;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;

        } //record 3
    if ((PINB & (1 << PB7)) == 0) //6 is pressed
          {
          key = 6;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;



        } //record 6
    if ((PINB & (1 << PB1)) == 0) //9 is pressed
        {
          key = 9;
          PORTC |= 1 << PC0;
          _delay_ms(200);
          PORTC &= 0 << PC0;


        } //record 9
}
int main(void)
{
  int lastKey;
  int correct;//correct MUST be 4 when the right numbers are pressed
  //ENABLE INTERRUPTS TO CALL keypadScan()
 	int passcode[4] = {0 ,0 ,0 ,0};
	//Putting voltage on these for Output
	DDRB |= 1 << DDB0; //Column of 1 4 7 *
	DDRD |= 1 << DDD7; //Column of 2 5 8 0
	DDRD |= 1 << DDD6; //Column of 3 6 9 #
	DDRC |= 1 << DDD0; //LED set for output
  //pull up resistor
  PORTD |= (1<<PD5);
  PORTB |= (1<<PB1);
  PORTB |= (1<<PB2);
  PORTB |= (1<<PB7);

while (1)
{
  switch(state)
  {
    case 0:
      //prompt user to begin entering passcode
      switch(key) // the passcode is 1  5  9  0 then # to enter. Any wrong key tap will
                  //erase and force the user to begin again
      {
        case 0:
        if (lastKey == 9)
          {
            correct++;
          }
        else
        {
          correct = 0;
        }
        lastKey = 0;


        case 1:
        if (lastKey == 0)
          {
            correct++;
          }
        else
        {
          correct = 0;
        }
        lastKey = 1;

        case 2:
        correct = 0;
        lastKey = 2;

        case 3:
        correct = 0;
        lastKey = 3;
      }
  }
}


}


    return 0;   /* never reached */
}
