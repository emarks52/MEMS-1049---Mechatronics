/*
 * Lab4.cpp
 *
 * Created: 2/12/2025 4:25:32 PM
 * Author : elima
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8)


//**GLOBAL VARIABLES**//
int number_of_states = 12; // number of states in the LED pattern, including the OFF state
char pattern[] = {'A','Z','B','Z','C','Z','D','Z','E','Z','F','Z'}; // this array contains the states of the LEDs in the on-off pattern
int run; // Variable for determining which light pattern to display
int button_counter0 = 0; // Variable for counting switch 0
int button_counter1 = 0; // Variable for counting switch 1


//**CALL FUNCTIONS**//
void wait(volatile int);
void change_led_state(char);


//Interrupt service routing for INT0
ISR(INT0_vect) // This routine is entered if Switch 0 goes high (button is pressed)
{
	
	if(button_counter0 == 0)
	{
		if(!(PINC & 0b00000100))// If LED 2 is currently on and button is pressed, turn on LED 6
			{
				button_counter0++; // Increase tally for button presses
				
				PORTB = PORTB & 0b00000001; // Turn on LED 6
				
				run = 2; // Set next LED pattern
				
			}
	
	}
				
		EIFR = 0b00000001; // Clear the INT0 flag (by writing 1 to it) in case it was reset while in the ISR (so it doesn't run again). Note that this ignores the INT1 flag
}
	

//Interrupt service routing for INT1
ISR(INT1_vect) // This routine is entered if Switch 1 goes high (button is pressed)
{
	if(button_counter1 == 0 && !(button_counter0 == 0))
	{
		if(!(PINC & 0b00100000))// If LED 5 is currently on and button is pressed, turn on LED 6
		{
			button_counter1++; // Increase tally for button presses
			
			PORTB = PORTB & 0b00000010; // Turn on LED 7
			
			run = 3; // Set next LED pattern
			
		}
		
	}
	
	EIFR = 0b00000010; // Clear the INT1 flag (by writing 1 to it) in case it was reset while in the ISR (so it doesn't run again). Note that this ignores the INT1 flag
}


int main(void)
{
	DDRD = 0b00000000; // All PORTD bits are inputs (which includes PD2 and PD3 that is attached to the switch)
	DDRC = 0b00111111; // Pins PC0, PC1, PC2, PC3, PC4, and PC5 are set as outputs for LEDs
	DDRB = 0b00000011; // Pins PB0 and PB1 are set as outputs for LED 6 and LED 7
	
	PORTB = 0b00000011; // Turn off LED 6 and 7 initially (assumes they are wired as active low)
	PORTC = 0b00111111; // Turn off the LEDs (assumes they are wired as active low)
	
	// Set up Interrupts
	EICRA = 0b00001010; // Trigger INT0 and INT1 on falling edge; bits 3 and 2 control INT1, while bits 1 and 0 control INT0 ('10' is falling edge for INTx)
	EIMSK = 1<<INT0 | 1<<INT1; // Enable INT0 and INT1
	sei(); // Enable global interrupt
	
	// Set initial light pattern
	run = 1;
	
	while(1)
	{
		while(run == 1) //Initial LED pattern
		{
			// Repeatedly display LED pattern
			for (int i = 0; i<number_of_states; i++) // scroll through the LED pattern states
			{
				change_led_state(pattern[i]); // pass current pattern state to the function to change to a new state
			}
		}
		
		while(run == 2) //LED pattern in which only LEDs 3, 4, and 5 are blinking
		{
			// Repeatedly display LED pattern
			for (int j = 6; j<number_of_states; j++) // scroll through the LED pattern states starting at LED 3
			{
				change_led_state(pattern[j]); // pass current pattern state to the function to change to a new state
			}
		}
		
		while(run == 3) //LED pattern in which all LEDs turn off and the system resets
		{
				wait(3000); // 3 second wait
						
				PORTB = 0b00000011; // Turn off LED 6 and 7
				PORTC = 0b00111111; // Turn off all scrolling LEDs
				
				button_counter0 = 0; // Reset button count for switch 0
				button_counter1 = 0; // Reset button count for switch 1
				
				wait(500);
			
			run = 1; // Reset LED pattern	
		}
		
	}
	
	return(0);
} //end main function


//**DEFINE FUNCTIONS**//

void change_led_state(char new_state)
{
	// This function changes the LED state into a new state defined by the passed argument
	
	int on_time = 400; // Amount of msec LED is ON
	int off_time = 400; // Amount of msec LED is OFF
	
	switch(new_state)
	{ // choose prescaler
		case 'A': // LED0 ON
			PORTC = 0b00111110; //Turn ON LED0 (PC0)
			
			wait(on_time);
		break;
		
		case 'B': // LED1 ON
			PORTC = 0b00111101; //Turn ON LED1 (PC1)
			wait(on_time);
		break;
		
		case 'C': // LED2 ON
			PORTC = 0b00111011; //Turn ON LED2 (PC2)
			wait(on_time);
		break;
		
		case 'D': // LED3 ON
			PORTC = 0b00110111; //Turn ON LED3 (PC3)
			wait(on_time);
		break;
		
		case 'E': // LED4 ON
			PORTC = 0b00101111; //Turn ON LED4 (PC4)
			wait(on_time);
		break;
		
		case 'F': // LED5 ON
			PORTC = 0b00011111; //Turn ON LED5 (PC5)
			wait(on_time);
		break;
		
		default: // All LEDs off
			PORTC = 0b00111111; //Turn OFF all LEDs
			wait(off_time);
		break;
		
	} // end switch
	
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

