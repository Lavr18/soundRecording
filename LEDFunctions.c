/*
 * This file contains the functions that provide a high level way to use AT91SAM7S256 LEDs.
 * Author: Aliaksei Laurynovich (unless otherwise stated).
 * Date of creation: 07.11.2018.
 * Organisation: Univeristy of Essex.
 */

#include <config/AT91SAM7S256.h>
#include "LEDFunctions.h"

#define LED_DATA_LINE AT91C_PIO_PA25		// Data line (Output)
#define LED_CLOCK_LINE AT91C_PIO_PA24		// Clock line (Output)


/*
 * This function setups the PIO pins for the LEDs operation.
 * It is used to keep the code clean and consise.
 */
void setupLEDoutputs()
{
	/*
	 * Setup the PIO pins to the correct direction.
	 *
	 * 1. All pins become PIO pins.
	 * 2. Select Output pins.
	 */
	AT91C_BASE_PIOA->PIO_PER = (AT91C_PIO_PA24 | AT91C_PIO_PA25);
	AT91C_BASE_PIOA->PIO_OER = (AT91C_PIO_PA24 | AT91C_PIO_PA25);
}

/*
 * Sets the specified LED to the chosen colour, while all the other LEDs are set to zero.
 */
void SetLEDcolour(LEDnumber led, LEDcolour colour)
{
	unsigned short ledConfig;
	short i;

	/* Setup output pins required for the LEDs*/
	setupLEDoutputs();

	/*
 	 * 1. Convert the LED number and the colour to the ledConfig value
	 */

	/* Do the range check. If the range is wrong, set all LEDs off*/
	if ((colour >= 0 && colour < 4) && (led >= 0 && led < 8))
	{
		ledConfig = colour; //the colour number corresponds to the bits position for a particular colour
		ledConfig <<= (2 * led); //shift ledConfig by twice the selected LED number to get the correct result
	}
	else
		ledConfig = 0;


	/*
 	 * 2. Setup all the LEDs by checking the bits of ledConfig
	 */

	/* Clear all the LEDs */
	AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK_LINE;
	AT91C_BASE_PIOA->PIO_SODR = LED_DATA_LINE;
	for (i = 0; i < 16; i++)
	{
		AT91C_BASE_PIOA->PIO_SODR = LED_CLOCK_LINE;
		AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK_LINE;
	}

	/* Set the LED */
	for (i = 0; i < 16; i++)
	{
		/*
		 * Check the most significant bit of a ledConfig.
		 * If it's HI, turn the LED on
 		 */
		if (ledConfig & 0x8000)
		{
			AT91C_BASE_PIOA->PIO_CODR = LED_DATA_LINE; //turn the LED on
		}
		else
		{
			AT91C_BASE_PIOA->PIO_SODR = LED_DATA_LINE; //turn the LED off
		}

		/* Do a clock pulse */
		AT91C_BASE_PIOA->PIO_SODR = LED_CLOCK_LINE;
		AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK_LINE;

		/* Shift the ledConfig 1 bit to the left in order to check the next bit in the next cycle */
		ledConfig <<= 1;
	}
}

void SetAllLEDs(LEDcolour ColourVector[])
{
	unsigned short ledConfig;
	unsigned short shiftedConfig = 0;
	short i;

	/* Setup output pins required for the LEDs*/
	setupLEDoutputs();

	/*
 	 * 1. Convert the ColourVector to the ledConfig value
	 */
	for (i = 7; i >= 0; i--)
	{
		/*
		 * If the colour is specified correctly, set 2 bits of ledConfig to it. Else, set these bits to zero.
		 */
		if (ColourVector[i] >= 0 && ColourVector[i] < 4)
		{
			/*
			 * Use shiftedConfig variable to store the shifted ledConfig value, so that all LED bits are set by the end
			 */
			ledConfig = shiftedConfig + ColourVector[i];
			shiftedConfig = ledConfig << 2;
		}
		else
		{
			/*
			 * Do the same as above, but the colour is 0 (i.e. the LED will be off)
			 */
			ledConfig = shiftedConfig + 0;
			shiftedConfig = ledConfig << 2;
		}
	}

	/*
 	 * 2. Setup all the LEDs by checking the bits of ledConfig
	 */

	/* Clear all the LEDs */
	AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK_LINE;
	AT91C_BASE_PIOA->PIO_SODR = LED_DATA_LINE;
	for (i = 0; i < 16; i++)
	{
		AT91C_BASE_PIOA->PIO_SODR = LED_CLOCK_LINE;
		AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK_LINE;
	}

	/* Set the LED */
	for (i = 0; i < 16; i++)
	{
		/*
		 * Check the most significant bit of a ledConfig.
		 * If it's HI, turn the LED on
 		 */
		if (ledConfig & 0x8000)
		{
			AT91C_BASE_PIOA->PIO_CODR = LED_DATA_LINE; //turn the LED on
		}
		else
		{
			AT91C_BASE_PIOA->PIO_SODR = LED_DATA_LINE; //turn the LED off
		}

		/* Do a clock pulse */
		AT91C_BASE_PIOA->PIO_SODR = LED_CLOCK_LINE;
		AT91C_BASE_PIOA->PIO_CODR = LED_CLOCK_LINE;

		/* Shift the ledConfig 1 bit to the left in order to check the next bit in the next cycle */
		ledConfig <<= 1;
	}
}

/* The function to set all LEDs off in the vector */
void resetLEDvector(LEDcolour Vector[])
{
	for (short i = 0; i < 8; i++)
		Vector[i] = OFF;
}

/* The function to change the next Vector element from LO to HI */
void setNextLOtoHI(LEDcolour Vector[])
{
	for (short i = 0; i < 8; i++)
	{
		if (Vector[i] == 0)
		{
			Vector[i] = 1;
			break;
		}
	}
}
