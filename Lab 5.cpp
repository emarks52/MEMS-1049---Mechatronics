/*
 * Lab 5.cpp
 *
 * Created: 2/20/2025 2:39:02 PM
 * Author : elima
 */ 

#include <avr/io.h>

//***GLOBAL VARIABLES***//
char pot_value = 0; // Value from 0-255 for potentiometer (only read upper 8 bits of the 10-bit number)

int main(void)
{
	// Set up pins
	DDRC = 0b00011111; // Set pins PC0-PC4 set as output for LEDs; Pin PC5 set as input for potentiometer
	PORTC = 0b00011111; // Initially turn off LEDs (assuming wired as active low)
	
	// Set Up A-D Conversion (ADC)
	ADMUX = 0b00100101;
		// Left justified (bit 5 = ADLAR = 1)
		// Sets ADC channel to PC5 (ADC 5) (bits 0-3)
		// Sets voltage reference to AREF (Pin 21) (bits 6-7)
	PRR = 0b00000000; // Clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 0b10000111;
		// Enables the ADC (bit 7)
		// ADC prescaler divider of 128 (bits 0-2)
		// Bit 4 is flag, can either say conversion done OR we can enable ADC to trigger interrupt
	
	while(1) // Infinite loop while program runs
	{
		ADCSRA |= (1<<ADSC); // Start A/D conversion (bit 6)
		
 		while((ADCSRA & 0b00010000) == 0); // Wait for conversion to finish (bit 5 = ADIF)
		
		pot_value = ADCH; // Keep high bits of 10-bit result (throw away last 2)
		
		//PORTC = ~pot_value>>3; // echo results back out to PORTC
		
		if(pot_value < 51) // Lowest voltage range (fast backward)
			{
				PORTC = 0b00011110; // Turn ON LED 0 (PC0), all others OFF
			}
		else if(pot_value < 102) // 2nd voltage range (slow backward)
			{
				PORTC = 0b00011101; // Turn ON LED 1 (PC1), all others OFF
			}
		else if(pot_value < 153) // 3rd voltage range (stopped)
			{
				PORTC = 0b00011011; // Turn ON LED 2 (PC2), all others OFF
			}
		else if(pot_value < 204) // 4th voltage range (slow forward)
			{
				PORTC = 0b00010111; // Turn ON LED 3 (PC3), all others OFF
			}
		else if(pot_value < 256) // Highest voltage range (fast forward)
			{
				PORTC = 0b00001111; // Turn ON LED 4 (PC4), all others OFF
			}
		
	}
	
	return(0);
	
} // end main