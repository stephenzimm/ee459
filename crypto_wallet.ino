#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Adafruit_PN532.h>
#include <Wire.h>

#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// PC4/PC5 used for I2C SDA/SCLK
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Serial init
//const char* first = "1";
char amountToSend[10];
//amountToSend = malloc(strlen(first)+1+6);
//strcpy(amountToSend, first);
//strcat(amountToSend, "2");


int state = 0;
int flag = 0;
char homeAddress[10];
char balance[5];
int sendMoneyFlag = 0;
volatile int key = 10;
int correct = 0;//correct MUST be 4 when the right numbers are pressed

//volatile char DestAddress[40];
volatile char data1[16]; // First 16 hex characters
volatile char data2[16]; // Second 16 hex characters
volatile char data3[16]; // Last 8 hex characters
volatile int lastKey = 10;
int initializationFlag = 0;
int passcodeKey1 = 0;
int passcodeKey2 = 2;
int passcodeKey3 = 7;
int passcodeKey4 = 9;

//state = 0 | ENTER PASSWORD
//state = 1 | HOME SCREEN
//state = 2 | SEND MONEY
//etc, add states as needed

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


void clearscreen()
{
  serial_out(254);
  serial_out(81);
  _delay_ms(10);
}
void passcode()
{
  if(key==passcodeKey1)
  {
    if (flag ==0)
      {
        correct++;
        flag = 1;
      }
    lastKey = key;
  }

  if(key == passcodeKey2)
  {
    if (lastKey == passcodeKey1)
      {
        correct++;

      }
    else if(lastKey == passcodeKey2)
    {
      //do nothing
    }
    else if(lastKey != passcodeKey1)
    {
      correct = 0;
    }
    lastKey = key;
    flag = 0;
  }


  if(key == passcodeKey3)
  {
    if (lastKey == passcodeKey2)
      {
        correct++;

    }
    else if(lastKey == passcodeKey3)
    {
      //do nothing
    }
    else if(lastKey !=passcodeKey2)
    {
      correct = 0;
    }
    lastKey = key;
    flag = 0;
  }


  if(key == passcodeKey4)
  {
    if (lastKey == passcodeKey3)
      {
        correct++;

    }
    else if(lastKey == passcodeKey4)
    {
      //do nothing
    }
    else if(lastKey !=passcodeKey3)
    {
      correct = 0;
    }
    lastKey = key;
    flag = 0;
  }

  else if ((key!=passcodeKey1) && (key!=passcodeKey2) && (key!=passcodeKey3) && (key!=passcodeKey4))
  {
    correct = 0;
    //serial_out(correct+48);
    flag = 0;
  }

}
int main(void)
{

  int welcomeFlag = 0;
  int i;
  // Call init sequence
  serial_init(47);
  DDRC |= 1 << DDC0;
  clearscreen();


  ///interuppt code
    DDRD &= ~(1 << DDD3);     // Clear the PD3 pin
    // PD2 (PCINT0 pin) is now an input
    PORTD |= (1 << PORTD3);    // turn On the Pull-up
    // PD3 is now an input with pull-up enabled
    EICRA = 0x02; //TRIGGER ON FALLING EDGE
    //EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT1);     // Turns on INT1
    sei();
  //////

  //Mux select pin set for output
  DDRC |= 1 << DDC1;
  DDRC |= 1 << DDC2;
  PORTC &= ~(1 << PC1);//Set the mux to 0 for screen
  PORTC &= ~(1 << PC2);

  //Send flag set for output
  DDRC |= 1 << DDC3;
  //Set check balance flag for output
  DDRD |= 1 << DDD4;

  // Set all keypad columns as outputs
  DDRB |= 1 << DDB0; // 1,4,7 Column
  DDRD |= 1 << DDD6; // 2,5,8 Column
  DDRD |= 1 << DDD7; // 3,6,9 Column

  PORTD &= ~(1 << PD7);
  PORTD &= ~(1 << PD6);
  PORTB &= ~(1 << PB0);

  DDRC |= 1 << DDD0; //LED set for output
  // Set all keypad rows as inputs
  DDRD &= ~(1 << DDD5); // 1,2,3 Row
  DDRB &= ~(1 << DDB7); // 4,5,6 Row
  DDRB &= ~(1 << DDB1); // 7,8,9 Row
  DDRB &= ~(1 << DDB2); // *,0,# Row
  // Pull-up row inputs
  PORTD |= (1<<PD5);
  PORTB |= (1<<PB7);
  PORTB |= (1<<PB1);
  PORTB |= (1<<PB2);


    if (initializationFlag == 0)
    {
      state = 20;

    }
    if (initializationFlag == 1)
    {
      state = 0;
    }

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    char notFound[] = "Didn't find PN53x board";
    for(i = 0; notFound[i] != 0; i++)
    {
      serial_out(notFound[i]);
    }
    while (1); // halt
  }
  _delay_ms(1000);

  // configure board to read RFID tags
  nfc.SAMConfig();
  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


while (1)
{
  while(1)
  {
  switch(state)
  {

      case 20:
      {
      // serial_out(254);
      // serial_out(81);
      // _delay_ms(10);
      // char word[] = "To begin, create your ";
      // for(i = 0; word[i] != 0; i++)
      // {
      //  serial_out(word[i]);
      // }
      // serial_out(0xFE);
      // serial_out(0x45);
      // serial_out(0x14);
      //
      // strcpy(word, "four digit passcode");
      // for(i = 0; word[i] != 0; i++)
      // {
      //  serial_out(word[i]);
      // }
      // _delay_ms(1500);

      serial_out(254);
      serial_out(81);
      _delay_ms(10);


      char word9[] = "Create Passcode now";
      for(i = 0; word9[i] != 0; i++)
      {
        serial_out(word9[i]);
      }

      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x14);


      char word10[] = "       ";
      for(i = 0; word10[i] != 0; i++)
      {
        serial_out(word10[i]);
      }

      serial_out(0x2D);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);

      while(key==10)
      {

      }
      passcodeKey1 = key;
      serial_out(passcodeKey1+48);
      _delay_ms(10);
      clearscreen();

      key=10;
      //strcpy(word9, "Enter the second key") ;
      for(i = 0; word9[i] != 0; i++)
      {
        serial_out(word9[i]);
      }
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x14);

      for(i = 0; word10[i] != 0; i++)
      {
        serial_out(word10[i]);
      }
      serial_out(passcodeKey1+48);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);

      while(key==10)
      {

      }
      passcodeKey2 = key;
      serial_out(passcodeKey2+48);
      _delay_ms(10);
      clearscreen();



      key=10;
      //strcpy(word9, "Enter the third key") ;
      for(i = 0; word9[i] != 0; i++)
      {
        serial_out(word9[i]);
      }
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x14);

      for(i = 0; word10[i] != 0; i++)
      {
        serial_out(word10[i]);
      }
      serial_out(passcodeKey1+48);
      _delay_ms(10);
      serial_out(passcodeKey2+48);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);

      while(key==10)
      {

      }
      passcodeKey3 = key;
      serial_out(passcodeKey3+48);
      _delay_ms(10);
      clearscreen();


      key=10;
      //strcpy(word9, "Enter the fourth key") ;
      for(i = 0; word9[i] != 0; i++)
      {
        serial_out(word9[i]);
      }
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x14);
      for(i = 0; word10[i] != 0; i++)
      {
        serial_out(word10[i]);
      }
      serial_out(passcodeKey1+48);
      _delay_ms(10);
      serial_out(passcodeKey2+48);
      _delay_ms(10);
      serial_out(passcodeKey3+48);
      _delay_ms(10);
      serial_out(0x2D);
      _delay_ms(10);
      while(key==10)
      {

      }
      passcodeKey4 = key;
      serial_out(passcodeKey4+48);
      _delay_ms(10);
      clearscreen();


      for(i = 0; word9[i] != 0; i++)
      {
        serial_out(word9[i]);
      }
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x14);

      for(i = 0; word10[i] != 0; i++)
      {
        serial_out(word10[i]);
      }
      serial_out(passcodeKey1+48);
      _delay_ms(10);
      serial_out(passcodeKey2+48);
      _delay_ms(10);
      serial_out(passcodeKey3+48);
      _delay_ms(10);
      serial_out(passcodeKey4+48);
      _delay_ms(10);

      _delay_ms(500);

      initializationFlag = 1;
      key = 10;
      state = 0;
      break;
      }



    case 0:
    {
      //prompt user to begin entering passcode
          i = 0; //just to prevent error of Declaration after Statement, not allowed in C
          clearscreen();
          char word[] = "Enter Passcode";
          for(i = 0; word[i] != 0; i++)
          {
            serial_out(word[i]);
          }
          //serial_out(word);
          _delay_ms(1);


     while (correct!=4)
          {
            _delay_ms(1);
            passcode();
          }


          for(i = 0; i < 5; i++)
          {
          PORTC |= 1 << PC0;
          _delay_ms(100);
          PORTC &= 0 << PC0;
          _delay_ms(100);
        }

          clearscreen();

        char word1[] = "Passcode Correct";
        for(i = 0; word1[i] != 0; i++)
        {
        serial_out(word1[i]);
        }
        _delay_ms(300);
        state = 1;
        break;

      }


      case 1: //CASE 1: HOME SCREEN.
      {
      //reset key to 0
      //clear screen
      clearscreen();
      key = 0;

      if (welcomeFlag ==0 )
      {
      char word2[] = "Welcome";
      for(i = 0; word2[i] != 0; i++)
      {
        serial_out(word2[i]);
      }
      _delay_ms(500);
      welcomeFlag = 1;
    }


      //CLEAR SCREEN HERE
      clearscreen();
      key = 0;


      char word[] = "Please select";
      char word1[] = "an option";
      for(i = 0; word[i] != 0; i++)
      {
        serial_out(word[i]);
      }

      //NEW LINE
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x40);

      for(i = 0; word1[i] != 0; i++)
      {
        serial_out(word1[i]);
      }

      _delay_ms(500);
      clearscreen();
      strcpy(word, "2: Send");
      strcpy(word1, "3: Recieve");

      for(i = 0; word[i] != 0; i++)
      {
        serial_out(word[i]);
      }

      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x40);

      for(i = 0; word1[i] != 0; i++)
      {
        serial_out(word1[i]);
      }
      strcpy(word, "4: Check Bal");
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x14);
      for(i = 0; word[i] != 0; i++)
      {
        serial_out(word[i]);
      }

      strcpy(word, "5: Lock");
      serial_out(0xFE);
      serial_out(0x45);
      serial_out(0x54);
      for(i = 0; word[i] != 0; i++)
      {
        serial_out(word[i]);
      }


      while (key == 0)
      {

      }
      _delay_ms(1);
      state = key;
      break;

    }



    case 2:
      {
        //Tell Rapberry Pi to be ready for dollar amount
        PORTC |= 1 << PC1;
        serial_out('s');
        _delay_ms(10);
        //serial_out('\n');
        //_delay_ms(10);
        PORTC &= ~(1 << PC1);
        clearscreen();
        char word2[] = "Send Money";
        for(i = 0; word2[i] != 0; i++)
        {
          serial_out(word2[i]);
        }
        _delay_ms(500);
        clearscreen();
        char prompt[] = "Tap for address.    ";
        for(i = 0; prompt[i] != 0; i++)
        {
          serial_out(prompt[i]);
        }

        uint8_t receivedAddr = 0;
        while (!(receivedAddr))
        {
          uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 5, 0, keya);
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 6, 0, keya);
          uint8_t success1 = nfc.mifareclassic_ReadDataBlock(4, data1);
          _delay_ms(50);
          uint8_t success2 = nfc.mifareclassic_ReadDataBlock(5, data2);
          _delay_ms(50);
          uint8_t success3 = nfc.mifareclassic_ReadDataBlock(6, data3);
          _delay_ms(50);
          if ((success1) && (success2) && (success3))
          {
            char received[] = "Received address.";
            // Print out to screen
            for(i = 0; received[i] != 0; i++)
            {
              serial_out(received[i]);
            }
            _delay_ms(500);
            clearscreen();
            _delay_ms(10);
            receivedAddr = 1;
          }
        }
        // Concatenate data blocks
        //strcat(DestAddress, data1);
        //strcat(DestAddress, data2);
        //strncat(DestAddress,data3,3);

        clearscreen();
        char word3[] = "Enter the amount";

        for(i = 0; word3[i] != 0; i++)
        {
          serial_out(word3[i]);
        }
        strcpy(word3, "you'd like to send");
        serial_out(0xFE);
        serial_out(0x45);
        serial_out(0x40);

        for(i = 0; word3[i] != 0; i++)
        {
          serial_out(word3[i]);
        }

        sendMoneyFlag = 1;
        //ISR routine takes car of sending to Pi

        serial_out(0xFE);
        serial_out(0x45);
        serial_out(0x54);

        while(key!=12)
        {
          //do nothing
        }



        



        _delay_ms(1000);
        clearscreen();
        strcpy(word3, "sending");
        for(i = 0; word3[i] != 0; i++)
        {
          serial_out(word3[i]);
        }
        for(i = 0; i< 6; i++)
        {
        serial_out(0x2E);
        _delay_ms(100);
        }



        clearscreen();
        strcpy(word3, "done");
        for(i = 0; word3[i] != 0; i++)
        {
          serial_out(word3[i]);
        }
        _delay_ms(1000);
        sendMoneyFlag = 0;
        state = 1;

        //Tell Rasberry Pi we are done

        break;
      }

      case 3:
      {
        clearscreen();
        char word10[] = "Receive Money";
        for(i = 0; word10[i] != 0; i++)
        {
          serial_out(word10[i]);
        }
        _delay_ms(500);
        clearscreen();
        strcpy(word10, "Your Address is: ");
        for(i = 0; word10[i] != 0; i++)
        {
          serial_out(word10[i]);
        }
        serial_out(0xFE);
        serial_out(0x45);
        serial_out(0x14);

        strcpy(word10, "2MyWPTpZPc7jGFow9");
        for(i = 0; word10[i] != 0; i++)
        {
          serial_out(word10[i]);
        }
        serial_out(0xFE);
        serial_out(0x45);
        serial_out(0x54);
        strcpy(word10, "JrFuww119L4Jx4vhpX");
        while(key!=12)
        {
          //####
        }
        state = 1;
        break;
      }
      case 4:
        {

          PORTC |= 1 << PC1;
          serial_out('c');
          _delay_ms(10);
          PORTC &= ~(1 << PC1);
          clearscreen();
          char word2[] = "Check Balance";
          for(i = 0; word2[i] != 0; i++)
          {
            serial_out(word2[i]);
          }
          _delay_ms(800);


          clearscreen();
          strcpy(word2, "Getting balance now");
          for(i = 0; word2[i] != 0; i++)
          {
            serial_out(word2[i]);
          }
          //Activate PC4

          //Change MUX
          PORTC |= 1 << PC1;
          int l = 0;
          for (l = 0; l < 10; l++)
          {
            //serial_out(serial_in());
            //_delay_ms(10);

            char input = serial_in();
            _delay_ms(10);
            serial_out(input);
            _delay_ms(10);
            char inputStr[] = " ";
            inputStr[0] = input;
            strcat(balance, inputStr);

          }

          /*
        //  while(l!=5)
        //  {
            //strcat(balance, serial_in());
            //strcat(balance,serial_in());
            serial_out(serial_in());
            _delay_ms(10);
            serial_out(serial_in());
            _delay_ms(10);
            serial_out(serial_in());
            _delay_ms(10);
            serial_out(serial_in());
            _delay_ms(10);
            serial_out(serial_in());
            _delay_ms(10);
            serial_out(serial_in());
            _delay_ms(10);
            serial_out(serial_in());
            _delay_ms(10);
            */
            //l++;
          //  _delay_ms(10);
        //  }

          PORTC &= ~(1 << PC1);




          clearscreen();
          for(i = 0; balance[i] != 0; i++)
          {
            serial_out(balance[i]);
          }
          //accept char array from serial buffer

          strcpy(word2, "  ETH");
          for(i = 0; word2[i] != 0; i++)
          {
            serial_out(word2[i]);
          }
          while(key!=12)
          {

          }
          memset(balance, 0, sizeof(balance));
          state = 1;
          break;
        }

        default:
        {
          clearscreen();
          char word2[] = "    Not a valid option";
          for(i = 0; word2[i] != 0; i++)
          {
            serial_out(word2[i]);
          }
          _delay_ms(2000);
          state = 1;
          break;
        }

        case 5:
        {
          correct = 0;
          state = 0;
          break;
        }

}
}
}
    return 0;   /* never reached */
}

/*
ISR(INT0_vect)
{
  //Routine for NFC
}
*/
ISR(INT1_vect)
{






  PORTD |= 1 << PD7;
  PORTD |= 1 << PD6;
  PORTB &= ~(1 << PB0);
  // Hold Column 1,4,7,* at 1
  if ((PIND & (1 << PD5)) == 0)  //1 is pressed
      {

        key = 1;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;

      }
    //record 1

  if ((PINB & (1 << PB7)) == 0)  //4 is pressed
      {

        key = 4;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;

      } //record 1
  if ((PINB & (1 << PB1)) == 0)  //7 is pressed
      {

        key = 7;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;


      } //record 7
  if ((PINB & (1 << PB2)) == 0)  //* is pressed
      {

        key = 11; //11 will be a period
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;


      } //record .



  PORTD &= ~(1 << PD7);
  PORTD |= 1 << PD6;
  PORTB |= 1 << PB0;
  if ((PIND & (1 << PD5)) == 0)//2 is pressed
      {

        key = 2;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;


      } //record 2
  if ((PINB & (1 << PB7)) == 0) //5 is pressed
        {

        key = 5;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;

      } //record 5
  if ((PINB & (1 << PB1)) == 0)//8 is pressed
      {

        key = 8;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;

      } //record 8
  if ((PINB & (1 << PB2)) == 0) //0 is pressed
      {

        key = 0;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;


      } //record 0


  PORTD |= 1 << PD7;
  PORTD &= ~(1 << PD6);
  PORTB |= 1 << PB0;
  if ((PIND & (1 << PB5)) == 0) //3 is pressed
      {

        key = 3;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;

      } //record 3
  if ((PINB & (1 << PB7)) == 0) //6 is pressed
        {

        key = 6;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;



      } //record 6
  if ((PINB & (1 << PB1)) == 0) //9 is pressed
      {
        key = 9;
        PORTC |= 1 << PC0;
        _delay_ms(90);
        PORTC &= 0 << PC0;


      } //record 9
      if ((PINB & (1 << PB2)) == 0) //# is pressed
          {
          key = 12;
          PORTC |= 1 << PC0;
          _delay_ms(90);
          PORTC &= 0 << PC0;


          } //record 9



      PORTD &= ~(1 << PD7);
      PORTD &= ~(1 << PD6);
      PORTB &= ~(1 << PB0);


      if(sendMoneyFlag==1)
      {

        if(key == 11)
        {
          _delay_ms(10);
          strcat(amountToSend,".");
          _delay_ms(10);
          key = 13;
          serial_out(0x2E);
        }



        if(key==12)
        {
          //print nothing on #
        }
        if((key!=12) && (key!= 11) && (key!=13))
        {
         serial_out(key+48);
        }
        //delay is to ensure anything outputting on serial has finished
        _delay_ms(10);





        if((key!=12) && (key!= 11) && (key!=13))
        {
        //change MUX
        //PORTC |= 1 << PC2;
        PORTC |= 1 << PC1;
        //mux select is now 11
        //send char to PI
        //serial_out(key+48);
        char keyChar = key + 48;
        char keyStr[] = " ";
        keyStr[0] = keyChar;
        strcat(amountToSend, keyStr);
        _delay_ms(10);
        //Change MUX back
        PORTC &= ~(1 << PC2);
        PORTC &= ~(1 << PC1);
        //00
        }


        if(key==12)
        {
          //char DestAddress[] = "2MyWPTpZPc7jGFow9JrFuww119L4Jx4vhpX" ;
          //ensure nothing is on the serial_in
          _delay_ms(10);
          //PORTC |= 1 << PC2;
          PORTC |= 1 << PC1;
          //send terminator to PI
          //serial_out(0x21);
          strcat(amountToSend,"\n");
          _delay_ms(10);
          //Change MUX back
          int i;


          for(i = 0; amountToSend[i] != 0; i++)
          {
            serial_out(amountToSend[i]);
          }

          _delay_ms(100);

          /*
          for(i = 0; DestAddress[i] != 0; i++)
          {
            serial_out(DestAddress[i]);
          }
          */
          for (i = 0; i < 16; i++)
          {
            serial_out(data1[i]);
          }
          _delay_ms(50);
          for (i = 0; i < 16; i++)
          {
            serial_out(data2[i]);
          }
          _delay_ms(50);
          for (i = 0; i < 3; i++)
          {
            serial_out(data3[i]);
          }
          _delay_ms(50);
          /*
          for(i = 0; i < 35; i++)
          {
            serial_out(DestAddress[i]);
          }
          */
          _delay_ms(10);
          serial_out('\n');
          _delay_ms(10);
          PORTC &= ~(1 << PC2);
          PORTC &= ~(1 << PC1);

          memset(amountToSend,0,sizeof(amountToSend));
        }

      }

}

