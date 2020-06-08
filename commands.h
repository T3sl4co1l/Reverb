#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "xmega64d3_corrections.h"
#include <avr/pgmspace.h>
#include "ansi.h"
#include "bufserial.h"

typedef uint8_t (*cmd_call_t )(uint8_t len, uint8_t* buf);

typedef struct cmd_command_t {
	uint8_t name[4];
	const uint8_t* cmdSpec;
	const uint8_t* cmdHelp;
	cmd_call_t cmd;
} cmd_command_s;

/**
 *	Flags in cmd_getset_t.params
 */
enum cmd_getset_idx_e {
	CMD_GETSET_MASK_PARAM	= 0x3f,		//	mask to exclude flags
	CMD_GETSET_SIZE_PARAM	= 0x40,		//	indicates parameters are uint16_t size (uint8_t default)
	CMD_GETSET_IDX_PARAM	= 0x80		//	indicates first parameter is an index
};

typedef struct cmd_getset_s {
	uint8_t params;				//	Total or maximum number of parameters to be read
	uint8_t indexRange;			//	0 <= index <= indexRange, if index flag set (else ignored)
	uint8_t** paramSet;			//	Index flag set: array of pointers, indexed by [index];
								//	  each element points to a parameter array.
								//	Index flag clear: cast to uint8_t*, pointing to a parameter array.
								//	In either case, final target must be [params] (index flag clear)
								//	or [params - 1] (index flag set) elements long.
								//	Target is uint8_t (size flag clear) or uint16_t (size flag set).
	//	strings in PROGMEM:
	const uint8_t* specStr;		//	Command line spec
	const uint8_t* helpStr;		//	Additional help string
	const uint8_t* indexStr;	//	common data message (e.g. "Index:", index, [readStr], ...[values])
	const uint8_t* readStr;		//	reading data message (0 (index false) or >= 1 (index true) parameters given)
} cmd_getset_t;


extern const cmd_command_s commandList[];

#endif // COMMANDS_H_INCLUDED
