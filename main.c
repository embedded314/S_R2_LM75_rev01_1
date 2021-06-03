#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "ADC.h"
#include "usart.h"
#include "TWI.h"
#include "LM75.h"
#include "timer.h"
#include "rtc.h"
#include "main.h"

const uint16_t TCA_period = 0x2000;
const uint16_t RTC_period = 0x0064;

volatile uint32_t UART_TX_timer;
int main(void){
	cli();

	CPU_init();
	
	WDT_init();

	ADC_init();

	USART_TX_que_init ();	
	USART_init();
	
	TWI_Init();
	TWI_Enable();

	TCA_init( TCA_period );

	RTC_init( RTC_period );

	sei();
	
	while (1) {
		//	check for RTC interrupt
		if (RTC.INTFLAGS & 0x2) {
			RTC.INTFLAGS |= 1 << 1;
			//	turn off RTC
			RTC.CTRLA &= ~RTC_RTCEN_bm;

			//	read light sensor value
			sensor_data.L_val = ADC_L_read();

			//	LM75 sensor read procedure
			T_sensor_read();

			//	turn on RTC
			RTC.CTRLA |= (1 << 0);
		}

		//	send to USART one data frame from queue
		if (TCA0.SINGLE.INTFLAGS & 0b00010000){
			TCA0.SINGLE.INTFLAGS |= (1 << 5);	//	Clear TCA0 interrupt
			TCA0.SINGLE.CNT = 0;

			TX_frame();
		}

		wdt_reset();
	}
}

void T_sensor_read ( void ) {
	//	move all data from queue up and add new data at the first position
	for (volatile uint8_t i=0; i < sizeof (T_val_buff) - 1; i++) {
		T_val_buff [ sizeof (T_val_buff) - 1 - i ] = T_val_buff [ sizeof (T_val_buff) - 2 - i ];
		avg_val += T_val_buff [ sizeof (T_val_buff) - 1 - i ];
	}
	
	TWI_read_LM75(&T_val_buff [ 0 ]);
	
	T_val_read_cnt++;

	//	TO DO: make this procedure universal for any array size
	//	calculate average value if data array is full
	if (T_val_read_cnt > 6) {
		avg_val = 0;
		T_val_read_cnt = sizeof (T_val_buff);	//	zatrzymanie licznika
		
		avg_val += T_val_buff [ 0 ];
		avg_val /= sizeof (T_val_buff);
		T_val_buff [ 0 ] = avg_val;
		
		//	copy all data to buffer array and leave original data in source array
		//	otherwise you can loose data order
		for (uint8_t i=0; i < 5; i++) {
			T_val_avg [ i ] = T_val_buff [ i ];
		}
		
		uint8_t swapped;
		do {
			swapped = 0;
			
			for (uint8_t i=0; i < 5 - 1; i++) {
				if (T_val_avg [i] < T_val_avg [i+1]) {
					T_val_avg [i+1] = T_val_avg [i+1] + T_val_avg [i];
					T_val_avg [i] = T_val_avg [i+1] - T_val_avg [i];
					T_val_avg [i+1] = T_val_avg [i+1] - T_val_avg [i];
					
					swapped = 1;
				}
			}
		} while (swapped != 0);
		
		//	find the median of a six-element array.
		sensor_data.T_val = (T_val_avg [2] + T_val_avg [3]) / 2;
	} else {
		//	skim median and send current sensor value if array is not completely filled
		sensor_data.T_val = T_val_buff [0];
	}
	 
	//	set TX frame request flag for T value
	UART_Tx_req_tab [ T_val_q ] = 1;
}

void CPU_init ( void ) {
	//	set CPU speed to 20Mhz
	//	use Configuration Change Protection (CCP)
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLB = 0;
}

void WDT_init ( void ) {
	//	WDT initialization
	CPU_CCP = 0xD8;
	WDT.CTRLA = WDT_PERIOD_8KCLK_gc;	/* 8K cycles (8.2s) */
	CPU_CCP = 0xD8;
}