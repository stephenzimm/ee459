#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Adafruit_PN532.h>
#include <Wire.h>

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
// Pin2 == PD2
#define PN532_IRQ   (2) 
// Using PB6 as reset pin, but hardcoded library to use this since Uno uses it as AREF
  // Doesn't matter what number is passed to reset, not used internally
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Or use this line for a breakout or shield with an I2C connection:
// PC4/PC5 used for I2C SDA/SCLK
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Serial init
//int state = 0;
int state = 5; // Test case
int flag = 0;
int terminator = 99;
int sendMoneyFlag = 0;
volatile int key = 10;
int correct = 0;//correct MUST be 4 when the right numbers are pressed

volatile int lastKey = 10;
int initializationFlag = 0;

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

//serial_out_word - Output a whole string
void serial_out_word ( char ch[] )
{
      for(int i = 0; ch[i] != 0; i++)
      {
        serial_out(ch[i]);
      }
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

int main(void)
{
  int i;
  // Call init sequence
  serial_init(47);
  DDRC |= 1 << DDC0;
  clearscreen();

  ///interuppt code
    DDRD &= ~(1 << DDD3);     // Clear the PD3 pin
    // PD2 (PCINT0 pin) is now an input (NFC interrupt pin)    
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
  PORTC &= ~(1 << PC2);//Set the mux to 0 for screen

  //Send flag
  DDRC |= 1 << DDC5;

  //Set NFC Reset pin (PD4) as output
  DDRD |= 1 << DDD4;

  DDRC |= 1 << DDD0; //LED set for output
  
  // Start NFC
  clearscreen();
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    char notFound[] = "Didn't find PN53x board";
    serial_out_word(notFound);
    while (1); // halt
  }
  // Got ok data, print it out!
  char found[] = "Found chip PN5";
  //serial_out_word(found);
  _delay_ms(1000);
  clearscreen();
 
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  if (initializationFlag == 0)
  {
    state = 10;

  }
  if (initializationFlag == 1)
  {
    state = 0;
  }
  
while (1)
{
  while(1)
  {
    
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)

    char data1[16]; // First 16 hex characters
    char data2[16]; // Second 16 hex characters
    char data3[16]; // Last 8 hex characters
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    /*
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 1, 0, keya);
    uint8_t success1 = nfc.mifareclassic_ReadDataBlock(1, data1);
    if (success1) {serial_out_word("Read Block 1, ");}
    _delay_ms(500);
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 2, 0, keya);
    uint8_t success2 = nfc.mifareclassic_ReadDataBlock(2, data2);
    if (success2) {serial_out_word("Read Block 2, ");}
    _delay_ms(500);
    */
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 5, 0, keya);
    uint8_t success2 = nfc.mifareclassic_ReadDataBlock(4, data1);
    _delay_ms(50);
    if (success2) {serial_out_word("R/W Block 4, ");}
    //memcpy(data1, (const char[]){ ' ', 't', 'o', ' ', 'b', 'e', ' ', 'a', ' ', 'w', 'a', 'l', 'l', 'e', 't', ' ' }, sizeof data1);
    //uint8_t success3 = nfc.mifareclassic_WriteDataBlock(5, data1);
    uint8_t success3 = nfc.mifareclassic_ReadDataBlock(5, data2);
    if (success3) {serial_out_word("R/W Block 5, ");}
    _delay_ms(50);
    clearscreen();
    serial_out_word(data1);
    _delay_ms(10);
    serial_out_word(data2);
    //serial_out_word(data3);
    _delay_ms(2000);
    /*
    for (int i = 1; i < 4; i++)
    {
      clearscreen();
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
      if (uidLength == 4)
      {
        // We probably have a Mifare Classic card ... 
        char mifare[] = "Connected, try to authenticate...";
        //serial_out_word(mifare);
      
        // Now we need to try to authenticate it for read/write access
        // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
        
        success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, i, 0, keya);
        //success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 2, 0, keya);
        //success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 3, 0, keya);
      
        if (success)
        {
          char mifareGood[] = "Authenticated...";
          //serial_out_word(mifareGood);
          char prepare[] = "Prepare to write...";
          //serial_out_word(prepare);
          // Flash the LED to get the user's attention
          PORTC |= 1 << PC0;
          _delay_ms(500);
          PORTC &= 0 << PC0;
          clearscreen();
          
          
          // If you want to write something to block 4 to test with, uncomment
          // the following line and this text should be read back in a minute
          // Public wallet address is 40 hex characters, or 20 bytes
          /*
          memcpy(data1, (const char[]){ 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 's', 'u', 'p', 'p', 'o', 's', 'e', 'd' }, sizeof data1);
          uint8_t success1 = nfc.mifareclassic_WriteDataBlock (1, data1);
          _delay_ms(50);
          memcpy(data1, (const char[]){ ' ', 't', 'o', ' ', 'b', 'e', ' ', 'a', ' ', 'w', 'a', 'l', 'l', 'e', 't', ' ' }, sizeof data1);
          uint8_t success2 = nfc.mifareclassic_WriteDataBlock (2, data2);
          _delay_ms(50);
          memcpy(data3, (const char[]){ 'a', 'd', 'd', 'r', 'e', 's', 's', '.', ' ', 'C', 'r', 'a', 'z', 'y', '.', ' ' }, sizeof data3);
          uint8_t success3 = nfc.mifareclassic_WriteDataBlock (3, data3);
          _delay_ms(50);

          // Try to read the contents of block 4
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 1, 0, keya);
          uint8_t success1 = nfc.mifareclassic_ReadDataBlock(1, data1);
          _delay_ms(500);
          
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 2, 0, keya);
          uint8_t success2 = nfc.mifareclassic_ReadDataBlock(2, data2);
          _delay_ms(500);
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 3, 0, keya);
          uint8_t success3 = nfc.mifareclassic_ReadDataBlock(3, data3);
          _delay_ms(500);
          
    
          if (success1 && success2 && success3)
          {
            // Data seems to have been read ... spit it out
            char readingBlock[] = "Read/Write successful";
            serial_out_word(readingBlock);
            
            nfc.PrintHexChar(data1, 16);
            _delay_ms(50);
            nfc.PrintHexChar(data2, 16);
            _delay_ms(50);
            nfc.PrintHexChar(data3, 16);
            _delay_ms(50);
            
            
            clearscreen();
            serial_out_word(data1);
            serial_out_word(data2);
            serial_out_word(data3);
            
            
            
        
            // Wait a bit before reading the card again
            _delay_ms(2000);
          }
          
  
        } // End of if(success)
      } // End of if(uidLength == 4)
      */
    
    

}
}
    return 0;   /* never reached */
}



