#ifndef ANSI_H_INCLUDED
#define ANSI_H_INCLUDED

#define ANSI_ESC_STR		"\x1b["
#define ANSI_ESC_LEN		2
#define ANSI_ESC1			0x1b
#define ANSI_ESC2			'['

//	Clears from cursor to end of line
#define ANSI_CLEAR_CODE		'K'
#define ANSI_CLEAR_STR		"\x1b[K"
#define ANSI_CLEAR_LEN		3
//	Erases screen
#define ANSI_ERASE_CODE		'J'
#define ANSI_ERASE_STR		"\x1b[2J"
#define ANSI_ERASE_LEN		4
//	Cursor movement
#define ANSI_SAVE_CODE		's'
#define ANSI_SAVE_STR		"\x1b[s"
#define ANSI_SAVE_LEN		3
#define ANSI_RESTORE_CODE	'u'
#define ANSI_RESTORE_STR	"\x1b[u"
#define ANSI_RESTORE_LEN	3
#define ANSI_MOVE_U_CODE	'A'
#define ANSI_MOVE_U_STR		"\x1b[A"
#define ANSI_MOVE_U_LEN		3
#define ANSI_MOVE_D_CODE	'B'
#define ANSI_MOVE_D_STR		"\x1b[B"
#define ANSI_MOVE_D_LEN		3
#define ANSI_MOVE_L_CODE	'D'
#define ANSI_MOVE_L_STR		"\x1b[D"
#define ANSI_MOVE_L_LEN		3
#define ANSI_MOVE_R_CODE	'C'
#define ANSI_MOVE_R_STR		"\x1b[C"
#define ANSI_MOVE_R_LEN		3
#define ANSI_MOVE_HOME_STR	"\x1b[80D"
#define ANSI_MOVE_HOME_LEN	5

#endif // ANSI_H_INCLUDED
