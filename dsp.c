#include "dsp.h"


/* * *  Global Variables  * * */

volatile uint16_t adcLiveSample __attribute__((address (0)));

volatile dsp_params_t dspParams = {
	DSP_DEFAULT_PARAMS
};


/* * *  Functions  * * */

void dspInit(void) {

	//	Enable timer C1, event triggered interrupt (copy ADC result)
	//	CCA compare triggered interrupt (run DSP operation)
	DSP_TIMER.PER = DSP_DECIMATION;
	DSP_TIMER.CTRLA = TC_CLKSEL_EVCH0_gc;
	DSP_TIMER.CTRLB = 0;
	DSP_TIMER.CCA = DSP_DECIMATION / 2;
	DSP_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc;
	DSP_TIMER.INTCTRLB = TC_CCAINTLVL_LO_gc;

	//	Event source ADC -- timer decimates ADC to 25kS/s
	EVSYS.CH0MUX = EVSYS_CHMUX_ADCA_CH0_gc;

	//	Initialize ADCA, x16 prescaler (2MHz), gain = 1,
	//	calibration, 12 bit right adjust, interrupt enabled,
	//	single ended, signed, mux channel 1, 200kS/s
	ADCA.CTRLA = ADC_ENABLE_bm;
	ADCA.CTRLB = ADC_FREERUN_bm;
	ADCA.REFCTRL = ADC_REFSEL_AREFA_gc;
	ADCA.PRESCALER = ADC_PRESCALER_DIV16_gc;
	ADCA.SAMPCTRL = ((8 << ADC_SAMPVAL_gp) & ADC_SAMPVAL_bm);
	ADCA.CAL = SP_ReadCalibrationByte(PROD_SIGNATURES_START + offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0))
			| SP_ReadCalibrationByte(PROD_SIGNATURES_START + offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1)) << 8;
	//	ADC channel 0, interrupt, PA1 input
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc;
	ADCA.CH0.INTCTRL = ADC_CH_INTLVL_HI_gc;
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc | ADC_CH_START_bm;

	sei();
	dspStart();

}

#pragma GCC optimize ("-O3")

#ifdef RAM_WORD_ACCESS
#ifndef DSP_USE_ASM
/**
 *	Reads a word from RAM at the specified address.
 */
uint16_t readRamWord(uint24_t a) {

	uint16_t r;

	//	Set address
	a &= 0x7fffe;
	PORT_RAMA1618.OUT = a >> 16;
	PORT_RAMA07.OUT = a;
	PORT_RAMA815.OUT = a >> 8;
	//	Clear data
	PORT_RAMD.DIR = 0;
	//	Strobe read
	AssertRdRam();

	a++;
//	_NOP(); _NOP();
	r = PORT_RAMD.IN;
//	DeassertRdRam();

	PORT_RAMA1618.OUT = a >> 16;
	PORT_RAMA07.OUT = a;
	PORT_RAMA815.OUT = a >> 8;
//	AssertRdRam();
	_NOP(); _NOP();
	r |= (uint16_t)PORT_RAMD.IN << 8;
	DeassertRdRam();
	PORT_RAMD.DIR = 0xff;

	return r;
}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
/**
 *	Writes a word to RAM at the specified address.
 */
void writeRamWord(uint24_t a, uint16_t d) {

	//	Set address
	a &= 0x7fffe;
	PORT_RAMA1618.OUT = a >> 16;
	PORT_RAMA07.OUT = a;
	PORT_RAMA815.OUT = a >> 8;
	//	Set data
	PORT_RAMD.OUT = d;
	PORT_RAMD.DIR = 0xff;
	//	Strobe write
	AssertWrRam();

	_NOP();
	a++;
	DeassertWrRam();

	//PORT_RAMA1618.OUT = a >> 16;
	PORT_RAMA07.OUT = a;
	//PORT_RAMA815.OUT = a >> 8;
	//	Set data
	PORT_RAMD.OUT = d >> 8;
	AssertWrRam();
	_NOP(); _NOP();
	DeassertWrRam();

}
#endif // DSP_USE_ASM

#else
/**
 *	Reads a byte from RAM at the specified address.
 */
uint8_t readRamByte(uint24_t a) {

	uint8_t r;

	//	Set address
	PORT_RAMA1618.OUT = (a >> 16) & 0x7;
	PORT_RAMA07.OUT = a & 0xff;
	PORT_RAMA815.OUT = (a >> 8) & 0xff;
	//	Clear data
	PORT_RAMD.DIR = 0;
	//	Strobe read
	AssertRdRam();
	_NOP(); _NOP();
	r = PORT_RAMD.IN;
	DeassertRdRam();
	PORT_RAMD.DIR = 0xff;

	return r;
}

/**
 *	Writes a byte to RAM at the specified address.
 */
void writeRamByte(uint24_t a, uint8_t d) {

	//	Set address
	PORT_RAMA1618.OUT = (a >> 16) & 0x7;
	PORT_RAMA07.OUT = a & 0xff;
	PORT_RAMA815.OUT = (a >> 8) & 0xff;
	//	Set data
	PORT_RAMD.OUT = d;
	PORT_RAMD.DIR = 0xff;
	//	Strobe write
	AssertWrRam();
	_NOP(); _NOP();
	DeassertWrRam();

}
#endif // RAM_WORD_ACCESS

#ifndef DSP_USE_ASM
/**
 *	Writes a word to the DAC.
 *	Converts the signed 16-bit operand to 12-bit unsigned for output.
 */
void writeDac(int16_t v) {

	uint16_t x;

	x = v;
	x >>= 3;
	//	Convert from signed, and help
	//	evade DAC1209's bad code at 0x800
	x += 0x830;
	//	Set data
	PORT_DACLO.OUT = x;
	PORT_DACLO.DIR = 0xff;
	PORT_DACHI.OUT = x >> 8;
	//	Strobe write
	AssertWrDac();
	_NOP();
	DeassertWrDac();

}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
/**
 *	Performs a multiply-accumulate operation.
 *	32 bit accumulator, 16 bit signed operands in SRAM.
 *	@param
 *	acc		accumulator to add to
 *	p1, p2	pointers to operands
 *	@return
 *	acc += (*p1) * (*p2)
 */
int32_t mac32p16p16(int32_t acc, const int16_t* p1, const int16_t* p2) {

	return acc + (int32_t)*p1 * *p2;
}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
/**
 *	Performs a multiply-accumulate operation.
 *	32 bit accumulator, 16 bit signed operand, 8 bit signed multiplier.
 *	@param
 *	acc	accumulator to add to
 *	r	16 bit signed register value to scale
 *	p	8 bit signed scaling value
 *	@return
 *	acc += r * (*p)
 */
int32_t mac32r16p8(int32_t acc, int16_t r, const int8_t* p) {

	return acc + (int32_t)r * *p;
}
#endif // DSP_USE_ASM

//#ifndef DSP_USE_ASM
int16_t dspGetLiveSample(void) {

	int16_t samp;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		samp = adcSlowSample;
	}

	/*	Magic number isn't all that consequential, but comes from
	 *	the ADC having a range of (0.0, 2.681V) --> (1524, 32760)
	 *	counts.  This hits the middle, and is close to the measured
	 *	quiescent level.  */
	samp -= 17184;

	return samp;
}
//#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
int16_t dspHighpass(int16_t samp) {

	static int32_t hp = 0;

	//	Highpass to remove DC offset
	samp -= hp >> 16;
	hp += (int32_t)samp * HIGHPASS_FEEDBACK;

	return samp;
}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
int16_t dspReverbTaps(int16_t samp, uint16_t headAddr) {

	int16_t ram;
	int32_t accum;
	int16_t* ptr16;
	int8_t* ptr8;
	uint8_t i;

	accum = (int32_t)samp << 8; accum >>= 1;
	ptr16 = (int16_t*)&dspParams.dlyList[0];
	ptr8 = (int8_t*)&dspParams.gainList[0];
	for (i = 0; i < dspParams.taps; i++) {
#ifdef RAM_WORD_ACCESS
		ram = readRamWord(headAddr - *ptr16++);
#else
		ram = readRamByte(headAddr - *ptr16) |
			(uint16_t)readRamByte(headAddr - *ptr16++ + 1) << 8;
#endif // RAM_WORD_ACCESS
//		accum += ((int32_t)ram * *ptr8++);
		accum = mac32r16p8(accum, ram, ptr8++);
	}
	if (accum >= 0x00400000) {
		samp = 32767;
	} else if (accum <= 0xffc00000) {
		samp = -32768;
	} else {
		samp = accum >> 8; samp <<= 1; accum &= 0x80; accum <<= 1; samp |= (accum >> 8) & 1;
		//samp = accum >> 7;
	}

	return samp;
}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
/**
 *	Digital biquad, direct form 1:
 *		y[n] = a0 * x[n] + a1 * x[n-1] + a2 * x[n-2]
 *						 - b1 * y[n-1] - b2 * y[n-2]
 *	Note only adds are used; b1 and b2 are to be given negated values.
 *	Past samples, used in the filter calculation, are stored locally.
 *	Coefficients a0, a1, a2, -b1, -b2 are read from dspParams.filt.
 *	@param in	Input sample for this timestep.
 */
int16_t dspBiquadFilter(int16_t samp) {

	static int16_t history[10];
	int16_t* ptr1 = (int16_t*)dspParams.filt;
	int16_t* ptr2 = history;
	int32_t accum;
	uint8_t i, j;

	if (dspParams.filtEn > 2) {
		dspParams.filtEn = 2;
	}
	for (j = 0; j < dspParams.filtEn; j++) {

		ptr2[2] = ptr2[1];
		ptr2[1] = ptr2[0];
		ptr2[0] = samp; accum = 0;
		for (i = 0; i < 5; i++) {
	//		accum += (int32_t)dspParams.filt[i] * history[i];
			accum = mac32p16p16(accum, ptr1++, ptr2++);
		}
		//samp = accum >> 14;
		samp = accum >> 16;
		if (samp >= 0x2000) {
			samp = 32767;
		} else if (samp < -0x1fff) {
			samp = -32767;
		} else {
			samp <<= 2;
			i = accum >> 8;
			i = i << 4 | i >> 4; // __builtin_avr_swap(i);
			i = (i >> 2) & 0x03;
			samp |= i;
		}
		ptr2 -= 5;
		ptr2[4] = ptr2[3];
		ptr2[3] = samp;

		ptr2 += 5;
	}

	return samp;
}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
/**
 *	Two channel mixer.  samp1 and samp2 are summed, scaled
 *	by the gains from dspParams.gainClean and gainReverb.
 */
int16_t dspMix(int16_t samp1, int16_t samp2) {

	int32_t accum = 0;

	accum = mac32r16p8(accum, samp1, (int8_t*)&dspParams.gainClean);
	accum = mac32r16p8(accum, samp2, (int8_t*)&dspParams.gainReverb);

	return (accum << 1) >> 8;
}
#endif // DSP_USE_ASM


/* * *  Interrupt Handlers  * * */

#ifndef DSP_USE_ASM
/**
 *	ADC conversion interrupt, 200kS/s
 */
ISR(ADCA_CH0_vect) {

	//	GPIOR0 <--> adcLiveSample[0]
	//	GPIOR1 <--> adcLiveSample[1]
	adcLiveSample += ADCA.CH0.RES;

}
#endif // DSP_USE_ASM

#ifndef DSP_USE_ASM
/**
 *	Triggered by ADC decimation event, 25kS/s.
 *	Copies the live sample to a buffer, so it can be
 *	processed at a lower bus priority over the bus.
 */
ISR(DSP_COPY_VECTOR) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		adcSlowSample = adcLiveSample;
		adcLiveSample = 0;
	}
}
#endif // DSP_USE_ASM

//#pragma GCC optimize ("-fno-inline")
/**
 *	Triggered by ADC decimation, out of phase with the buffer
 *	trigger.  Performs DSP operations and DAC output.
 */
ISR(DSP_EVAL_VECTOR) {

	static uint16_t headAddr = 0;
	int16_t samp = 0, samp2;

	samp = dspGetLiveSample();

	samp = dspHighpass(samp);
	samp2 = samp;

	if (dspParams.taps > numelem(dspParams.dlyList)) {
		dspParams.taps = numelem(dspParams.dlyList);
	}
	samp = dspReverbTaps(samp, headAddr);

	writeDac(dspMix(samp2, samp));

	//	Filter into the reverb buffer to give a more natural rolloff
	samp = dspBiquadFilter(samp);

	headAddr += 2;
#ifdef RAM_WORD_ACCESS
	writeRamWord(headAddr, samp);
#else
	writeRamByte(headAddr - 2, samp & 0xff);
	writeRamByte(headAddr - 1, samp >> 8);
#endif // RAM_WORD_ACCESS

}
