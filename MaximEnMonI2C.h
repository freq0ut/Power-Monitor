/*
 * MaximEnMonI2C.h
 * Created: 6/8/2015 1:21:37 PM
 *  Author: zgoyetche
 */ 

#ifndef MAXIMENMONI2C_H_
#define MAXIMENMONI2C_H_

#include <avr/io.h>

/*---REGISTER DEFINITIONS---*/
#define MAX_ADDRESS_W                0x00 // (Write to MAX78615) S.23
#define MAX_ADDRESS_R                0x01 // (Read from MAX78615) S.23
#define IA_RMS                       0x3E // (RMS Current for IA source) S.23
#define VA_RMS                       0x2B // (RMS Voltage for VA source) S.23
#define FREQ                         0x6D // (Line Frequency) S.16
#define WATT_A                       0x4B // (True Power for source A) S.23
#define VAR_A                        0x51 // (Reactive Power for source A) S.23
#define PFA                          0x65 // (Source A Power Factor) S.23
#define VA_A                         0x4E // (Volt-Amperes for source A) S.23
#define VFSCALE                      0x83 // (Voltage Scale Register) S.23
#define IFSCALE                      0x84 // (Current Scale Register) S.23
#define VA_PEAK                      0x3A // (Instantaneous Voltage A) S.23
#define SAMPLES                      0x08 // (High-Rate Samples per Low Rate) INT
#define STATUS                       0x1A // (Alarm and Device Status Bits) INT
#define STATUS_RESET                 0x1C // (Used to Reset Status Bits) INT
#define HPF_COEF_I                   0x16 // (HPF coefficient for S1 and S3 current inputs)
#define HPF_COEF_V                   0x17 // (HPF coefficient for S0 and S2 voltage inputs)

#define PWMgreen                     PB2
#define PWMblue                      PD3

float returnVal(uint8_t regLocation);
float convertBitsS23(uint8_t highByte, uint8_t midByte, uint8_t lowByte);
float convertBitsS16(uint8_t highByte, uint8_t midByte, uint8_t lowByte);

uint16_t scaleFactor; // (667 for VRMS, 31 for IRMS, 667*31 for TRUE & APPARENT POWER)
uint8_t freq_flag; // flag for calculating frequency using S.16 signed binary point format

#endif /* MAXIMENMONI2C_H_ */