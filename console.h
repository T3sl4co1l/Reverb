#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include "xmega64d3_corrections.h"
#include <avr/pgmspace.h>
#include "ansi.h"
#include "bufserial.h"
#include "commands.h"

#ifndef numelem
#define numelem(x)			( sizeof(x) / sizeof(x[0]) )
#endif // numelem

#define CMD_BUF_SIZE				64
#define CMD_FILENAME_MAX_LENGTH		8

#define WELCOME_STRING		"Reverb Effect Box, by Tim Williams, " __DATE__ "\r\r"

typedef struct token_s {
	uint8_t str;
	uint8_t len;
} token_t;

uint8_t cmdExitCode;
const uint8_t Str_Welc[sizeof(WELCOME_STRING)];

void     cmdParseBuffer(uint8_t len, uint8_t* buf);
bool     cmdGetInputLine(void);
void     cmdOverwriteLine(uint8_t len, int8_t cur, const uint8_t* buf);
void     ansiMoveVar(int8_t n, uint8_t* buf);
void     bufInsertByte(uint8_t* buf, uint8_t cur, uint8_t b, uint8_t len);
uint8_t  bufDeleteByte(uint8_t* buf, uint8_t cur, uint8_t len);
uint16_t hexToNum(uint8_t len, const uint8_t* buf);
int8_t   asciiToNibble(uint8_t c);
uint8_t  nibbleToAscii(uint8_t c);
void     toUcase(uint8_t len, uint8_t* str);
bool     isLetter(uint8_t c);
bool     isSpace(uint8_t c);
token_t  strNextToken(const uint8_t st, const uint8_t len, const uint8_t* buf);
uint8_t  strCountTokens(const uint8_t st, const uint8_t len, const uint8_t* buf);

#endif	//	CONSOLE_H_INCLUDED
