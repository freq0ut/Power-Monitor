// Functions for i2c communication
#include <avr/io.h>
//#include "pinDefines.h"

void initI2C(void);				// sets pull-ups and initializes bus speed to 100kHz (at FCPU=8MHz)
    
void i2cWaitForComplete(void);	// waits until the hardware sets the TWINT flag
                       
void i2cStart(void);			// sends a start condition (sets TWSTA)
                              
void i2cStop(void);				// sends a stop condition (sets TWSTO)
                                
void i2cSend(uint8_t data);		// loads data, sends it out, waiting for completion
                   
uint8_t i2cReadAck(void);		// read in from slave, sending ACK when done (sets TWEA)
              
uint8_t i2cReadNoAck(void);		// read in from slave, sending NOACK when done (no TWEA)