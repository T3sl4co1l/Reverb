#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#define F_CPU				32000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pindefs.h"
#include "dsp.h"

//	Clear display command
#define LCD_CLS				0x01
//	Return to 0, reset shift
#define LCD_HOME			0x02
//	Entry mode set, cursor dec.
#define LCD_DECREMENT		0x04
//	Entry mode set, cursor inc.
#define LCD_INCREMENT		0x06
//	Display control; use with following bits (OR to activate/mask to deactivate)
#define LCD_DISP_CTRL		0x08
//	display on (active)
#define LCD_DISP_ON			0x04
//	cursor blinks (underline)
#define LCD_DISP_CURSOR		0x02
//	cursor blinks (full character)
#define LCD_DISP_BLINK		0x01
//	Set character (CGRAM) address (0..63)
#define LCD_CADDR			0x40
//	Set display (DDRAM) address (0..127)
#define LCD_DADDR			0x80

//	Function set (additive flags follow)
#define LCD_INIT_CMD		0x20
#define LCD_INIT_BYTE		0x10
#define LCD_INIT_LINES		0x08
#define LCD_INIT_FONT		0x04

//	Delays (us)
#define LCD_INIT_DELAY		4000
#define LCD_INIT_LONG_DELAY	40000
#define LCD_INIT_SHRT_DELAY	80
#define LCD_WRITE_DELAY		42

#define LCD_TIMER			TCD0
#define LCD_VECTOR			TCD0_OVF_vect

#define lcdTimerEnable()	LCD_TIMER.CTRLA = TC_CLKSEL_DIV64_gc
#define lcdTimerDisable()	LCD_TIMER.CTRLA = 0

#define lcdStartTimer(d)													\
	do {																	\
		lcdTimerDisable();													\
		lcdTimerFlag = false;												\
		LCD_TIMER.PER = ( (uint32_t)(d / 2) > 65535 ? 65535 : (d / 2) );	\
		LCD_TIMER.CNT = 0;													\
		lcdTimerEnable();													\
	} while (0);

#define lcdWaitTimer()														\
	do {																	\
		lcdTimerEnable();													\
		while (!lcdTimerFlag);												\
		lcdTimerDisable();													\
	} while (0);

#define _delay_us(d) lcdStartTimer(d); lcdWaitTimer()

void lcdInit(void);
uint8_t lcdPrintFill(const uint8_t chr, const uint8_t len);
uint8_t lcdPrintStr(uint8_t* s);
uint8_t lcdPrintStrLen(uint8_t* buf, uint8_t len);
uint8_t lcdPrintNumDec(uint16_t n);
void lcdCursor(bool c);
void lcdLocate(uint8_t col, uint8_t row);
void lcdWrite(uint8_t d, bool ctrl);

#endif // LCD_H_INCLUDED
