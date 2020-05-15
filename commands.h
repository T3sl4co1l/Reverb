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

enum cmd_gsn_e {
	CMD_GSN_INT8 = 1,
	CMD_GSN_INT16 = 2
};

extern const cmd_command_s commandList[];

#endif // COMMANDS_H_INCLUDED
