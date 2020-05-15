; * * *  Interrupt Handlers  * * *

#define __SFR_OFFSET 0

#include "dsp.h"

#ifdef DSP_USE_ASM

; equs for struct:
;
;	typedef struct dsp_params_s {
;		uint8_t taps;
;		bool filtEn;
;		uint16_t dlyList[4];
;		uint8_t gainList[4];
;		int16_t filt[5];
;		uint8_t name[MENU_TITLE_LEN];
;	} dsp_params_t;
.equiv dsp_param_taps, 0
.equiv dsp_param_filtEn, 1
.equiv dsp_param_dlyList, 2
.equiv dsp_param_gainList, 10
.equiv dsp_param_filt, 14
.equiv dsp_param_name, 24

.text

.global ADCA_CH0_vect
#ifdef RAM_WORD_ACCESS
.global readRamWord
.global writeRamWord
#endif
.global mac32r16p8
#if DSP_USE_ASM == 1
.global mac32p16p16
#endif
.global writeDac
.global dspHighpass
.global dspReverbTaps
.global dspBiquadFilter

#define adcLiveSampleL		GPIOR0
#define adcLiveSampleH		GPIOR1

;
;	ISR(ADCA_CH0_vect)
;
; ADC conversion interrupt, 200kS/s.
;
; Params:
;	none
; Returns:
;	nothing
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
readRamWord:
		; r24:r23:r22 = addr & 0x7fffe (forces word aligned access)
		andi r22, 0xfe
		andi r24, BIT_A16 | BIT_A17 | BIT_A18
		sts PORT_RAMA1618_OUT, r24
		out PORT_RAMA07_OUT, r22
		out PORT_RAMA815_OUT, r23
		out PORT_RAMD_DIR, r1	; deassert data bus
		cbi PORT_nRDRAM_OUT, PIN_nRDRAM
		subi r22, 0xff			; addr++
		;nop						; no carry to mid or high byte because word align
		;nop
		sbci r23, 0xff
		sbci r24, 0xff
		andi r24, BIT_A16 | BIT_A17 | BIT_A18
		in r21, PORT_RAMD_IN	; r21 = low byte
		sts PORT_RAMA1618_OUT, r24
		out PORT_RAMA07_OUT, r22
		out PORT_RAMA815_OUT, r23
		nop
		nop
		nop
		in r25, PORT_RAMD_IN	; r25 = high byte
		sbi PORT_nRDRAM_OUT, PIN_nRDRAM
		mov r24, r21			; return r25:r24
		ldi r21, 0xff
		out PORT_RAMD_DIR, r21	; reassert data bus
		ret


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
; 23/24 cycles (linear/clipping)
;
; Clobber:
;	r20 = 0xff
;
writeDac:

; right adjust bits, with rounding

		asr r25
		ror r24
		asr r25
		ror r24
		adiw r24, 1
		asr r25
		ror r24

; clamp to range -2048...2047

		cpi r25, hi8(-2048)		; only need to check hi8(arg)
		brge 1f					; if (v < -2048)
		ldi r24, lo8(-2048)		;   v = -2048
		ldi r25, hi8(-2048)
1:
		cpi r25, hi8(2048)
		brlt 2f					; if (v >= 2048)
		ldi r24, lo8(2047)		;   v = 2047
		ldi r25, hi8(2047)
2:
		subi r25, hi8(2048)		; convert to unsigned DAC output

; output to DAC

		out PORT_DACLO_OUT, r24
; data bus is active by default
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
#if DSP_USE_ASM != 2
mac32p16p16:
		movw r26, r20
		movw r30, r18
		ld r18, X+
		ld r19, X+			; get [p1]
		ld r20, Z+
		ld r21, Z+			; get [p2]

; r25:r24:r23:r22 += mul_signed(r21:r20, r19:r18)

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

		eor r1, r1
		ret
#endif


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
; 20 cycles
;
; Clobber:
;	r18 = *(int8_t*)p
;	r19 = 0
;	Z = p + 1
;
mac32r16p8:
		movw r30, r18
		ld r18, Z+			; get multiplier

; r25:r24:r23:r22 += mul_signed(r21:r20, r18)

		eor r19, r19		; use r19 as zero reg
		mulsu r18, r20		; p*lo
		sbc r24, r19
		sbc r25, r19		; sign extend
		add r22, r0
		adc r23, r1
		adc r24, r19
		adc r25, r19
		muls r18, r21		; p*hi
		sbc r25, r19
		add r23, r0
		adc r24, r1
		adc r25, r19
		eor r1, r1
		ret


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
; x cycles
;
; Clobber:
;
;
dspReverbTaps:
		push r7
		push r8
		push r9
		push r10
		push r11
		push r12
		push r13
		push r14
		push r15
		push r16
		push r17
		push r28
		push r29
		movw r12, r22
		movw r8, r24
		add r25, r25
		sbc r10, r10
		sbc r11, r11
		mov r11, r10
		mov r10, r9
		mov r9, r8
		eor r8, r8
		asr r11
		ror r10
		ror r9
		ror r8
		lds r24, 0x214E ; 0x80214e <dspParams>
		and r24, r24
		breq .+76      ; 0x15ea <dspReverbTaps+0x88>
		mov r7, r1
		ldi r16, 0x58 ; 88
		ldi r17, 0x21 ; 33
		ldi r28, 0x50 ; 80
		ldi r29, 0x21 ; 33
		ld r24, Y+
		ld r25, Y+
		movw r22, r12
		sub r22, r24
		sbc r23, r25
		ldi r24, 0x00 ; 0
		call 0x5da ; 0x5da <readRamWord>
		movw r14, r16
		ldi r18, 0xFF ; 255
		sub r14, r18
		sbc r15, r18
		movw r18, r16
		movw r20, r24
		movw r24, r10
		movw r22, r8
		call 0x6a6 ; 0x6a6 <mac32r16p8>
		movw r8, r22
		movw r10, r24
		inc r7
		lds r24, 0x214E ; 0x80214e <dspParams>
		movw r16, r14
		cp r7, r24
		brcs .-52      ; 0x15a8 <dspReverbTaps+0x46>
		ldi r24, 0xFF ; 255
		cp r8, r24
		cpc r9, r24
		ldi r24, 0x3F ; 63
		cpc r10, r24
		cpc r11, r1
		brge .+98      ; 0x164c <dspReverbTaps+0xea>
		ldi r18, 0x02 ; 2
		cp r8, r18
		cpc r9, r1
		ldi r18, 0xC0 ; 192
		cpc r10, r18
		ldi r18, 0xFF ; 255
		cpc r11, r18
		brlt .+88      ; 0x1652 <dspReverbTaps+0xf0>
		eor r27, r27
		sbrc r11, 7
		dec r27
		mov r26, r11
		mov r25, r10
		mov r24, r9
		movw r18, r24
		add r18, r18
		adc r19, r19
		movw r26, r10
		movw r24, r8
		add r24, r24
		adc r25, r25
		adc r26, r26
		adc r27, r27
		mov r24, r25
		mov r25, r26
		mov r26, r27
		eor r27, r27
		sbrc r26, 7
		dec r27
		andi r24, 0x01 ; 1
		eor r25, r25
		eor r26, r26
		eor r27, r27
		or r24, r18
		or r25, r19
		pop r29
		pop r28
		pop r17
		pop r16
		pop r15
		pop r14
		pop r13
		pop r12
		pop r11
		pop r10
		pop r9
		pop r8
		pop r7
		ret
		ldi r24, 0xFF ; 255
		ldi r25, 0x7F ; 127
		rjmp .-34      ; 0x1630 <dspReverbTaps+0xce>
		ldi r24, 0x01 ; 1
		ldi r25, 0x80 ; 128
		rjmp .-40      ; 0x1630 <dspReverbTaps+0xce>


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
; 187/186 cycles (linear/clipped, DSP_USE_ASM = 2)
;
; Clobber:
;	r19:r18 = history[4]
;	r21:r20 = filt[4]
;	r22 = lo8(accum)
;	r23 = fractional bits of samp
;	X = &history[4]
;	Z = &dspParams.filt[4]
;
.lcomm	dspBQhist	10
dspBiquadFilter:
		push r16
		push r17
		push r28
#if DSP_USE_ASM != 2
		push r29
#endif
		ldi r26, lo8(dspBQhist + 2)
		ldi r27, hi8(dspBQhist + 2)
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
#if DSP_USE_ASM != 2
; ptr1 = dspParams.filt
		ldi r28, lo8(dspParams + dsp_param_filt)
		ldi r29, hi8(dspParams + dsp_param_filt)
; ptr2 = history
		movw r16, r26
; for (i = 0; i < 5; i++) {
;	accum = mac32p16p16(accum, ptr1++, ptr2++)
.LdspBQfor:
		movw r18, r16
		movw r20, r28
		call mac32p16p16
		adiw r28, 2
		subi r16, 0xfe
		sbci r17, 0xff
		ldi r18, hi8(dspParams + dsp_param_filt + 10)
		cpi r28, lo8(dspParams + dsp_param_filt + 10)
		cpc r29, r18
		brne .LdspBQfor
; }
#else
; ptr1 = history
; ptr2 = dspParams.filt
		ldi r30, lo8(dspParams + dsp_param_filt)
		ldi r31, hi8(dspParams + dsp_param_filt)
		ldi r28, hi8(dspParams + dsp_param_filt + 10)
; for (i = 0; i < 5; i++) {
;	accum = mac32p16p16(accum, ptr1++, ptr2++);
.LdspBQfor:
		;call mac32p16p16_asm
; INLINED BELOW
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
		; inlined function saves 30 cycles!
		cpi r30, lo8(dspParams + dsp_param_filt + 10)
		cpc r31, r28
		brne .LdspBQfor
		eor r1, r1			; zero reg
#endif
; if (samp >= 0x2000) {
		;cpi r24, lo8(0x2000)
		;ldi r17, hi8(0x2000)
		;cpc r25, r17
		cpi r25, hi8(0x2000)	; don't need 16-bit compares for low byte 0x00
		brge .LdspBQge
; } else if (samp <= -0x2000) {
		;cpi r24, lo8(-0x2000)
		;ldi r17, hi8(-0x2000)
		;cpc r25, r17
		cpi r25, hi8(-0x2000)
		brlt .LdspBQlt
; } else {
;	samp = (accum >> 16) << 2
		lsl r23
		rol r24
		rol r25
		lsl r23
		rol r24
		rol r25				; return samp
; history[4] = history[3]
; history[3] = samp
.LdspBQout:
		ldi r26, lo8(dspBQhist + 6)
		ldi r27, hi8(dspBQhist + 6)
		ld r18, X+
		ld r19, X+
		st X+, r18			; history[4] = history[3]
		st X+, r19
		sbiw r26, 4
		st X+, r24			; history[3] = samp
		st X+, r25
#if DSP_USE_ASM != 2
		pop r29
#endif
		pop r28
		pop r17
		pop r16
		ret
.LdspBQge:
;	samp = 32767
		ldi r24, lo8(32767)
		ldi r25, hi8(32767)
		rjmp .LdspBQout
.LdspBQlt:
;	samp = -32767
		ldi r24, lo8(-32767)
		ldi r25, hi8(-32767)
		rjmp .LdspBQout


#endif

.end
