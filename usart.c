#include <avr/io.h>
#include "main.h"
#include "timer.h"
#include "usart.h"

uint16_t testVal;
uint8_t testValA, testValB;

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
//	UART 9600 8 N 1
uint8_t TX_frame ( void ) {
	//	przejscie przez tablice ACK i wysylanie ramek
	//  wykonuj dopoki nie trafisz na flage == 1;
	while ((UART_Tx_req_tab == 0) && (UART_frame_que_cnt < sizeof (UART_Tx_req_tab))) {
		UART_frame_que_cnt++;
	}
	if (UART_Tx_req_tab [UART_frame_que_cnt] == 1) {
		switch (UART_frame_que_cnt) {
			case 0:
				COMM_UART_Tx ( 0x0A, id_T_val, sensor_data.T_val);
				break;
			case 1:
				COMM_UART_Tx ( 0x0A, id_L_val, sensor_data.L_val);
				break;
			case 2:
				COMM_UART_Tx ( 0x0A, id_H_val, 0);
				break;
			case 3:
				COMM_UART_Tx ( 0x32, 0x00, 0x00);
				break;
		}
	}
	if (UART_frame_que_cnt < sizeof (UART_Tx_req_tab)) UART_frame_que_cnt++; else UART_frame_que_cnt = 0;

	return 0;
}
uint16_t COMM_UART_Tx ( volatile uint8_t frame_type, volatile uint16_t register_code, volatile uint16_t data_to_send) {
	USART_Transmit( 0x00 );    //  synchro
	USART_Transmit( 0x00 );	
	//  [0] rodzaj ramki
	USART_Transmit( frame_type );
	//  ---- source Id 3B + 2B
	#define ID1 0b00000001  //  0x01 -closed area controllers
	#define ID2 0b00000000  //  0x00 -PASV (read only)
	#define ID3 0b00000010  //  0x02 -sensor T,L
	USART_Transmit( ID1 );
	USART_Transmit( ID2 );
	USART_Transmit( ID3 );
	
	//  2 * 1B	serial ID
	USART_Transmit( SIGROW_SERNUM0 );
	USART_Transmit( SIGROW_SERNUM1 );

	//	---- dest ID 3B + 2B
	#define DEST_ID1 0b00000001 //  0x01
	#define DEST_ID2 0b00001000 //  0x08
	#define DEST_ID3 0b00000010 //  0x02
	USART_Transmit( DEST_ID1 );
	USART_Transmit( DEST_ID2 );
	USART_Transmit( DEST_ID3 );
	//  2 * 1B	serial ID
	USART_Transmit( 0x00 );    //  0x00	received remote Id
	USART_Transmit( 0x00 );    //  0x00 received remote Id
	
	//	requested DATA ID
	volatile uint8_t frame_crc = 0;
	//  ilosc bajtow w ramce = 5
	USART_Transmit( 0x05 );
	//  polecenie   0x12
	USART_Transmit( 0x12 );
	
	//  rejestr
	testVal = 0;
	testValA = 0;
	testVal  = register_code;
	testValA = (testVal >> 8);
	testValB = testVal & 0xff;

	frame_crc += testValA;
	frame_crc += testValB;

	//  kod rejestru, 16bit
	USART_Transmit( testValA ); //  0x00
	USART_Transmit( testValB ); //  0x01
	
	testVal = 0;
	testValA = 0;
	testVal  = data_to_send;
	//  wartosc do zmiany
	testValA = (testVal >> 8);
	testValB = testVal & 0xff;;

	frame_crc += testValA;
	frame_crc += testValB;

	//  dane do wyslania, 16bit 366 = 0x
	USART_Transmit( testValA );  //  0x01
	USART_Transmit( testValB );  //  0x6E
	
	//  crc dla
	USART_Transmit( frame_crc );    //  0xFF
	
	return register_code;
}
void USART_init( void ) {
	//	Set pin 6 as a TX line 
	PORTA.DIR |= PIN6_bm;
	PORTA.OUT |= PIN6_bm;
	
	//	Set pin 7 as a RX line 
	PORTA.DIR &= ~PIN7_bm;

	USART0.BAUD =  (uint16_t)USART0_BAUD_RATE(9600);

	USART0.CTRLA = 0 << USART_ABEIE_bp /* Auto-baud Error Interrupt Enable: disabled */
	| 0 << USART_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
	| 0 << USART_LBME_bp /* Loop-back Mode Enable: disabled */
	| USART_RS485_OFF_gc /* RS485 Mode disabled */
	| 0 << USART_RXCIE_bp /* Receive Complete Interrupt Enable: enabled */
	| 0 << USART_RXSIE_bp /* Receiver Start Frame Interrupt Enable: disabled */
	| 0 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */

	USART0.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
	| 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
	| 0 << USART_RXEN_bp     /* Receiver enable: enabled */
	| USART_RXMODE_NORMAL_gc /* Normal mode */
	| 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
	| 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */
}
void USART_Transmit(char c)
{
	while (!(USART0.STATUS & USART_DREIF_bm))
	{
		;
	}
	USART0.TXDATAL = c;
}/**
 * \brief Check if USART_0 data is transmitted
 *
 * \return Receiver ready status
 * \retval true  Data is not completely shifted out of the shift register
 * \retval false Data completely shifted out if the USART shift register
 */
uint8_t USART_0_is_tx_busy()
{
	return (!(USART0.STATUS & USART_TXCIF_bm));
}
void USART_TX_que_init ( void ) {
	//	Tx array initialization
	for (uint8_t i=0;i<sizeof(UART_Tx_req_tab);i++) {
		UART_Tx_req_tab [i] = 1;
	}
}