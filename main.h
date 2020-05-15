#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define F_CPU				32000000UL

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/cpufunc.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "pindefs.h"
#include "bufserial.h"
#include "ccpwrite.h"
#include "console.h"
#include "lcd.h"
#include "dsp.h"
#include "menu.h"

#ifndef numelem
#define numelem(x)			( sizeof(x) / sizeof(x[0]) )
#endif

//	Number of samples of pressed input before the debounce routine detects it
#define DEBOUNCE_SAMPLES	4
//	Samples before repeat mode begins
#define DEBOUNCE_WAIT		28
//	Samples between repeats
#define DEBOUNCE_REPEAT		6

typedef __uint24 uint24_t;

int main(void);
void doMenu(void);
uint8_t debouncePBs(void);
void initialize(void);

#endif // MAIN_H_INCLUDED
