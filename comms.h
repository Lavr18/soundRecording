/*
 * comms.h
 *
 *  Created on: 8 Dec 2018
 *      Author: Aliaksei Laurynovich
 */
#include <config/AT91SAM7S256.h>
#include <board/I2C-lib.h>
#include <AudioDAC.h>
#include "LEDFunctions.h" 			/* Provide access to the LEDs */

#ifndef COMMS_H_
#define COMMS_H_

/*
 * Write a value to the specified Audio DAC register in a format:
 * top 7 bits being the register address, and the bottom 9 bits being the register value.
 */
void WriteAudioDACreg(unsigned char reg, unsigned short value);

/* Function to make SSC work like I2S. Initialise it as a transmitter and as a receiver. */
void SSC_initialise(void);

/* Receive a sound and store it in the array using a polling loop */
void receiveSoundSamples(short* data, short len);

/*
 * Transmit a sound from the data array using a polling loop.
 *  Set echo to 1, if the transmission with echo is needed
 */
void transmitSoundSamples(short* data, short len, char echo);



#endif /* COMMS_H_ */
