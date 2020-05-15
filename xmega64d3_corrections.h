#ifndef XMEGA64D3_CORRECTIONS_H_INCLUDED
#define XMEGA64D3_CORRECTIONS_H_INCLUDED

/*  Corrections to the IO Header  */

#ifdef _AVR_ATxmega64D3_H_

/*	Missing REMAP register definitions  */
#define REMAP				reserved_0x0E
#define PORT_TC0A_bm		0x01
#define PORT_TC0A_bp		0
#define PORT_TC0B_bm		0x02
#define PORT_TC0B_bp		1
#define PORT_TC0C_bm		0x04
#define PORT_TC0C_bp		2
#define PORT_TC0D_bm		0x08
#define PORT_TC0D_bp		3
#define PORT_USART0_bm		0x10
#define PORT_USART0_bp		4
#define PORT_SPI_bm			0x20
#define PORT_SPI_bp			5

/*  Missing ADC register definitions  */
#define TEMP				reserved_0x07
#define SAMPCTRL			reserved_0x08
#define ADC_SAMPVAL_bm		0x3f
#define ADC_SAMPVAL_gp		0
#define SCAN				reserved_0x6
#define ADC_CH_COUNT_bm		0x0f
#define ADC_CH_COUNT_gp		0
#define ADC_CH_OFFSET_bm	0xf0
#define ADC_CH_OFFSET_gp	4
#undef ADC_CH_MUXNEG_gm
#define ADC_CH_MUXNEG_gm	0x07
#define ADC_CH_MUXNEG_gp	0
#define ADC_CH_MUXNEG0_bm	(1<<0)
#define ADC_CH_MUXNEG0_bp	0
#define ADC_CH_MUXNEG1_bm	(1<<1)
#define ADC_CH_MUXNEG1_bp	1
#define ADC_CH_MUXNEG2_bm	(1<<2)
#define ADC_CH_MUXNEG2_bp	2

#define ADCA_CH0RESL		_SFR_MEM8(0x0210)
#define ADCA_CH0RESH		_SFR_MEM8(0x0211)

/*  Extra ADC register definitions  */
#undef ADC_SWEEP_gm
#undef ADC_SWEEP_gp
#undef ADC_SWEEP0_bm
#undef ADC_SWEEP0_bp
#undef ADC_SWEEP1_bm
#undef ADC_SWEEP1_bp
#undef ADC_CH_MUXINT_gm
#undef ADC_CH_MUXINT_gp
#undef ADC_CH_MUXINT0_bm
#undef ADC_CH_MUXINT0_bp
#undef ADC_CH_MUXINT1_bm
#undef ADC_CH_MUXINT1_bp
#undef ADC_CH_MUXINT2_bm
#undef ADC_CH_MUXINT2_bp
#undef ADC_CH_MUXINT3_bm
#undef ADC_CH_MUXINT3_bp

/*  Extra GPIO register definitions  */
#undef GPIOR4
#undef GPIOR5
#undef GPIOR6
#undef GPIOR7
#undef GPIOR8
#undef GPIOR9
#undef GPIORA
#undef GPIORB
#undef GPIORC
#undef GPIORD
#undef GPIORE
#undef GPIORF
#undef GPIO_GPIOR4
#undef GPIO_GPIOR5
#undef GPIO_GPIOR6
#undef GPIO_GPIOR7
#undef GPIO_GPIOR8
#undef GPIO_GPIOR9
#undef GPIO_GPIORA
#undef GPIO_GPIORB
#undef GPIO_GPIORC
#undef GPIO_GPIORD
#undef GPIO_GPIORE
#undef GPIO_GPIORF

#endif // _AVR_ATxmega64D3_H_

#endif // XMEGA64D3_CORRECTIONS_H_INCLUDED
