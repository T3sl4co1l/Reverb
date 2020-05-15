#ifndef PINDEFS_H_INCLUDED
#define PINDEFS_H_INCLUDED

#define USE_VIRTUAL_PORTS

#define BIT_PIN0		PIN0_bm
#define BIT_PIN1		PIN1_bm
#define BIT_PIN2		PIN2_bm
#define BIT_PIN3		PIN3_bm
#define BIT_PIN4		PIN4_bm
#define BIT_PIN5		PIN5_bm
#define BIT_PIN6		PIN6_bm
#define BIT_PIN7		PIN7_bm

#define PIN_VREF		PIN0
#define BIT_VREF		(1 << PIN0)
#define PORT_VREF		PORTA
#define PIN_AIN0		PIN1
#define BIT_AIN0		(1 << PIN1)
#define PORT_AIN0		PORTA
#define PIN_D0			PIN0
#define BIT_D0			(1 << PIN0)
#define PORT_D0			PORTB
#define PIN_D1			PIN1
#define BIT_D1			(1 << PIN1)
#define PORT_D1			PORTB
#define PIN_D2			PIN2
#define BIT_D2			(1 << PIN2)
#define PORT_D2			PORTB
#define PIN_D3			PIN3
#define BIT_D3			(1 << PIN3)
#define PORT_D3			PORTB
#define PIN_D4			PIN4
#define BIT_D4			(1 << PIN4)
#define PORT_D4			PORTB
#define PIN_D5			PIN5
#define BIT_D5			(1 << PIN5)
#define PORT_D5			PORTB
#define PIN_D6			PIN6
#define BIT_D6			(1 << PIN6)
#define PORT_D6			PORTB
#define PIN_D7			PIN7
#define BIT_D7			(1 << PIN7)
#define PORT_D7			PORTB
#define PIN_A0D8		PIN0
#define BIT_A0D8		(1 << PIN0)
#define PORT_A0D8		PORTC
#define PIN_A1D9		PIN1
#define BIT_A1D9		(1 << PIN1)
#define PORT_A1D9		PORTC
#define PIN_A2D10		PIN2
#define BIT_A2D10		(1 << PIN2)
#define PORT_A2D10		PORTC
#define PIN_A3D11		PIN3
#define BIT_A3D11		(1 << PIN3)
#define PORT_A3D11		PORTC
#define PIN_A4			PIN4
#define BIT_A4			(1 << PIN4)
#define PORT_A4			PORTC
#define PIN_A5			PIN5
#define BIT_A5			(1 << PIN5)
#define PORT_A5			PORTC
#define PIN_A6			PIN6
#define BIT_A6			(1 << PIN6)
#define PORT_A6			PORTC
#define PIN_A7			PIN7
#define BIT_A7			(1 << PIN7)
#define PORT_A7			PORTC
#define PIN_A8			PIN0
#define BIT_A8			(1 << PIN0)
#define PORT_A8			PORTD
#define PIN_A9			PIN1
#define BIT_A9			(1 << PIN1)
#define PORT_A9			PORTD
#define PIN_A10			PIN2
#define BIT_A10			(1 << PIN2)
#define PORT_A10		PORTD
#define PIN_A11			PIN3
#define BIT_A11			(1 << PIN3)
#define PORT_A11		PORTD
#define PIN_A12			PIN4
#define BIT_A12			(1 << PIN4)
#define PORT_A12		PORTD
#define PIN_A13			PIN5
#define BIT_A13			(1 << PIN5)
#define PORT_A13		PORTD
#define PIN_A14			PIN6
#define BIT_A14			(1 << PIN6)
#define PORT_A14		PORTD
#define PIN_A15			PIN7
#define BIT_A15			(1 << PIN7)
#define PORT_A15		PORTD
#define PIN_RXD			PIN2
#define BIT_RXD			(1 << PIN2)
#define PORT_RXD		PORTE
#define PIN_TXD			PIN3
#define BIT_TXD			(1 << PIN3)
#define PORT_TXD		PORTE
#define PIN_A16			PIN0
#define BIT_A16			(1 << PIN0)
#define PORT_A16		PORTF
#define PIN_A17			PIN1
#define BIT_A17			(1 << PIN1)
#define PORT_A17		PORTF
#define PIN_A18			PIN2
#define BIT_A18			(1 << PIN2)
#define PORT_A18		PORTF

//	Pushbuttons must be on contiguous bits with PB1 in the lowest position
#define PIN_PB1			PIN3
#define BIT_PB1			(1 << PIN3)
#define PORT_PB1		PORTF
#define PIN_PB2			PIN4
#define BIT_PB2			(1 << PIN4)
#define PORT_PB2		PORTF
#define PIN_PB3			PIN5
#define BIT_PB3			(1 << PIN5)
#define PORT_PB3		PORTF
#define PIN_PB4			PIN6
#define BIT_PB4			(1 << PIN6)
#define PORT_PB4		PORTF

#define PUSHBUTTONS		4
#define PB_SHIFT		PIN_PB1

#ifdef USE_VIRTUAL_PORTS

#define VPORT_CFG_DATA		PORTCFG_VP0MAP_PORTB_gc
#define VPORT_CFG_A0A7		PORTCFG_VP1MAP_PORTC_gc
#define VPORT_CFG_A8A15		PORTCFG_VP2MAP_PORTD_gc
#define VPORT_CFG_CTRL		PORTCFG_VP3MAP_PORTE_gc

//	RAM, DAC, LCD: assume D0-D7 on a single port, pin 0-7 respectively
//	DACHI: assume D8-D11 on a single port, pin 0-3 respectively
//	RAM: assume A0-A7 on a single port, pin 0-7 respectively;
//	A8-A15 on a single port, pin 0-7 respectively;
//	A16-A18 on a single port, pin 0-2 respectively
#define PIN_LCD_RS		PIN0
#define BIT_LCD_RS		(1 << PIN0)
#define PIN_WRLCD		PIN4
#define BIT_WRLCD		(1 << PIN4)
#define PIN_nWRDAC		PIN5
#define BIT_nWRDAC		(1 << PIN5)
#define PIN_nRDRAM		PIN6
#define BIT_nRDRAM		(1 << PIN6)
#define PIN_nWRRAM		PIN7
#define BIT_nWRRAM		(1 << PIN7)
#define PORT_LCD_DAT	VPORT0
#define PORT_DACLO		VPORT0
#define PORT_RAMD		VPORT0
#define PORT_LCD_RS		VPORT1
#define PORT_DACHI		VPORT1
#define PORT_RAMA07		VPORT1
#define PORT_RAMA815	VPORT2
#define PORT_RAMA1618	PORTF
#define PORT_WRLCD		VPORT3
#define PORT_nWRDAC		VPORT3
#define PORT_nRDRAM		VPORT3
#define PORT_nWRRAM		VPORT3

#define PORT_RAMD_IN		VPORT0_IN
#define PORT_RAMD_OUT		VPORT0_OUT
#define PORT_RAMD_DIR		VPORT0_DIR
#define PORT_DACLO_OUT		VPORT0_OUT
#define PORT_DACLO_DIR		VPORT0_DIR
#define PORT_DACHI_OUT		VPORT1_OUT
#define PORT_RAMA07_OUT		VPORT1_OUT
#define PORT_RAMA815_OUT	VPORT2_OUT
#define PORT_RAMA1618_OUT	PORTF_OUT
#define PORT_nWRDAC_OUT		VPORT3_OUT
#define PORT_nRDRAM_OUT		VPORT3_OUT
#define PORT_nWRRAM_OUT		VPORT3_OUT

#define AssertLcdRs()		(PORT_LCD_RS.OUT |=  BIT_LCD_RS)
#define DeassertLcdRs()		(PORT_LCD_RS.OUT &= ~BIT_LCD_RS)
#define AssertWrLcd()		(PORT_WRLCD.OUT  |=  BIT_WRLCD )
#define DeassertWrLcd()		(PORT_WRLCD.OUT  &= ~BIT_WRLCD )
#define AssertWrDac()		(PORT_nWRDAC.OUT &= ~BIT_nWRDAC)
#define DeassertWrDac()		(PORT_nWRDAC.OUT |=  BIT_nWRDAC)
#define AssertRdRam()		(PORT_nRDRAM.OUT &= ~BIT_nRDRAM)
#define DeassertRdRam()		(PORT_nRDRAM.OUT |=  BIT_nRDRAM)
#define AssertWrRam()		(PORT_nWRRAM.OUT &= ~BIT_nWRRAM)
#define DeassertWrRam()		(PORT_nWRRAM.OUT |=  BIT_nWRRAM)

#else

#define PIN_nWRDAC		PIN5
#define BIT_nWRDAC		(1 << PIN5)
#define PORT_nWRDAC		PORTE
#define PIN_nRDRAM		PIN6
#define BIT_nRDRAM		(1 << PIN6)
#define PORT_nRDRAM		PORTE
#define PIN_nWRRAM		PIN7
#define BIT_nWRRAM		(1 << PIN7)
#define PORT_nWRRAM		PORTE
#define PORT_LCD_DAT	PORTB
#define PIN_LCD_RS		PIN0
#define BIT_LCD_RS		(1 << PIN0)
#define PORT_LCD_RS		PORTC
#define PIN_WRLCD		PIN4
#define BIT_WRLCD		(1 << PIN4)
#define PORT_WRLCD		PORTE
#define PORT_DACLO		PORTB
#define PORT_DACHI		PORTC
#define PORT_RAMD		PORTB
#define PORT_RAMA07		PORTC
#define PORT_RAMA815	PORTD
#define PORT_RAMA1618	PORTF

#define AssertLcdRs()		(PORT_LCD_RS.OUTSET = BIT_LCD_RS)
#define DeassertLcdRs()		(PORT_LCD_RS.OUTCLR = BIT_LCD_RS)
#define AssertWrLcd()		(PORT_WRLCD.OUTSET = BIT_WRLCD)
#define DeassertWrLcd()		(PORT_WRLCD.OUTCLR = BIT_WRLCD)
#define AssertWrDac()		(PORT_nWRDAC.OUTCLR = BIT_nWRDAC)
#define DeassertWrDac()		(PORT_nWRDAC.OUTSET = BIT_nWRDAC)
#define AssertWrRam()		(PORT_nWRRAM.OUTCLR = BIT_nWRRAM)
#define DeassertWrRam()		(PORT_nWRRAM.OUTSET = BIT_nWRRAM)
#define AssertRdRam()		(PORT_nRDRAM.OUTCLR = BIT_nRDRAM)
#define DeassertRdRam()		(PORT_nRDRAM.OUTSET = BIT_nRDRAM)

#endif // USE_VIRTUAL_PORTS

#endif	//	PINDEFS_H_INCLUDED
