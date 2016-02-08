#include <asf.h>
#include <stdio.h>
#include <string.h>
#include "init.h"
#include "transmitter.h"

void stoping_robot (void);
void Timer_on (void) ;
void Timer_show (void) ;

char Buf_Tx[2][Max_Robot][_Buffer_Size] ;
char Buf_Rx[Max_Robot][_Buffer_Size];
char Address[_Address_Width] = { 0x11, 0x22, 0x33, 0x44, 0x55};
int  Robot_Select ;
int  LED_time;
uint8_t r_side ;
uint8_t l_side ;
uint16_t pck_timeout[2][Max_Robot];
uint8_t side	;
//! main variables
int16_t r_id = 0 , l_id = 6 ;
uint16_t timer;

int main (void)
{
    sysclk_init();
	pmic_init();
    port_init();
	tc_init();
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_32CLK);
	wdt_enable();
	usart_init();
	spi_init();
	
	char str[200];
	uint8_t count ;
	count = sprintf(str,"RESET");
	for (uint8_t i=0;i<count;i++)
	usart_putchar(&USARTE0,str[i]);
	nrf_init(Address);
	sei();
	

	for (uint8_t i=0;i<Max_Robot;i++)
	{
		Robot_D_tmp[2][i].RID=12;
	}

	while (1)
	{
		
	}
}

// running time : 8755 clk
ISR(TCC0_OVF_vect)
{
	wdt_reset();
	LED_time++;
	if (LED_time > 10)
	{
		LED_White_R_PORT.OUTCLR = LED_White_R_PIN_bm;
		LED_White_L_PORT.OUTCLR = LED_White_L_PIN_bm;
		LED_Green_R_PORT.OUTCLR = LED_Green_R_PIN_bm;
		LED_Green_L_PORT.OUTCLR = LED_Green_L_PIN_bm;
		
		LED_time=0;
	}

	
	if (TCE0_CNT > 20)
	{
		TCE0_CNT = 0;
		/////////////////////////////////////////////
		r_id ++ ;
		if (r_id == 12)
		{
			r_id = 0 ;
		}
		
		l_id = (r_id + 6)%12;
		///////////////////////////////////////////////
		if (Menu_PORT.IN & Menu_Side_Select_PIN_bm)
		{
			side = L ;
		}
		else
		{
			side = R ;
		}
		
		if (r_id == 6)
		{
			NRF24L01_R_Set_CH(_CH_L);
			r_side = side ;
			
			NRF24L01_L_Set_CH(_CH_R);
			l_side = R;
		}
		else if (r_id == 0)
		{
			NRF24L01_R_Set_CH(_CH_R);
			r_side = R;
			
			NRF24L01_L_Set_CH(_CH_L);
			l_side = side ;
		}
		
		
		Address[4] =   ((r_id) << 4) | r_id ;
		/*		NRF24L01_R_Set_RX_Pipe(0, Address, 5, 32);*/
		NRF24L01_R_WriteRegBuf(W_REGISTER | (RX_ADDR_P0), Address, _Address_Width);
		NRF24L01_R_Set_TX_Address(Address, 5); // Set Transmit address
		NRF24L01_R_Write_TX_Buf(Buf_Tx[r_side][r_id], _Buffer_Size);
		NRF24L01_R_RF_TX();
		
		Address[4] =   ((l_id) << 4) | l_id ;
		/*NRF24L01_L_Set_RX_Pipe(0, Address, 5, 32);*/
		NRF24L01_L_WriteRegBuf(W_REGISTER | (RX_ADDR_P0), Address, _Address_Width);
		NRF24L01_L_Set_TX_Address(Address, 5); // Set Transmit address
		NRF24L01_L_Write_TX_Buf(Buf_Tx[l_side][l_id], _Buffer_Size);
		NRF24L01_L_RF_TX();
		
	}
	stoping_robot();
	while(Menu_PORT.IN & Menu_Side_Switch_PIN_bm);
	wdt_reset();
}

ISR(PRX_R)//ID:0=>5
{
	wdt_reset();
	uint8_t status_R = NRF24L01_R_ReadReg(STATUSe);
	if((status_R & _RX_DR) == _RX_DR)
	{
		LED_White_R_PORT.OUTSET = LED_White_R_PIN_bm;
		//		tmprid = ((status_R&0x0e)>>1);
		//1) read payload through SPI,
		NRF24L01_R_Read_RX_Buf(Buf_Rx[Robot_Select], _Buffer_Size);
		//2) clear RX_DR IRQ,
		status_R=NRF24L01_R_WriteReg(W_REGISTER | STATUSe, _RX_DR );
		//3) read FIFO_STATUS to check if there are more payloads available in RX FIFO,
		//4) if there are more data in RX FIFO, repeat from step 1).Buf_Tx[R]
		
		if ( Robot_Select == r_id)
		{
			char str[200];
			uint8_t count ;
			count = sprintf(str,"%d,%d,%d,%d,%d\r",
			((int)(Buf_Rx[Robot_Select][0]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][1]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][2]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][3]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][4]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][5]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][6]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][7]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][8]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][9]) & 0x0ff));
			for (uint8_t i=0;i<count;i++)
			usart_putchar(&USARTE0,str[i]);
		}

	}
	
	status_R = NRF24L01_R_WriteReg(W_REGISTER|STATUSe,_TX_DS|_MAX_RT);
	if((status_R&_TX_DS) == _TX_DS)
	{
		LED_Green_R_PORT.OUTSET = LED_Green_R_PIN_bm;
	}
	
	if ((status_R&_MAX_RT) == _MAX_RT)
	{
		NRF24L01_R_Flush_TX();
		LED_White_R_PORT.OUTSET = LED_White_R_PIN_bm;
	}
}

ISR(PRX_L)//ID:6=>11
{
	wdt_reset();
	uint8_t status_L = NRF24L01_L_ReadReg(STATUSe);
	if((status_L & _RX_DR) == _RX_DR)
	{
		LED_White_L_PORT.OUTSET = LED_White_L_PIN_bm;
		//		tmprid = ((status_L&0x0e)>>1);
		//1) read payload through SPI,
		NRF24L01_L_Read_RX_Buf(Buf_Rx[Robot_Select], _Buffer_Size);
		//2) clear RX_DR IRQ,
		status_L=NRF24L01_L_WriteReg(W_REGISTER | STATUSe, _RX_DR );
		//3) read FIFO_STATUS to check if there are more payloads available in RX FIFO,
		//4) if there are more data in RX FIFO, repeat from step 1).
		
		if (Robot_Select == l_id)
		{
			char str[200];
			uint8_t count ;
			count = sprintf(str,"%d,%d,%d,%d,%d\r",
			((int)(Buf_Rx[Robot_Select][0]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][1]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][2]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][3]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][4]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][5]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][6]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][7]) & 0x0ff),
			((int)(Buf_Rx[Robot_Select][8]<<8) & 0xff00) | ((int)(Buf_Rx[Robot_Select][9]) & 0x0ff));
			for (uint8_t i=0;i<count;i++)
			usart_putchar(&USARTE0,str[i]);
		}

	}
	
	status_L = NRF24L01_L_WriteReg(W_REGISTER|STATUSe,_TX_DS|_MAX_RT);
	if((status_L&_TX_DS) == _TX_DS)
	{
		LED_Green_L_PORT.OUTSET = LED_Green_L_PIN_bm;
	}
	if ((status_L&_MAX_RT) == _MAX_RT)
	{
		NRF24L01_L_Flush_TX();
		LED_White_L_PORT.OUTSET = LED_White_L_PIN_bm;

	}
}

ISR(USART_R_RXC_vect)
{
	GetNewData(USARTC0_DATA,R);
}
ISR(USART_L_RXC_vect)
{

	if (Menu_PORT.IN & Menu_Side_Select_PIN_bm)
	{
		GetNewData(USARTE0_DATA,L);
	}
	else
	{
		char data;
		data=USARTE0_DATA;
		
		switch (data)// used character : {w,s,123456(for robot id),!@#$(for motors id)}
		{
			
			
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			Robot_Select = data - '0' ;
			break;
			
			case '`'://non of robots send ACK to wireless board
			Robot_Select = 12 ;//- '0';
			NRF24L01_L_WriteReg(W_REGISTER | EN_AA, 0x00);
			NRF24L01_R_WriteReg(W_REGISTER | EN_AA, 0x00);
			break;

			
		};
		
	}


}

// running time : 1411 clk
void stoping_robot(void)
{
	for (uint8_t i=0;i<Max_Robot;i++)
	{
		int side;
		if (Menu_PORT.IN & Menu_Side_Select_PIN_bm)
		{
			if(i<6)side = R;
			if(i>5)side = L;
		}
		else
		{
			side = R;
		}

		//if there is no order from pc for a robot
		//an order of brake and then free wheel will
		//be send to that robot
		pck_timeout[side][i]++;
		if (pck_timeout[side][i]>=300)
		{
			if(pck_timeout[side][i]<=600)
			{//brake order
				Buf_Tx[side][i][0] = i;
				Buf_Tx[side][i][1] = 0;
				Buf_Tx[side][i][2] = 1;
				Buf_Tx[side][i][3] = 0;
				Buf_Tx[side][i][4] = 1;
				Buf_Tx[side][i][5] = 0;
				Buf_Tx[side][i][6] = 1;
				Buf_Tx[side][i][7] = 0;
				Buf_Tx[side][i][8] = 1;
				Buf_Tx[side][i][9] = 0;
				Buf_Tx[side][i][10] = 0;
			}
			else
			{//freewheel order
				Buf_Tx[side][i][0] = i;
				Buf_Tx[side][i][1] = 1;
				Buf_Tx[side][i][2] = 2;
				Buf_Tx[side][i][3] = 3;
				Buf_Tx[side][i][4] = 4;
				Buf_Tx[side][i][5] = 0;
				Buf_Tx[side][i][6] = 0;
				Buf_Tx[side][i][7] = 0;
				Buf_Tx[side][i][8] = 0;
				Buf_Tx[side][i][9] = 0;
				Buf_Tx[side][i][10]= 0;
			}
		}
	}
}


//starting counter
void Timer_on(void)
{
	TCE1_CNT = 0 ;
}

//stopping counter and showing time through USART
//running time : about 26400 clk
void Timer_show (void)
{
	timer = TCE1_CNT - 17; // 17 clk is for excessive clk counted
	char str[200];
	uint8_t count ;
	count = sprintf(str,"                               %u\r",timer);
	for (uint8_t i=0;i<count;i++)
	usart_putchar(&USARTE0,str[i]);
}