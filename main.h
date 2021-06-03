#ifndef MAIN_H_
#define MAIN_H_

//	Required sensor data structure.
volatile struct {
	uint16_t T_val;
	uint16_t L_val;
	uint8_t CRC_val;
} sensor_data;

#define T_VAL_BUFFER_SIZE	6

volatile uint16_t T_val_buff [ T_VAL_BUFFER_SIZE ];
volatile uint16_t T_val_avg [ 6 ];
volatile uint8_t T_val_read_cnt;

volatile uint16_t avg_val;

void T_sensor_read ( void );
void CPU_init ( void );
void WDT_init ( void );

#endif /* MAIN_H_ */