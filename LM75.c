#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/delay.h>

#include "TWI.h"
#include "LM75.h"

#define LM75_ADDRESS_W               0b10010000
#define LM75_ADDRESS_R               0b10010001
#define LM75_TEMP_REGISTER           0b00000000
#define LM75_CONFIG_REGISTER         0b00000001
#define LM75_THYST_REGISTER          0b00000010
#define LM75_TOS_REGISTER            0b00000011


#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR 0b1001000
#define LM75_REG_TEMP (0x00) // Temperature Register

volatile uint8_t MSB, LSB;

void TWI_read_LM75 ( uint16_t *T_val ) {
	char *wskM = MSB;
	char *wskL = LSB;
	volatile uint16_t T_val_LM;

 	TWI_Start (0b10010001);
 	MSB = TWI_Read(wskM, 1);
 	LSB = TWI_Read(wskL, 0);
  	TWI_Stop();

	//	oblicz temperature 11 bitow z LM75
	//*(T_val) =  
	volatile uint16_t T_val_shit;
	T_val_shit = ((MSB << 8) | LSB);
	
	T_val_LM = T_val_shit >> 5;
	volatile float T_val_c;
	T_val_c = (float)T_val_LM * 0.125;
	*(T_val) = (T_val_c * 10);
}
