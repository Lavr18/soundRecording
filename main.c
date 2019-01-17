/*
 * CE865 Assignment 2.
 *
 * Date: 	08.12.2018.
 * Author: 	Aliaksei Laurynovich.
 * Info: 	Records and plays the sound for around 3 seconds,
 * 		 	the sound can be played with echos as well.
 */

#include <board/Button-lib.h> 		/* Provide access to buttons */
#include "LEDFunctions.h" 			/* Provide access to the LEDs */
#include "comms.h"					/* Provide access to communication functions */

#define ARRAY_LENGTH 25000			/* The length of an array storing the audio samples */

/* Contains initialisations and the infinite loop with functionalities */
int main()
{
	short dataBuffer[ARRAY_LENGTH]; //The sound data is stored here
	LEDcolour ledVector[8] = {OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF}; //LED array with LEDs turned off

	/* First set the I2C bus speed to 16kbps. */
	SetI2CBusSpeed(16000);

	/*
	 * Now configure the Synchronous Serial Controller (SSC) interface.
	 * This is configured to perform the same as the I2S audio interface.
	 */
	SSC_initialise();

	/* Now setup the Audio DAC registers. */
	WriteAudioDACreg(AIC_REG_RESET, 0x00);
	WriteAudioDACreg(AIC_REG_POWER, (0 << 1)); //Turn on the microphone
	WriteAudioDACreg(AIC_REG_LINVOL, 0x117);
	WriteAudioDACreg(AIC_REG_RINVOL, 0x117);

	/* Set up sampling rate. */
	WriteAudioDACreg(AIC_REG_SRATE, AIC_SR1 | AIC_SR0 | AIC_USB);  // 8k sample rate

	WriteAudioDACreg(AIC_REG_AN_PATH, AIC_DAC | AIC_MICM); // enable DAC,input select=Line
	WriteAudioDACreg(AIC_REG_DIG_PATH, 0 << 3); // disable soft mute
	WriteAudioDACreg(AIC_REG_DIG_FORMAT, (1 << 6) | 2); // master, I2S left aligned
	WriteAudioDACreg(AIC_REG_DIG_ACT, 1 << 0); // activate digital interface

	/* Turn all LEDs off */
	SetAllLEDs(ledVector);

	/*
	 * Infinite while loop, where buttons 1 and 2 are monitored.
	 * If btn1 is pressed, start receiving the data and reading it to the data buffer.
	 * If btn2 is pressed, start transmitting the data from the data buffer without the echo.
	 * If btn3 is pressed, start transmitting the data from the data buffer with the echo.
	 */
	while (1)
	{
		if (IsButtonPressed(BUTTON1))
			receiveSoundSamples(dataBuffer, ARRAY_LENGTH);

		else if (IsButtonPressed(BUTTON2))
			transmitSoundSamples(dataBuffer, ARRAY_LENGTH, 0);

		else if (IsButtonPressed(BUTTON3))
			transmitSoundSamples(dataBuffer, ARRAY_LENGTH, 1);
	}
	return 0;
}
