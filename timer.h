#ifndef TMR0_H_
#define TMR0_H_


volatile uint8_t TCB0_int_flag;

int8_t TCB0_init_measure_mode();
int8_t TCB0_init_1ms();
void TCA_init( uint16_t period );

#endif /* TMR0_H_ */