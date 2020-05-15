/*
 *	Serial Console
 *	Tim Williams, 2019-11-03
 *
 *	Put this in your main() loop:
 *
 *	//	Read serial input
 *	cmdGetInputLine();
 *
 */

#include "console.h"
#include "commands.h"

/* * *  Strings (in PROGMEM)  * * */

const uint8_t Str_Ansi_Er[]	PROGMEM = ANSI_ERASE_STR;
const uint8_t Str_Ansi_ML[]	PROGMEM = ANSI_MOVE_L_STR;
const uint8_t Str_Ansi_Cl[]	PROGMEM = ANSI_CLEAR_STR;
const uint8_t Str_Ansi_Sv[]	PROGMEM = ANSI_SAVE_STR;
const uint8_t Str_Ansi_Re[]	PROGMEM = ANSI_RESTORE_STR;
const uint8_t Str_Welc[]	PROGMEM = WELCOME_STRING;
const uint8_t Str_Caret[]	PROGMEM = "\r>";
const uint8_t Str_Cmd_Unk[]	PROGMEM = "\rUnknown command: ";


uint8_t cmdExitCode = 0;

/* * *  Functions  * * */

/**
 *	Parses the buffer for commands.
 *	@param len	length of buffer to parse
 *	@param buf	pointer to buffer
 */
void cmdParseBuffer(uint8_t len, uint8_t* buf) {

	token_t t;
	uint8_t i;

	//	Scan to command name
	serWaitTransmit();
	t = strNextToken(0, len, buf);
	if (t.len == 0) {
		goto cmdParseBufferEnd;
	}
	//	hack: make name ASCIIZ to use strcmp style functions
	buf[t.str + t.len] = 0;
	//	Capitalize the command for easy checking
	toUcase(t.len, buf + t.str);

	//	Check if command is in list
    for (i = 0; pgm_read_byte(&commandList[i].name[0]) || i == __UINT8_MAX__; i++) {
		//	Search for matching name
		if (!strncmp_P((char*)buf + t.str, (const char*)&commandList[i].name, 4)) {
			serPutByte('\r');
			buf[t.str + t.len] = ' ';	//	undo ASCIIZ hack for parsing
			cmdExitCode = ((cmd_call_t)pgm_read_word(&commandList[i].cmd))(
					len - t.str - t.len, buf + t.str + t.len);
			serWaitTransmit();
			goto cmdParseBufferEnd;
		}
	};

	serPutStringPgm(Str_Cmd_Unk);	//	"Unknown command: "
	serWaitTransmit();
	serPutString(t.len, buf + t.str);
cmdParseBufferEnd:
	serPutStringPgm(Str_Caret);
	serWaitTransmit();

}

/**
 *	Processes input from the serial port, into rxInputBuf.
 *	Returns true when a complete line has been entered.
 */
bool cmdGetInputLine(void) {

	/**  Input byte  */
	uint8_t rx;
	/**  Input string length (eventually passed to rxInputLen)  */
	static uint8_t len = 0;
	/**  Index to cursor  */
	static int8_t cur = 0;
	/**  ANSI escape sequence counter  */
	static int8_t ansiChars = 0;
	/**  Scratch buffer and length for storing ANSI escape sequences (less ESC, '[')  */
	static uint8_t ansiBuf[8];
	static uint8_t ansiBufLen = 0;
	/** Command line buffer */
	static uint8_t cmdInputBuf[CMD_BUF_SIZE - 1];
	static uint8_t cmdInputLen = 0;

	while (!serRxBufferEmpty()) {
		rx = serGetByte();

		if (ansiChars < 2) {
			switch (rx) {
			case ANSI_ESC1:		//	ESC, may be beginning of ANSI escape sequence
			case ANSI_ESC2:		//	escape code found, switch modes
				if (ansiChars <= 1) ansiChars++;
				break;
			case '\b':
				cur--;
				if (cur >= 0) serPutStringPgm(Str_Ansi_ML);
			case 0x7f:
				if (cur < 0) {
					cur = 0;
					serPutByte('\a');	//	Ding, can't backspace before start
				} else if (cur >= len) {
					cur = len;
					serPutByte('\a');	//	Ding, can't delete past end
				} else {
					bufDeleteByte(cmdInputBuf, cur, len--);
					cmdOverwriteLine(len, cur, cmdInputBuf);
				}
				break;
			case '\n':	//	Enter: parse the string
			case '\r':
				ansiChars = 0; ansiBufLen = 0;
				cur = 0; cmdInputLen = len; len = 0;
				cmdParseBuffer(cmdInputLen, cmdInputBuf);
				return true;
			default:
				if (rx >= ' ' && len < numelem(cmdInputBuf)) {	//	Insert at cursor position (ignore other ASCII control codes)
					bufInsertByte(cmdInputBuf, cur++, rx, ++len);
					//	Refresh terminal state
					serPutByte(rx);
					if (cur < len) {
						cmdOverwriteLine(len, cur, cmdInputBuf);
					}
				} else {
					//	Buffer full or invalid character, refuse additional input
					serPutByte('\a');
				}
			}
		} else {
			ansiBuf[ansiBufLen++] = rx;
			if (ansiBufLen >= sizeof(ansiBuf)) {
				//	Not much we can do at this point but let it
				//	overflow (keep overwriting the last byte)
				ansiBufLen = sizeof(ansiBuf) - 1;
			}
			//	Wait for a letter to terminate the sequence
			if (isLetter(rx)) {
				//	Parse it (see: http://ascii-table.com/ansi-escape-sequences.php )

				switch (rx) {
				case ANSI_MOVE_U_CODE:		//	Cursor up (treat as HOME)
					//	NOTE: ansiChars re-purposed for cursor count in this clause
					//	(ansiBuf also used for response)
					ansiChars = -cur; cur = 0;
					goto cmdGetInputOutDir;
				case ANSI_MOVE_R_CODE:		//	Cursor right
						ansiChars = 1; cur++;
					goto cmdGetInputOutDir;
				case ANSI_MOVE_D_CODE:		//	Cursor down (treat as END)
					ansiChars = len - cur; cur = len;
					goto cmdGetInputOutDir;
				case ANSI_MOVE_L_CODE:		//	Cursor left
					ansiChars = -1; cur--;
cmdGetInputOutDir:
					if (cur > len) {
						cur = len;
						serPutByte('\a');
					} else if (cur < 0) {
						cur = 0;
						serPutByte('\a');
					} else {
						ansiMoveVar(ansiChars, ansiBuf);
						serPutString(BUFFER_AND_LENGTH(ansiBuf));
					}
					break;
				case 'K':	//	END, clears all characters from cursor position to end of the line
					len = cur;
					serPutStringPgm(Str_Ansi_Cl);
					break;
				}
				ansiChars = 0; ansiBufLen = 0;
			}

		}

	}
	return false;
}

/**
 *	Sends ANSI escape codes to overwrite the terminal with the
 *	given string, at the current cursor position, and preserving
 *	cursor position.
 *	@param len	length of string buffer (total)
 *	@param cur	cursor position, point in buffer to start overwriting from
 *	@param buf	pointer to buffer
 */
void cmdOverwriteLine(uint8_t len, int8_t cur, const uint8_t* buf) {
	serPutStringPgm(Str_Ansi_Sv);
	serPutStringPgm(Str_Ansi_Cl);
	serPutString(len - cur, buf + cur);
	serPutStringPgm(Str_Ansi_Re);
}

/**
 *	Writes an ANSI move-left/right escape sequence to buffer buf.
 *	buf receives: <ESC>[<n><code><null>
 *	Buffer must be >= 7 bytes long.
 *	Returns null string for n == 0.
 *	@param n	number of positions to move right by (negative = left).
 */
void ansiMoveVar(int8_t n, uint8_t* buf) {

	uint8_t l;
	uint8_t code = ANSI_MOVE_R_CODE;

	if (n == 0) {
		buf[0] = 0;
		return;
	}
	if (n < 0) {
		code = ANSI_MOVE_L_CODE;
		n = -n;
	}
	buf[0] = ANSI_ESC1; buf[1] = ANSI_ESC2;
	itoa(n, (char*)buf + 2, 10);
	l = strlen((char*)buf);
	buf[l++] = code;
	buf[l] = 0;

}

/**
 *	Inserts a byte b, in buffer buf, at cursor position cur.
 *	All bytes are pushed up, from the cursor to the length
 *	of the buffer, len.
 */
void bufInsertByte(uint8_t* buf, uint8_t cur, uint8_t b, uint8_t len) {

	if (cur >= len || len == 0) {
		return;
	}
	if (len == 1) {
		buf[0] = b;
		return;
	}
	for (len--; len > cur; len--) {
		buf[len] = buf[len - 1];
	}
	buf[cur] = b;
}

/**
 *	Deletes a byte from buffer buf, at cursor position cur.
 *	All bytes are shoved down, from the cursor to the length
 *	of the buffer, len.
 *	@return byte deleted.
 *	Also returns zero if operation is impossible (cur >= len, len = 0).
 */
uint8_t bufDeleteByte(uint8_t* buf, uint8_t cur, uint8_t len) {

	uint8_t b = 0;

	if (cur >= len || len == 0) {
		return 0;
	}
	b = buf[cur];
	if (len > 1) {
		for (len--; cur < len; cur++) {
			buf[cur] = buf[cur + 1];
		}
	}
	return b;
}

/**
 *	Converts the passed buffer to a number, attempting to parse it
 *	as ASCII-coded hexadecimal.  Starts reading from st, until
 *	a maximum of min(len, 4) bytes are read, or a non-hex character
 *	is found.  (Returns zero if the first character read is non-hex.)
 *	@param	len	length of string (st + len = one past end)
 *	@param	buf	pointer to string buffer
 *	@return	converted number
 */
uint16_t hexToNum(uint8_t len, const uint8_t* buf) {

	uint16_t r = 0;
	int8_t b;

	if (len > 4) {
		len = 4;
	}
	for (; len > 0; len--) {
		b = asciiToNibble(*buf++);
		if (b < 0) {
			break;
		}
		r <<= 4;
		r += b;
	}
	return r;
}

/**
 *	Converts the byte to a number as ASCII coded hexadecimal.
 *	Returns -1 if invalid.
 */
int8_t asciiToNibble(uint8_t c) {

	int8_t r = -1;

	if (c >= '0' && c <= '9') {
		r = c - '0';
	} else if (c >= 'A' && c <= 'F') {
		r = c - 'A' + 10;
	} else if (c >= 'a' && c <= 'f') {
		r = c - 'a' + 10;
	}
	return r;
}

/**
 *	Converts the low 4 bits of c to ASCII coded hexadecimal.
 */
uint8_t nibbleToAscii(uint8_t c) {

	c &= 0x0f;
	if (c >= 10) {
		c += 'A' - 10;
	} else {
		c += '0';
	}
	return c;
}

/**
 *	Sets the string to all uppercase (i.e., a-z --> A-Z,
 *	all other values unaffected).
 *	@param	len	Length of the string (one past last valid index)
 *	@param	str	String to convert (modified in place).
 */
void toUcase(uint8_t len, uint8_t* str) {
	uint8_t pos;

	for (pos = 0; pos < len; pos++) {
		if ((str[pos] >= 'a') && (str[pos] <= 'z')) {
			str[pos] = str[pos] + 'A' - 'a';
		}
	}
}

/**
 *	Checks if c is a letter (A-Z, a-z).
 */
bool isLetter(uint8_t c) {
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		return true;
	}
	return false;
}

/**
 *	Checks if c is a whitespace character.
 */
bool isSpace(uint8_t c) {
	if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r') {
		return true;
	}
	return false;
}

/**
 *	Locates the offset of the next token in string buf,
 *	starting from st, up to a maximum of len.
 *	@param	st	starting point to scan forward from
 *	@param	len	maximum length of buffer
 *	@param	buf	pointer to string buffer
 *	@return	start of token (len if no tokens by end of string).
 */
uint8_t strNextTokenStart(uint8_t st, const uint8_t len, const uint8_t* buf) {

	for (; st < len; st++) {
		if (!isSpace(buf[st])) {
			break;
		}
	}
	return st;
}

/**
 *	Locates the end of the token in string buf,
 *	starting from st, up to a maximum of len.
 *	@param	st	starting point to scan forward from
 *	@param	len	maximum length of buffer
 *	@param	buf	pointer to string buffer
 *	@return	length of token (0 if buf[st] is whitespace).
 */
uint8_t strNextTokenLen(const uint8_t st, const uint8_t len, const uint8_t* buf) {

	uint8_t r = st;

	if (st < len) {
		for (r++; r < len; r++) {
			if (isSpace(buf[r])) {
				break;
			}
		}
	}
	return r - st;
}

//	Almost no difference in size here, might as well go for speed
#pragma GCC push_options
#pragma GCC optimize ("-O3")
/**
 *	Locates the start and end of the next token in string buf,
 *	starting from st, up to a maximum of len.
 *	@param	st	starting point to scan forward from
 *	@param	len	maximum length of buffer
 *	@param	buf	pointer to string buffer
 *	@return	start and length of token.  Length is 0 if buf[st]
 *			is all whitespace.
 */
token_t strNextToken(const uint8_t st, const uint8_t len, const uint8_t* buf) {

	token_t r;
	r.str = strNextTokenStart(st, len, buf);
	r.len = strNextTokenLen(r.str, len, buf);
	return r;

	for (r.str = st; r.str < len; r.str++) {
		if (!isSpace(buf[r.str])) {
			break;
		}
	}

	if (r.str < len) {
		r.len = r.str;
		for (r.len++; r.len < len; r.len++) {
			if (isSpace(buf[r.len])) {
				break;
			}
		}
	}
	r.len -= r.str;
	return r;
}
#pragma GCC pop_options

/**
 *	Counts the number of tokens in string buf, from st to len.
 *	@param	st	starting point to scan forward from
 *	@param	len	maximum length of buffer
 *	@param	buf	pointer to string buffer
 *	@return	number of tokens in string (0 if all whitespace).
 */
uint8_t strCountTokens(const uint8_t st, const uint8_t len, const uint8_t* buf) {

	uint8_t count = 0;
	token_t t = {st, 0};

	while (true) {
		t = strNextToken(t.str + t.len, len, buf);
		if (t.len == 0 || t.str > len) {
			break;
		}
		count++;
	};
	return count;
}
