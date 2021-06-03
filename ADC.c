#include <avr/io.h>
#include "main.h"
#include "ADC.h"

void ADC_init ( void ) {
	PORTA_DIR &= ~(1<<1);
	PORTA_DIR &= ~(1<<2);
	PORTA_DIR &= ~(1<<3);
	
	ADC0.CTRLB = 0x00;
	//	256 CLK delay before sampling
	ADC0.CTRLD |= ADC_INITDLY0_bm || ADC_INITDLY2_bm;
	ADC0.CTRLD |= ADC_SAMPDLY0_bm || ADC_SAMPDLY3_bm;
	ADC0.CTRLC = 0x07;			//	/256 CLK_PER
	ADC0.SAMPCTRL = 0x07;
}
uint16_t ADC_L_read ( void ) {
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
	ADC0.CTRLB = 0x00;
	ADC0.CTRLC |= (1<<4);
	
	ADC0.MUXPOS = 0x03;	//	pin A3

	ADC0.CTRLA  = 0x01;	//	SINGLE, EN
	ADC0.COMMAND = 0x01;
	while (ADC0.COMMAND == 0x01);
	//	10bit result
	uint16_t ADC_val = (ADC0.RES & 0b0000001111111111);
	return ADC_val;
}
