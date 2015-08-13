/******************************
 * MaximEnMonI2C.c
 * Created: 6/2/2015 4:15:39 PM
 * Author: zgoyetche
 ******************************/ 

#include "MaximEnMonI2C.h"
#include "i2c.h"
#include <math.h>
//#include "main.h"

float returnVal(uint8_t regLocation)
{
	uint8_t tempHighByte, tempMidByte, tempLowByte;
	float val;
	
	/*---Initiate start, send slave address, read, and register location.---*/		
	i2cStart();
	i2cSend(MAX_ADDRESS_W);
	i2cSend(regLocation);
	
	/*---To restart, just send start again.---*/
	i2cStart();    

	/*---Send address, with read bit.---*/
	i2cSend(MAX_ADDRESS_R);
		
	/*---Receive three bytes of data.---*/
	tempHighByte = i2cReadAck();
	tempMidByte = i2cReadAck();
	tempLowByte = i2cReadNoAck();
	
//	printString(" ");
//	printBinaryByte(tempHighByte);printString(" ");printBinaryByte(tempMidByte);printString(" ");printBinaryByte(tempLowByte);printString("  ");

	i2cStop();
	
	if(freq_flag == 1)
	{
		freq_flag = 0;
		val = convertBitsS16(tempHighByte, tempMidByte, tempLowByte); // S.16 for frequency
	}	
	else
		val = convertBitsS23(tempHighByte, tempMidByte, tempLowByte)*scaleFactor; // S.23 for the rest...
	return(val);
}

float convertBitsS23(uint8_t highByte, uint8_t midByte, uint8_t lowByte)
{
	uint8_t bit;
	float high = 0;
	float mid = 0;
	float low = 0;	
	int8_t i = -1;
	float val = 0;
	
	if(bit_is_set(highByte, 7)) // this handles the only bit to the left of the decimal
		val = -1;				// if bit is a 1, then = 1*(-2^0) 
	else
		val = 0;                // if bit is a 0, then = 0*(-2^0)
	
	for(bit = 6; bit < 255; bit--) // this handles the lower 7 bits of the most significant byte
	{
		if(bit_is_set(highByte, bit))
			high = high + pow(2,i);
		i--;
	}
	for(bit = 7; bit < 255; bit--) // this handles the entire middle byte.
	{
		if(bit_is_set(midByte, bit))
			mid = mid + pow(2,i);
		i--;
	}
	for(bit = 7; bit < 255; bit--) // this handles the entire lest significant byte.
	{
		if(bit_is_set(lowByte, bit))
			low = low + pow(2,i);
		i--;
	}
	val = high + mid + low; // sums up the fractional bits (lower 23 bits to right of binary point)
	return val;
}

float convertBitsS16(uint8_t highByte, uint8_t midByte, uint8_t lowByte)
{
	uint8_t bit;
	float high = highByte;
	float mid = 0;
	float low = 0;
	int8_t i = -1;
	float val = 0;

	for(bit = 7; bit < 255; bit--) // this handles the entire middle byte.
	{
		if(bit_is_set(midByte, bit))
		mid = mid + pow(2,i);
		i--;
	}
	for(bit = 7; bit < 255; bit--) // this handles the entire lest significant byte.
	{
		if(bit_is_set(lowByte, bit))
		low = low + pow(2,i);
		i--;
	}
	val = high + mid + low; // sums up the fractional bits (lower 23 bits to right of binary point)
	return val;
}