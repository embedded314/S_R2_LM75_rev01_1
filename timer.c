#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "timer.h"

void TCA_init( uint16_t period ) {
	TCA0.SINGLE.CMP0 = period;
	
	//TCA0.SINGLE.INTCTRL |= (1 << 0);	//enable OVF interrupt
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | (1 << TCA_SINGLE_ENABLE_bp);
	TCA0.SINGLE.CTRLB |= (1 << 4);
	TCA0.SINGLE.INTCTRL |= (1 << 4);
}
