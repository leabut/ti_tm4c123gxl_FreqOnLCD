/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * For further information for that class
 * see int main(void)
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"

//classes for Kentec QVGA LCD Display
#include "Kentec320x240x16_ssd2119_8bit.h"
//calculating class for measuring the external frequency
#include "measure.h"

//bootloader image from TI Workshop
extern const uint8_t g_pui8Image[];
//display grlib base
tContext sContext;
//clearing rectangle
tRectangle sRect;

//freqString from measure.h
extern char freqString[8];

//initializing the timer, interrupts and GPIO
extern void initTimerAndGPIO();
//measuring method for external frequency
extern void calc();

/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * This method is writing a rectangle sRect over
 * the whole LCD so it appears as the LCD would
 * be cleaned
 */
void ClrScreen() {
	sRect.i16XMin = 0;
	sRect.i16YMin = 0;
	sRect.i16XMax = 319;
	sRect.i16YMax = 239;
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &sRect);
	GrFlush(&sContext);
}

/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * Is waiting for system cycles in milliseconds
 * 1 second = 1000 milliseconds
 */
void delayMS(int ms) {
	SysCtlDelay((SysCtlClockGet() / (3 * 1000)) * ms);
}

/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * Initializes the Kentec QVGA LCD
 * Sets the GPIO and sContext from grlib
 */
void initLCD() {
	Kentec320x240x16_SSD2119Init();
	GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
	ClrScreen();
}

/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * Draws the boot image from TI Workshop onto LCD
 */
void drawIMGLCD() {
	GrImageDraw(&sContext, g_pui8Image, 0, 0);
	GrFlush(&sContext);
}

/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * Draws bootloader test strings to the LCD
 *
 */
void drawOntoLCD() {
	sRect.i16XMin = 1;
	sRect.i16YMin = 1;
	sRect.i16XMax = 318;
	sRect.i16YMax = 238;
	GrContextForegroundSet(&sContext, ClrRed);
	GrContextFontSet(&sContext, &g_sFontCmss30b);
	GrStringDraw(&sContext, "", -1, 110, 2, 0);
	GrStringDraw(&sContext, "Test", -1, 80, 32, 0);
	GrStringDraw(&sContext, "Testing LCD", -1, 100, 62, 0);
	GrStringDraw(&sContext, "Test", -1, 135, 92, 0);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);
	GrFlush(&sContext);
}

/**
 * @author TI Launchpad Workshop
 * @editor leabut
 *
 * Is drawing continously in while(1) loop onto LCD
 * Is refreshing the freqString and LCD in general
 */
void drawOntoLCD2() {
	sRect.i16XMin = 1;
	sRect.i16YMin = 1;
	sRect.i16XMax = 318;
	sRect.i16YMax = 238;
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);
	GrFlush(&sContext);

	GrContextForegroundSet(&sContext, ClrRed);
	GrContextFontSet(&sContext, &g_sFontCmss30b);
	GrStringDraw(&sContext, "", -1, 110, 2, 0);
	GrStringDraw(&sContext, "Output in Hz:", -1, 80, 32, 0);
}

/**
 * @author leabut
 *
 * Is setting the system clock as well as
 * controlling the whole program
 * It sets the GPIO ports of Kentec QVGA LCD Display
 * as well as the interrupts, timers and GPIO ports of
 * measure.h
 *
 * For further information about calculating external frequencies
 * goto measure.h
 */
int main(void) {

	//setting system clock
	SysCtlClockSet(
			SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_8MHZ);

	//using measure class for enabling GPIO Timer0 and Timer2
	//and enabling the UART for debug prints
	initTimerAndGPIO();

	//starting LCD
	initLCD();
	//draw bootscreen image ot LCD
	drawIMGLCD();

	//delay 1 Sec
	SysCtlDelay(SysCtlClockGet());

	//clear the screen
	ClrScreen();

	//draw static information to LCD
	drawOntoLCD();

	//delay 1 Sec
	SysCtlDelay(SysCtlClockGet());

	// and here
	ClrScreen();

	//draw other static information to LCD
	drawOntoLCD2();

	//initialize clear rectangle for freqString
	tRectangle tRect;
	tRect.i16XMin = 90;
	tRect.i16YMin = 60;
	tRect.i16XMax = 150;
	tRect.i16YMax = 90;

	while (1) {

		//calc frequency
		calc();

		//clear freqString content on LCD
		GrContextForegroundSet(&sContext, ClrBlack);
		GrRectDraw(&sContext, &tRect);
		GrRectFill(&sContext, &tRect);

		//write freqString onto LCD
		GrContextForegroundSet(&sContext, ClrRed);
		GrStringDraw(&sContext, freqString, -1, 100, 62, 0);
		GrStringDraw(&sContext, "", -1, 135, 92, 0);

		//wait
		delayMS(10);
	}
}
