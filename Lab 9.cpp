/*
 * Lab 9.cpp
 *
 * Created: 3/19/2025 4:44:43 PM
 * Author : elima
 */ 

#include <avr/io.h>
#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8)


//***CALL FUNCTIONS***//
int send_to_MAX7221(unsigned char, unsigned char);
void wait(volatile int);

int main(void)
{
      // Setup
      DDRB = 1 << PORTB5 | 1 << PORTB3 | 1 << PORTB2;  // Set pins SCK (PB5), MOSI (PB3), and SS (PB2) as output

      // Set up SPI (Main)
      SPCR = 0b01010001; // SPCR = 1 << SPE | 1 << MSTR | 1 << SPR0;
	  // (SPIE = 0, SPE = 1 (enables SPI), DORD = 0, MSTR = 1 (working in Main mode), CPOL = 0, CPHA = 0, SPR1 = 0, SPR0 = 1 (sets SCK freq))
      // Enable the SPI, set to Main mode 0, SCK = Fosc/16, lead with MSB
      
	  // Initial settings for MAX7221
	  send_to_MAX7221(0b00001111, 0x00); // Display normal operation
	  send_to_MAX7221(0b00001001, 0b00000011); // Set decoding mode ON for both digits
      send_to_MAX7221(0b00001011, 0b00000001); // Set scan limit to only digits 0 & 1
	  send_to_MAX7221(0b00001100, 0x01); // Turn on display for both digits
	  send_to_MAX7221(0b00001010, 0x0F); // Set max light intensity

      while(1) // Infinite loop for displaying numbers
	  {
		  // Date = 03/21
		  
		  // month for 1 sec
		  send_to_MAX7221(0b00000010, 0x00); // Put a '0' in digit 1
		  send_to_MAX7221(0b00000001, 0x03); // Put a '3' in digit 0
		  wait(1000);
		  
		  // day for 1 sec
		  send_to_MAX7221(0b00000010, 0x02); // Put a '2' in digit 1
		  send_to_MAX7221(0b00000001, 0x01); // Put a '1' in digit 0  
		  wait(1000);
		  
		  // blank for 2 sec
		  send_to_MAX7221(0b00001001, 0x00); // Set decoding mode OFF for both digits)
		  send_to_MAX7221(0b00000001, 0x00); // Blank digit 1
		  send_to_MAX7221(0b00000010, 0x00); // Blank digit 0
		  wait(2000);
		  
		  send_to_MAX7221(0b00001001, 0b00000011); // Turn decoding mode back ON for both digits
	  }
		  
} // end main()

int send_to_MAX7221(unsigned char command, unsigned char data)
{
		
	PORTB = PORTB & 0b11111011; // Clear PB2, which is the SS bit, so that transmission can begin
	
	SPDR = command; // Send command
	while(!(SPSR & (1<<SPIF))); // Wait for transmission to finish
	
	SPDR = data; // Send data
	while(!(SPSR & (1<<SPIF))); // Wait for transmission to finish
	
	PORTB = PORTB | 0b00000100; // Return PB2 to 1, which is the SS bit, to end transmission
	
	return 0;
	
} // end send_to_MAX7221()

void wait(volatile int number_of_msec)
{
	// This subroutine creates a delay equal to number_of_msec*T, where T is 1 msec
	// It changes depending on the frequency defined by FREQ_CLK
	
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
	
} // end wait()
