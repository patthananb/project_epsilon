#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "headerfile.h"
#define F_CPU 16000000

#define buttonPin  PB0   // Pin 8
#define ledPin     PD2   // Pin 2

//====================
//Functions Prototype
//====================

// LCD functions with I2C communication
void lcdbegin(void);
void setPosition(uint8_t x, uint8_t y);
void print(const char *str);
void lcdclear(void);
void setBacklight(uint8_t data);

void init_ADC(void);
void init_timer(void);
int map_adc_to_10_steps(uint16_t adc_value);
void turnOffLED(void);

int buttonState = 0;
int buttonPressCount = 0; // Variable to count button presses

int inputPassword[3];
int password[] = {1, 2, 3};
int index = 0;
int currentvalue = 0;
char currentvalueString[2]; // Increase size to store two characters
uint8_t mapped_value;

extern void setup();


int main(void)
{
	DDRB = 0x00; // Set port B as input
	DDRD = 0xFF; // Set port D as output

	init_ADC();
	init_timer();
	sei();

	// Welcome message [test lcd]
	lcdbegin();
	lcdclear();
	setPosition(0, 0);
	print("Zeig");
	_delay_ms(10000); // Change from 10000 to 1000 to reduce delay
	setPosition(0, 1);
	print("Heil");
	_delay_ms(10000); // Change from 10000 to 1000 to reduce delay
	lcdclear();

	while (1)
	{
		// Read the state of the push button
		int buttonState = (PINB & (1 << buttonPin));

		// Check if the push button is pressed
		if (buttonState != 0)
		{
			inputPassword[index] = mapped_value;
			index++;

			if (index >= 3)
			{
				bool match = true;
				for (int i = 0; i < 3; i++)
				{
					if (inputPassword[i] != password[i])
					match = false;
					break; // Move break outside of if statement
				}
				if (match)
				{ // Turn LED on
					PORTD |= (1 << ledPin);
					lcdclear();
					setPosition(0, 0);
					print("Password matched!");
				}
				else
				{
					lcdclear();
					setPosition(0, 0);
					print("Password incorrect!");
				}
				index = 0;
				memset(inputPassword, 0, sizeof(inputPassword));
			}
			else
			{
				// Increment button press count
				buttonPressCount++;

				// Check if the button has been pressed three times
				if (buttonPressCount == 3)
				{
					turnOffLED();
					buttonPressCount = 0; // Reset button press count
				}
			}
			_delay_ms(5000);
		}
		setPosition(0, 1);
		print("Current Value :");
		sprintf(currentvalueString, "%d", mapped_value); // Correct sprintf usage
		print(currentvalueString); // Print converted string
		_delay_ms(100);             // Delay for a moment to avoid rapid toggling and excessive printing
	}
}

void init_ADC(void)
{
	// Set ADC reference voltage to AVCC with external capacitor at AREF pin
	ADMUX = (1 << REFS0); // AVCC with external capacitor at AREF pin

	// Set ADC prescaler to 128 for a 16MHz clock, results in 125kHz ADC clock
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// Enable ADC and ADC interrupt
	ADCSRA |= (1 << ADEN) | (1 << ADIE);
}

void init_timer(void)
{
	// Set Timer/Counter1 to CTC (Clear Timer on Compare Match) mode
	TCCR1B |= (1 << WGM12);

	// Set prescaler to 64, giving a timer frequency of 250kHz for a 16MHz clock
	TCCR1B |= (1 << CS11) | (1 << CS10);

	// Set the compare match value for a 1ms interrupt
	OCR1A = 250;

	// Enable Timer/Counter1 Output Compare A Match interrupt
	TIMSK1 |= (1 << OCIE1A);
}

// Timer/Counter1 Output Compare A Match interrupt service routine
ISR(TIMER1_COMPA_vect)
{
	// Start ADC conversion
	ADCSRA |= (1 << ADSC);
}

// ADC conversion complete interrupt service routine
ISR(ADC_vect)
{
	// Read ADC value from ADC data registers ADCL and ADCH
	uint16_t adc_value = ADCL | (ADCH << 8);

	// Map ADC value to 10 steps (0 to 9) using switch-case
	mapped_value = map_adc_to_10_steps(adc_value);
}

int map_adc_to_10_steps(uint16_t adc_value)
{
	// Define ranges for each step
	if (adc_value <= 102)
	{
		return 0;
	}
	else if (adc_value <= 205)
	{
		return 1;
	}
	else if (adc_value <= 306)
	{
		return 2;
	}
	else if (adc_value <= 408)
	{
		return 3;
	}
	else if (adc_value <= 510)
	{
		return 4;
	}
	else if (adc_value <= 612)
	{
		return 5;
	}
	else if (adc_value <= 714)
	{
		return 6;
	}
	else if (adc_value <= 816)
	{
		return 7;
	}
	else if (adc_value <= 918)
	{
		return 8;
	}
	else
	{
		return 9;
	}
}

void turnOffLED(void)
{
	PORTD &= ~(1 << ledPin); // Turn off LED
}
