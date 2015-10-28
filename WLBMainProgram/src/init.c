//besmellaherahmanerahim

#include "init.h"

void port_init(void)
{
	PORTB_DIR = Segment_SideL_R_Enable_bm | Segment_SideL_L_Enable_bm | Segment_SideR_R_Enable_bm | Segment_SideR_L_Enable_bm
	| LED_Green_L_PIN_bm | LED_Green_R_PIN_bm | LED_White_R_PIN_bm | LED_White_L_PIN_bm;
	PORTC_DIR = NRF24L01_R_CE_LINE | /*NRF24L01_R_CS_LINE |*/ /*NRF24L01_R_MOSI_LINE |*/ /*NRF24L01_R_SCK_LINE |*/ Wireless_TX_R_PIN_bm;
	PORTC_PIN0CTRL |= PORT_ISC_LEVEL_gc;
	PORTC_INTCTRL  |= PORT_INT0LVL_LO_gc;
	PORTC_INT0MASK |= PIN0_bm;
	
	ioport_configure_port_pin(&PORTC, NRF24L01_R_CS_LINE, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);//?
	//??ioport_configure_port_pin(&PORTC, PIN4_bm, IOPORT_PULL_UP | IOPORT_DIR_INPUT);
	ioport_configure_port_pin(&PORTC, NRF24L01_R_MOSI_LINE, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);
	ioport_configure_port_pin(&PORTC, NRF24L01_R_MISO_LINE, IOPORT_DIR_INPUT);
	ioport_configure_port_pin(&PORTC, NRF24L01_R_SCK_LINE, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);
	
	PORTD_DIR = Buzzer_PIN_bm;
	PORTE_DIR = NRF24L01_L_CE_LINE | NRF24L01_L_CS_LINE | NRF24L01_L_MOSI_LINE | NRF24L01_L_SCK_LINE | Wireless_TX_L_PIN_bm;
	PORTE_PIN0CTRL |= PORT_ISC_LEVEL_gc;
	PORTE_INTCTRL  |= PORT_INT0LVL_LO_gc;
	PORTE_INT0MASK |= PIN0_bm;
	
	ioport_configure_port_pin(&PORTE, NRF24L01_L_CS_LINE, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);//?
	//??ioport_configure_port_pin(&PORTE, PIN4_bm, IOPORT_PULL_UP | IOPORT_DIR_INPUT);
	ioport_configure_port_pin(&PORTE, NRF24L01_L_MOSI_LINE, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);
	ioport_configure_port_pin(&PORTE, NRF24L01_L_MISO_LINE, IOPORT_DIR_INPUT);
	ioport_configure_port_pin(&PORTE, NRF24L01_L_SCK_LINE, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);
	
	PORTF_DIR = Segment_A_bm | Segment_B_bm | Segment_C_bm | Segment_D_bm | Segment_E_bm | Segment_F_bm | Segment_G_bm | Segment_DP_bm;
	Wireless_R_PORT.OUTSET=Wireless_TX_R_PIN_bm;

}

void tc_init(void)
{
	//! Clock 1ms interrupt
	tc_enable(&TCC0);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 125);
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV256_gc);
	
	tc_enable(&TCE0);
	tc_set_wgm(&TCE0,TC_WG_NORMAL);
	tc_write_period(&TCE0,0xffff);
	tc_write_clock_source(&TCE0,TC_CLKSEL_DIV256_gc);
	
	tc_enable(&TCE1);
	tc_set_wgm(&TCE1,TC_WG_NORMAL);
	tc_write_period(&TCE1,0xffff);
	tc_write_clock_source(&TCE1,TC_CLKSEL_DIV256_gc);
}

void usart_init(void)
{
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 115200,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	usart_init_rs232(&Wireless_R_USART, &USART_SERIAL_OPTIONS);
	usart_set_rx_interrupt_level(&Wireless_R_USART,USART_INT_LVL_MED);
	usart_init_rs232(&Wireless_L_USART, &USART_SERIAL_OPTIONS);
	usart_set_rx_interrupt_level(&Wireless_L_USART,USART_INT_LVL_MED);
}

void spi_init(void)
{	
	sysclk_enable_peripheral_clock(&NRF24L01_R_SPI);
	spi_xmega_set_baud_div(&NRF24L01_R_SPI,8000000UL,F_CPU);
	spi_enable_master_mode(&NRF24L01_R_SPI);
	spi_enable(&NRF24L01_R_SPI);
	
	sysclk_enable_peripheral_clock(&NRF24L01_L_SPI);
	spi_xmega_set_baud_div(&NRF24L01_L_SPI,8000000UL,F_CPU);
	spi_enable_master_mode(&NRF24L01_L_SPI);
	spi_enable(&NRF24L01_L_SPI);
}


void nrf_init(char *Address)
{
		delay_ms(11);
		NRF24L01_L_Clear_Interrupts();
		NRF24L01_R_Clear_Interrupts();
		NRF24L01_L_Flush_TX();
		NRF24L01_R_Flush_TX();
		NRF24L01_L_Flush_RX();
		NRF24L01_R_Flush_RX();

		NRF24L01_L_Init_milad(_TX_MODE, _CH_L, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
		NRF24L01_L_WriteReg(W_REGISTER | DYNPD,0x01);
		NRF24L01_L_WriteReg(W_REGISTER | FEATURE,0x06);

		NRF24L01_R_Init_milad(_TX_MODE, _CH_R, _2Mbps, Address, _Address_Width, _Buffer_Size, RF_PWR_MAX);
		NRF24L01_R_WriteReg(W_REGISTER | DYNPD,0x01);
		NRF24L01_R_WriteReg(W_REGISTER | FEATURE,0x06);

		delay_us(130);
}



// 
// 
// void En_RC32M(void)
// {
// 	// Start RC32M OSC
// 	OSC_CTRL |= OSC_RC32MEN_bm;
// 	while(!(OSC_STATUS & OSC_RC32MRDY_bm));
// 
// 	// Select the system clock source: 32 MHz Internal RC Osc.
// 	CCP = CCP_IOREG_gc;
// 	CLK_CTRL = CLK_SCLKSEL_RC32M_gc;
// 
// 	// Disable the unused oscillators: 2 MHz, internal 32 kHz, external clock/crystal oscillator, PLL
// 	OSC_CTRL &= ~(OSC_RC2MEN_bm | OSC_RC32KEN_bm | OSC_XOSCEN_bm | OSC_PLLEN_bm);
// };
// void PORT_init(void)
// {
// 	PORTB_DIR = Segment_SideL_R_Enable_bm | Segment_SideL_L_Enable_bm | Segment_SideR_R_Enable_bm | Segment_SideR_L_Enable_bm
// 	| LED_Green_L_PIN_bm | LED_Green_R_PIN_bm | LED_White_R_PIN_bm | LED_White_L_PIN_bm;
// 	PORTC_DIR = NRF24L01_R_CE_LINE | NRF24L01_R_CS_LINE | NRF24L01_R_MOSI_LINE | NRF24L01_R_SCK_LINE | Wireless_TX_R_PIN_bm;
// 	PORTC_PIN0CTRL |= PORT_ISC_LEVEL_gc;
// 	PORTC_INTCTRL  |= PORT_INT0LVL_LO_gc;
// 	PORTC_INT0MASK |= PIN0_bm;
// 	PORTD_DIR = Buzzer_PIN_bm;
// 	PORTE_DIR = NRF24L01_L_CE_LINE | NRF24L01_L_CS_LINE | NRF24L01_L_MOSI_LINE | NRF24L01_L_SCK_LINE | Wireless_TX_L_PIN_bm;
// 	PORTE_PIN0CTRL |= PORT_ISC_LEVEL_gc;
// 	PORTE_INTCTRL  |= PORT_INT0LVL_LO_gc;
// 	PORTE_INT0MASK |= PIN0_bm;
// 	PORTF_DIR = Segment_A_bm | Segment_B_bm | Segment_C_bm | Segment_D_bm | Segment_E_bm | Segment_F_bm | Segment_G_bm | Segment_DP_bm;
// 	Wireless_R_PORT.OUTSET=Wireless_TX_R_PIN_bm;
// };
// 
// #define TIMERD0_PER 0x7C
// void TimerD0_init(void)
// {
// 	tc_write_clock_source(&TCD0,TC_CLKSEL_DIV256_gc);
// 	tc_set_wgm(&TCD0,TC_WG_NORMAL);
// 	tc_set_8bits_mode(&TCD0);
// 	tc_set_overflow_interrupt_level(&TCD0,TC_INT_LVL_LO);
// 	tc_write_period(&TCD0,TIMERD0_PER);
// 	tc_set_direction(&TCD0,TC_UP);
// 	tc_enable(&TCD0);
// };
// 
// void TimerE0_init(void)
// {
// 	tc_write_clock_source(&TCE0,TC_CLKSEL_DIV256_gc);
// 	tc_set_wgm(&TCE0,TC_WG_NORMAL);
// 	tc_write_period(&TCE0,0xffff);
// 	tc_set_direction(&TCE0,TC_UP);
// 	tc_enable(&TCE0);
// };
// 
// void TimerE1_init(void)
// {
// 	tc_write_clock_source(&TCE1,TC_CLKSEL_DIV256_gc);
// 	tc_set_wgm(&TCE1,TC_WG_NORMAL);
// 	tc_write_period(&TCE1,0xffff);
// 	tc_set_direction(&TCE1,TC_UP);
// 	tc_enable(&TCE1);
// };
// 
// 
// void USART_R_init(void)
// {
// 	usart_set_mode(&Wireless_R_USART,USART_CMODE_ASYNCHRONOUS_gc);
// 	usart_format_set(&Wireless_R_USART,USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc,false);
// 	usart_set_rx_interrupt_level(&Wireless_R_USART,USART_INT_LVL_MED);
// 	//usart_set_tx_interrupt_level(&Wireless_R_USART,USART_INT_LVL_MED);
// 	usart_set_baudrate(&Wireless_R_USART,USART_BUADRATE,F_CPU);
// 	usart_tx_enable(&Wireless_R_USART);
// 	usart_rx_enable(&Wireless_R_USART);
// }
// 
// 
// void USART_L_init(void)
// {
// 	usart_set_mode(&Wireless_L_USART,USART_CMODE_ASYNCHRONOUS_gc);
// 	usart_format_set(&Wireless_L_USART,USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc,false);
// 	usart_set_rx_interrupt_level(&Wireless_L_USART,USART_INT_LVL_MED);
// 	//usart_set_tx_interrupt_level(&Wireless_L_USART,USART_INT_LVL_MED);
// 	usart_set_baudrate(&Wireless_L_USART,USART_BUADRATE,F_CPU);
// 	usart_tx_enable(&Wireless_L_USART);
// 	usart_rx_enable(&Wireless_L_USART);
// }
