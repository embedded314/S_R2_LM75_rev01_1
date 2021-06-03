#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <util/delay.h>

#define FRAME_QUE_SIZE	3	//	maximum number of frames to be sent.

#define T_val_q		0
#define H_val_q		1
#define L_val_q		2

#define id_config_req_ACK   0x32
#define id_T_val    0x0011
#define id_H_val    0x0021
#define id_L_val    0x0049




volatile uint8_t USART_irq_flag;

volatile uint8_t UART_Tx_req_tab [4];
volatile uint8_t UART_frame_que_cnt;

volatile uint8_t GW_UART2_frame_decoder_busy_flag, GW_UART2_frame_byte_pos;
volatile uint8_t GW_UART2_Rx_buff [20];

void USART_init( void );
void USART_Transmit(char c);
void USART_ISR_Interrupt(void);
uint8_t TX_frame ( void );
void RX_frame ( void );


uint16_t COMM_UART_Tx ( volatile uint8_t frame_type, volatile uint16_t register_code, volatile uint16_t data_to_send);
void USART_TX_que_init ( void );


#endif /* UART_H_ */