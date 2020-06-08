#ifndef DSP_H_INCLUDED
#define DSP_H_INCLUDED

/**
 *	Undefine to use C language, single byte SRAM access.
 *  (ASM versions not implemented.)
 */
#define RAM_WORD_ACCESS

/**
 *	Undefine to use C language routines.
 *	Set to 1 for basic (C-compatible) ASM.
 *	Set to 2 for fully integrated (custom ABI) ASM.
 */
#define DSP_USE_ASM			2


#include <avr/io.h>
#include "xmega64d3_corrections.h"
#include "pindefs.h"
#ifndef __ASSEMBLER__
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/builtins.h>
#include <inttypes.h>
#include <stdbool.h>
#include <util/atomic.h>
#include "readcal.h"

#define DSP_DECIMATION		7

//	Note: duplicated from menu.h
#define MENU_TITLE_LEN		8

#define DSP_DEFAULT_PARAMS	\
.taps = 0,					\
.filtEn = 0,				\
.gainClean = 0x7f,			\
.gainReverb = 0x7f,			\
.dlyList = {				\
	0x0a00,					\
	0x1000,					\
	0x1800,					\
	0x3000,					\
	0x5000,					\
	0x8000					\
},							\
.gainList = {				\
	0x1e,					\
	0x16,					\
	0x16,					\
	0x12,					\
	0x12,					\
	0x0a					\
},							\
/*	Biquad filter coefficients a0, a1, a2, b1, b2 (b0 normalized) x 2   */	\
.filt = {					\
	 0.748178	*  16384,	\
	-0.978082	*  16384,	\
	 0.367963	*  16384,	\
	-1.410055	* -16384,	\
	 0.548114	* -16384,	\
							\
	 0.748178	*  16384,	\
	-0.978082	*  16384,	\
	 0.367963	*  16384,	\
	-1.410055	* -16384,	\
	 0.548114	* -16384	\
},							\
.name = "Free    "

typedef __uint24 uint24_t;

/*	Warning: keep synced with equs in asmdsp.S  */
typedef struct dsp_params_s {
	uint8_t taps;
	uint8_t filtEn;
	int8_t gainClean;
	int8_t gainReverb;
	uint16_t dlyList[6];
	uint8_t gainList[6];
	int16_t filt[10];
	uint8_t name[MENU_TITLE_LEN];
} dsp_params_t;

#ifndef numelem
#define numelem(x)			( sizeof(x) / sizeof(x[0]) )
#endif

#define DSP_TIMER			TCC1
#define DSP_COPY_VECTOR		TCC1_OVF_vect
#define DSP_EVAL_VECTOR		TCC1_CCA_vect

/** The DSP must be stopped only briefly, to prevent delaying results. */
#define dspStart()			DSP_TIMER.INTCTRLB = TC_CCAINTLVL_LO_gc
#define dspStop()			DSP_TIMER.INTCTRLB = TC_CCAINTLVL_OFF_gc

#endif // __ASSEMBLER__
//	10Hz HP cutoff
#define HIGHPASS_FEEDBACK	225
#ifndef __ASSEMBLER__

volatile int16_t adcSlowSample __attribute__((address (2)));
volatile dsp_params_t dspParams;

void dspInit(void);
#ifndef RAM_WORD_ACCESS
uint8_t readRamByte(uint24_t a);
void writeRamByte(uint24_t a, uint8_t d);
#else
uint16_t readRamWord(uint24_t a);
void writeRamWord(uint24_t a, uint16_t d);
#endif // RAM_WORD_ACCESS
void writeDac(int16_t v);
int32_t mac32p16p16(int32_t acc, const int16_t* p1, const int16_t* p2);
int32_t mac32r16p8(int32_t acc, int16_t r, const int8_t* p);
int16_t dspGetLiveSample(void);
int16_t dspHighpass(int16_t samp);
int16_t dspReverbTaps(int16_t samp, uint16_t headAddr);
int16_t dspBiquadFilter(int16_t samp);
int16_t dspMix(int16_t samp1, int16_t samp2);
#endif // __ASSEMBLER__

#endif // DSP_H_INCLUDED
