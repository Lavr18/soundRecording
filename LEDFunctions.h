/*
 * LEDFunctions.h
 *
 *  Created on: 7 Nov 2018
 *      Author: Aliaksei Laurynovich
 */

#ifndef LEDFUNCTIONS_H_
#define LEDFUNCTIONS_H_

/*
 * Define the enumerated type LEDnumber
 * LED1 is 0,
 * LED2 is 1,
 * etc
 * LED8 is 7;
 */
typedef enum {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8} LEDnumber;

/*
 * Define the LEDcolour type:
 * OFF is 0
 * GREEN is 1
 * RED is 2
 * ORANGE is 3
 */
typedef enum {OFF, RED, GREEN, ORANGE} LEDcolour;




/* Sets the specified LED to the chosen colour, while all the other LEDs are set to zero. */
void SetLEDcolour(LEDnumber led, LEDcolour colour);

/* Sets all LEDs as in the ColourVector */
void SetAllLEDs(LEDcolour ColourVector[]);

/* The function to set all LEDs off in the vector */
void resetLEDvector(LEDcolour Vector[]);

/* The function to change the next Vector element from LO to HI */
void setNextLOtoHI(LEDcolour Vector[]);


#endif /* LEDFUNCTIONS_H_ */
