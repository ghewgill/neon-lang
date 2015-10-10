#include <curses.h>

#include "number.h"

namespace rtl {

extern const Number curses$ERR = number_from_sint32(ERR);
extern const Number curses$OK = number_from_sint32(OK);

extern const Number curses$BUTTON_RELEASED = number_from_uint32(BUTTON_RELEASED);
extern const Number curses$BUTTON_PRESSED = number_from_uint32(BUTTON_PRESSED);
extern const Number curses$BUTTON_CLICKED = number_from_uint32(BUTTON_CLICKED);
extern const Number curses$BUTTON_DOUBLE_CLICKED = number_from_uint32(BUTTON_DOUBLE_CLICKED);
extern const Number curses$BUTTON_TRIPLE_CLICKED = number_from_uint32(BUTTON_TRIPLE_CLICKED);
extern const Number curses$BUTTON_MOVED = number_from_uint32(BUTTON_MOVED);
extern const Number curses$WHEEL_SCROLLED = number_from_uint32(WHEEL_SCROLLED);
extern const Number curses$BUTTON_ACTION_MASK = number_from_uint32(BUTTON_ACTION_MASK);

extern const Number curses$PDC_BUTTON_SHIFT = number_from_uint32(PDC_BUTTON_SHIFT);
extern const Number curses$PDC_BUTTON_CONTROL = number_from_uint32(PDC_BUTTON_CONTROL);
extern const Number curses$PDC_BUTTON_ALT = number_from_uint32(PDC_BUTTON_ALT);
extern const Number curses$BUTTON_MODIFIER_MASK = number_from_uint32(BUTTON_MODIFIER_MASK);
extern const Number curses$MOUSE_X_POS = number_from_uint32(MOUSE_X_POS);
extern const Number curses$MOUSE_Y_POS = number_from_uint32(MOUSE_Y_POS);

extern const Number curses$PDC_MOUSE_MOVED = number_from_uint32(PDC_MOUSE_MOVED);
extern const Number curses$PDC_MOUSE_POSITION = number_from_uint32(PDC_MOUSE_POSITION);
extern const Number curses$PDC_MOUSE_WHEEL_UP = number_from_uint32(PDC_MOUSE_WHEEL_UP);
extern const Number curses$PDC_MOUSE_WHEEL_DOWN = number_from_uint32(PDC_MOUSE_WHEEL_DOWN);

extern const Number curses$A_BUTTON_CHANGED = number_from_uint32(A_BUTTON_CHANGED);
extern const Number curses$MOUSE_MOVED = number_from_uint32(MOUSE_MOVED);
extern const Number curses$MOUSE_POS_REPORT = number_from_uint32(MOUSE_POS_REPORT);
extern const Number curses$MOUSE_WHEEL_UP = number_from_uint32(MOUSE_WHEEL_UP);
extern const Number curses$MOUSE_WHEEL_DOWN = number_from_uint32(MOUSE_WHEEL_DOWN);

extern const Number curses$BUTTON1_RELEASED = number_from_uint32(BUTTON1_RELEASED);
extern const Number curses$BUTTON1_PRESSED = number_from_uint32(BUTTON1_PRESSED);
extern const Number curses$BUTTON1_CLICKED = number_from_uint32(BUTTON1_CLICKED);
extern const Number curses$BUTTON1_DOUBLE_CLICKED = number_from_uint32(BUTTON1_DOUBLE_CLICKED);
extern const Number curses$BUTTON1_TRIPLE_CLICKED = number_from_uint32(BUTTON1_TRIPLE_CLICKED);

extern const Number curses$BUTTON2_RELEASED = number_from_uint32(BUTTON2_RELEASED);
extern const Number curses$BUTTON2_PRESSED = number_from_uint32(BUTTON2_PRESSED);
extern const Number curses$BUTTON2_CLICKED = number_from_uint32(BUTTON2_CLICKED);
extern const Number curses$BUTTON2_DOUBLE_CLICKED = number_from_uint32(BUTTON2_DOUBLE_CLICKED);
extern const Number curses$BUTTON2_TRIPLE_CLICKED = number_from_uint32(BUTTON2_TRIPLE_CLICKED);

extern const Number curses$BUTTON3_RELEASED = number_from_uint32(BUTTON3_RELEASED);
extern const Number curses$BUTTON3_PRESSED = number_from_uint32(BUTTON3_PRESSED);
extern const Number curses$BUTTON3_CLICKED = number_from_uint32(BUTTON3_CLICKED);
extern const Number curses$BUTTON3_DOUBLE_CLICKED = number_from_uint32(BUTTON3_DOUBLE_CLICKED);
extern const Number curses$BUTTON3_TRIPLE_CLICKED = number_from_uint32(BUTTON3_TRIPLE_CLICKED);

extern const Number curses$BUTTON4_RELEASED = number_from_uint32(BUTTON4_RELEASED);
extern const Number curses$BUTTON4_PRESSED = number_from_uint32(BUTTON4_PRESSED);
extern const Number curses$BUTTON4_CLICKED = number_from_uint32(BUTTON4_CLICKED);
extern const Number curses$BUTTON4_DOUBLE_CLICKED = number_from_uint32(BUTTON4_DOUBLE_CLICKED);
extern const Number curses$BUTTON4_TRIPLE_CLICKED = number_from_uint32(BUTTON4_TRIPLE_CLICKED);

extern const Number curses$BUTTON5_RELEASED = number_from_uint32(BUTTON5_RELEASED);
extern const Number curses$BUTTON5_PRESSED = number_from_uint32(BUTTON5_PRESSED);
extern const Number curses$BUTTON5_CLICKED = number_from_uint32(BUTTON5_CLICKED);
extern const Number curses$BUTTON5_DOUBLE_CLICKED = number_from_uint32(BUTTON5_DOUBLE_CLICKED);
extern const Number curses$BUTTON5_TRIPLE_CLICKED = number_from_uint32(BUTTON5_TRIPLE_CLICKED);

extern const Number curses$MOUSE_WHEEL_SCROLL = number_from_uint32(MOUSE_WHEEL_SCROLL);
extern const Number curses$BUTTON_MODIFIER_SHIFT = number_from_uint32(BUTTON_MODIFIER_SHIFT);
extern const Number curses$BUTTON_MODIFIER_CONTROL = number_from_uint32(BUTTON_MODIFIER_CONTROL);
extern const Number curses$BUTTON_MODIFIER_ALT = number_from_uint32(BUTTON_MODIFIER_ALT);

extern const Number curses$ALL_MOUSE_EVENTS = number_from_uint32(ALL_MOUSE_EVENTS);
extern const Number curses$REPORT_MOUSE_POSITION = number_from_uint32(REPORT_MOUSE_POSITION);

extern const Number curses$BUTTON_SHIFT = number_from_uint32(BUTTON_SHIFT);
extern const Number curses$BUTTON_CONTROL = number_from_uint32(BUTTON_CONTROL);
extern const Number curses$BUTTON_ALT = number_from_uint32(BUTTON_ALT);

extern const Number curses$A_CHARTEXT = number_from_uint32(A_CHARTEXT);
extern const Number curses$A_NORMAL = number_from_uint32(A_NORMAL);
extern const Number curses$A_ALTCHARSET = number_from_uint32(A_ALTCHARSET);
extern const Number curses$A_INVIS = number_from_uint32(A_INVIS);
extern const Number curses$A_UNDERLINE = number_from_uint32(A_UNDERLINE);
extern const Number curses$A_REVERSE = number_from_uint32(A_REVERSE);
extern const Number curses$A_BLINK = number_from_uint32(A_BLINK);
extern const Number curses$A_BOLD = number_from_uint32(A_BOLD);
extern const Number curses$A_ATTRIBUTES = number_from_uint64(A_ATTRIBUTES);
extern const Number curses$A_COLOR = number_from_uint32(A_COLOR);
extern const Number curses$A_PROTECT = number_from_uint32(A_PROTECT);

extern const Number curses$COLOR_BLACK = number_from_uint32(COLOR_BLACK);
extern const Number curses$COLOR_RED = number_from_uint32(COLOR_RED);
extern const Number curses$COLOR_GREEN = number_from_uint32(COLOR_GREEN);
extern const Number curses$COLOR_BLUE = number_from_uint32(COLOR_BLUE);
extern const Number curses$COLOR_CYAN = number_from_uint32(COLOR_CYAN);
extern const Number curses$COLOR_MAGENTA = number_from_uint32(COLOR_MAGENTA);
extern const Number curses$COLOR_YELLOW = number_from_uint32(COLOR_YELLOW);
extern const Number curses$COLOR_WHITE = number_from_uint32(COLOR_WHITE);

extern const Number curses$KEY_CODE_YES = number_from_uint32(KEY_CODE_YES);

extern const Number curses$KEY_BREAK = number_from_uint32(KEY_BREAK);
extern const Number curses$KEY_DOWN = number_from_uint32(KEY_DOWN);
extern const Number curses$KEY_UP = number_from_uint32(KEY_UP);
extern const Number curses$KEY_LEFT = number_from_uint32(KEY_LEFT);
extern const Number curses$KEY_RIGHT = number_from_uint32(KEY_RIGHT);
extern const Number curses$KEY_HOME = number_from_uint32(KEY_HOME);
extern const Number curses$KEY_BACKSPACE = number_from_uint32(KEY_BACKSPACE);

extern const Number curses$KEY_F0 = number_from_uint32(KEY_F0);
extern const Number curses$KEY_DL = number_from_uint32(KEY_DL);
extern const Number curses$KEY_IL = number_from_uint32(KEY_IL);
extern const Number curses$KEY_DC = number_from_uint32(KEY_DC);
extern const Number curses$KEY_IC = number_from_uint32(KEY_IC);
extern const Number curses$KEY_EIC = number_from_uint32(KEY_EIC);
extern const Number curses$KEY_CLEAR = number_from_uint32(KEY_CLEAR);
extern const Number curses$KEY_EOS = number_from_uint32(KEY_EOS);
extern const Number curses$KEY_EOL = number_from_uint32(KEY_EOL);
extern const Number curses$KEY_SF = number_from_uint32(KEY_SF);
extern const Number curses$KEY_SR = number_from_uint32(KEY_SR);
extern const Number curses$KEY_NPAGE = number_from_uint32(KEY_NPAGE);
extern const Number curses$KEY_PPAGE = number_from_uint32(KEY_PPAGE);
extern const Number curses$KEY_STAB = number_from_uint32(KEY_STAB);
extern const Number curses$KEY_CTAB = number_from_uint32(KEY_CTAB);
extern const Number curses$KEY_CATAB = number_from_uint32(KEY_CATAB);
extern const Number curses$KEY_ENTER = number_from_uint32(KEY_ENTER);
extern const Number curses$KEY_SRESET = number_from_uint32(KEY_SRESET);
extern const Number curses$KEY_RESET = number_from_uint32(KEY_RESET);
extern const Number curses$KEY_PRINT = number_from_uint32(KEY_PRINT);
extern const Number curses$KEY_LL = number_from_uint32(KEY_LL);
extern const Number curses$KEY_SHELP = number_from_uint32(KEY_SHELP);
extern const Number curses$KEY_BTAB = number_from_uint32(KEY_BTAB);
extern const Number curses$KEY_BEG = number_from_uint32(KEY_BEG);
extern const Number curses$KEY_CANCEL = number_from_uint32(KEY_CANCEL);
extern const Number curses$KEY_CLOSE = number_from_uint32(KEY_CLOSE);
extern const Number curses$KEY_COMMAND = number_from_uint32(KEY_COMMAND);
extern const Number curses$KEY_COPY = number_from_uint32(KEY_COPY);
extern const Number curses$KEY_CREATE = number_from_uint32(KEY_CREATE);
extern const Number curses$KEY_END = number_from_uint32(KEY_END);
extern const Number curses$KEY_EXIT = number_from_uint32(KEY_EXIT);
extern const Number curses$KEY_FIND = number_from_uint32(KEY_FIND);
extern const Number curses$KEY_HELP = number_from_uint32(KEY_HELP);
extern const Number curses$KEY_MARK = number_from_uint32(KEY_MARK);
extern const Number curses$KEY_MESSAGE = number_from_uint32(KEY_MESSAGE);
extern const Number curses$KEY_MOVE = number_from_uint32(KEY_MOVE);
extern const Number curses$KEY_NEXT = number_from_uint32(KEY_NEXT);
extern const Number curses$KEY_OPEN = number_from_uint32(KEY_OPEN);
extern const Number curses$KEY_OPTIONS = number_from_uint32(KEY_OPTIONS);
extern const Number curses$KEY_PREVIOUS = number_from_uint32(KEY_PREVIOUS);
extern const Number curses$KEY_REDO = number_from_uint32(KEY_REDO);
extern const Number curses$KEY_REFERENCE = number_from_uint32(KEY_REFERENCE);
extern const Number curses$KEY_REFRESH = number_from_uint32(KEY_REFRESH);
extern const Number curses$KEY_REPLACE = number_from_uint32(KEY_REPLACE);
extern const Number curses$KEY_RESTART = number_from_uint32(KEY_RESTART);
extern const Number curses$KEY_RESUME = number_from_uint32(KEY_RESUME);
extern const Number curses$KEY_SAVE = number_from_uint32(KEY_SAVE);

extern const Number curses$KEY_SBEG = number_from_uint32(KEY_SBEG);
extern const Number curses$KEY_SCANCEL = number_from_uint32(KEY_SCANCEL);
extern const Number curses$KEY_SCOMMAND = number_from_uint32(KEY_SCOMMAND);
extern const Number curses$KEY_SCOPY = number_from_uint32(KEY_SCOPY);
extern const Number curses$KEY_SCREATE = number_from_uint32(KEY_SCREATE);
extern const Number curses$KEY_SDC = number_from_uint32(KEY_SDC);
extern const Number curses$KEY_SDL = number_from_uint32(KEY_SDL);
extern const Number curses$KEY_SELECT = number_from_uint32(KEY_SELECT);
extern const Number curses$KEY_SEND = number_from_uint32(KEY_SEND);
extern const Number curses$KEY_SEOL = number_from_uint32(KEY_SEOL);
extern const Number curses$KEY_SEXIT = number_from_uint32(KEY_SEXIT);
extern const Number curses$KEY_SFIND = number_from_uint32(KEY_SFIND);
extern const Number curses$KEY_SHOME = number_from_uint32(KEY_SHOME);
extern const Number curses$KEY_SIC = number_from_uint32(KEY_SIC);
extern const Number curses$KEY_SLEFT = number_from_uint32(KEY_SLEFT);
extern const Number curses$KEY_SMESSAGE = number_from_uint32(KEY_SMESSAGE);
extern const Number curses$KEY_SMOVE = number_from_uint32(KEY_SMOVE);
extern const Number curses$KEY_SNEXT = number_from_uint32(KEY_SNEXT);
extern const Number curses$KEY_SOPTIONS = number_from_uint32(KEY_SOPTIONS);
extern const Number curses$KEY_SPREVIOUS = number_from_uint32(KEY_SPREVIOUS);
extern const Number curses$KEY_SPRINT = number_from_uint32(KEY_SPRINT);
extern const Number curses$KEY_SREDO = number_from_uint32(KEY_SREDO);
extern const Number curses$KEY_SREPLACE = number_from_uint32(KEY_SREPLACE);
extern const Number curses$KEY_SRIGHT = number_from_uint32(KEY_SRIGHT);
extern const Number curses$KEY_SRSUME = number_from_uint32(KEY_SRSUME);
extern const Number curses$KEY_SSAVE = number_from_uint32(KEY_SSAVE);
extern const Number curses$KEY_SSUSPEND = number_from_uint32(KEY_SSUSPEND);
extern const Number curses$KEY_SUNDO = number_from_uint32(KEY_SUNDO);
extern const Number curses$KEY_SUSPEND = number_from_uint32(KEY_SUSPEND);
extern const Number curses$KEY_UNDO = number_from_uint32(KEY_UNDO);
extern const Number curses$KEY_MOUSE = number_from_uint32(KEY_MOUSE);

extern const Number curses$KEY_MIN = number_from_uint32(KEY_MIN);
extern const Number curses$KEY_MAX = number_from_uint32(KEY_MAX);

}
