/*
 * initialize.h
 *
 * Created: 3/22/2012 12:50:44 AM
 *  Author: Milad
 */ 


#ifndef INITIALIZE_H_
#define INITIALIZE_H_
#include <asf.h>
#include "nrf24l01_L.h"
#include "nrf24l01_R.h"

//#define F_CPU 32000000UL


// void En_RC32M(void);
// void PORT_init(void);
void port_init(void);

// void TimerD0_init(void);
// 
// void TimerE0_init(void);
// 
// void TimerE1_init(void);

void tc_init(void);


/*#define USART_BUADRATE 115200*/
// void USART_R_init(void);
// void USART_L_init(void);
void usart_init(void);
void spi_init(void);
void nrf_init(char *Address);

// void OUT_Bling(PORT_t *OUT_PORT,uint8_t OUT_PIN_bp,uint8_t Speed,uint32_t *Time_ON,uint32_t time_ms);

#endif /* INITIALIZE_H_ */