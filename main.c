/*
 *	Synth/Reverb Box
 *	Tim Williams, 2020-04-09
 *
 *	MCU: ATXMEGA64D3
 *	RAM: CY62148EV30LL-55SXI
 *	DAC: DAC1209LCD
 *
 *	MCU Pinout			Function (by peripheral if applicable)
 *	--------------------------------------------
 *	Pin		Name	Other	RAM		DAC		LCD
 *	1		PA3		NC
 *	2		PA4		NC
 *	3		PA5		NC
 *	4		PA6		NC
 *	5		PA7		NC
 *	6		PB0				D0		D0		D0
 *	7		PB1				D1		D1		D1
 *	8		PB2				D2		D2		D2
 *	9		PB3				D3		D3		D3
 *	10		PB4				D4		D4		D4
 *	11		PB5				D5		D5		D5
 *	12		PB6				D6		D6		D6
 *	13		PB7				D7		D7		D7
 *	14		GND		GND
 *	15		VDD		+3.3V
 *	16		PC0				A0		D8
 *	17		PC1				A1		D9
 *	18		PC2				A2		D10
 *	19		PC3				A3		D11
 *	20		PC4				A4
 *	21		PC5				A5
 *	22		PC6				A6
 *	23		PC7				A7
 *	24		GND		GND
 *	25		VDD		+3.3V
 *	26		PD0				A8
 *	27		PD1				A9
 *	28		PD2				A10
 *	29		PD3				A11
 *	30		PD4				A12
 *	31		PD5				A13
 *	32		PD6				A14
 *	33		PD7				A15
 *	34		GND		GND
 *	35		VDD		+3.3V
 *	36		PE0		NC
 *	37		PE1		NC
 *	38		PE2		RXD
 *	39		PE3		TXD
 *	40		PE4								nWRLCD
 *	41		PE5						nWRDAC
 *	42		PE6				nRDRAM
 *	43		PE7				nWRRAM
 *	44		GND		GND
 *	45		VDD		+3.3V
 *	46		PF0				A16
 *	47		PF1				A17
 *	48		PF2				A18
 *	49		PF3		PB1 (down)
 *	50		PF4		PB2 (right)
 *	51		PF5		PB3 (up)
 *	52		GND		GND
 *	53		VDD		+3.3V
 *	54		PF6		PB4 (left)
 *	55		PF7		NC
 *	56		PDI_DAT	PDI_DAT
 *	57		PDI_CLK	PDI_CLK
 *	58		XTAL2	XTAL2
 *	59		XTAL1	XTAL1
 *	60		GND		GND
 *	61		AVDD	+3.3V
 *	62		PA0		+3.3V
 *	63		PA1		AIN0
 *	64		PA2		NC
 *
 *	PDI Programming port: 2x3-pin AVRISP header
 *  Pins: 1 = PDI_DAT, 2 = VCC, 3 = NC, 4 = NC, 5 = PDI_CLK, 6 = GND
 *	Crystal: 8MHz + 2 x 22pF loading caps
 *	Serial console: PE2, PE3 (USARTE0) to MAX232 to DE-9 jack
 *	Pushbuttons: momentary to GND (use weak pull-ups)
 *
 *	For filter design, recommend:
 *	https://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
 *	Copy the results, multiply by 16384, round to integer, and negate
 *	the last two numbers (b1 and b2).  These can be entered directly,
 *	via LCD menu and pushbuttons, or serial console (convert to hex
 *	first).
 *
 */

#include "main.h"
//	Redundant hack to get Code::Blocks to pick up header references
#ifndef _AVR_ATxmega64D3_H_
#include <avr/iox64d3.h>
#endif // _AVR_ATxmega64D3_H_


/* * *  Global Variables  * * */

volatile bool timerFlag = false;
uint16_t cpuCycle = 0;


/* * *  Nonvolatile Data  * * */

NVM_FUSES_t __fuse FUSEMEM = {
	.FUSEBYTE0 = FUSE0_DEFAULT,
	.FUSEBYTE1 = FUSE1_DEFAULT,
	.FUSEBYTE2 = FUSE2_DEFAULT,
	.FUSEBYTE4 = FUSE4_DEFAULT,
	.FUSEBYTE5 = FUSE5_DEFAULT
};

dsp_params_t Presets[16] EEMEM = {
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS },
	{ DSP_DEFAULT_PARAMS }
};


/* * *  Functions  * * */

/**
 *	Main entry point
 */
int main(void) {

	uint24_t cpuCounts = 0;
	uint8_t timerCounts = 0;
	uint8_t buttonFlags, buttonFlagTot = 1;

	initialize();
	serInit();
	lcdInit();
	dspInit();
	menuInit(MenuMain);

	//	Main loop
	while (1) {

		cmdGetInputLine();

		if (timerFlag) {
			timerFlag = false;
			timerCounts++;

			buttonFlags = debouncePBs();
			menuAct(buttonFlags);
			buttonFlagTot |= buttonFlags;

			if (timerCounts >= 8) {
				timerCounts = 0;
				cpuCycle = cpuCounts >> 8;
				cpuCounts &= 0x0000ff;

				if (buttonFlagTot) {
					menuDraw();
					buttonFlagTot = 0;
				}
			}
		}
		cpuCounts++;
	}

	return 0;
}

/**
 *	Reads and debounces the pushbuttons.
 *	Call periodically e.g. every 10ms or so.
 *	@return bit mask of which button press events occurred this call
 */
uint8_t debouncePBs(void) {

	static uint8_t pbs[PUSHBUTTONS];
	uint8_t bits;
	uint8_t i;
	uint8_t r = 0;

	//	Read port and right-justify
	bits = PORT_PB1.IN & (BIT_PB1 | BIT_PB2 | BIT_PB3 | BIT_PB4);
	//bits >>= PB_SHIFT;
	bits <<= 1;
	bits = (bits << 4) | (bits >> 4);

	for (i = 0; i < PUSHBUTTONS; i++) {
		r >>= 1;
		if (bits & 0x01) {
			pbs[i] = 0;
		} else {
			pbs[i]++;
		}
		if (pbs[i] == DEBOUNCE_SAMPLES ||
				pbs[i] == (DEBOUNCE_WAIT + DEBOUNCE_SAMPLES) ||
				pbs[i] == (DEBOUNCE_WAIT + DEBOUNCE_SAMPLES + DEBOUNCE_REPEAT) ) {
			//	Register a button press
			r |= 1 << (PUSHBUTTONS - 1);
			//	Loop to repeat press
			if (pbs[i] >= (DEBOUNCE_WAIT + DEBOUNCE_SAMPLES + DEBOUNCE_REPEAT)) {
				pbs[i] -= DEBOUNCE_REPEAT;
			}
		}
		bits >>= 1;
	}

	return r;
}

/**
 *	Set up all the registers needed for operation.
 */
void initialize(void) {

	//	Set up GPIO ports
	PORTA.OUT = 0;
	PORTA.DIR = 0;
	PORTB.OUT = 0;
	PORTB.DIR = BIT_D0 | BIT_D1 | BIT_D2 | BIT_D3 | BIT_D4 | BIT_D5 | BIT_D6 | BIT_D7;
	PORTC.OUT = 0;
	PORTC.DIR = BIT_A0D8 | BIT_A1D9 | BIT_A2D10 | BIT_A3D11 | BIT_A4 | BIT_A5 | BIT_A6 | BIT_A7;
	PORTD.OUT = 0;
	PORTD.DIR = BIT_A8 | BIT_A9 | BIT_A10 | BIT_A11 | BIT_A12 | BIT_A13 | BIT_A14 | BIT_A15;
	PORTE.OUT = BIT_nWRDAC | BIT_nRDRAM | BIT_nWRRAM;
	PORTE.DIR = BIT_TXD | BIT_nWRDAC | BIT_nRDRAM | BIT_nWRRAM;
	PORTCFG.MPCMASK = BIT_RXD | BIT_nWRDAC | BIT_nRDRAM | BIT_nWRRAM;
	PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTF.DIR = BIT_A16 | BIT_A17 | BIT_A18;
	PORTCFG.MPCMASK = BIT_PB1 | BIT_PB2 | BIT_PB3 | BIT_PB4;
	PORTF.PIN0CTRL = PORT_OPC_PULLUP_gc;

	//	Enable virtual port mapping for quick access
#ifdef USE_VIRTUAL_PORTS
	PORTCFG.VPCTRLA = VPORT_CFG_DATA | VPORT_CFG_A0A7;
	PORTCFG.VPCTRLB = VPORT_CFG_A8A15 | VPORT_CFG_CTRL;
#endif // USE_VIRTUAL_PORTS

	//	Set up external oscillator
	//	XOSCSEL to external, PLLSRC to XOSC, SCLKSEL to PLL,
	//	PLL multiplier 4x, system prescalers to 1x (default)
	OSC.XOSCCTRL = OSC_FRQRANGE_2TO9_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	OSC.CTRL |= OSC_XOSCEN_bm;
	//	Set PLL to XOSC, no divider, 4x (= 32MHz); wait for ref startup
	OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | (4 << OSC_PLLFAC_gp);
	while(!(OSC.STATUS & OSC_XOSCRDY_bm));
	//	Turn on PLL; wait for PLL startup
	OSC.CTRL |= OSC_PLLEN_bm;
	while(!(OSC.STATUS & OSC_PLLRDY_bm));
	//	Select PLL source
	CCPWrite(&(CLK.CTRL), CLK_SCLKSEL_PLL_gc);

	//	Enable timer C0, 12.5ms period
	TCC0.PER = 6250;
	TCC0.CTRLB = 0;
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;

	//	Enable EEPROM memory window
	NVM.CTRLB = NVM_EEMAPEN_bm;

	//	Enable interrupt sources
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	sei();

}


/* * *  Interrupt Handlers  * * */

/**
 *	Timer fires every 100ms
 */
ISR(TCC0_OVF_vect) {

	timerFlag = true;

}

/**
 *	Catch-all dummy stub
 */
ISR(BADISR_vect) {

	//	Just reset
	RST.CTRL = RST_SWRST_bm;

}
