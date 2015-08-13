
/*---
  quick and dirty functions that make serial communications work.

  note that receiveByte() blocks -- it sits and waits _forever_ for
  a byte to come in.  to do anything that's more interesting,
  implement this with interrupts.

  initUSART requires BAUDRATE to be defined in order to calculate
  the bit-rate multiplier.  9600 is default.
---*/

#include "USART.h"
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "MaximEnMonI2C.h"



void initUSART(void) 
{													  /* requires BAUD */
  UBRR0H = UBRRH_VALUE;                        /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;	//these are the upper and lower bytes of the buadrate
#if USE_2X
  UCSR0A |= (1 << U2X0); //2x feature enabled
#else
  UCSR0A &= ~(1 << U2X0); //2x feature disabled
#endif
                                  /* Enable USART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit */
  
  UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
  sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

uint8_t strIndex = 0; //declare these outside of the ISR so that they do not get over written each time ISR is called
char ReceivedString[5]; //declare these outside of the ISR so that they do not get over written each time ISR is called

ISR(USART_RX_vect)
{
	char ReceivedByte; // a byte available to store incoming bytes from serial RX
	
	ReceivedByte = UDR0; // Fetch the received byte value into the variable "ByteReceived"
	//UDR0 = ReceivedByte; // Echo back the received byte back to the computer
	if(ReceivedByte == 13)
	{
		//printString("New Line Detected!\n\r");
		//printString(ReceivedString); //echo back received string once a new line character (13) is received
		//printString("\n\r");
		if(ReceivedString[4] == 'v')
		{
			//printString("measv works!");
			//printString("\n\r");
			printFloat(Vrms);
		}
			
		else if(ReceivedString[4] == 'i')
		{
			//printString("measi works!");
			//printString("\n\r");
			printFloat(Irms);
		}
			
		else if(ReceivedString[4] == 'p')
		{
			//printString("measp works!");
			//printString("\n\r");
			printFloat(watts);
		}
			
		printString("\n");
		memset(ReceivedString,0,strlen(ReceivedString)); //zero out the string after it has been printed
		strIndex = 0; //set the string index back to zero for future strings
	}
	else
	{
		ReceivedString[strIndex] = ReceivedByte; //store received byte into string
		strIndex++; //increment string index counter
		//printString("Recieved Byte!\n\r");
		//printByte(strIndex);
		//printString("\n\r");
	}
}

void transmitByte(uint8_t data) 
{
                                     /* Wait for empty transmit buffer */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data;                                            /* send data */
}

uint8_t receiveByte(void) 
{
  loop_until_bit_is_set(UCSR0A, RXC0);       /* Wait for incoming data */
  return UDR0;                                /* return register value */
}
                       
// a bunch of useful printing commands
void printString(const char myString[]) 
{
  uint8_t i = 0;
  while (myString[i]) 
  {
    transmitByte(myString[i]);
    i++;
  }
}

void readString(char myString[], uint8_t maxLength) 
{
  char response;
  uint8_t i;
  i = 0;
  while (i < (maxLength - 1)) 
  {                   /* prevent over-runs */
    response = receiveByte();
    transmitByte(response);                                    /* echo */
    if (response == '\r') 
	{                     /* enter marks the end */
      break;
    }
    else 
	{
      myString[i] = response;                       /* add in a letter */
      i++;
    }
  }
  myString[i] = 0;                          /* terminal NULL character */
}

/*--- Converts a byte to a string of decimal text, sends it ---*/
void printByte(uint8_t byte) 
{             
  transmitByte('0' + (byte / 100));                        /* Hundreds */
  transmitByte('0' + ((byte / 10) % 10));                      /* Tens */
  transmitByte('0' + (byte % 10));                             /* Ones */
}

/*--- Converts a word to a string of decimal text, sends it ---*/
void printWord(uint16_t word) 
{
  transmitByte('0' + (word / 10000));                 /* Ten-thousands */
  transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
  transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
  transmitByte('0' + ((word / 10) % 10));                      /* Tens */
  transmitByte('0' + (word % 10));                             /* Ones */
}

/*--- Prints out a byte as a series of 1's and 0's ---*/
void printBinaryByte(uint8_t byte) 
{                      
  uint8_t bit;
  for (bit = 7; bit < 255; bit--) 
  {
    if (bit_is_set(byte, bit))
      transmitByte('1');
    else
      transmitByte('0');
  }
}

/*--- Converts 4 bits into hexadecimal ---*/
char nibbleToHexCharacter(uint8_t nibble) 
{                                 
  if (nibble < 10) 
  {
    return ('0' + nibble);
  }
  else 
  {
    return ('A' + nibble - 10);
  }
}

/*--- Prints a byte as its hexadecimal equivalent ---*/
void printHexByte(uint8_t byte) 
{                       
  uint8_t nibble;
  nibble = (byte & 0b11110000) >> 4;
  transmitByte(nibbleToHexCharacter(nibble));
  nibble = byte & 0b00001111;
  transmitByte(nibbleToHexCharacter(nibble));
}

/*-- Gets a numerical 0-255 from the serial port.
     Converts from string to number. ---*/
uint8_t getNumber(void) 
{
  char hundreds = '0';
  char tens = '0';
  char ones = '0';
  char thisChar = '0';
  do /* shift over */
  {      
    hundreds = tens;
    tens = ones;
    ones = thisChar;
    thisChar = receiveByte(); /* get a new character */
    transmitByte(thisChar); /* echo */
  } 
  while (thisChar != '\r'); /* until type return */
  return (100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
}

void printFloat(float number)
{
	number = round(number * 100) / 100; /* round off to 2 decimal places */
	if(number >= 900)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 900)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 800)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 800)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 700)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 700)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 600)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 600)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 500)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 500)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 400)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 400)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 300)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 300)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 200)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 200)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 100)
	{
		transmitByte('0' + number / 100);                            /* hundreds place */
		transmitByte('0' + (number - 100)/10);                       /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 10)
	{
		transmitByte('0' + number / 10);                             /* tens place */
		transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number < 10 && number >= 0)
	{
		transmitByte('0' + number);									 /* ones */
	}
	else if(number < 0)
	{
		transmitByte('-');
		transmitByte('0' + number);									 /* ones */
	}
	else
	transmitByte('0');
	transmitByte('.');
	transmitByte('0' + (number * 10) - floor(number) * 10);          /* tenths */
	transmitByte('0' + (number * 100) - floor(number * 10) * 10);    /* hundredths place */
	printString("   "); //clear out left over decimals from larger previous number.
}