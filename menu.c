#include "menu.h"

/* * *  Global Function Declarations  * * */

void menuSetupPresets(void);
void menuActMain(int8_t moveX, int8_t moveY);
void menuActPresets(int8_t moveX, int8_t moveY);
void menuSavePreset(void);
void menuLoadPreset(void);


/* * *  Global Variables  * * */

uint8_t PresetIndex = 1;

//	Shared from main.c -- in EEPROM
dsp_params_t Presets[16];
menu_item_t MenuPreset[];

uint8_t menuStrPresetsTitle[] = "s";
uint8_t menuStrReverbTitle[] = "b Taps -";
uint8_t menuStrFilterTitle[] = " Coefs -";

menu_item_t MenuMain[] = {
	{ .i = {
		.type = MENU_TITLE,
		.ptr = NULL,
		.title = "- Main -"
	} },
	{ .i = {
		.type = MENU_SUBMENU,
		.ptr = MenuPreset,
		.title = "Presets"
	} },
	{ .i = {
		.type = MENU_INT8 | MENU_NOEDIT,
		.ptr = &PresetIndex,
		.title = "File #"
	} },
	{ .i = {
		.type = MENU_FUNC,
		.ptr = menuLoadPreset,
		.title = "Load"
	} },
	{ .i = {
		.type = MENU_FUNC,
		.ptr = menuSavePreset,
		.title = "Save"
	} },
	{ .i = {
		.type = MENU_FIXEDSTRING8,
		.ptr = &dspParams.name,
		.title = "Name "
	} },
	{ .i = {
		.type = MENU_TITLE,
		.ptr = menuStrReverbTitle,
		.title = "- Rever"
	} },
	{ .i = {
		.type = MENU_INT8,
		.ptr = &dspParams.taps,
		.title = "Taps"
	} },
	{ .i = {
		.type = MENU_INT16,
		.ptr = &dspParams.dlyList[0],
		.title = "Dly 1"
	} },
	{ .i = {
		.type = MENU_INT8 | MENU_SIGNED,
		.ptr = &dspParams.gainList[0],
		.title = "Gain 1"
	} },
	{ .i = {
		.type = MENU_INT16,
		.ptr = &dspParams.dlyList[1],
		.title = "Dly 2"
	} },
	{ .i = {
		.type = MENU_INT8 | MENU_SIGNED,
		.ptr = &dspParams.gainList[1],
		.title = "Gain 2"
	} },
	{ .i = {
		.type = MENU_INT16,
		.ptr = &dspParams.dlyList[2],
		.title = "Dly 3"
	} },
	{ .i = {
		.type = MENU_INT8 | MENU_SIGNED,
		.ptr = &dspParams.gainList[2],
		.title = "Gain 3"
	} },
	{ .i = {
		.type = MENU_INT16,
		.ptr = &dspParams.dlyList[3],
		.title = "Dly 4"
	} },
	{ .i = {
		.type = MENU_INT8 | MENU_SIGNED,
		.ptr = &dspParams.gainList[3],
		.title = "Gain 4"
	} },
	{ .i = {
		.type = MENU_TITLE,
		.ptr = menuStrFilterTitle,
		.title = "- Filter"
	} },
	{ .i = {
		.type = MENU_INT8,
		.ptr = &dspParams.filtEn,
		.title = "Filt En."
	} },
	{ .i = {
		.type = MENU_INT16 | MENU_SIGNED,
		.ptr = &dspParams.filt[0],
		.title = "a0"
	} },
	{ .i = {
		.type = MENU_INT16 | MENU_SIGNED,
		.ptr = &dspParams.filt[1],
		.title = "a1"
	} },
	{ .i = {
		.type = MENU_INT16 | MENU_SIGNED,
		.ptr = &dspParams.filt[2],
		.title = "a2"
	} },
	{ .i = {
		.type = MENU_INT16 | MENU_SIGNED,
		.ptr = &dspParams.filt[3],
		.title = "b1"
	} },
	{ .i = {
		.type = MENU_INT16 | MENU_SIGNED,
		.ptr = &dspParams.filt[4],
		.title = "b2"
	} },/*
	{ .i = {
		.type = MENU_,
		.ptr = NULL,
		.title = ""
	} },
*/
	{ .e = {
		.type = MENU_END,
		.ptr = MenuMain,
		.offs = 0,
		.setup = NULL,
		.act = menuActMain,
		.leave = NULL
	} }
};

uint8_t menuStrPresetMenuTitle[] = "s -";

//	This menu is handled specially by menuSetupPresets and menuActPresets
menu_item_t MenuPreset[] = {
	{ .i = {
		.type = MENU_TITLE,
		.ptr = menuStrPresetMenuTitle,
		.title = "- Preset"
	} },
//	Place as many elements here as MENU_Y_RANGE:
	{ .i = {
		.type = MENU_FIXEDSTRING8 | MENU_NOEDIT,
		.ptr = &Presets[0].name,
		.title = ""
	} },
	{ .i = {
		.type = MENU_FIXEDSTRING8 | MENU_NOEDIT,
		.ptr = &Presets[1].name,
		.title = ""
	} },
	{ .e = {
		.type = MENU_END,
		.ptr = MenuMain,
		.offs = 0,
		.setup = menuSetupPresets,
		.act = menuActPresets,
		.leave = NULL
	} }
};

/** Current menu array */
menu_item_t* menuMenu = MenuMain;
menu_item_t* menuEnd = &MenuMain[numelem(MenuMain) - 1];
/** Current item index into menuMenu */
uint8_t menuIndex;
/** Total items in menuMenu */
uint8_t menuTotal;
/** Lateral position of the cursor on the current menu item */
uint8_t menuPosX, menuPosY;


/* * *  Functions  * * */

/**
 *	Initializes to the specified menu.
 */
void menuInit(menu_item_t* m) {

	uint8_t i;

	for (i = 0; i < 255; i++) {
		if (m[i].i.type == MENU_END)
			break;
	}
	menuTotal = i;
	menuMenu = m;
	menuEnd = &m[i];
	menuIndex = 0;
	menuPosX = 0; menuPosY = 0;
	if (menuEnd->e.setup != NULL)
		menuEnd->e.setup();

}

/**
 *	Advances the menu state machine.
 *	@param inp	Bitmask representing the inputs to evaluate.
 */
void menuAct(uint8_t inp) {

	int8_t moveX = 0, moveY = 0;

	if (inp & MENU_ACT_RIGHT)	moveX++;
	if (inp & MENU_ACT_LEFT)	moveX--;
	if (inp & MENU_ACT_DOWN)	moveY++;
	if (inp & MENU_ACT_UP)		moveY--;


	if (menuEnd->e.act != NULL)
		menuEnd->e.act(moveX, moveY);

}

/**
 *	Returns how many (menuAct::menuPosX) positions
 *	can be occupied in the given menu item type.
 */
uint8_t menuItemXRange(uint8_t type) {


	if (type & MENU_NOEDIT) {
		return 0;
	}
	switch (type & MENU_FLAGMASK) {
	case MENU_FIXEDSTRING8:
		return MENU_FIXSTR8LEN;
	case MENU_INT8:
		if (type & MENU_SIGNED) {
			return 4;
		}
		return 3;
	case MENU_INT16:
		if (type & MENU_SIGNED) {
			return 6;
		}
		return 5;
	case MENU_TITLE:
	case MENU_SUBMENU:
	case MENU_END:
	case MENU_FUNC:
	default:
		return 0;
	}
}

/**
 *	Gets the length (digits) of a number when formatted in decimal.
 */
uint8_t numLen(uint16_t n) {

	uint8_t r = 1;
	if (n >= 10)	r++;
	if (n >= 100)	r++;
	if (n >= 1000)	r++;
	if (n >= 10000)	r++;
	return r;
}

/**
 *	Refreshes the menu display.
 */
void menuDraw(void) {

	uint8_t c, i, remaining, right, t;
	uint16_t v;

	//	Calculate item index numbers
	c = menuEnd->e.offs + 1;
	for (i = 0; i < menuIndex && i < menuTotal; i++) {
		t = menuMenu[i].i.type;
		if (t != MENU_END && t != MENU_TITLE)
			c++;
	}
	for (; i < menuIndex + MENU_Y_RANGE && i < menuTotal; i++) {
		//	Print menu index and title
		lcdLocate(MENU_X_OFFSET, MENU_Y_OFFSET + i - menuIndex);
		//	Decode type, print value if applicable
		t = menuMenu[i].i.type;

		remaining = MENU_X_RANGE;	//	Bytes remaining to fill line
		if (t != MENU_END && t != MENU_TITLE) {
			//	Don't print item index number on titles
			remaining -= lcdPrintNumDec(c) + 2;
			c++;
			lcdWrite('.', true);
			lcdWrite(' ', true);
		}
		//	Print title
		if (t != MENU_END) {
			remaining -= lcdPrintStrLen(menuMenu[i].i.title, MENU_TITLE_LEN);
		}
		switch (t & MENU_FLAGMASK) {
		case MENU_TITLE:	//	Title: also print attached string
			if (menuMenu[i].i.ptr != NULL)
				remaining -= lcdPrintStr((uint8_t*)menuMenu[i].i.ptr);
			break;
		case MENU_FIXEDSTRING8:	//	Editable string
			right = menuItemXRange(t & ~MENU_NOEDIT);
			if (menuMenu[i].i.ptr != NULL) {
				if (remaining < right) {
					lcdLocate(MENU_X_OFFSET + MENU_X_RANGE - right, MENU_Y_OFFSET + i - menuIndex);
					remaining = right;
				}
				remaining -= lcdPrintFill(' ', remaining - right);
				remaining -= lcdPrintStrLen((uint8_t*)menuMenu[i].i.ptr, MENU_FIXSTR8LEN);
			}
			break;
		case MENU_INT8:
		case MENU_INT16:	//	Editable number
			right = menuItemXRange(t & ~MENU_NOEDIT);
			if (remaining < right) {
				lcdLocate(MENU_X_OFFSET + MENU_X_RANGE - right, MENU_Y_OFFSET + i - menuIndex);
				remaining = right;
			}
			remaining -= lcdPrintFill(' ', remaining - right);
			v = *(uint16_t*)menuMenu[i].i.ptr;
			if ((t & MENU_FLAGMASK) == MENU_INT8)
				v = (int8_t)(v & 0x00ff);
			if (t & MENU_SIGNED && (int16_t)v < 0) {
				v = -v;
				lcdWrite('-', true);
				remaining--;
			}
			remaining -= lcdPrintFill(' ', remaining - numLen(v));
			remaining -= lcdPrintNumDec(v);
			break;
		case MENU_END:	//	What are you doing here?!
			i = menuTotal;
			break;
		case MENU_SUBMENU:
		case MENU_FUNC:
		default:
			break;
		}
		if (remaining >= 128) remaining = 0;
		lcdPrintFill(' ', remaining);
	}

	right = 0;
	if (menuPosX > 0) {
		right = -menuItemXRange(menuMenu[menuIndex + menuPosY].i.type)
				+ menuPosX + MENU_X_RANGE - 1;
	}
	lcdLocate(MENU_X_OFFSET + right, MENU_Y_OFFSET + menuPosY);

}

/**
 *	Calculate movement on the main menu.
 */
void menuActMain(int8_t moveX, int8_t moveY) {

	uint8_t m, t;
	int16_t add;

	/*	Handle Y moves  */

	if (menuPosX == 0) {
		/*	Cursor in left column: menu navigation  */

		//	Clamp Y range, overflowing into menuIndex (and clamping that to 0...menuTotal)
		if (menuPosY < -moveY) {
			moveY += menuPosY;
			menuPosY = 0;
			if (menuIndex < -moveY) {
				moveY = -menuIndex;
			}
			menuIndex += moveY;
			moveY = 0;
		} else if (menuPosY + moveY > MENU_Y_RANGE - 1) {
			moveY -= MENU_Y_RANGE - 1 - menuPosY;
			menuPosY = MENU_Y_RANGE - 1;
			if (menuIndex + moveY + MENU_Y_RANGE > menuTotal) {
				moveY = menuTotal - menuIndex - MENU_Y_RANGE;
			}
			menuIndex += moveY;
			moveY = 0;
		}
		menuPosY += moveY;
	} else {
		/*	Cursor in item: editing  */
		t = menuMenu[menuIndex + menuPosY].i.type;
		m = menuItemXRange(t);
		switch (t & MENU_FLAGMASK) {
		case MENU_FIXEDSTRING8:
			m = ((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr))[menuPosX - 1];
			m -= moveY;
			//	Wrap to printable ASCII range
			if (m < 32)
				m = 126;
			if (m > 126)
				m = 32;
			((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr))[menuPosX - 1] = m;
			break;
		case MENU_INT8:
			if (menuPosX == 1 && t & MENU_SIGNED) {	//	Toggle sign
				if (moveY & 0x01) {	//	Only parity matters
					*((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr)) =
						-*((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
				}
			} else {
				//	menuPosX = 2..4, m = 4, MENU_SIGNED; or, menuPosX = 1..3, m = 3, !MENU_SIGNED
				m = m - menuPosX;
				//	m = 2..0
				for (add = 1; m; m--) {
					add *= 10;
				}
				add *= -moveY;
				if (t & MENU_SIGNED) {
					add += *((int8_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
					if (add > 127) add = 127;
					if (add < -128) add = -128;
				} else {
					add += *((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
					if (add > 255) add = 255;
					if (add < 0) add = 0;
				}
				*((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr)) = (uint8_t)add;
			}
			break;
		case MENU_INT16:
			if (menuPosX == 1 && t & MENU_SIGNED) {	//	Toggle sign
				if (moveY & 0x01) {	//	Only parity matters
					*((uint16_t*)(menuMenu[menuIndex + menuPosY].i.ptr)) =
						-*((uint16_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
				}
			} else {
				//	menuPosX = 2..6, m = 6, MENU_SIGNED; or, menuPosX = 1..5, m = 5, !MENU_SIGNED
				m = m - menuPosX;
				//	m = 4..0
				for (add = 1; m; m--) {
					add *= 10;
				}
				add *= -moveY;
				add += *((int16_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
				/*
				//	TODO: figure out what to do with overflow/saturation
				if (t & MENU_SIGNED) {
					add += *((int8_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
					if (add > 127) add = 127;
					if (add < -128) add = -128;
				} else {
					add += *((uint8_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
					if (add > 255) add = 255;
					if (add < 0) add = 0;
				}
				*/
				*((int16_t*)(menuMenu[menuIndex + menuPosY].i.ptr)) = add;
			}
			break;
		case MENU_TITLE:
		case MENU_SUBMENU:
		case MENU_END:
		case MENU_FUNC:
		default:
			break;
		}

	}

	/*	Handle X moves  */

	t = menuMenu[menuIndex + menuPosY].i.type & MENU_FLAGMASK;

	//	Move left, off the menu: go to previous menu, if present
	if (menuPosX < -moveX) {
		moveX = -menuPosX;
		if (menuEnd->e.leave != NULL)
			menuEnd->e.leave();
		if (menuEnd->e.ptr != NULL && menuEnd->e.ptr != menuMenu) {
			//	Checks out, change over to it
			menuInit(menuEnd->e.ptr);
		}
	}

	//	Move right, into a submenu or function call
	if (moveX > 0) {
		if (t == MENU_SUBMENU) {
			if (menuMenu[menuIndex + menuPosY].i.ptr != NULL) {
				menuInit((menu_item_t*)(menuMenu[menuIndex + menuPosY].i.ptr));
				return;
			}
		} else if (t == MENU_FUNC) {
			if (menuMenu[menuIndex + menuPosY].i.ptr != NULL) {
				((void(*)(void))menuMenu[menuIndex + menuPosY].i.ptr)();
			}
		}
	}

	//	Move left or right within item
	menuPosX += moveX;
	m = menuItemXRange(menuMenu[menuIndex + menuPosY].i.type);
	if (menuPosX > m) {
		menuPosX = m;
	}

}

/**
 *	Initializes the preset menu with data from EEPROM.
 */
void menuInitPresets(void) {

	uint8_t i;

	for (i = 0; i < MENU_Y_RANGE; i++) {
		//	Read from memory mapped EEPROM
		MenuPreset[i + 1].i.ptr = &Presets[menuEnd->e.offs + i].name[0] + MAPPED_EEPROM_START;
	}

	eeprom_busy_wait();
	//	Re-enable EEPROM memory window in case write disabled it
	NVM.CTRLB = NVM_EEMAPEN_bm;

}

/**
 *	Initializes the preset menu with data from EEPROM.
 */
void menuSetupPresets(void) {

	menuEnd->e.offs = 0;
	menuInitPresets();

}

/**
 *	Calculate movement on the preset menu.
 */
void menuActPresets(int8_t moveX, int8_t moveY) {

	/*	Handle Y moves  */

	//	Clamp Y range, overflowing into menuIndex and offs
	if (menuPosY < -moveY) {
		moveY += menuPosY;
		menuPosY = 0;
		if (menuEnd->e.offs < -moveY) {
			moveY += menuEnd->e.offs;
			menuEnd->e.offs = 0;
			if (menuIndex < -moveY) {
				menuIndex = 0;
				moveY = 0;
			}
			menuIndex += moveY;
			moveY = 0;
		}
		menuEnd->e.offs += moveY;
		moveY = 0;
	} else if (menuPosY + moveY > MENU_Y_RANGE - 1) {
		moveY -= MENU_Y_RANGE - 1 - menuPosY;
		menuPosY = MENU_Y_RANGE - 1;
		if (menuIndex + moveY + MENU_Y_RANGE > menuTotal) {
			moveY -= menuTotal - menuIndex - MENU_Y_RANGE;
			menuIndex = menuTotal - MENU_Y_RANGE;
			if (menuEnd->e.offs + moveY + MENU_Y_RANGE > numelem(Presets)) {
				menuEnd->e.offs = numelem(Presets) - moveY - MENU_Y_RANGE + 1;
				moveY = 0;
			}
			menuEnd->e.offs += moveY;
			moveY = 0;
		}
		menuIndex += moveY;
		moveY = 0;
	}
	menuPosY += moveY;
	menuInitPresets();

	/*	Handle X moves  */

	menuPosX = 0;
	//	Move left: go to previous menu, if present
	if (moveX < 0) {
		if (menuEnd->e.leave != NULL)
			menuEnd->e.leave();
		if (menuEnd->e.ptr != NULL && menuEnd->e.ptr != menuMenu) {
			//	Checks out, change over to it
			menuInit(menuEnd->e.ptr);
		}
	}

	//	Move right: select file and leave menu (ignore title item)
	if (moveX > 0 && (menuIndex > 0 || menuPosY > 0)) {
		PresetIndex = menuEnd->e.offs + menuIndex + menuPosY;
		if (PresetIndex > numelem(Presets))
			PresetIndex = numelem(Presets);
		if (menuEnd->e.ptr != NULL) {
			menuInit(menuEnd->e.ptr);
		}
	}

}

/**
 *	Saves the current dspParams into the current file index.
 */void menuSavePreset(void) {

	eeprom_write_block((void*)&dspParams, (void*)&Presets[PresetIndex - 1], sizeof(dspParams));

}

/**
 *	Loads dspParams from the current file index.
 */void menuLoadPreset(void) {

	eeprom_read_block((void*)&dspParams, (void*)&Presets[PresetIndex - 1], sizeof(dspParams));
	//memcpy((void*)&dspParams, (void*)&Presets[PresetIndex] + MAPPED_EEPROM_START, sizeof(dspParams));

}
