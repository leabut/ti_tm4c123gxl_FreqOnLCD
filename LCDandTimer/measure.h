//default includes
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

//timer and interrupts
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

//uart includes
#include <driverlib/uart.h>
#include <utils/uartstdio.c>

//*****************************************************************************
//
// Flags that contain the current value of the interrupt indicator as displayed
// on the OLED display.
//
//*****************************************************************************
//#define CCP2_PERIPH             SYSCTL_PERIPH_GPIOD
unsigned long g_ulFlags;
//unsigned long count;
unsigned long timer;
char freqString[8];

/*
 * 	Function prototype
 */
void calc();

//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
void Timer0IntHandler(void) {	//
								// Clear the timer interrupt.
								//
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//
	// Get the counter value
	//
	timer = TimerValueGet(TIMER2_BASE, TIMER_A);

	//
	// Reset the counter value to 10000
	//
	TimerLoadSet(TIMER2_BASE, TIMER_A, 10000);

}

//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void Timer2IntHandler(void) {
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void ConfigureUART(void) {
	//
	// Enable the GPIO Peripheral used by the UART.
	//
	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);

	//
	// Enable UART0
	//
	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_UART0);

	//
	// Configure GPIO Pins for UART mode.
	//
	ROM_GPIOPinConfigure (GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure (GPIO_PA1_U0TX);

	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Use the internal 16MHz oscillator as the UART clock source.
	//
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	//
	// Initialize the UART for console I/O.
	//
	UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// This example application demonstrates the use of the timers to generate
// periodic interrupts.
//
//*****************************************************************************
void initTimerAndGPIO(void) {
	//enabling UART for Serialprint/UARTprintf
	ConfigureUART();

	// Enable the peripherals used
	SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER2);

	//
	// Enable the B peripheral used by the TIMER2 pin T2CCP0
	//
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);

	//
	// This function takes one of the valid names for a Timer pin and configures
	// the pin for its Timer functionality depending on the part that is defined.
	//
	GPIOPinConfigure (GPIO_PB0_T2CCP0);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_0);

	// Enable processor interrupts.
	IntMasterEnable();

	// Configure the two 32-bit periodic timers.
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_A_CAP_COUNT);

	// This function configures the timer load value; if the timer is running
	// then the value is immediately loaded into the timer.
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 10); //8000000/10=80000=100 milliseconds

	//TimerLoadSet(TIMER1_BASE, TIMER_A,10000);
	// This function configures the signal edge(s) that triggers the
	// timer when in capture mode.
	TimerControlEvent(TIMER2_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);

	// Setup the interrupt for the Timer0-TimerA timeouts.
	IntEnable (INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Enable the timers.
	TimerEnable(TIMER0_BASE, TIMER_A);
	TimerEnable(TIMER2_BASE, TIMER_A);

	// While the timers runs we are going to make the calculations (We have to take care to keep the processing time under the interrupt duration).
	/*
	 while(1)
	 {
	 calc();
	 }
	 */
}

/**
 * This method is casting integers/floats/doubles into strings
 * the char* result is the resulting string
 */
void itoa(long unsigned int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) {
		*result = '\0';
	}
	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;
	do {
		tmp_value = value;
		value /= base;
		*ptr++ =
				"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35
						+ (tmp_value - value * base)];
	} while (value);
	// Apply negative sign
	if (tmp_value < 0)
		*ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
}

/**
 * This method is calculating the frequency
 * of the given input
 *
 */
void calc() {
	//amount of pulses counted
	unsigned long count;
	//factor to convert to seconds
	//in this case 100ms * 10 = 1 sec
	float time = 10;
	//frequency in Hz
	float freq;

	//10000 - timer = 0.001 Hz
	//0.001 Hz * 100 = 1 Hz
	//multiply by 2 because of correction
	count = (10000 - timer) * 10000 * 2;
	//calculate the frequency by amount_of_pulses / time
	freq = count / time;

	if (freq < 1) {
		freq = 0;
	}

	UARTprintf("Frequency in Hz: ");
	//cast float to string
	itoa(freq, freqString, 10);
	UARTprintf(freqString);
	UARTprintf("\n");
}
