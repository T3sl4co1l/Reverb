#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "lcd.h"
#include <avr/eeprom.h>

/** Cursor range on the display; typically, the resolution of character LCD */
#define MENU_X_OFFSET	0
#define MENU_Y_OFFSET	0
#define MENU_X_RANGE	16
#define MENU_Y_RANGE	2

//	Note: sync with same define in dsp.h
#define MENU_TITLE_LEN	8
#define MENU_FIXSTR8LEN	8

#define MENU_FLAGMASK	0x3f

/** Menu item type identifiers. */
enum menu_type_e {
	MENU_END			= 0,	//	Menu list terminator
	MENU_TITLE			= 1,	//	Title item, displays a string
	MENU_SUBMENU		= 2,	//	Submenu item
	MENU_FUNC			= 3,	//	Function call item
	MENU_FIXEDSTRING8	= 4,	//	Adjustable string, 8 characters
	MENU_INT8			= 5,	//	Adjustable parameter, unsigned 8-bit
	MENU_INT16			= 6,	//	Adjustable parameter, unsigned 16-bit
	MENU_NOEDIT			= 0x40,	//	Flag, field cannot be edited
	MENU_SIGNED			= 0x80	//	Flag, numeric type is signed
};
typedef uint8_t menu_type_t;
/** Flags for menu actions, movement, etc. */
enum menu_action_e {
	MENU_ACT_UP		= 4,
	MENU_ACT_RIGHT	= 2,
	MENU_ACT_DOWN	= 1,
	MENU_ACT_LEFT	= 8
};

/**
 *	Menu item.
 *	type	selects the item's function.
 *	ptr		pointer to relevant data, e.g., ASCIIZ string extending the title,
 *			string or number to display or edit.
 *	title	text displayed following the index number.
 *			Additional text can be printed using a MENU_TITLE type.
 */
struct menu_item_s {
	menu_type_t type;
	volatile void* ptr;
	uint8_t title[MENU_TITLE_LEN];
};

/**
 *	Menu end.
 *	type	Must be MENU_END
 *	ptr		Pointer to previous menu
 *	offs	Offset for displayed item numbers
 *	tot		Total number of items in the menu
 *	setup	Function called when menu is: entered,
 *	act			acted on,
 *	leave		exited.
 */
struct menu_end_s {
	menu_type_t type;
	void* ptr;
	uint8_t offs;
	void (*setup)(void);
	void (*act)(int8_t, int8_t);
	void (*leave)(void);
};

typedef union menu_item_u {
	struct menu_item_s i;
	struct menu_end_s e;
} menu_item_t;

extern menu_item_t MenuMain[];

void menuInit(menu_item_t* m);
void menuAct(uint8_t inp);
uint8_t menuItemXRange(uint8_t type);
void menuDraw(void);

#endif // MENU_H_INCLUDED
