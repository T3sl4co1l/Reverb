#include "console.h"
#include "lcd.h"


/* * *  Resources  * * */

uint8_t cmdHelpFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdHelpSpec[]		PROGMEM = "?";
const uint8_t cmdHelpHelp[]		PROGMEM =  "                List commands";
const uint8_t Str_Welc[];	//	note: imported from main
const uint8_t cmdStrHelp1[]		PROGMEM = "Commands:\r";

uint8_t cmdReadRamFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdReadRamSpec[]	PROGMEM = "R <addr> [byte]";
const uint8_t cmdReadRamHelp[]	PROGMEM =				 "  SRAM <read>/[write] addr";
const uint8_t cmdStrReadRam1[]	PROGMEM = "RAM at: ";
const uint8_t cmdStrReadRam2[]	PROGMEM = "h: ";
const uint8_t cmdStrReadRam3[]	PROGMEM = "Write: (";
const uint8_t cmdStrReadRam4[]	PROGMEM = "h) <= ";

uint8_t cmdDumpRamFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdDumpRamSpec[]	PROGMEM = "DR <addr> [len]";
const uint8_t cmdDumpRamHelp[]	PROGMEM =				 "  Dump SRAM range";

uint8_t cmdDumpPgmFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdDumpPgmSpec[]	PROGMEM = "DP <addr> [len]";
const uint8_t cmdDumpPgmHelp[]	PROGMEM =				 "  Dump PROGMEM range";

uint8_t cmdCpuFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdCpuSpec[]		PROGMEM = "CPU";
const uint8_t cmdCpuHelp[]		PROGMEM =	 "              Read CPU idle count";

const uint8_t cmdStrGetSetAssign[]	PROGMEM = " <= ";
const uint8_t cmdStrGetSetRead[]	PROGMEM = ": ";
const uint8_t cmdStrGetSetComma[]	PROGMEM = ", ";

uint8_t cmdTapDlyFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdTapDlySpec[]	PROGMEM = "DLY [<n> [<d>]]";
const uint8_t cmdTapDlyHelp[]	PROGMEM =				 "  Tap n delay";
const uint8_t cmdTapDlyHelp2[]	PROGMEM = "n = 0..5, delay = 0..FFFF";
const uint8_t cmdStrTapDly1[]	PROGMEM = "Tap";
const uint8_t cmdStrTapDly2[]	PROGMEM = " delay";

uint8_t cmdTapGainFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdTapGainSpec[]	PROGMEM = "GAIN [<n> [<g>]]";
const uint8_t cmdTapGainHelp[]	PROGMEM =				  " Tap n gain";
const uint8_t cmdTapGainHelp2[]	PROGMEM = "n = 0..5, gain = 00..FF";
#define		  cmdStrTapGain1	cmdStrTapDly1
const uint8_t cmdStrTapGain2[]	PROGMEM = " gain";

uint8_t cmdMixFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdMixSpec[]		PROGMEM = "MIX <n> [g]";
const uint8_t cmdMixHelp[]		PROGMEM =			 "      Mixer channel gain";
const uint8_t cmdMixHelp2[]		PROGMEM = "n = 0..1, gain = 00..FF";
const uint8_t cmdStrMix1[]		PROGMEM = "Channel";
#define		  cmdStrMix2		cmdStrTapGain2

uint8_t cmdFiltFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdFiltSpec[]		PROGMEM = "FILT [n [a0 a1 a2 b1 b2]]";
const uint8_t cmdFiltHelp[]		PROGMEM =						   " Filter coefs";
const uint8_t cmdFiltHelp2[]	PROGMEM = "n = filter bank 0..1, a0..b2 in 2.14 fixed point";
const uint8_t cmdStrFilt1[]		PROGMEM = "Filter";
const uint8_t cmdStrFilt2[]		PROGMEM = " coefficients";

uint8_t cmdFiltEnFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdFiltEnSpec[]	PROGMEM = "FE [0-2]";
const uint8_t cmdFiltEnHelp[]	PROGMEM =		  "         Get[set] filter stages";
const uint8_t cmdStrFiltEn[]	PROGMEM = "Filter stages";

uint8_t cmdTapsFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdTapsSpec[]		PROGMEM = "TAPS [0-6]";
const uint8_t cmdTapsHelp[]		PROGMEM =			"       Get[set] tap count";
const uint8_t cmdStrTaps[]		PROGMEM = "Taps";

uint8_t cmdFileFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdFileSpec[]		PROGMEM = "FILE [n]";
const uint8_t cmdFileHelp[]		PROGMEM =		  "         Get[set] file number";
const uint8_t cmdStrFile[]		PROGMEM = "File number";

uint8_t cmdNameFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdNameSpec[]		PROGMEM = "NAME [s]";
const uint8_t cmdNameHelp[]		PROGMEM =		  "         Get[set] Name (0-8 char)";
const uint8_t cmdStrName1[]		PROGMEM = "Name: ";
const uint8_t cmdStrName2[]		PROGMEM = "Name <= ";

uint8_t cmdLoadFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdLoadSpec[]		PROGMEM = "LOAD";
const uint8_t cmdLoadHelp[]		PROGMEM =	  "             Load file from EEPROM";
const uint8_t cmdStrLoad[]		PROGMEM = "Loaded file: ";

uint8_t cmdSaveFunc(uint8_t len, uint8_t* buf);
const uint8_t cmdSaveSpec[]		PROGMEM = "SAVE";
const uint8_t cmdSaveHelp[]		PROGMEM =	  "             Save file to EEPROM";
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
		.name = "DLY",
		.cmdSpec = cmdTapDlySpec,
		.cmdHelp = cmdTapDlyHelp,
		.cmd = cmdTapDlyFunc
	},
	{
		.name = "GAIN",
		.cmdSpec = cmdTapGainSpec,
		.cmdHelp = cmdTapGainHelp,
		.cmd = cmdTapGainFunc
	},
	{
		.name = "MIX",
		.cmdSpec = cmdMixSpec,
		.cmdHelp = cmdMixHelp,
		.cmd = cmdMixFunc
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
		serWaitTransmit();
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
			for (i = 2; i < numelem(asciiBuf) - 1; i++) {
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

//	Shared from dsp.c
#include "dsp.h"
volatile dsp_params_t dspParams;

//	Shared from menu.c
void menuSavePreset(void);
void menuLoadPreset(void);
void menuDraw(void);

uint8_t PresetIndex;
dsp_params_t Presets[16];

/**
 *	Universal getter-setter function for memory mapped variables.
 *	(Saves about 600 bytes over the original functions.)
 *	(Below, names refer to spec properties; "params" means
 *	(spec->params & CMD_GETSET_MASK_PARAM) unless noted otherwise.)
 *
 *	Size flag determines size of target(s) (flag true = uint16_t, false = uint8_t).
 *	Additional args (beyond params) are ignored.  (If params == 0, or index flag
 *	set and params == 1, the function is read-only.)
 *
 *	When index flag clear:
 *		paramSet is a direct pointer to the target.  No help string is printed.
 *		indexRange is ignored.
 *
 *		args < params: read from target.
 *		args >= params: write to target.  Only [params] elements are written.
 *		(If params == 0, no data is written; do not use this setting.)
 *
 *	When index flag set:
 *		paramSet is a pointer to an array of target pointers.  The first argument
 *		indexes this array, up to a maximum indexRange.  The pointer is fetched
 *		and target values are read/written from there.  The target is an array of
 *		[params] elements.
 *
 *		args == 0 || index > indexRange: prints help string.
 *		1 <= args < params: read from indexed target.
 *		args >= params: write to indexed target.  Only [params - 1] elements are
 *			written.  (If params <= 1, no data is written; do not use this
 *			setting.)
 *
 *	@param
 *		len		Length of command line string
 *		buf		Pointer to start of command line string
 *		spec	Pointer to specifier structure in PROGMEM
 */
uint8_t cmdGetSetFunc(uint8_t len, uint8_t* buf, const cmd_getset_t* spec) {

	token_t t;
	uint8_t tokens;
	uint8_t pars;
	bool size_16bit;
	uint8_t idxRange;
	uint8_t i;
	uint16_t val;
	uint8_t** parSet;
	uint8_t* target;

	pars = pgm_read_byte(&(spec->params));
	idxRange = pgm_read_byte(&(spec->indexRange));
	parSet = (uint8_t**)pgm_read_word((void*)&(spec->paramSet));
	target = (uint8_t*)parSet;	//	Assume parSet is direct target

	size_16bit = !!(pars & CMD_GETSET_SIZE_PARAM);

	tokens = strCountTokens(0, len, buf);
	t = strNextToken(0, len, buf);

	if (pars & CMD_GETSET_IDX_PARAM) {

		if (tokens == 0) {
getsetHelpEnd:
			//	Didn't give index, print help message
			serPutStringPgm((uint8_t*)pgm_read_word(&(spec->specStr)));
			serWaitTransmit();
			if (target != NULL) {
				serPutByte('\r');
				serPutStringPgm((uint8_t*)pgm_read_word(&(spec->helpStr)));
			}
			return 0;
		}
		tokens--;

		//	Get the index
		i = hexToNum(t.len, buf + t.str);
		t = strNextToken(t.str + t.len, len, buf);
		if (i > idxRange) {
			goto getsetHelpEnd;	//	also leave if index out of range
		}
		target = (uint8_t*)pgm_read_word(&(parSet[i]));
		serPutStringPgm((uint8_t*)pgm_read_word(&(spec->indexStr)));
		serPutByte(' ');
		serPutNumHex(i);

	}

	pars &= CMD_GETSET_MASK_PARAM;

	serPutStringPgm((uint8_t*)pgm_read_word(&(spec->readStr)));
	if (tokens >= pars) {	//	Sufficient arguments, write value(s)
		serPutStringPgm(cmdStrGetSetAssign);
	} else {	//	Insufficient arguments, read value(s)
		serPutStringPgm(cmdStrGetSetRead);
	}

	for (i = pars; i; ) {

		if (tokens >= pars) {	//	Parse argument and write to target
			val = hexToNum(t.len, buf + t.str);
			t = strNextToken(t.str + t.len, len, buf);
			if (size_16bit) {
				*(uint16_t*)target = val;
			} else {	//	8 bit size
				*target = val;
			}
		}
		if (size_16bit) {
			serPutNumHex(*(uint16_t*)target);
			target += 2;
		} else {	//	8 bit size
			serPutNumHex(*target);
			target += 1;
		}

		i--;
		if (!i)
			break;
		serPutStringPgm(cmdStrGetSetComma);
	}

	menuDraw();
	return tokens;
}

//	Shared from main.c
uint16_t cpuCycle;

uint8_t cmdCpuFunc(uint8_t len, uint8_t* buf) {

	serPutNumHex(cpuCycle);

	return 0;
}

uint8_t cmdTapDlyFunc(uint8_t len, uint8_t* buf) {

	static volatile uint16_t* const paramBlock[] PROGMEM = {
		&dspParams.dlyList[0],
		&dspParams.dlyList[1],
		&dspParams.dlyList[2],
		&dspParams.dlyList[3],
		&dspParams.dlyList[4],
		&dspParams.dlyList[5]
	};
	static cmd_getset_t const spec PROGMEM = {
		.params		= 1 | CMD_GETSET_IDX_PARAM | CMD_GETSET_SIZE_PARAM,
		.indexRange	= 5,
		.paramSet	= (uint8_t**)paramBlock,
		.helpStr	= cmdTapDlyHelp2,
		.indexStr	= cmdStrTapDly1,
		.specStr	= cmdTapDlySpec,
		.readStr	= cmdStrTapDly2
	};

	return cmdGetSetFunc(len, buf, &spec);
}

uint8_t cmdTapGainFunc(uint8_t len, uint8_t* buf) {

	static volatile uint8_t* const paramBlock[] PROGMEM = {
		&dspParams.gainList[0],
		&dspParams.gainList[1],
		&dspParams.gainList[2],
		&dspParams.gainList[3],
		&dspParams.gainList[4],
		&dspParams.gainList[5]
	};
	static cmd_getset_t const spec PROGMEM = {
		.params		= 1 | CMD_GETSET_IDX_PARAM,
		.indexRange	= 5,
		.paramSet	= (uint8_t**)paramBlock,
		.helpStr	= cmdTapGainHelp2,
		.indexStr	= cmdStrTapGain1,
		.specStr	= cmdTapGainSpec,
		.readStr	= cmdStrTapGain2
	};

	return cmdGetSetFunc(len, buf, &spec);
}

uint8_t cmdMixFunc(uint8_t len, uint8_t* buf) {

	static uint8_t* const paramBlock[] PROGMEM = {
		(uint8_t*)&dspParams.gainClean,
		(uint8_t*)&dspParams.gainReverb
	};
	static cmd_getset_t const spec PROGMEM = {
		.params		= 1 | CMD_GETSET_IDX_PARAM,
		.indexRange	= 1,
		.paramSet	= (uint8_t**)paramBlock,
		.helpStr	= cmdMixHelp2,
		.indexStr	= cmdStrMix1,
		.specStr	= cmdMixSpec,
		.readStr	= cmdStrMix2
	};

	return cmdGetSetFunc(len, buf, &spec);
}

uint8_t cmdFiltFunc(uint8_t len, uint8_t* buf) {

	static volatile uint8_t* const paramBlock[] PROGMEM = {
		(uint8_t*)&dspParams.filt[0],
		(uint8_t*)&dspParams.filt[5]
	};
	static cmd_getset_t const spec PROGMEM = {
		.params		= 5 | CMD_GETSET_IDX_PARAM | CMD_GETSET_SIZE_PARAM,
		.indexRange	= numelem(paramBlock) - 1,
		.paramSet	= (uint8_t**)paramBlock,
		.helpStr	= cmdFiltHelp2,
		.indexStr	= cmdStrFilt1,
		.specStr	= cmdFiltSpec,
		.readStr	= cmdStrFilt2
	};

	return cmdGetSetFunc(len, buf, &spec);
}

uint8_t cmdFiltEnFunc(uint8_t len, uint8_t* buf) {

	static cmd_getset_t const spec PROGMEM = {
		.params		= 1,
		.indexRange	= 0,
		.paramSet	= (uint8_t**)&(dspParams.filtEn),
		.helpStr	= NULL,
		.indexStr	= NULL,
		.specStr	= cmdFiltEnSpec,
		.readStr	= cmdStrFiltEn
	};

	return cmdGetSetFunc(len, buf, &spec);
}

uint8_t cmdTapsFunc(uint8_t len, uint8_t* buf) {

	static cmd_getset_t const spec PROGMEM = {
		.params		= 1,
		.indexRange	= 0,
		.paramSet	= (uint8_t**)&(dspParams.taps),
		.helpStr	= NULL,
		.indexStr	= NULL,
		.specStr	= cmdTapsSpec,
		.readStr	= cmdStrTaps
	};

	return cmdGetSetFunc(len, buf, &spec);
}

uint8_t cmdFileFunc(uint8_t len, uint8_t* buf) {

	static cmd_getset_t const spec PROGMEM = {
		.params		= 1,
		.indexRange	= 0,
		.paramSet	= (uint8_t**)&PresetIndex,
		.helpStr	= NULL,
		.indexStr	= NULL,
		.specStr	= cmdFileSpec,
		.readStr	= cmdStrFile
	};
	uint8_t r;

	r = cmdGetSetFunc(len, buf, &spec);
	if (PresetIndex > numelem(Presets))
		PresetIndex = numelem(Presets);
	if (PresetIndex < 1)
		PresetIndex = 1;

	return r;
}

uint8_t cmdNameFunc(uint8_t len, uint8_t* buf) {

	if (len <= 1) {
		//	Get value
		serPutStringPgm(cmdStrName1);
	} else {
		//	Set value
		serPutStringPgm(cmdStrName2);
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
