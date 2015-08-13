/*--- functions to initialize, send, receive over USART
	  initUSART requires BAUD to be defined in order to calculate
      the bit-rate multiplier. ---*/


//#include <avr/io.h>

#include <avr/power.h>
#include <string.h>
#include <math.h>

#define F_CPU 8000000UL
#ifndef BAUD                          /* if not defined in Makefile... */
#define BAUD  9600                    /* set a safe default baud rate */
#endif


float Vrms, Irms, watts, vas, pf, freq, vars, angle, peakV;

/*--- These are defined for convenience ---*/
#define   USART_HAS_DATA   bit_is_set(UCSR0A, RXC0)
#define   USART_READY      bit_is_set(UCSR0A, UDRE0)


/*--- takes the defined BAUD and F_CPU,
      calculates the bit-clock multiplier,
      and configures the hardware USART ---*/
void initUSART(void);

/*--- blocking transmit and receive functions...
      when receiveByte() is called, program will hang until
      data comes through. improve on this later. ---*/

void transmitByte(uint8_t data);

uint8_t receiveByte(void);

void printString(const char myString[]);		// a utility function to transmit an entire string from ram
    
void readString(char myString[], uint8_t maxLength);	// define string variable, pass to function. 
														// string contains what ever was typed over serial.

void printByte(uint8_t byte);					// prints a byte out as its 3-digit ACII equivalent.
            
void printWord(uint16_t word);					// prints a word out as its 5-digit ASCII equivalent.
        
void printBinaryByte(uint8_t byte);				// prints a byte out in binary.
                                     
char nibbleToHex(uint8_t nibble);				// prints a nibble out in hex.

void printHexByte(uint8_t byte);				// prints a byte out in hex
                               
uint8_t getNumber(void);						// takes in 3 ASCII digits and converts them to a 
												// byte when enter is pressed.
												
void printFloat(float number);