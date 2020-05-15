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
.filtEn = false,			\
.dlyList = {				\
	0x000800,				\
	0x000d00,				\
	0x001200,				\
	0x002000,				\
},							\
.gainList = {				\
	0x18,					\
	0x14,					\
	0x10,					\
	0x00					\
},							\
/*	Biquad filter coefficients a0, a1, a2, b1, b2 (b0 normalized)  */	\
.filt = {					\
	 0.748178	*  16384,	\
	-0.978082	*  16384,	\
	 0.367963	*  16384,	\
	-1.410055	* -16384,	\
	 0.548114	* -16384	\
},							\
.name = "Free    "

typedef __uint24 uint24_t;

typedef struct dsp_params_s {
	uint8_t taps;
	bool filtEn;
	uint16_t dlyList[4];
	uint8_t gainList[4];
	int16_t filt[5];
	uint8_t name[MENU_TITLE_LEN];
} dsp_params_t;

#ifndef numelem
#define numelem(x)			( sizeof(x) / sizeof(x[0]) )
#endif

#define DSP_TIMER			TCC1
#define DSP_VECTOR			TCC1_OVF_vect
/** The DSP should only be stopped briefly, and well before a new cycle begins, to prevent delaying results. */
#define dspStart()			DSP_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc
#define dspStop()			DSP_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc
//	DSP activity has many more dependencies; add here as needed
#define dspActive()			(!!(DSP_TIMER.INTCTRLA & TC_OVFINTLVL_HI_gc))
/** A DSP cycle is imminent if there is less than 10us before the next cycle begins. */
#define dspImminent()		(DSP_TIMER.CNTL >= DSP_DECIMATION - 1)

#endif // __ASSEMBLER__
//	10Hz HP cutoff
#define HIGHPASS_FEEDBACK	225
#ifndef __ASSEMBLER__

volatile int16_t adcSlowSample;
volatile dsp_params_t dspParams;

void dspInit(void);
#ifndef RAM_WORD_ACCESS
uint8_t readRamByte(uint24_t a);
void writeRamByte(uint24_t a, uint8_t d);
#else
uint16_t readRamWord(uint24_t a);
void writeRamWord(uint24_t a, uint16_t d);
#endif // RAM_WORD_ACCESS
void writeDac(const int16_t v);
int32_t mac32p16p16(int32_t acc, const int16_t* p1, const int16_t* p2);
int32_t mac32r16p8(int32_t acc, int16_t r, const int8_t* p);
int16_t dspGetLiveSample(void);
int16_t dspHighpass(int16_t samp);
int16_t dspReverbTaps(int16_t samp, uint16_t headAddr);
int16_t dspBiquadFilter(int16_t samp);
#endif // __ASSEMBLER__

#endif // DSP_H_INCLUDED
