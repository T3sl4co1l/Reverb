#include "console.h"
#include "lcd.h"


/* * *  Resources  * * */

uint8_t cmdHelpFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdHelpSpec[]		PROGMEM = "?";
const uint8_t cmdHelpHelp[]		PROGMEM =  "                 List commands";
const uint8_t Str_Welc[];	//	note: imported from main
const uint8_t cmdStrHelp1[]		PROGMEM = "Commands:\r";

uint8_t cmdReadRamFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdReadRamSpec[]	PROGMEM = "R <addr> [byte]";
const uint8_t cmdReadRamHelp[]	PROGMEM = 				 "   SRAM <read>/[write] addr";
const uint8_t cmdStrReadRam1[]	PROGMEM = "RAM at: ";
const uint8_t cmdStrReadRam2[]	PROGMEM = "h = ";
const uint8_t cmdStrReadRam3[]	PROGMEM = "Write: (";
const uint8_t cmdStrReadRam4[]	PROGMEM = "h) <= ";

uint8_t cmdDumpRamFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdDumpRamSpec[]	PROGMEM = "DR <addr> [len]";
const uint8_t cmdDumpRamHelp[]	PROGMEM = 				 "   Dump SRAM range";

uint8_t cmdDumpPgmFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdDumpPgmSpec[]	PROGMEM = "DP <addr> [len]";
const uint8_t cmdDumpPgmHelp[]	PROGMEM = 				 "   Dump PROGMEM range";

uint8_t cmdCpuFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdCpuSpec[]		PROGMEM = "CPU";
const uint8_t cmdCpuHelp[]		PROGMEM = 	 "               Read CPU idle count";

uint8_t cmdTapFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdTapSpec[]		PROGMEM = "TAP [<n> <d> <g>]";
const uint8_t cmdTapHelp[]		PROGMEM = 				   " Tap n, delay & gain";
const uint8_t cmdStrTap1[]		PROGMEM = "Tap ";
const uint8_t cmdStrTap2[]		PROGMEM = " <= Delay: ";
const uint8_t cmdStrTap3[]		PROGMEM = ", Delay: ";
const uint8_t cmdStrTap4[]		PROGMEM = ", Gain: ";

uint8_t cmdFiltFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdFiltSpec[]		PROGMEM = "FILT [<a0> <a1> <a2> <b1> <b2>]";
const uint8_t cmdFiltHelp[]		PROGMEM = 								 " Filter coefs";
const uint8_t cmdStrFilt[]		PROGMEM = "Filter Coefficients: ";

uint8_t cmdFiltEnFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdFiltEnSpec[]	PROGMEM = "FE [0|1]";
const uint8_t cmdFiltEnHelp[]	PROGMEM = 		  "          Enable/disable filtering";
const uint8_t cmdStrFiltEn1[]	PROGMEM = "Filter enabled";
const uint8_t cmdStrFiltEn2[]	PROGMEM = "Filter disabled";

uint8_t cmdTapsFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdTapsSpec[]		PROGMEM = "TAPS [n]";
const uint8_t cmdTapsHelp[]		PROGMEM = 		  "          Get[set] tap count";
const uint8_t cmdStrTaps1[]		PROGMEM = "Taps: ";
const uint8_t cmdStrTaps2[]		PROGMEM = "Taps <= ";

uint8_t cmdFileFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdFileSpec[]		PROGMEM = "FILE [n]";
const uint8_t cmdFileHelp[]		PROGMEM = 		  "          Get[set] file number";
const uint8_t cmdStrFile1[]		PROGMEM = "File: ";
const uint8_t cmdStrFile2[]		PROGMEM = "File <= ";

uint8_t cmdNameFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdNameSpec[]		PROGMEM = "NAME [s]";
const uint8_t cmdNameHelp[]		PROGMEM = 		  "          Get[set] Name (0-8 char)";
const uint8_t cmdStrName1[]		PROGMEM = "Name: ";
const uint8_t cmdStrName2[]		PROGMEM = "Name <= ";

uint8_t cmdLoadFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdLoadSpec[]		PROGMEM = "LOAD";
const uint8_t cmdLoadHelp[]		PROGMEM = 	  "              Load file from EEPROM";
const uint8_t cmdStrLoad[]		PROGMEM = "Loaded file: ";

uint8_t cmdSaveFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdSaveSpec[]		PROGMEM = "SAVE";
const uint8_t cmdSaveHelp[]		PROGMEM = 	  "              Save file to EEPROM";
const uint8_t cmdStrSave[]		PROGMEM = "Saved file: ";

const cmd_command_s commandList[] PROGMEM = {
	{
		.name = "?",
		.cmdSpec = cmdHelpSpec,
		.cmdHelp = cmdHelpHelp,
		.cmd = cmdHelpFunc
	},
	{
		.name = "R",
		.cmdSpec = cmdReadRamSpec,
		.cmdHelp = cmdReadRamHelp,
		.cmd = cmdReadRamFunc
	},
	{
		.name = "DR",
		.cmdSpec = cmdDumpRamSpec,
		.cmdHelp = cmdDumpRamHelp,
		.cmd = cmdDumpRamFunc
	},
	{
		.name = "DP",
		.cmdSpec = cmdDumpPgmSpec,
		.cmdHelp = cmdDumpPgmHelp,
		.cmd = cmdDumpPgmFunc
	},
	{
		.name = "CPU",
		.cmdSpec = cmdCpuSpec,
		.cmdHelp = cmdCpuHelp,
		.cmd = cmdCpuFunc
	},
	{
		.name = "TAP",
		.cmdSpec = cmdTapSpec,
		.cmdHelp = cmdTapHelp,
		.cmd = cmdTapFunc
	},
	{
		.name = "FILT",
		.cmdSpec = cmdFiltSpec,
		.cmdHelp = cmdFiltHelp,
		.cmd = cmdFiltFunc
	},
	{
		.name = "FE",
		.cmdSpec = cmdFiltEnSpec,
		.cmdHelp = cmdFiltEnHelp,
		.cmd = cmdFiltEnFunc
	},
	{
		.name = "TAPS",
		.cmdSpec = cmdTapsSpec,
		.cmdHelp = cmdTapsHelp,
		.cmd = cmdTapsFunc
	},
	{
		.name = "FILE",
		.cmdSpec = cmdFileSpec,
		.cmdHelp = cmdFileHelp,
		.cmd = cmdFileFunc
	},
	{
		.name = "NAME",
		.cmdSpec = cmdNameSpec,
		.cmdHelp = cmdNameHelp,
		.cmd = cmdNameFunc
	},
	{
		.name = "LOAD",
		.cmdSpec = cmdLoadSpec,
		.cmdHelp = cmdLoadHelp,
		.cmd = cmdLoadFunc
	},
	{
		.name = "SAVE",
		.cmdSpec = cmdSaveSpec,
		.cmdHelp = cmdSaveHelp,
		.cmd = cmdSaveFunc
	},

	//	Add more above

	{	//	Last entry zeroed out as sentinel
	}
};


/* * *  Functions  * * */

uint8_t cmdHelpFunc(uint8_t len, uint8_t* buf) {

	uint8_t i;

	//	Print welcome message and list stored commands
	serPutStringPgm(Str_Welc);
	serPutStringPgm(cmdStrHelp1);
	serWaitTransmit();

	for (i = 0; pgm_read_byte(&commandList[i].name[0]); i++) {
		serPutStringPgm((uint8_t*)pgm_read_word(&commandList[i].cmdSpec));
		serPutStringPgm((uint8_t*)pgm_read_word(&commandList[i].cmdHelp));
		serPutByte('\r');
		serWaitTransmit();
	}

	return 0;
}

uint8_t cmdReadRamFunc(uint8_t len, uint8_t* buf) {

	uint8_t val;
	token_t t;
	uint16_t addr;

	//	Point to first parameter
	t = strNextToken(0, len, buf);
	if (t.len == 0) {
		//	Didn't say where to read, print help message
		serPutStringPgmL(STRING_AND_LENGTH(cmdReadRamSpec));
		return 0;
	}

	addr = hexToNum(t.len, buf + t.str);
	t = strNextToken(t.str + t.len, len, buf);
	if (t.len == 0) {
		//	Read from RAM
		serPutStringPgm(cmdStrReadRam1);	//	"RAM at: "
		serPutNumHex(addr);
		serPutStringPgm(cmdStrReadRam2);	//	"h = "
		val = *(uint8_t*)addr;
	} else {
		//	Write to RAM
		serPutStringPgm(cmdStrReadRam3);	//	"Write: ("
		serPutNumHex(addr);
		val = hexToNum(t.len, buf + t.str) & 0xff;
		serPutStringPgm(cmdStrReadRam4);	//	"h) <= "
		*(uint8_t*)addr = val;
	}
	serPutNumHex(val);
	serPutByte('h');

	return val;
}

enum cmd_dump_ram_e {
	CMD_DUMP_RAM_SRAM = 0,
	CMD_DUMP_RAM_PGM = 1
};

uint8_t cmdDumpMem(uint8_t len, uint8_t* buf, uint8_t memtype) {

	uint8_t i, j;
	token_t t;
	uint16_t addr, dumpLen;
	uint8_t asciiBuf[18];

	//	Point to first parameter
	t = strNextToken(0, len, buf);

	if (t.len == 0) {
		//	Didn't say where to read, print help message
		if (memtype == CMD_DUMP_RAM_SRAM) {
			serPutStringPgmL(STRING_AND_LENGTH(cmdDumpRamSpec));
		} else {//if (memtype == CMD_DUMP_RAM_PGM) {
			serPutStringPgmL(STRING_AND_LENGTH(cmdDumpPgmSpec));
		}
		return 1;
	}
	addr = hexToNum(t.len, buf + t.str);
	t = strNextToken(t.str + t.len, len, buf);

	//	Default length, or get specified length if present
	dumpLen = 0x80;
	if (t.len) {
		dumpLen = hexToNum(t.len, buf + t.str);
	}

	for (i = 0; i < numelem(asciiBuf); i++) {
		asciiBuf[i] = ' ';
	}
	//	Put white space for first partial row, if applicable
    serPutNumHexFixed_w(addr); serPutByte(' ');
	j = addr & 0x0f;
    for (i = j * 3 + (j >= 8); i; i--) {
		serPutByte(' ');
    }
	while (1) {
		j = addr & 0x0f;
		if (memtype == CMD_DUMP_RAM_SRAM) {
			i = *(uint8_t*)addr++;
		} else {//if (memtype == CMD_DUMP_RAM_PGM) {
			i = pgm_read_byte((uint16_t*)addr++);
		}
		serPutNumHexFixed_b(i);
		if (i < 32 || i >= 127) {
			i = '.';
		}
		asciiBuf[j + 2] = i;
		if (j == 7) {
			serPutByte(' ');
			serWaitTransmit();
		}
		if (dumpLen <= 1) {
			//	put padding spaces, and last buffer
			j = 15 - j;
			for (i = j * 3 + (j > 8); i; i--) {
				serPutByte(' ');
			}
			serPutString(numelem(asciiBuf), asciiBuf);
			break;
		}
		if (j == 15) {
			//	End of line, put ascii version and return
			serPutString(numelem(asciiBuf), asciiBuf);
			for (i = numelem(asciiBuf); i > 2; i--) {
				asciiBuf[i] = ' ';
			}
			serWaitTransmit();
			serPutByte('\r');
			serPutNumHexFixed_w(addr);
		}
		serPutByte(' ');
		dumpLen--;
	}

	return 0;
}

uint8_t cmdDumpRamFunc(uint8_t len, uint8_t* buf) {
	return cmdDumpMem(len, buf, CMD_DUMP_RAM_SRAM);
}

uint8_t cmdDumpPgmFunc(uint8_t len, uint8_t* buf) {
	return cmdDumpMem(len, buf, CMD_DUMP_RAM_PGM);
}

//	Shared from main.c
uint16_t cpuCycle;

uint8_t cmdCpuFunc(uint8_t len, uint8_t* buf) {

	serPutNumHex(cpuCycle);

	return 0;
}

//	Shared from dsp.c
#include "dsp.h"
volatile dsp_params_t dspParams;

//	Shared from menu.c
void menuSavePreset(void);
void menuLoadPreset(void);
void menuDraw(void);

uint8_t PresetIndex;
dsp_params_t Presets[16];

uint8_t cmdTapFunc(uint8_t len, uint8_t* buf) {

	token_t t;
	uint8_t tokens;
	uint8_t tap;
	int8_t gain;
	uint16_t dly;

	tokens = strCountTokens(0, len, buf);
	if (tokens == 0) {
		//	Didn't say what tap to read, print help message
		serPutStringPgm(cmdTapSpec);
		return 0;
	}
	//	Get the tap number
	t = strNextToken(0, len, buf);
	tap = hexToNum(t.len, buf + t.str);
	if (tap > numelem(dspParams.dlyList))
		tap = numelem(dspParams.dlyList);
	serPutStringPgm(cmdStrTap1);
	serPutNumHex(tap);

	if (tokens < 3) {
		//	Read out the tap parameters
		serPutStringPgm(cmdStrTap3);
		serPutNumHex(dspParams.dlyList[tap]);
		serPutStringPgm(cmdStrTap4);
		gain = dspParams.gainList[tap];
		serPutNumHex(gain);
		return tap;
	}

	//	Parse the tap parameters
	serPutStringPgm(cmdStrTap2);
	t = strNextToken(t.str + t.len, len, buf);
	dly = hexToNum(t.len, buf + t.str);
	dspParams.dlyList[tap] = dly;
	serPutNumHex(dly);
	serPutStringPgm(cmdStrTap4);
	t = strNextToken(t.str + t.len, len, buf);
	gain = hexToNum(t.len, buf + t.str);
	dspParams.gainList[tap] = gain;
	serPutNumHex(gain);

	menuDraw();
	return tap;
}

uint8_t cmdFiltFunc(uint8_t len, uint8_t* buf) {

	token_t t;
	uint8_t tokens;
	uint8_t i;
	int16_t v;

	tokens = strCountTokens(0, len, buf);
	serPutStringPgm(cmdStrFilt);
	t = strNextToken(0, len, buf);
	for (i = 0; ; ) {
		if (tokens < numelem(dspParams.filt)) {
			//	Read out the tap parameters
			v = dspParams.filt[i];
		} else {
			v = hexToNum(t.len, buf + t.str);
			dspParams.filt[i] = v;
		}
		serPutNumHex(v);
		i++;
		if (i >= numelem(dspParams.filt))
			break;
		serPutByte(','); serPutByte(' ');
		t = strNextToken(t.str + t.len, len, buf);
	}

	menuDraw();
	return tokens;
}

uint8_t cmdFiltEnFunc(uint8_t len, uint8_t* buf) {

	token_t t;
	bool r;

	//	Point to first parameter
	t = strNextToken(0, len, buf);
	if (t.len == 0) {
		//	Get value
		r = dspParams.filtEn;
	} else {
		//	Set value
		r = !!hexToNum(t.len, buf + t.str);
	}
	dspParams.filtEn = r;
	if (r) {
		serPutStringPgm(cmdStrFiltEn1);
	} else {
		serPutStringPgm(cmdStrFiltEn2);
	}

	menuDraw();
	return r;
}

uint8_t cmdTapsFunc(uint8_t len, uint8_t* buf) {

	token_t t;

	//	Point to first parameter
	t = strNextToken(0, len, buf);
	if (t.len == 0) {
		//	Get value
		serPutStringPgm(cmdStrTaps1);
	} else {
		//	Set value
		serPutStringPgm(cmdStrTaps2);
		dspParams.taps = hexToNum(t.len, buf + t.str);
		if (dspParams.taps > numelem(Presets))
			dspParams.taps = numelem(Presets);
	}
	serPutNumHex(dspParams.taps);

	menuDraw();
	return dspParams.taps;
}

uint8_t cmdFileFunc(uint8_t len, uint8_t* buf) {

	token_t t;

	//	Point to first parameter
	t = strNextToken(0, len, buf);
	if (t.len == 0) {
		//	Get value
		serPutStringPgm(cmdStrFile1);
	} else {
		//	Set value
		serPutStringPgm(cmdStrFile2);
		PresetIndex = hexToNum(t.len, buf + t.str);
		if (PresetIndex > numelem(Presets))
			PresetIndex = numelem(Presets);
		if (PresetIndex < 1)
			PresetIndex = 1;
	}
	serPutNumHex(PresetIndex);

	menuDraw();
	return PresetIndex;
}

uint8_t cmdNameFunc(uint8_t len, uint8_t* buf) {

	if (len <= 1) {
		//	Get value
		serPutStringPgm(cmdStrFile1);
	} else {
		//	Set value
		serPutStringPgm(cmdStrFile2);
		len--;
		if (len > MENU_TITLE_LEN)
			len = MENU_TITLE_LEN;
		strncpy((char*)&dspParams.name, (char*)buf + 1, len);
		for (; len < MENU_TITLE_LEN; len++) {
			dspParams.name[len] = ' ';
		}
	}
	serPutString(MENU_TITLE_LEN, (uint8_t*)&dspParams.name[0]);

	menuDraw();
	return len;
}

uint8_t cmdLoadFunc(uint8_t len, uint8_t* buf) {

	menuLoadPreset();
	serPutStringPgm(cmdStrLoad);
	serPutNumHex(PresetIndex);

	menuDraw();
	return PresetIndex;
}

uint8_t cmdSaveFunc(uint8_t len, uint8_t* buf) {

	menuSavePreset();
	serPutStringPgm(cmdStrSave);
	serPutNumHex(PresetIndex);

	menuDraw();
	return PresetIndex;
}
