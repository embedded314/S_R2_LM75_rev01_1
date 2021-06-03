#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "rtc.h"

void RTC_init(int RTCdelay)
{
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    // 32.768kHz Internal Crystal Oscillator (INT32K)

	while (RTC.STATUS > 0);               // Wait for all register to be synchronized
	RTC.PER = RTCdelay;                   // Set period for delay
	RTC.INTCTRL |= RTC_OVF_bm;            // Enable overflow Interrupt which will trigger ISR
	RTC.CTRLA = RTC_PRESCALER_DIV32_gc    // 32768 / 32 = 1024 (sec) ~ 1 ms
	| RTC_RTCEN_bm                        // Enable: enabled
	| RTC_RUNSTDBY_bm;                    // Run In Standby: enabled
}
