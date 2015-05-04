#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"

#include "Kentec320x240x16_ssd2119_8bit.h"
#include "measure.h"

extern const uint8_t g_pui8Image[];
tContext sContext;
tRectangle sRect;

extern char freqString[8];

extern void initTimerAndGPIO();
extern void calc();

void ClrScreen() {
	sRect.i16XMin = 0;
	sRect.i16YMin = 0;
	sRect.i16XMax = 319;
	sRect.i16YMax = 239;
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &sRect);
	GrFlush(&sContext);
}

void delayMS(int ms) {
	SysCtlDelay((SysCtlClockGet() / (3 * 1000)) * ms);
}

void initLCD() {
	Kentec320x240x16_SSD2119Init();
	GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
	ClrScreen();
}

void drawIMGLCD() {
	GrImageDraw(&sContext, g_pui8Image, 0, 0);
	GrFlush(&sContext);
}

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

int main(void) {

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
