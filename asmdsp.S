; * * *  Interrupt Handlers  * * *

#define __SFR_OFFSET 0

#include "dsp.h"

#ifdef DSP_USE_ASM

; equs for dsp_params_t
.equiv dsp_param_taps,			0
.equiv dsp_param_filtEn,		1
.equiv dsp_param_gainClean,		2
.equiv dsp_param_gainReverb,	3
.equiv dsp_param_dlyList,		4
.equiv dsp_param_gainList,		16
.equiv dsp_param_filt,			22
.equiv dsp_param_name,			42

.text

.global ADCA_CH0_vect
.global TCC1_OVF_vect
#ifdef RAM_WORD_ACCESS
;.global readRamWord
.global writeRamWord
#endif
;.global mac32r16p8
;.global mac32p16p16
.global writeDac
.global dspHighpass
.global dspReverbTaps
.global dspBiquadFilter
.global dspMix

#define adcLiveSampleL		GPIOR0
#define adcLiveSampleH		GPIOR1
#define adcSlowSampleL		GPIOR2
#define adcSlowSampleH		GPIOR3

;
;	ISR(ADCA_CH0_vect)
;
; ADC conversion interrupt, 200kS/s.
;
; 25 cycles
;
ADCA_CH0_vect:

; adcLiveSample += ADCA_CH0_RES;

		push	r24
		in	r24, SREG
		push	r24
		push	r25
		lds	r24, ADCA_CH0RESL
		in	r25, adcLiveSampleL
		add	r24, r25
		out	adcLiveSampleL, r24
		lds	r24, ADCA_CH0RESH
		in	r25, adcLiveSampleH
		adc	r24, r25
		out	adcLiveSampleH, r24
		pop	r25
		pop	r24
		out	SREG, r24
		pop	r24
		reti


;
;	ISR(TCC1_OVF_vect)
;
; ADC decimation interrupt, 25kS/s.
; Copies the ADC running total (adcFastSample) into a
; buffer (adcSlowSample).
;
; 21 cycles
;
TCC1_OVF_vect:
		push r1
		in r1, SREG
		push r24
		cli
		in r24, adcLiveSampleL
		out adcSlowSampleL, r24
		in r24, adcLiveSampleH
		out adcSlowSampleH, r24
		eor r24, r24
		out adcLiveSampleL, r24
		out adcLiveSampleH, r24
		sei
		pop r24
		out SREG, r1
		pop r1
		reti


;
;	uint16_t readRamWord(uint24_t a)
;
; Reads a word from RAM at the specified address.
; Address must be word aligned.
;
; Params:
;	r24:r23:r22 = address (and successor) to read from
; Returns:
;	r25:r24 = word read
;
; 26 cycles
;
; Clobber:
;	r21 = 0xff
;	r22 = lo8(addr + 1)
;	r23 = hi8(addr)
;
;readRamWord:
;		; r24:r23:r22 = addr & 0x7fffe (forces word aligned access)
;		andi r22, 0xfe
;		andi r24, BIT_A16 | BIT_A17 | BIT_A18
;		sts PORT_RAMA1618_OUT, r24
;		out PORT_RAMA07_OUT, r22
;		out PORT_RAMA815_OUT, r23
;		out PORT_RAMD_DIR, r1	; deassert data bus
;		cbi PORT_nRDRAM_OUT, PIN_nRDRAM
;		subi r22, 0xff			; addr++
;		;nop						; no carry to mid or high byte because word align
;		;nop
;		sbci r23, 0xff
;		sbci r24, 0xff
;		andi r24, BIT_A16 | BIT_A17 | BIT_A18
;		in r21, PORT_RAMD_IN	; r21 = low byte
;		sts PORT_RAMA1618_OUT, r24
;		out PORT_RAMA07_OUT, r22
;		out PORT_RAMA815_OUT, r23
;		nop
;		nop
;		nop
;		in r25, PORT_RAMD_IN	; r25 = high byte
;		sbi PORT_nRDRAM_OUT, PIN_nRDRAM
;		mov r24, r21			; return r25:r24
;		ldi r21, 0xff
;		out PORT_RAMD_DIR, r21	; reassert data bus
;		ret


;
;	void writeRamWord(uint24_t a, uint16_t d)
;
; Writes a word to RAM at the specified address.
; Address must be word aligned.
;
; Params:
;	r24:r23:r22 = address (and successor) to write to
;	r21:r20 = data
; Returns:
;	nothing
;
; 24 cycles
;
; Clobber:
;	r19 = 0xff
;	r22 = lo8(addr + 1)
;	r23 = hi8(addr)
;	r24 = hhi8(addr)
;
writeRamWord:
		; r24:r23:r22 = addr & 0x7fffe (forces word aligned access)
		andi r22, 0xfe
		andi r24, BIT_A16 | BIT_A17 | BIT_A18
		sts PORT_RAMA1618_OUT, r24
		out PORT_RAMA07_OUT, r22
		out PORT_RAMA815_OUT, r23
		out PORT_RAMD_OUT, r20	; r20 = low byte
		ldi r19, 0xff
		out PORT_RAMD_DIR, r19	; assert data bus just in case
		cbi PORT_nWRRAM_OUT, PIN_nWRRAM
		subi r22, 0xff			; addr++
		;nop						; no carry to mid or high byte because word align
		;nop
		sbci r23, 0xff
		sbci r24, 0xff
		sbi PORT_nWRRAM_OUT, PIN_nWRRAM
		sts PORT_RAMA1618_OUT, r24
		out PORT_RAMA07_OUT, r22
		out PORT_RAMA815_OUT, r23
		out PORT_RAMD_OUT, r21
		cbi PORT_nWRRAM_OUT, PIN_nWRRAM
		nop
		nop
		nop
		sbi PORT_nWRRAM_OUT, PIN_nWRRAM
		ret


;
;	void writeDac(const int16_t v)
;
; Writes a word to the DAC.
; Converts the signed 16-bit operand to 12-bit unsigned for output.
;
; Params:
;	r25:r24 = value to set
; Returns:
;	nothing
;
; 19 cycles
;
; Clobber:
;	r24, r25
;
writeDac:
		swap r24			; right adjust bits
		andi r24, 0x0f		; r24 >>= 4
		swap r25
		mov r23, r25		; r25 = top nibble
		andi r23, 0xf0		; r23 = middle nibble
		or r24, r23			; put r25 low nibble into r24 high nibble
		subi r25, 8			; convert from signed
		andi r25, 0x0f

; output to DAC
		out PORT_DACLO_OUT, r24
		ldi r24, 0xff
		out PORT_DACLO_DIR, r24
		out PORT_DACHI_OUT, r25
		cbi PORT_nWRDAC_OUT, PIN_nWRDAC	; AssertWrDac()
		nop
		nop
		sbi PORT_nWRDAC_OUT, PIN_nWRDAC	; DeassertWrDac()
		ret


;
;	int32_t mac32p16p16(int32_t acc, int16_t* p1, int16_t* p2)
;
; Performs a multiply-accumulate operation.
; 32 bit accumulator, 16 bit operands.
; accum += (*p1) * (*p2)
;
; Params:
;	r25:r24:r23:r22 = acc
;	r21:r20 = p1
;	r19:r18 = p2
; Returns:
;	r25:r24:r23:r22 = result
;
; 29 cycles
;
; Clobber:
;	r0 = intermediate result, lo8(hi*lo)
;	r19:r18 = *p1
;	r21:r20 = *p2
;	r26 = 0
;	r27 = hi8(lo*lo)
;	Z = p2 + 2
;
;mac32p16p16:
;		movw r26, r20
;		movw r30, r18
;		ld r18, X+
;		ld r19, X+			; get [p1]
;		ld r20, Z+
;		ld r21, Z+			; get [p2]
;
;; r25:r24:r23:r22 += mul_signed(r21:r20, r19:r18)
;
;		mul r18, r20		; lo*lo
;		movw r26, r0		; hold onto result..
;		muls r19, r21		; hi*hi
;		adc r22, r26
;		adc r23, r27
;		adc r24, r0
;		adc r25, r1
;		eor r26, r26		; use r26 as zero reg
;		mulsu r19, r20		; hi*lo
;		sbc r25, r26		; sign extend
;		add r23, r0
;		adc r24, r1
;		adc r25, r26
;		mulsu r21, r18		; lo*hi
;		sbc r25, r26
;		add r23, r0
;		adc r24, r1
;		adc r25, r26
;
;		eor r1, r1
;		ret


;
;	int32_t mac32r16p8(int32_t acc, int16_t r, int8_t* p)
;
; Performs a multiply-accumulate operation.
; 32 bit accumulator, 16 bit signed operand, 8 bit signed multiplier.
; accum += r * (*p)
;
; Params:
;	r25:r24:r23:r22 = acc
;	r21:r20 = r
;	r19:r18 = p
; Returns:
;	r25:r24:r23:r22 = result
;
; 21 cycles
;
; Clobber:
;	r18 = *(int8_t*)p
;	r19 = 0
;	Z = p + 1
;
;mac32r16p8:
;		movw r30, r18
;		ld r18, Z+			; get multiplier
;
;; r25:r24:r23:r22 += mul_signed(r21:r20, r18)
;
;		eor r19, r19		; use r19 as zero reg
;		mulsu r18, r20		; p*lo
;		sbc r24, r19
;		sbc r25, r19		; sign extend
;		add r22, r0
;		adc r23, r1
;		adc r24, r19
;		adc r25, r19
;		muls r18, r21		; p*hi
;		sbc r25, r19
;		add r23, r0
;		adc r24, r1
;		adc r25, r19
;		eor r1, r1
;		ret


;
;	int16_t dspHighpass(int16_t samp)
;
; Calculates a timestep of a highpass filter on samp.
; Uses local static variable.
;
; Params:
;	r25:r24 = samp
; Returns:
;	r25:r24 = result
;
; 34/40 cycles (8/16 bit HIGHPASS_FEEDBACK)
;
; Clobber:
;	r19:r18 = HIGHPASS_FEEDBACK
;	r21:r20 = samp
;	r23:r22 = lo16(hp)
;	X = no/yes (8/16 bit HIGHPASS_FEEDBACK)
;	Z = hp+3
;
.lcomm	dspHPaccum	4
dspHighpass:
		movw r20, r24		; r21:r20 = samp
		ldi r30, lo8(dspHPaccum)
		ldi r31, hi8(dspHPaccum)
		ld r22, Z+			; r25:r24:r23:r22 = hp
		ld r23, Z+
		ld r24, Z+
		ld r25, Z
		; samp -= hp >> 16;
		sub r20, r24
		sbc r21, r25
		; hp += samp * HIGHPASS_FEEDBACK
		ldi r18, lo8(HIGHPASS_FEEDBACK)
		ldi r19, hi8(HIGHPASS_FEEDBACK)

#if (HIGHPASS_FEEDBACK >> 8) == 0
; s16 x u8 + 32 multiply-accumulate
		; use r19 as zero reg
		mul r20, r18		; lo*fb
		add r22, r0
		adc r23, r1
		adc r24, r19
		adc r25, r19
		mulsu r21, r18		; hi*fb
		sbc r25, r19		; sign extend
		add r23, r0
		adc r24, r1
		adc r25, r19
#else
; s16 x s16 + 32 multiply-accumulate
		mul r18, r20		; lo*lo
		movw r26, r0		; hold onto result..
		muls r19, r21		; hi*hi
		adc r22, r26
		adc r23, r27
		adc r24, r0
		adc r25, r1
		eor r26, r26		; use r26 as zero reg
		mulsu r19, r20		; hi*lo
		sbc r25, r26		; sign extend
		add r23, r0
		adc r24, r1
		adc r25, r26
		mulsu r21, r18		; lo*hi
		sbc r25, r26
		add r23, r0
		adc r24, r1
		adc r25, r26
#endif
		sbiw r30, 3
		st Z+, r22
		st Z+, r23
		st Z+, r24
		st Z, r25
		movw r24, r20		; return samp
		eor r1, r1
		ret


;
;	int16_t dspReverbTaps(int16_t samp, uint16_t headAddr)
;
; Using dspParams and external SRAM, fetches reverb tap signals
; and sums them onto samp.
;
; Params:
;	r25:r24 = samp
;	r23:r22 = headAddr
; Returns:
;	r25:r24 = result
;
; 42 + 34*taps + 0/4/5 (overflow none/+/-) cycles
;
; Clobber:
;
;
dspReverbTaps:
		push r15
		push r16
		push r17

		movw r16, r22		; r17:r16 = headAddr

		mov r23, r24		; accum = r24:r23:r22
		mov r24, r25		; accum = samp (sign extend) << 8
		eor r22, r22
		add r25, r25
		ror r24				; accum >>= 1
		ror r23
		ror r22
; for (i = 0; i < dspParams.taps; )
		lds r18, dspParams + dsp_param_taps
		tst r18
		breq .LdspRT_no_taps
		mov r15, r1			; r15 = i
; X = ptr16 = &dspParams.dlyList[0]
		ldi r26, lo8(dspParams + dsp_param_dlyList)
		ldi r27, hi8(dspParams + dsp_param_dlyList)
; Z = ptr8 = &dspParams.gainList[0]
		ldi r30, lo8(dspParams + dsp_param_gainList)
		ldi r31, hi8(dspParams + dsp_param_gainList)

		sts PORT_RAMA1618_OUT, r1	; high bits always zero (16 bit address)
		out PORT_RAMD_DIR, r1	; deassert data bus

.LdspRT_loop:
		movw r18, r16		; r19:r18 = headAddr - *ptr16
		ld r20, X+
		ld r21, X+
		sub r18, r20
		sbc r19, r21

; -- readRamWord inlined and customized below --
; Input parameter changed to r19:r18 (uint16_t)
; Output parameter changed to r21:r20
		andi r18, 0xfe		; force word access
		out PORT_RAMA07_OUT, r18
		out PORT_RAMA815_OUT, r19
		cbi PORT_nRDRAM_OUT, PIN_nRDRAM
		inc r18
		inc r15				; for (; ; i++)
		nop
		in r20, PORT_RAMD_IN	; get low byte
		out PORT_RAMA07_OUT, r18
		ld r18, Z+			; get multiplier
		eor r19, r19		; use r19 as zero reg
		in r21, PORT_RAMD_IN	; get high byte
		sbi PORT_nRDRAM_OUT, PIN_nRDRAM

; -- mac32r16p8 inlined and customized below --
; r24:r23:r22 += mul_su(r18, r21:r20)
		mulsu r18, r20		; p*lo
		sbc r24, r19		; sign extend
		add r22, r0
		adc r23, r1
		adc r24, r19
		muls r18, r21		; p*hi
		add r23, r0
		adc r24, r1
; for ( ; i < dspParams.taps; i++)
		lds r18, dspParams + dsp_param_taps
		cp r15, r18
		brcs .LdspRT_loop

		eor r1, r1
		ldi r18, 0xff
		out PORT_RAMD_DIR, r18	; reassert data bus

.LdspRT_no_taps:
; accum <<= 1
		lsl r22
		rol r23
		rol r24
		brvs .LdspRT_clip	; compare to +/- 0x4000
; samp = accum >> 8
		mov r25, r24
		mov r24, r23
.LdspRT_end:
		pop r17
		pop r16
		pop r15
		ret
.LdspRT_clip:
		brcs .LdspRT_clip_n
		ldi r24, lo8(32767)		; positive clipping
		ldi r25, hi8(32767)
		rjmp .LdspRT_end
.LdspRT_clip_n:
		ldi r24, lo8(-32767)	; negative clipping
		ldi r25, hi8(-32767)
		rjmp .LdspRT_end


;
;	int16_t dspBiquadFilter(int16_t samp)
;
; Calculates a timestep of a 2nd-order biquad filter on samp.
; Past samples, used in the filter calculation, are stored locally.
; Coefficients a0, a1, a2, -b1, -b2 are read from dspParams.filt.
;
; Digital biquad, direct form 1:
;	y[n] = a0 * x[n] + a1 * x[n-1] + a2 * x[n-2]
;					 - b1 * y[n-1] - b2 * y[n-2]
; Note only adds are used; b1 and b2 are to be given negated values.
;
; Params:
;	r25:r24 = input sample
; Returns:
;	r25:r24 = result
;
; 29 + 195*filtEn + 0/6/7 (overflow none/+/-) cycles
;
; Clobber (filtEn > 0):
;	r19:r18 = history[4+5*(filtEn-1)]
;	r21:r20 = filt[4+5*(filtEn-1)]
;	r22 = lo8(accum)
;	r23 = fractional bits of samp
;	X = &history[1+5*filtEn]
;	Z = &dspParams.filt[5*filtEn]
;
.lcomm	dspBQ_hist	20
dspBiquadFilter:
		push r16
		push r17
		push r28
		push r29

; j = dspParams.filtEn
		lds r28, dspParams + dsp_param_filtEn
; if (j > 2) {
;	j = 2; dspParams.filtEn = j;
; }
		cpi r28, 2
		brlo .LdspBQ_fe
		ldi r28, 2
		sts dspParams + dsp_param_filtEn, r28
.LdspBQ_fe:
		tst r28
		breq .LdspBQ_final

; ptr1 = &history[1]
		ldi r26, lo8(dspBQ_hist + 2)
		ldi r27, hi8(dspBQ_hist + 2)
; ptr2 = dspParams.filt
		ldi r30, lo8(dspParams + dsp_param_filt)
		ldi r31, hi8(dspParams + dsp_param_filt)
; for (; j; j--)
.LdspBQ_stage:
		ld r18, X+
		ld r19, X+
		st X+, r18			; history[2] = history[1]
		st X+, r19
		sbiw r26, 6
		ld r18, X
		st X+, r24			; history[0] = samp (lo)
		ld r19, X
		st X+, r25			; history[0] = samp (hi)
		st X+, r18			; history[1] = history[0]
		st X+, r19
		sbiw r26, 4
; accum = 0
		ldi r22, 0
		ldi r23, 0
		movw r24, r22

		ldi r29, 5			; i = 5
; for (i = 5; i; i--) {
;	accum = mac32p16p16(accum, ptr1++, ptr2++);
.LdspBQ_for:
; -- mac32p16p16 inlined below --
		ld r18, X+
		ld r19, X+			; *p1
		ld r20, Z+
		ld r21, Z+			; *p2
; r25:r24:r23:r22 += mul_signed(r21:r20, r19:r18)
		mul r18, r20		; lo*lo
		movw r16, r0		; hold onto result..
		muls r19, r21		; hi*hi
		adc r22, r16
		adc r23, r17
		adc r24, r0
		adc r25, r1
		eor r16, r16		; r16 = zero reg
		mulsu r19, r20		; hi*lo
		sbc r25, r16		; sign extend
		add r23, r0
		adc r24, r1
		adc r25, r16
		mulsu r21, r18		; lo*hi
		sbc r25, r16
		add r23, r0
		adc r24, r1
		adc r25, r16

		dec r29
		brne .LdspBQ_for

		lsl r23
		rol r24
		rol r25
		lsl r23
		rol r24
		rol r25				; return samp
		brvs .LdspBQ_clip	; compare to +/- 0x4000
.LdspBQ_end:
; history[4] = history[3]
; history[3] = samp
		sbiw r26, 4
		ld r18, X+
		ld r19, X+
		st X+, r18			; history[4] = history[3]
		st X+, r19
		sbiw r26, 4
		st X+, r24			; history[3] = samp
		st X+, r25
		adiw r26, 4			; point at history[1] for next round

		dec r28
		brne .LdspBQ_stage

		eor r1, r1
.LdspBQ_final:
		pop r29
		pop r28
		pop r17
		pop r16
		ret
.LdspBQ_clip:
		brcs .LdspBQ_clip_n
		ldi r24, lo8(32767)		; positive clipping
		ldi r25, hi8(32767)
		rjmp .LdspBQ_end
.LdspBQ_clip_n:
		ldi r24, lo8(-32767)	; negative clipping
		ldi r25, hi8(-32767)
		rjmp .LdspBQ_end


;
;	int16_t dspMix(int16_t samp1, int16_t samp2)
;
; Two channel mixer.  samp1 and samp2 are summed, scaled
; by the gains from dspParams.gainClean and gainReverb.
;
; Params:
;	r25:r24 = samp1
;	r23:r22 = samp2
; Returns:
;	r25:r24 = result
;
; 38 + 0/4/5 (overflow none/+/-) cycles
;
; Clobber:
;	r18 = gainReverb
;	r19 = 0
;	r21:r20 = samp2
;	r23 = fractional result
;	X = samp2
;	Z = &dspParams.gainReverb + 1
;
dspMix:
		movw r26, r22			; save samp2

		ldi r30, lo8(dspParams + dsp_param_gainClean)
		ldi r31, hi8(dspParams + dsp_param_gainClean)
		movw r20, r24		; samp1
		ldi r23, 0			; accum = 0
		ldi r24, 0
		ldi r25, 0

; mac32r16p8 inlined and modified
		ld r18, Z+			; get multiplier
		eor r19, r19		; use r19 as zero reg
		mulsu r18, r20		; p*lo
		sbc r25, r19		; sign extend
		add r23, r0
		adc r24, r1
		adc r25, r19
		muls r18, r21		; p*hi
		add r24, r0
		adc r25, r1

		movw r20, r26		; samp2

		ld r18, Z+			; get multiplier
		mulsu r18, r20		; p*lo
		sbc r25, r19		; sign extend
		add r23, r0
		adc r24, r1
		adc r25, r19
		muls r18, r21		; p*hi
		add r24, r0
		adc r25, r1
		eor r1, r1

		lsl r23
		rol r24
		rol r25
		brvs .LdspM_clip	; compare to +/- 0x4000
		ret
.LdspM_clip:
		brcs .LdspM_clip_n
		ldi r24, lo8(32767)		; positive clipping
		ldi r25, hi8(32767)
		ret
.LdspM_clip_n:
		ldi r24, lo8(-32767)	; negative clipping
		ldi r25, hi8(-32767)
		ret


#endif

.end
