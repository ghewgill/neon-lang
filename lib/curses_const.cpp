#include <curses.h>

#include "number.h"

namespace rtl {

namespace curses {

extern const Number CONSTANT_CURSES_ERR = number_from_sint32(ERR);
extern const Number CONSTANT_CURSES_OK = number_from_sint32(OK);

extern const Number CONSTANT_A_CHARTEXT = number_from_uint32(A_CHARTEXT);
extern const Number CONSTANT_A_NORMAL = number_from_uint32(A_NORMAL);
extern const Number CONSTANT_A_ALTCHARSET = number_from_uint32(A_ALTCHARSET);
extern const Number CONSTANT_A_INVIS = number_from_uint32(A_INVIS);
extern const Number CONSTANT_A_UNDERLINE = number_from_uint32(A_UNDERLINE);
extern const Number CONSTANT_A_REVERSE = number_from_uint32(A_REVERSE);
extern const Number CONSTANT_A_BLINK = number_from_uint32(A_BLINK);
extern const Number CONSTANT_A_BOLD = number_from_uint32(A_BOLD);
extern const Number CONSTANT_A_ATTRIBUTES = number_from_uint64(A_ATTRIBUTES);
extern const Number CONSTANT_A_COLOR = number_from_uint32(A_COLOR);
extern const Number CONSTANT_A_PROTECT = number_from_uint32(A_PROTECT);

extern const Number CONSTANT_COLOR_BLACK = number_from_uint32(COLOR_BLACK);
extern const Number CONSTANT_COLOR_RED = number_from_uint32(COLOR_RED);
extern const Number CONSTANT_COLOR_GREEN = number_from_uint32(COLOR_GREEN);
extern const Number CONSTANT_COLOR_BLUE = number_from_uint32(COLOR_BLUE);
extern const Number CONSTANT_COLOR_CYAN = number_from_uint32(COLOR_CYAN);
extern const Number CONSTANT_COLOR_MAGENTA = number_from_uint32(COLOR_MAGENTA);
extern const Number CONSTANT_COLOR_YELLOW = number_from_uint32(COLOR_YELLOW);
extern const Number CONSTANT_COLOR_WHITE = number_from_uint32(COLOR_WHITE);

extern const Number CONSTANT_KEY_CODE_YES = number_from_uint32(KEY_CODE_YES);

extern const Number CONSTANT_KEY_BREAK = number_from_uint32(KEY_BREAK);
extern const Number CONSTANT_KEY_DOWN = number_from_uint32(KEY_DOWN);
extern const Number CONSTANT_KEY_UP = number_from_uint32(KEY_UP);
extern const Number CONSTANT_KEY_LEFT = number_from_uint32(KEY_LEFT);
extern const Number CONSTANT_KEY_RIGHT = number_from_uint32(KEY_RIGHT);
extern const Number CONSTANT_KEY_HOME = number_from_uint32(KEY_HOME);
extern const Number CONSTANT_KEY_BACKSPACE = number_from_uint32(KEY_BACKSPACE);

extern const Number CONSTANT_KEY_F0 = number_from_uint32(KEY_F0);
extern const Number CONSTANT_KEY_DL = number_from_uint32(KEY_DL);
extern const Number CONSTANT_KEY_IL = number_from_uint32(KEY_IL);
extern const Number CONSTANT_KEY_DC = number_from_uint32(KEY_DC);
extern const Number CONSTANT_KEY_IC = number_from_uint32(KEY_IC);
extern const Number CONSTANT_KEY_EIC = number_from_uint32(KEY_EIC);
extern const Number CONSTANT_KEY_CLEAR = number_from_uint32(KEY_CLEAR);
extern const Number CONSTANT_KEY_EOS = number_from_uint32(KEY_EOS);
extern const Number CONSTANT_KEY_EOL = number_from_uint32(KEY_EOL);
extern const Number CONSTANT_KEY_SF = number_from_uint32(KEY_SF);
extern const Number CONSTANT_KEY_SR = number_from_uint32(KEY_SR);
extern const Number CONSTANT_KEY_NPAGE = number_from_uint32(KEY_NPAGE);
extern const Number CONSTANT_KEY_PPAGE = number_from_uint32(KEY_PPAGE);
extern const Number CONSTANT_KEY_STAB = number_from_uint32(KEY_STAB);
extern const Number CONSTANT_KEY_CTAB = number_from_uint32(KEY_CTAB);
extern const Number CONSTANT_KEY_CATAB = number_from_uint32(KEY_CATAB);
extern const Number CONSTANT_KEY_ENTER = number_from_uint32(KEY_ENTER);
extern const Number CONSTANT_KEY_SRESET = number_from_uint32(KEY_SRESET);
extern const Number CONSTANT_KEY_RESET = number_from_uint32(KEY_RESET);
extern const Number CONSTANT_KEY_PRINT = number_from_uint32(KEY_PRINT);
extern const Number CONSTANT_KEY_LL = number_from_uint32(KEY_LL);
extern const Number CONSTANT_KEY_SHELP = number_from_uint32(KEY_SHELP);
extern const Number CONSTANT_KEY_BTAB = number_from_uint32(KEY_BTAB);
extern const Number CONSTANT_KEY_BEG = number_from_uint32(KEY_BEG);
extern const Number CONSTANT_KEY_CANCEL = number_from_uint32(KEY_CANCEL);
extern const Number CONSTANT_KEY_CLOSE = number_from_uint32(KEY_CLOSE);
extern const Number CONSTANT_KEY_COMMAND = number_from_uint32(KEY_COMMAND);
extern const Number CONSTANT_KEY_COPY = number_from_uint32(KEY_COPY);
extern const Number CONSTANT_KEY_CREATE = number_from_uint32(KEY_CREATE);
extern const Number CONSTANT_KEY_END = number_from_uint32(KEY_END);
extern const Number CONSTANT_KEY_EXIT = number_from_uint32(KEY_EXIT);
extern const Number CONSTANT_KEY_FIND = number_from_uint32(KEY_FIND);
extern const Number CONSTANT_KEY_HELP = number_from_uint32(KEY_HELP);
extern const Number CONSTANT_KEY_MARK = number_from_uint32(KEY_MARK);
extern const Number CONSTANT_KEY_MESSAGE = number_from_uint32(KEY_MESSAGE);
extern const Number CONSTANT_KEY_MOVE = number_from_uint32(KEY_MOVE);
extern const Number CONSTANT_KEY_NEXT = number_from_uint32(KEY_NEXT);
extern const Number CONSTANT_KEY_OPEN = number_from_uint32(KEY_OPEN);
extern const Number CONSTANT_KEY_OPTIONS = number_from_uint32(KEY_OPTIONS);
extern const Number CONSTANT_KEY_PREVIOUS = number_from_uint32(KEY_PREVIOUS);
extern const Number CONSTANT_KEY_REDO = number_from_uint32(KEY_REDO);
extern const Number CONSTANT_KEY_REFERENCE = number_from_uint32(KEY_REFERENCE);
extern const Number CONSTANT_KEY_REFRESH = number_from_uint32(KEY_REFRESH);
extern const Number CONSTANT_KEY_REPLACE = number_from_uint32(KEY_REPLACE);
extern const Number CONSTANT_KEY_RESTART = number_from_uint32(KEY_RESTART);
extern const Number CONSTANT_KEY_RESUME = number_from_uint32(KEY_RESUME);
extern const Number CONSTANT_KEY_SAVE = number_from_uint32(KEY_SAVE);

extern const Number CONSTANT_KEY_SBEG = number_from_uint32(KEY_SBEG);
extern const Number CONSTANT_KEY_SCANCEL = number_from_uint32(KEY_SCANCEL);
extern const Number CONSTANT_KEY_SCOMMAND = number_from_uint32(KEY_SCOMMAND);
extern const Number CONSTANT_KEY_SCOPY = number_from_uint32(KEY_SCOPY);
extern const Number CONSTANT_KEY_SCREATE = number_from_uint32(KEY_SCREATE);
extern const Number CONSTANT_KEY_SDC = number_from_uint32(KEY_SDC);
extern const Number CONSTANT_KEY_SDL = number_from_uint32(KEY_SDL);
extern const Number CONSTANT_KEY_SELECT = number_from_uint32(KEY_SELECT);
extern const Number CONSTANT_KEY_SEND = number_from_uint32(KEY_SEND);
extern const Number CONSTANT_KEY_SEOL = number_from_uint32(KEY_SEOL);
extern const Number CONSTANT_KEY_SEXIT = number_from_uint32(KEY_SEXIT);
extern const Number CONSTANT_KEY_SFIND = number_from_uint32(KEY_SFIND);
extern const Number CONSTANT_KEY_SHOME = number_from_uint32(KEY_SHOME);
extern const Number CONSTANT_KEY_SIC = number_from_uint32(KEY_SIC);
extern const Number CONSTANT_KEY_SLEFT = number_from_uint32(KEY_SLEFT);
extern const Number CONSTANT_KEY_SMESSAGE = number_from_uint32(KEY_SMESSAGE);
extern const Number CONSTANT_KEY_SMOVE = number_from_uint32(KEY_SMOVE);
extern const Number CONSTANT_KEY_SNEXT = number_from_uint32(KEY_SNEXT);
extern const Number CONSTANT_KEY_SOPTIONS = number_from_uint32(KEY_SOPTIONS);
extern const Number CONSTANT_KEY_SPREVIOUS = number_from_uint32(KEY_SPREVIOUS);
extern const Number CONSTANT_KEY_SPRINT = number_from_uint32(KEY_SPRINT);
extern const Number CONSTANT_KEY_SREDO = number_from_uint32(KEY_SREDO);
extern const Number CONSTANT_KEY_SREPLACE = number_from_uint32(KEY_SREPLACE);
extern const Number CONSTANT_KEY_SRIGHT = number_from_uint32(KEY_SRIGHT);
extern const Number CONSTANT_KEY_SRSUME = number_from_uint32(KEY_SRSUME);
extern const Number CONSTANT_KEY_SSAVE = number_from_uint32(KEY_SSAVE);
extern const Number CONSTANT_KEY_SSUSPEND = number_from_uint32(KEY_SSUSPEND);
extern const Number CONSTANT_KEY_SUNDO = number_from_uint32(KEY_SUNDO);
extern const Number CONSTANT_KEY_SUSPEND = number_from_uint32(KEY_SUSPEND);
extern const Number CONSTANT_KEY_UNDO = number_from_uint32(KEY_UNDO);

extern const Number CONSTANT_KEY_MIN = number_from_uint32(KEY_MIN);
extern const Number CONSTANT_KEY_MAX = number_from_uint32(KEY_MAX);

} // namespace curses

} // namespace rtl
