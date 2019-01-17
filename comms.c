/*
 * comms.c
 *
 *  Created on: 8 Dec 2018
 *      Author: Aliaksei Laurynovich
 *      Info: contains functions that are used to control and
 *      receive/transmit the sound to the audio expansion board
 */

#include "comms.h"

#define AUDIO_DAC_ADDR	0x1A		/* I2C address of Audio converter */

/*
 * Write a value to the specified Audio DAC register in a format:
 * top 7 bits being the register address, and the bottom 9 bits being the register value.
 */
void WriteAudioDACreg(unsigned char reg, unsigned short value)
{
	unsigned char message[2];

	/*
	 * Create the required 2 byte message
	 */
	message[0] = (reg << 1) | ((value >> 8) & 0x01);
	message[1] = value & 0xFF;

	/*
	 * Now write the message to the device.
	 */
	WriteI2CDataList(AUDIO_DAC_ADDR, 0, 0, message, 2);
}

/* Function to make SSC work like I2S. Initialise it as a transmitter and as a receiver. */
void SSC_initialise(void)
{
	AT91PS_PMC pPMC = AT91C_BASE_PMC;

	/*
	 * Initialise the codec clock to 12MHz
	 *
	 * So we want,
	 *    select source (MAIN CLK = external clock)
	 *    select prescaler (1)
	 */
	pPMC->PMC_PCKR[2] = (AT91C_PMC_PRES_CLK_8 | AT91C_PMC_CSS_PLL_CLK);

	/*
	 * Now enable PCLK2 and wait for it to start
	 */
	*AT91C_PMC_SCER = AT91C_PMC_PCK2;
	while( !(*AT91C_PMC_SR & AT91C_PMC_PCK2RDY) );

	/*
	 * Next make the output available on the PIO
	 */
	*AT91C_PIOA_BSR = AT91C_PA31_PCK2;
	*AT91C_PIOA_PDR = AT91C_PA31_PCK2;

	/*
	 * Finally we setup the I2S interface
	 */
	*AT91C_PMC_PCER = (1 << AT91C_ID_SSC); // Enable Clock for SSC controller
	*AT91C_SSC_CR = AT91C_SSC_SWRST; // reset
	*AT91C_SSC_CMR = 0; // no divider

	/*
	 * Configure transmit side
	 */
	*AT91C_PIOA_PDR = AT91C_PA16_TK | AT91C_PA15_TF | AT91C_PA17_TD | AT91C_PA18_RD; // enable pins

	*AT91C_SSC_TCMR = AT91C_SSC_CKS_RK |		// external clock on TK pin (macro name is misleading)
	                  AT91C_SSC_START_EDGE_RF |	// any edge
	                  (0 << 16);				// STTDLY = 0!
	*AT91C_SSC_TFMR = (15) |					// 16 bit word length
	                  (0 << 8) |				// DATNB = 0 => 1 words per frame
	                  AT91C_SSC_MSBF;			// MSB first
	*AT91C_SSC_CR = AT91C_SSC_TXEN; // enable TX

	/*
	 * Configure receive side
	 */
	*AT91C_SSC_RCMR = AT91C_SSC_CKS_TK |		// Use same clock as TX
	                  AT91C_SSC_START_TX |
	                  AT91C_SSC_CKI |			// sample on rising clock edge
	                  (1 << 16);				// STTDLY = 0
	*AT91C_SSC_RFMR = (15) |					// 16 bit word length
	                  (0 << 8) |				// DATNB = 0 => 1 words per frame
	                  AT91C_SSC_MSBF;			// MSB first
	*AT91C_SSC_CR = AT91C_SSC_RXEN;				// enable RX
}

/* Receive a sound and store it in the array using a polling loop */
void receiveSoundSamples(short* data, short len)
{
	short tempBuffer[2]; //Temporary buffer used for discarding every 2nd sample
	LEDcolour leds[8] = {OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF}; //LED array with LEDs turned off

	/* Turn on all LEDs off */
	SetAllLEDs(leds);

	for (short i = 0; i < len; i++)
	{
		for (short j = 0; j < 2; j++)
		{

			/* Wait for the status READY in SSC_SR */
			while (!(*AT91C_SSC_SR & AT91C_SSC_RXRDY));
			tempBuffer[j] = *AT91C_SSC_RHR; // Put the received data in the temporal buffer
		}

		data[i] = tempBuffer[0]; // Put just the first sample from the tempBuffer into the final data buffer

		/* Turn on LEDs during the recording in equal intervals of time. A single LED at a time. */
		if (i % (len / 8) == 0)
		{
			/* Set red the next LED */
			setNextLOtoHI(leds);
			/* Turn on the new LED configuration */
			SetAllLEDs(leds);
		}
	}
}

/*
 * Transmit a sound from the data array using a polling loop.
 *  Set echo to 1, if the transmission with echo is needed
 */
void transmitSoundSamples(short* data, short len, char echo)
{
	LEDcolour leds[8] = {OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF}; //LED array with LEDs turned off
	float attenuationFactor = 0.8;

	/* Turn all LEDs off */
	SetAllLEDs(leds);

	/* Transmit a data in mono. Each second sample is zero. */
	for (long i = 0; i < 2 * len; i++)
	{
		if (i % 2)
			*AT91C_SSC_THR = 0;
		else
			if (echo)
			{
				/*
				 * If echo is set, then delay the first half of the data,
				 * attenuate it, and add it to the second half of the initial data
				 */
				if (i < len)
					*AT91C_SSC_THR = data[i / 2]; //transmit as usual during the first half
				else
					*AT91C_SSC_THR = data[i / 2] + (short) (attenuationFactor * data[(i / 2) - (len / 2)]); //add the delayed attenuated data during the second half
			}
			else
				*AT91C_SSC_THR = data[i / 2]; // else transmit everything as normal

		/* Turn on LEDs during the recording in equal intervals of time. A single LED at a time. */
		if (i % (2 * len / 8) == 0)
		{
			/* Set red the next LED */
			setNextLOtoHI(leds);
			/* Turn on the new LED configuration */
			SetAllLEDs(leds);
		}

		/* Wait until the transmit register is ready */
		while (!(*AT91C_SSC_SR & AT91C_SSC_TXRDY));
	}
}
