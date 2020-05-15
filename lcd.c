#include "lcd.h"

volatile bool lcdTimerFlag = false;

/**
 *	Initialize display.  Assumes bus is free for use.
 */
void lcdInit(void) {

	//	Set up timer for consistent hardware delays
	LCD_TIMER.PER = LCD_WRITE_DELAY;
	LCD_TIMER.CTRLB = 0;
	LCD_TIMER.CTRLA = 0;
	LCD_TIMER.INTCTRLA = TC_OVFINTLVL_LO_gc;

	//	Set up port pins
	DeassertWrLcd();
#ifdef USE_VIRTUAL_PORTS
	PORT_WRLCD.DIR |= BIT_WRLCD;
	PORT_LCD_RS.DIR |= BIT_LCD_RS;
#else
	PORT_WRLCD.DIRSET = BIT_WRLCD;
	PORT_LCD_RS.DIRSET = BIT_LCD_RS;
#endif // USE_VIRTUAL_PORTS

	//	Reset LCD
	_delay_us(LCD_INIT_LONG_DELAY);
	lcdWrite(LCD_INIT_CMD | LCD_INIT_BYTE, false);
	_delay_us(LCD_INIT_DELAY);
	lcdWrite(LCD_INIT_CMD | LCD_INIT_BYTE, false);
	_delay_us(LCD_INIT_SHRT_DELAY);
	lcdWrite(LCD_INIT_CMD | LCD_INIT_BYTE, false);

	//	Initialize
	lcdWrite(LCD_INIT_CMD | LCD_INIT_BYTE | LCD_INIT_LINES, false);
	//	Display off
	lcdWrite(LCD_DISP_CTRL, false);
	_delay_us(LCD_INIT_DELAY);
	lcdWrite(LCD_INCREMENT, false);
	//	Display on
	lcdWrite(LCD_DISP_CTRL | LCD_DISP_ON | LCD_DISP_CURSOR | LCD_DISP_BLINK, false);

}

/**
 *	Prints a string of [chr] to the display.
 *	@param	chr	Character to fill.
 *			len	Number of bytes to print.
 *	@return	bytes written.
 */
uint8_t lcdPrintFill(const uint8_t chr, const uint8_t len) {

	uint8_t i;

	for (i = 0; i < len; i++) {
		lcdWrite(chr, true);
	}
	return len;

}

/**
 *	Prints an ASCIIZ string the display.
 *	@param	s
 *	@return	characters printed (mod 256)
 */
uint8_t lcdPrintStr(uint8_t* s) {

	uint8_t l;

	while (*s) {
		lcdWrite(*s, true);
		s++; l++;
	}
	return l;
}

/**
 *	Prints an ASCIIZ string to the display,
 *	up to a maximum buffer size.
 *	@param	buf	Buffer to print
 *			len	Maximum length of buffer
 *	@return Bytes written
 */
uint8_t lcdPrintStrLen(uint8_t* buf, uint8_t len) {

	uint8_t i;

	for (i = 0; i < len && *buf; i++) {
		lcdWrite(*buf++, true);
	}

	return i;
}

/**
 *	Prints an ASCII formatted decimal number to the display.
 *	@param	n
 *	@return	characters printed
 */
uint8_t lcdPrintNumDec(uint16_t n) {

	uint8_t l, buf[8];

	utoa(n, (char*)buf, 10);
	for (l = 0; buf[l]; l++) {
		lcdWrite(buf[l], true);
	}
	return l;
}

/**
 *	Sets the cursor state.
 */
void lcdCursor(bool c) {
	lcdWrite(LCD_DISP_CTRL | LCD_DISP_ON | (LCD_DISP_CURSOR & (-(int8_t)c)), false);
}

/**
 *	Sets the cursor (DDRAM) location.
 */
void lcdLocate(uint8_t col, uint8_t row) {
	lcdWrite((col + 0x40 * (row & 0x03)) | LCD_DADDR, false);
}

/**
 *	Writes a byte to the LCD.
 *	@param d	Data byte
 *	@param ctrl	true = data, false = control
 */
void lcdWrite(uint8_t d, bool ctrl) {

	lcdWaitTimer();
	//	Wait for bus available
	while (dspActive() && dspImminent());
	//	Ok, safe to take control of bus
	dspStop();
	if (ctrl) {
		AssertLcdRs();
	} else {
		DeassertLcdRs();
	}
	//	Set data
	PORT_LCD_DAT.OUT = d;
	PORT_LCD_DAT.DIR = 0xff;
	//	Strobe write
	AssertWrLcd();
	_NOP(); _NOP();
	DeassertWrLcd();
	dspStart();
	lcdStartTimer(LCD_WRITE_DELAY);

}

/**
 *	Timer interrupt handler.
 */
ISR(LCD_VECTOR) {
	lcdTimerDisable();
	lcdTimerFlag = true;
}
