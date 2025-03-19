/*
 * Lab 6.cpp
 *
 * Created: 2/25/2025 1:22:44 PM
 * Author : elima
 */ 

#include <avr/io.h>

//***GLOBAL VARIABLES***//
char pot_value = 0; // Value from 0-255 for potentiometer (only read upper 8 bits of the 10-bit number)

int main(void)
{
	// Set up pins
	DDRC = 0b00011111; // Set pins PC0-PC4 = output for LEDs; Pin PC5 = input for potentiometer
	PORTC = 0b00011111; // Initially turn off LEDs (assuming wired as active low)
	DDRD = 0b01000000; // Data register; set pin PD6 as output (PWM OC0A pin)
	DDRB = 0b00000110; // Data register; setting PB1 & PB2 as output (Motor Direction Pins) (PB1 = forward, PB2 = reverse)
	PORTB = 0b00000000; // Motor pins initially off
	
	//***Set Up A-D Conversion (ADC)***//
	
	ADMUX = 0b00100101;
		// Left justified (bit 5 = ADLAR = 1); Sets ADC channel to PC5 (ADC 5) (bits 0-3); Sets voltage reference to AREF (Pin 21) (bits 6-7)
	PRR = 0b00000000; // Clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 0b10000111;
		// Enables ADC (bit 7); ADC prescaler divider of 128 (bits 0-2); Bit 4 is flag, can either say conversion done OR can enable ADC to trigger interrupt
	
	//***Set up Motor Control***//
	
	OCR0A = 0x00; // Load $00 into OCR0 to set initial duty cycle to 0 (motor off)
	TCCR0A = 0b10000011; // Set non-inverting mode on OC0A pin (COMA1:0 = 10); Fast PWM (WGM1:0 bits = bits 1:0 = 11); (Note: that we are not affecting OC0B because COMB0:1 bits stay at default = 00)
	TCCR0B = 0b00000011; // Set base PWM frequency (CS02:0 bits 0-2 = 011 for prescaler of 64), for approximately 1 kHz base frequency
	
	while(1) // Infinite loop while program runs
	{
		ADCSRA = ADCSRA | 0b01000000; // Start A/D conversion (bit 6)
		
		while((ADCSRA & 0b00010000) == 0); // Wait for conversion to finish (bit 5 = ADIF)
		
		pot_value = ADCH; // Keep high bits of 10-bit result (throw away last 2)
		
		if(pot_value < 51) // Lowest voltage range (-60% to -100% duty cycle)
		{
			PORTC = 0b11111110; // Turn LED 0 (PC0) on, all others off
			PORTB = 0b00000010; // Motor reversed (PB1)
			OCR0A = ((pot_value*2)-255)*(-1);
		}
		else if(pot_value < 102)// 2nd lowest voltage range (-20% to -60% duty cycle)
		{
			PORTC = 0b11111101; // Turn LED 1 (PC1) on, all others off
			PORTB = 0b00000010; // Motor reversed (PB1)
			OCR0A = ((pot_value*2)-255)*(-1);
		}

		else if(pot_value < 153) // Middle voltage range (0% duty cycle)
		{
			PORTC = 0b11111011; // Turn LED 2 (PC2) on, all others off
			PORTB = 0b00000000; // Motor off
			OCR0A = 0; // Motor speed = 0
		}
		else if(pot_value < 204) // 2nd highest voltage range (20% to 60% duty cycle)
		{
			PORTC = 0b11110111; // Turn LED 3 (PC3) on, all others off
			PORTB = 0b00000100; // Motor forward (PB2)
			OCR0A = (pot_value*2)-255;
		}
		else if(pot_value < 256) // Highest voltage range (60% to 100% duty cycle)
		{
			PORTC = 0b11101111; // Turn LED 4 (PC4) on, all others off
			PORTB = 0b00000100; // Motor forward (PB2)
			OCR0A = (pot_value*2)-255;
		}
	
	}
	
	return(0);
	
} // end main

