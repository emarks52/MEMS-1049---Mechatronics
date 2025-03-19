/*
 * Lab 7.cpp
 *
 * Created: 3/12/2025 10:21:34 PM
 * Author : elima
 */ 

#include <avr/io.h>
#define FREQ_CLK 1000000  // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8)

// Call Functions

void step_CW(void);
void step_CCW(void);
void wait(volatile int mulitple);

// Global Variables

int phase_step = 1;  // We will use wave stepping, so the steps are numbered 1 - 4, then will repeat

// Main Loop

int main(void)
{
	// Set Up
	
	DDRB  = 0b00000011;  // Data register B, set pin PB0 & PB1 as outputs for LEDs
	PORTB = 0b00000011;  // Set output of PORTB high to turn off LEDs (wired active low)
	DDRC  = 0b00000000;  // Data register C, pin PC0 set as input for switch
	DDRD  = 0b11110000;  // Data register D, pins PD4 - PD7 set as outputs for stepper motor control
	
	//  Loop for Demo
	
	while (1)
	{
		if(!(PINC & 0b00000001))  // If switch PC0 is pressed
		{
			for(int i = 1; i <= (270/7.5); i++)  // CW 270 degrees (270/7.5 step = 36)  // 7.5 degrees is the stride angle for our motor (ST-PM35-15-11C)
			{
				// "step size" = 7.5 degrees based on slides
				PORTB = 0b11111110;  // Turn LED 0 on (First LED -> PB0)
				step_CW();  // Clockwise step
				wait(50);  // Delay so that motor doesn't skip steps
			}
			
			PORTB = 0b11111111;  // Turn LEDs off
			wait(500);  // Pause for a half a second
			
			for(int k = 1; k <= (90/7.5); k++)  // CCW 90 degrees (90/7.5 step = 12)
			{
				PORTB = 0b11111101;  // Turn LED 1 on (Second LED -> PB1)
				step_CCW();  // Counterclockwise step
				wait(50);  // Delay so that the motor doesn't skip steps
			}
			
			PORTB = 0b11111111;  // Turn LEDs off
			wait(500);  // Pause for a half a second
			
			for(int j = 1; j <= (180/7.5); j++)  // CW 180 degrees (180/7.5 step = 24)
			{
				PORTB = 0b11111110;  // Turn LED 0 on (First LED -> PB0)
				step_CW();  // Clockwise step
				wait(75);  // Delay so motor doesn't skip steps
			}
			
			PORTB = 0b11111111;  // LEDs off
		}
	}
}
	
	
//***Counterclockwise Turn Function***//
void step_CCW()
{
		// This function advances the motor counter-clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course.
		// phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4

		switch (phase_step) 
		{
			case 1:
			// step to 2
				PORTD = 0b00010000; // The last 4 bits are connected to the respective motor inputs (PD7 --> 1a, PD6 --> 1b, PD5 --> 2a, and PD4 --> 2b)
				phase_step = 2; // The numbers increase for counter-clockwise motion (See chart in notes)
			break;
			
			case 2:
			// step to 3
				PORTD = 0b01000000;
				phase_step = 3;
			break;
			
			case 3:
			// step to 4;
				PORTD = 0b00100000;
				phase_step = 4;
			break;
			
			case 4:
			// step to 1;
				PORTD = 0b10000000;
				phase_step = 1;
			break;
		}
}  // end step_CCW


//***Clockwise Turn Function***//
	void step_CW() 
	{
		// This function advances the motor clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. Our stepper motor phases are assigned to Port pins as follows:
		// phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4

		switch (phase_step) 
		{
			case 1:
			// step to 4
			PORTD = 0b00100000; // The last 4 bits are connected to the respective motor inputs (PD7 --> 1a, PD6 --> 1b, PD5 --> 2a, and PD4 --> 2b)
			phase_step = 4; // The numbers decrease for clockwise motion (See chart in notes)
			break;
			
			case 2:
			// step to 1
			PORTD = 0b10000000;
			phase_step = 1;
			break;
			
			case 3:
			// step to 2;
			PORTD = 0b00010000;
			phase_step = 2;
			break;
			
			case 4:
			// step to 3;
			PORTD = 0b01000000;
			phase_step = 3;
			break;  
		}
	} // end step_CW


//***Wait Function***//
void wait(volatile int number_of_msec) 
{
		// This subroutine creates a delay equal to number_of_msec*T, where T is 1 msec
		// It changes depending on the frequency defined by FREQ_CLK
		char register_B_setting;
		char count_limit;
		// Some typical clock frequencies:
		switch(FREQ_CLK) 
		{
			case 16000000:
			register_B_setting = 0b00000011; // This will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
			count_limit = 250; // For prescaler of 64, a count of 250 will require 1 msec
			break;
			
			case 8000000:
			register_B_setting = 0b00000011; // This will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
			count_limit = 125; // For prescaler of 64, a count of 125 will require 1 msec
			break;
			
			case 1000000:
			register_B_setting = 0b00000010; // This will start the timer in Normal mode with prescaler of 8 (CS02 = 0, CS01 = 1, CS00 = 0).
			count_limit = 125; // For prescaler of 8, a count of 125 will require 1 msec
			break;
			
		}
		
		while (number_of_msec > 0) 
		{
			TCCR0A = 0x00; // Clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
			TCNT0 = 0; // Preload value for testing on count = 250
			TCCR0B = register_B_setting; // Start TIMER0 with the settings defined above
			while (TCNT0 < count_limit); // Exits when count = the required limit for a 1 msec delay
			TCCR0B = 0x00; // Stop TIMER0
			number_of_msec--;
		}
} // end wait()
