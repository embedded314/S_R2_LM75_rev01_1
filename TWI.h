#ifndef TWI_H_
#define TWI_H_

void TWI_Init();
void TWI_Enable();
char TWI_Start(char addr);
uint8_t TWI_Read(uint8_t * c, char ACKorNACK);
uint8_t TWI_Write(uint8_t *data);
void TWI_Stop(void);
void TWI_ReStart(void);


#endif /* TWI_H_ */