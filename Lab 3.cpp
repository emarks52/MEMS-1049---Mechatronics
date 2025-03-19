/*
 * Lab3.cpp
 *
 * Created: 2/6/2025 12:03:48 PM
 * Author : elima
 */ 

#include <avr/io.h>
#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8)

//**CALL FUNCTIONS**//
void wait(volatile int);
void change_led_state(char);

int main(void)
{
    int number_of_states = 22; // number of states in the LED pattern, including the 2-second OFF state
	char pattern[] = {'A','C','B','C','A','C','A','D','A','D','B','C','B','C','A','D','B','C','B','C','B','E'}; // this array contains the states of the LEDs in the on-off pattern
	
	DDRD = 0b00000000; // All pins are inputs (which includes PD2 that is attached to the switch)
	DDRC = 0b00000011; // Pins PC0 and PC1 are set as outputs for LEDs
	PORTC = 0b00000011; // Turn off the LEDs (assumes they are wired as active low)
	
	while (PIND & 0b00000100) // this logic causes the program to stay in the loop if pin PD2 is high (switch is not pressed)
	{
		// this loop does nothing; it exits when the switch is pressed
	}
	
	while(1)
	{
		// Repeatedly display LED pattern
		for (int i=0; i<number_of_states; i++) // scroll through the LED pattern states
		{
			change_led_state(pattern[i]); // pass current pattern state to the function to change to a new state
		}
	}
	
	return(0);
} //end main function



//**DEFINE FUNCTIONS**//

void change_led_state(char new_state)
{
	// This function changes the LED state into a new state defined by the passed argument
	// The argument is coded to include the duration of the new state (A = C = 200 ms, B = D = 600 ms, anything else = 2000 ms)
	
	switch(new_state)
	{ // choose prescaler
		case 'A': // short ON period
			PORTC = 0b00000000; //Turn ON LEDs
			wait(200);
		break;
		
		case 'B': // long ON period
			PORTC = 0b00000000; //Turn ON LEDs
			wait(600);
		break;
		
		case 'C': // short OFF period
			PORTC = 0b00000011; //Turn OFF LEDs
			wait(200);
		break;
		
		case 'D': // long OFF period
			PORTC = 0b00000011; //Turn OFF LEDs
			wait(600);
		break;
		
		default: // 2 second OFF period before repeating pattern
			PORTC = 0b00000011; //Turn OFF LEDs
			wait(2000);
		break;
	} // end switch
	
	while(!(PIND & 0b00000100)) //this logic stays in the loop if pin PD2 is low (switch is pressed), which pauses the LED pattern
	{
		// Wait for switch debounce delay (note that this 50 ms would need to be subtracted from the next LED state if the switch pattern were resumed instantly
		// ^ It is expected that the user will press and hold the switch longer than switch bounce, so the subtraction won't be needed
		wait(50); 
	}
} // end change_LED_state()


void wait(volatile int number_of_msec)
{
	//This subroutine creates a delay equal to number_of_msec*T, where T is 1 msec
	//It changes depending on the frequency defined by FREQ_CLK
	
	char register_B_setting;
	char count_limit;
	
	// Some typical clock frequencies
	switch(FREQ_CLK)
	{
		case 16000000:
			register_B_setting = 0b00000011; // this will start the time in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1)
			count_limit = 250; // For prescaler of 64, a count of 250 will require 1 msec
		break;
		
		case 8000000:
			register_B_setting = 0b00000011; // this will start the time in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1)
			count_limit = 125; // For prescaler of 64, a count of 125 will require 1 msec
		break;
		
		case 1000000:
			register_B_setting = 0b00000010; // this will start the time in Normal mode with prescaler of 8 (CS02 = 0, CS01 = 1, CS00 = 0)
			count_limit = 125; // For prescaler of 8, a count of 125 will require 1 msec
		break;
	}
	
	while(number_of_msec > 0)
	{
		TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in Normal mode
		TCNT0 = 0; // preload value for testing on count = 250
		TCCR0B = register_B_setting; // start TIMER0 with the settings defined above
		while(TCNT0 < count_limit); // exits when count = the required limit for a 1 ms delay
		TCCR0B = 0x00; // Stop TIMER0
		number_of_msec--;
	}
	
} //end wait()