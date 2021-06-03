#include <avr/interrupt.h>

#include "main.h"
#include "TWI.h"

#define TWI0_BAUD(F_SCL, T_RISE)	((((((float)20000000.000 / (float)F_SCL)) - 10 - ((float)20000000.000 * T_RISE / 1000000))) / 2)

void TWI_Init()
{
	TWI0.MBAUD = (uint8_t)TWI0_BAUD(100000, 0); /* set MBAUD register */
	TWI0.MCTRLB |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
}
void TWI_Enable()
{
	TWI0.MCTRLA = 1<<TWI_SMEN_bp | 1 << TWI_ENABLE_bp;
	TWI0_MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}
char TWI_Start(char addr)
{
	if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_BUSY_gc)
	{
		TWI0.MCTRLB &= ~(1<<TWI_ACKACT_bp);
		TWI0.MADDR = addr;
		
		if (addr & 1)
		{
			while (!(TWI0_MSTATUS & TWI_RIF_bm));
		}
		else
		{
			while (!(TWI0_MSTATUS & TWI_WIF_bm));
		}
		return TWI0.MSTATUS;
	}
	else
	return TWI0.MSTATUS;
}
uint8_t TWI_Read(uint8_t *c, char ACKorNACK)
{
	if ((TWI0.MSTATUS & TWI_BUSSTATE_gm)==TWI_BUSSTATE_OWNER_gc)
	{
		while (!(TWI0.MSTATUS & TWI_RIF_bm));
		
		if (ACKorNACK)
		TWI0.MCTRLB &= ~(1<<TWI_ACKACT_bp);	//Send ACK
		else
		TWI0.MCTRLB |= 1<<TWI_ACKACT_bp;	//Send NACK
		
		c = TWI0.MDATA;
		
		return (uint16_t)c;
	}
	else
	return TWI0.MSTATUS;
}
uint8_t timeout_cnt;
#define WRITE_TIMEOUT	5
uint8_t TWI_Write(uint8_t *data)									// write data, return status
{
	timeout_cnt = 0;												// reset timeout counter, will be incremented by ms tick interrupt
	if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_OWNER_gc)	// if master controls bus
	{
		TWI0.MDATA = *data;
		while (!(TWI0.MSTATUS & TWI_WIF_bm))						// wait until WIF set, status register contains ACK/NACK bit
		{
			if (timeout_cnt > WRITE_TIMEOUT) return 0xff;			// return timeout error
		}
		if (TWI0.MSTATUS & TWI_BUSERR_bm) return 4;					// Bus Error, abort
		if (TWI0.MSTATUS & TWI_RXACK_bm) return 1;					// Slave replied with NACK, abort
		return 0;													// no error
	}
	else return 8;													// master does not control bus
}
void TWI_Stop(void)
{
	TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}
void TWI_ReStart(void)
{
	TWI0.MCTRLB |= TWI_MCMD_REPSTART_gc;
}
