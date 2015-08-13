/*
 * main.c
 * Created: 6/8/2015 1:26:29 PM
 *  Author: zgoyetche
 */ 

#include "main.h"

int main(void)
{
	BLA_DDR |= _BV(BLA);
	BLA_PORT |= _BV(BLA);
	LED_DDR |= _BV(LED);
	LED_PORT |= _BV(LED); // turn on backlight
	
	DDRB |= _BV(PWMgreen);
	DDRD |= _BV(PWMblue);
	PORTB &= ~_BV(PWMgreen);
	PORTD &= ~_BV(PWMblue);

	st7565_init();
	st7565_command(CMD_DISPLAY_ON);
	st7565_command(CMD_SET_ALLPTS_NORMAL);
	clear_screen();
	st7565_set_brightness(0x20);
	
	write_buffer(buffer);
	_delay_ms(3000);
	clear_buffer(buffer);
	clear_screen();
	
	initUSART();
	initI2C();
		
	//initialize sample rate to 1024
	i2cStart();
	i2cSend(MAX_ADDRESS_W);
	i2cSend(SAMPLES);
	i2cSend(0b00000000);
	i2cSend(0b00000100);
	i2cSend(0b00000000);
	i2cStop();
	
	//initialize voltage scale to 667 Vp
	i2cStart();
	i2cSend(MAX_ADDRESS_W);
	i2cSend(VFSCALE);
	i2cSend(0b00000000);
	i2cSend(0b00000010);
	i2cSend(0b10011011);
	i2cStop();
	
//	initialize current scale to 31 A
	i2cStart();
	i2cSend(MAX_ADDRESS_W);
	i2cSend(IFSCALE);
	i2cSend(0b00000000);
	i2cSend(0b00000000);
	i2cSend(0b00011111);
	i2cStop();
	
	
	uint8_t hpf;
	uint8_t pf_flag = 0;
	uint8_t vas_flag = 0;
	uint8_t DC_flag = 0;
	
	float vscale = 675;
	float iscale = 31.25;
	float pscale = vscale*iscale;
	
	if(returnVal(FREQ) >= 15 && returnVal(FREQ) <= 80)
	{
		i2cStart();
		i2cSend(MAX_ADDRESS_W);
		i2cSend(HPF_COEF_I);
		i2cSend(0b01000000);
		i2cSend(0b00000000);
		i2cSend(0b00000000);
		i2cStop();
		
		i2cStart();
		i2cSend(MAX_ADDRESS_W);
		i2cSend(HPF_COEF_V);
		i2cSend(0b01000000);
		i2cSend(0b00000000);
		i2cSend(0b00000000);
		i2cStop();
		hpf = 1; //AC MODE
		DC_flag = 0;
	}
	else
	{
		i2cStart();
		i2cSend(MAX_ADDRESS_W);
		i2cSend(HPF_COEF_I);
		i2cSend(0b00000000);
		i2cSend(0b00000000);
		i2cSend(0b00000000);
		i2cStop();
		
		i2cStart();
		i2cSend(MAX_ADDRESS_W);
		i2cSend(HPF_COEF_V);
		i2cSend(0b00000000);
		i2cSend(0b00000000);
		i2cSend(0b00000000);
		i2cStop();
		hpf = 0; //DC MODE
		DC_flag = 1;
	}
	freq = 0;
	freq_flag = 0;
	
	initUSART();
	printString("Hello World!\r\n");
	
	//MAIN LOOP
	while(1)
	{
		//check if ac...switch on LPF
		if(hpf == 0 && freq >= 15 && freq <= 80) //checks to make sure it was in AC mode before switching to DC to avoid continuous writes...
		{
			i2cStart();
			i2cSend(MAX_ADDRESS_W);
			i2cSend(HPF_COEF_I);
			i2cSend(0b01000000);
			i2cSend(0b00000000);
			i2cSend(0b00000000);
			i2cStop();
		
			i2cStart();
			i2cSend(MAX_ADDRESS_W);
			i2cSend(HPF_COEF_V);
			i2cSend(0b01000000);
			i2cSend(0b00000000);
			i2cSend(0b00000000);
			i2cStop();
			hpf = 1;
			DC_flag = 0;
			//printString("SET FOR AC");
			//printString("\n\r");	
		}
		
		//check if DC... turn off LPF				
		if(hpf == 1 && (freq < 15 || freq > 80)) //checks to make sure it was in DC mode before switching to AC to avoid continuous writes...
		{
			i2cStart();
			i2cSend(MAX_ADDRESS_W);
			i2cSend(HPF_COEF_I);
			i2cSend(0b00000000);
			i2cSend(0b00000000);
			i2cSend(0b00000000);
			i2cStop();
			
			i2cStart();
			i2cSend(MAX_ADDRESS_W);
			i2cSend(HPF_COEF_V);
			i2cSend(0b00000000);
			i2cSend(0b00000000);
			i2cSend(0b00000000);
			i2cStop();
			hpf = 0;
			DC_flag = 1;
			//printString("SET FOR DC");
			//printString("\n\r");
		}
		
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//RMS VOLTAGE
		scaleFactor = vscale;
		Vrms = returnVal(VA_RMS);
		if(Vrms <= 0.3)
			Vrms = 0;
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//INST. VOLTAGE
		peakV = returnVal(VA_PEAK);
		if(peakV <= 0.6)
		peakV = 0;
		
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//WATTS
		scaleFactor = pscale;
		watts = returnVal(WATT_A);
		if((watts <= 1.2 || watts > 1000) && hpf == 1)
			watts = 0;
		else if(watts <= 0.2 || watts > 1000) 
			watts = 0;
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//RMS CURRENT
		scaleFactor = iscale;
		if(watts > 0)
			Irms = returnVal(IA_RMS);
		else
			Irms = 0;
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//FREQUENCY
		freq_flag = 1; //this flag is used to switch to S.16 notation instead of S.23 (used for most other values)
		if(Vrms > 0.3)
			freq = returnVal(FREQ);
		else
			freq = 0.00;
			
		if(Irms <= 0.01)
		{
			Irms = 0;
			pf_flag = 1; //this flag sets PF to 1 assuming Irms <= 10mA
			vas_flag = 1; //this flag sets VAs to 0 assuming Irms <= 10mA
		}
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//VOLT-AMPERES
		scaleFactor = pscale; //changed from 667 to 677
		if(vas_flag != 1)
		{
			vas = returnVal(VA_A);
			if(vas <= 0 || vas >= 1000)
			vas = 0;
		}
			
		else
		{
			vas = 0;
			vas_flag = 0;
		}
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//VARS
		if(watts > 0 && watts <= 1000 && hpf == 1)
			vars = sqrtf(pow(vas,2)-pow(watts,2));
		else
			vars = 0;
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//PHASE ANGLE
		if(watts > 0 && watts <= 1000  && hpf == 1)
			angle = acosf(watts/vas)*180/3.14159;
		else		
			angle = 0; //set angle to zero so it doesn't hover when there is no load
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//POWER FACTOR
		scaleFactor = 2;
		if(pf_flag != 1 && hpf == 1)
		{
			if(watts > 0)
				pf = returnVal(PFA);
		}			 
		else
		{
			pf = 1; //display PF as unity when there is no load
			pf_flag = 0; //display actual PF when there is a load present (current draw > 90mA)
		}
		if(pf > 1)
		pf = 1;
	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									//LEAD/LAG
		if(returnVal(VAR_A) > 0 && hpf == 1)
		{
			drawstring(buffer, 84, 7, "  Lag  ");
		}
		else if(returnVal(VAR_A) < 0 && hpf == 1)
		{
			drawstring(buffer, 84, 7, "  Lead ");
		}
		else if(hpf == 0)
			drawstring(buffer, 84, 7, "  DC   ");
		
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//THIS IS USED TO READ REGISTERS THAT DEAL WITH SAMPLING RATES!
//ALSO: REFER TO MaximEnMonI2c.c and comment in lines 31 and 32!
//Fix proceeding for loop to restore cursor properly!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		
/*									//I COEFF
		
 		printString("I COEFF:      ");
		Vrms = returnVal(HPF_COEF_I);
	
		printString("\n\r");
		
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 							//V COEFF
		
		printString("V COEFF:      ");
		Vrms = returnVal(HPF_COEF_V);
		printString("\n\r");
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	
		for(uint8_t i=0; i<11; i++)
			printString("\033[F"); // restore cursor to beginning of top line in terminal
*/			
		
//128x64 LCD DISPLAY COMMANDS
		drawstring(buffer, 0, 0, "Vrms   : ");
		drawstring(buffer, 0, 1, "Vpeak  : ");
		drawstring(buffer, 0, 2, "Freq.  : ");	
		drawstring(buffer, 0, 3, "Irms   : ");	
		drawstring(buffer, 0, 4, "PWR (P): ");
		drawstring(buffer, 0, 5, "PWR (Q): ");
		drawstring(buffer, 0, 6, "Angle  : ");
		drawstring(buffer, 0, 7, "P.F.   : ");
		uint8_t horiz = 48;	// horizontal spacing for values
		lcd_float(buffer, horiz, 0, Vrms);
		lcd_float(buffer, horiz, 1, peakV);
		
		if(freq > 90 || DC_flag == 1)
			drawstring(buffer, horiz, 2, "0.00  ");
		else
			lcd_float(buffer, horiz, 2, freq);
		lcd_float(buffer, horiz, 3, Irms);
		lcd_float(buffer, horiz, 4, watts);
		lcd_float(buffer, horiz, 5, vars);
		lcd_float(buffer, horiz, 6, angle); 
		lcd_float(buffer, horiz, 7, pf);
	
		horiz = 96; //horizontal spacing for units
		drawstring(buffer, horiz, 0, "Volts");
		drawstring(buffer, horiz, 1, "Volts");
		drawstring(buffer, horiz, 2, "Hertz");
		drawstring(buffer, horiz, 3, "Amps");
		drawstring(buffer, horiz, 4, "Watts");
		drawstring(buffer, horiz, 5, "VARs");
		drawstring(buffer, horiz, 6, "Deg");		
		
		write_buffer(buffer);
		//clearCounter = clearCounter + 1;
		//if(clearCounter > 60)
		//{
			//clear_screen();
			//clearCounter = 0;
		//}
		//_delay_ms(250);
	}		
	return 0; //should never get here.
}