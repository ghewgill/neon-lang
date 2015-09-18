#include <vector>

#include <curses.h>

#include "number.h"

// Unsupported: all wchar_t and cchar_t functions
// Unsupported: getstr (use getnstr)
// Unsupported: all other *nstr functions
// Unsupported: newterm, set_term, delscreen
// Unsupported: typeahead
// Unimplemented: attr_* (curs_attr)
// Unimplemented: inch[n]str
// Unimplemented: in[n]str
// Unimplemented: ripoffline
// Unimplemented: curs_mouse
// Unimplemented: printw, scanw
// Unimplemented: slk_*
// Unimplemented: curs_termcap, curs_terminfo
// Unimplemented: putwin, getwin
// TODO: use OUT parameters for functions like getyx

static std::vector<chtype> chtypes_from_numbers(const std::vector<Number> &chstr)
{
    std::vector<chtype> r;
    for (auto c: chstr) {
        r.push_back(number_to_uint32(c));
    }
    return r;
}

namespace rtl {

// NCurses #defines...

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
extern const Number curses$KEY_ABORT = number_from_uint32(KEY_ABORT);
extern const Number curses$KEY_SHELP = number_from_uint32(KEY_SHELP);
extern const Number curses$KEY_LHELP = number_from_uint32(KEY_LHELP);
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

extern const Number curses$ALT_0 = number_from_uint32(ALT_0);
extern const Number curses$ALT_1 = number_from_uint32(ALT_1);
extern const Number curses$ALT_2 = number_from_uint32(ALT_2);
extern const Number curses$ALT_3 = number_from_uint32(ALT_3);
extern const Number curses$ALT_4 = number_from_uint32(ALT_4);
extern const Number curses$ALT_5 = number_from_uint32(ALT_5);
extern const Number curses$ALT_6 = number_from_uint32(ALT_6);
extern const Number curses$ALT_7 = number_from_uint32(ALT_7);
extern const Number curses$ALT_8 = number_from_uint32(ALT_8);
extern const Number curses$ALT_9 = number_from_uint32(ALT_9);
extern const Number curses$ALT_A = number_from_uint32(ALT_A);
extern const Number curses$ALT_B = number_from_uint32(ALT_B);
extern const Number curses$ALT_C = number_from_uint32(ALT_C);
extern const Number curses$ALT_D = number_from_uint32(ALT_D);
extern const Number curses$ALT_E = number_from_uint32(ALT_E);
extern const Number curses$ALT_F = number_from_uint32(ALT_F);
extern const Number curses$ALT_G = number_from_uint32(ALT_G);
extern const Number curses$ALT_H = number_from_uint32(ALT_H);
extern const Number curses$ALT_I = number_from_uint32(ALT_I);
extern const Number curses$ALT_J = number_from_uint32(ALT_J);
extern const Number curses$ALT_K = number_from_uint32(ALT_K);
extern const Number curses$ALT_L = number_from_uint32(ALT_L);
extern const Number curses$ALT_M = number_from_uint32(ALT_M);
extern const Number curses$ALT_N = number_from_uint32(ALT_N);
extern const Number curses$ALT_O = number_from_uint32(ALT_O);
extern const Number curses$ALT_P = number_from_uint32(ALT_P);
extern const Number curses$ALT_Q = number_from_uint32(ALT_Q);
extern const Number curses$ALT_R = number_from_uint32(ALT_R);
extern const Number curses$ALT_S = number_from_uint32(ALT_S);
extern const Number curses$ALT_T = number_from_uint32(ALT_T);
extern const Number curses$ALT_U = number_from_uint32(ALT_U);
extern const Number curses$ALT_V = number_from_uint32(ALT_V);
extern const Number curses$ALT_W = number_from_uint32(ALT_W);
extern const Number curses$ALT_X = number_from_uint32(ALT_X);
extern const Number curses$ALT_Y = number_from_uint32(ALT_Y);
extern const Number curses$ALT_Z = number_from_uint32(ALT_Z);

extern const Number curses$CTL_LEFT = number_from_uint32(CTL_LEFT);
extern const Number curses$CTL_RIGHT = number_from_uint32(CTL_RIGHT);
extern const Number curses$CTL_PGUP = number_from_uint32(CTL_PGUP);
extern const Number curses$CTL_PGDN = number_from_uint32(CTL_PGDN);
extern const Number curses$CTL_HOME = number_from_uint32(CTL_HOME);
extern const Number curses$CTL_END = number_from_uint32(CTL_END);

extern const Number curses$KEY_A1 = number_from_uint32(KEY_A1);
extern const Number curses$KEY_A2 = number_from_uint32(KEY_A2);
extern const Number curses$KEY_A3 = number_from_uint32(KEY_A3);
extern const Number curses$KEY_B1 = number_from_uint32(KEY_B1);
extern const Number curses$KEY_B2 = number_from_uint32(KEY_B2);
extern const Number curses$KEY_B3 = number_from_uint32(KEY_B3);
extern const Number curses$KEY_C1 = number_from_uint32(KEY_C1);
extern const Number curses$KEY_C2 = number_from_uint32(KEY_C2);
extern const Number curses$KEY_C3 = number_from_uint32(KEY_C3);    

extern const Number curses$PADSLASH = number_from_uint32(PADSLASH);
extern const Number curses$PADENTER = number_from_uint32(PADENTER);
extern const Number curses$CTL_PADENTER = number_from_uint32(CTL_PADENTER);
extern const Number curses$ALT_PADENTER = number_from_uint32(ALT_PADENTER);
extern const Number curses$PADSTOP = number_from_uint32(PADSTOP);
extern const Number curses$PADSTAR = number_from_uint32(PADSTAR);
extern const Number curses$PADMINUS = number_from_uint32(PADMINUS);
extern const Number curses$PADPLUS = number_from_uint32(PADPLUS);
extern const Number curses$CTL_PADSTOP = number_from_uint32(CTL_PADSTOP);
extern const Number curses$CTL_PADCENTER = number_from_uint32(CTL_PADCENTER);
extern const Number curses$CTL_PADPLUS = number_from_uint32(CTL_PADPLUS);
extern const Number curses$CTL_PADMINUS = number_from_uint32(CTL_PADMINUS);
extern const Number curses$CTL_PADSLASH = number_from_uint32(CTL_PADSLASH);
extern const Number curses$CTL_PADSTAR = number_from_uint32(CTL_PADSTAR);
extern const Number curses$ALT_PADPLUS = number_from_uint32(ALT_PADPLUS);
extern const Number curses$ALT_PADMINUS = number_from_uint32(ALT_PADMINUS);
extern const Number curses$ALT_PADSLASH = number_from_uint32(ALT_PADSLASH);
extern const Number curses$ALT_PADSTAR = number_from_uint32(ALT_PADSTAR);
extern const Number curses$ALT_PADSTOP = number_from_uint32(ALT_PADSTOP);
extern const Number curses$CTL_INS = number_from_uint32(CTL_INS);
extern const Number curses$ALT_DEL = number_from_uint32(ALT_DEL);
extern const Number curses$ALT_INS = number_from_uint32(ALT_INS);
extern const Number curses$CTL_UP = number_from_uint32(CTL_UP);
extern const Number curses$CTL_DOWN = number_from_uint32(CTL_DOWN);
extern const Number curses$CTL_TAB = number_from_uint32(CTL_TAB);
extern const Number curses$ALT_TAB = number_from_uint32(ALT_TAB);
extern const Number curses$ALT_MINUS = number_from_uint32(ALT_MINUS);
extern const Number curses$ALT_EQUAL = number_from_uint32(ALT_EQUAL);
extern const Number curses$ALT_HOME = number_from_uint32(ALT_HOME);
extern const Number curses$ALT_PGUP = number_from_uint32(ALT_PGUP);
extern const Number curses$ALT_PGDN = number_from_uint32(ALT_PGDN);
extern const Number curses$ALT_END = number_from_uint32(ALT_END);
extern const Number curses$ALT_UP = number_from_uint32(ALT_UP);
extern const Number curses$ALT_DOWN = number_from_uint32(ALT_DOWN);
extern const Number curses$ALT_RIGHT = number_from_uint32(ALT_RIGHT);
extern const Number curses$ALT_LEFT = number_from_uint32(ALT_LEFT);
extern const Number curses$ALT_ENTER = number_from_uint32(ALT_ENTER);
extern const Number curses$ALT_ESC = number_from_uint32(ALT_ESC);
extern const Number curses$ALT_BQUOTE = number_from_uint32(ALT_BQUOTE);
extern const Number curses$ALT_LBRACKET = number_from_uint32(ALT_LBRACKET);
extern const Number curses$ALT_RBRACKET = number_from_uint32(ALT_RBRACKET);
extern const Number curses$ALT_SEMICOLON = number_from_uint32(ALT_SEMICOLON);
extern const Number curses$ALT_FQUOTE = number_from_uint32(ALT_FQUOTE);
extern const Number curses$ALT_COMMA = number_from_uint32(ALT_COMMA);
extern const Number curses$ALT_STOP = number_from_uint32(ALT_STOP);
extern const Number curses$ALT_FSLASH = number_from_uint32(ALT_FSLASH);
extern const Number curses$ALT_BKSP = number_from_uint32(ALT_BKSP);
extern const Number curses$CTL_BKSP = number_from_uint32(CTL_BKSP);
extern const Number curses$PAD0 = number_from_uint32(PAD0);
extern const Number curses$CTL_PAD0 = number_from_uint32(CTL_PAD0);
extern const Number curses$CTL_PAD1 = number_from_uint32(CTL_PAD1);
extern const Number curses$CTL_PAD2 = number_from_uint32(CTL_PAD2);
extern const Number curses$CTL_PAD3 = number_from_uint32(CTL_PAD3);
extern const Number curses$CTL_PAD4 = number_from_uint32(CTL_PAD4);
extern const Number curses$CTL_PAD5 = number_from_uint32(CTL_PAD5);
extern const Number curses$CTL_PAD6 = number_from_uint32(CTL_PAD6);
extern const Number curses$CTL_PAD7 = number_from_uint32(CTL_PAD7);
extern const Number curses$CTL_PAD8 = number_from_uint32(CTL_PAD8);
extern const Number curses$CTL_PAD9 = number_from_uint32(CTL_PAD9);
extern const Number curses$ALT_PAD0 = number_from_uint32(ALT_PAD0);
extern const Number curses$ALT_PAD1 = number_from_uint32(ALT_PAD1);
extern const Number curses$ALT_PAD2 = number_from_uint32(ALT_PAD2);
extern const Number curses$ALT_PAD3 = number_from_uint32(ALT_PAD3);
extern const Number curses$ALT_PAD4 = number_from_uint32(ALT_PAD4);
extern const Number curses$ALT_PAD5 = number_from_uint32(ALT_PAD5);
extern const Number curses$ALT_PAD6 = number_from_uint32(ALT_PAD6);
extern const Number curses$ALT_PAD7 = number_from_uint32(ALT_PAD7);
extern const Number curses$ALT_PAD8 = number_from_uint32(ALT_PAD8);
extern const Number curses$ALT_PAD9 = number_from_uint32(ALT_PAD9);
extern const Number curses$CTL_DEL = number_from_uint32(CTL_DEL);
extern const Number curses$ALT_BSLASH = number_from_uint32(ALT_BSLASH);
extern const Number curses$CTL_ENTER = number_from_uint32(CTL_ENTER);

extern const Number curses$SHF_PADENTER = number_from_uint32(SHF_PADENTER);
extern const Number curses$SHF_PADSLASH = number_from_uint32(SHF_PADSLASH);
extern const Number curses$SHF_PADSTAR = number_from_uint32(SHF_PADSTAR);
extern const Number curses$SHF_PADPLUS = number_from_uint32(SHF_PADPLUS);
extern const Number curses$SHF_PADMINUS = number_from_uint32(SHF_PADMINUS);
extern const Number curses$SHF_UP = number_from_uint32(SHF_UP);
extern const Number curses$SHF_DOWN = number_from_uint32(SHF_DOWN);
extern const Number curses$SHF_IC = number_from_uint32(SHF_IC);
extern const Number curses$SHF_DC = number_from_uint32(SHF_DC);

extern const Number curses$KEY_MOUSE = number_from_uint32(KEY_MOUSE);
extern const Number curses$KEY_SHIFT_L = number_from_uint32(KEY_SHIFT_L);
extern const Number curses$KEY_SHIFT_R = number_from_uint32(KEY_SHIFT_R);
extern const Number curses$KEY_CONTROL_L = number_from_uint32(KEY_CONTROL_L);
extern const Number curses$KEY_CONTROL_R = number_from_uint32(KEY_CONTROL_R);
extern const Number curses$KEY_ALT_L = number_from_uint32(KEY_ALT_L);
extern const Number curses$KEY_ALT_R = number_from_uint32(KEY_ALT_R);
extern const Number curses$KEY_RESIZE = number_from_uint32(KEY_RESIZE);
extern const Number curses$KEY_SUP = number_from_uint32(KEY_SUP);
extern const Number curses$KEY_SDOWN = number_from_uint32(KEY_SDOWN);
extern const Number curses$KEY_MIN = number_from_uint32(KEY_MIN);
extern const Number curses$KEY_MAX = number_from_uint32(KEY_MAX);

// NCurses Functions...

Number curses$KEY_F(Number c)
{
    return number_from_uint32(KEY_F(number_to_uint32(c)));
}

Number curses$COLS()
{
    return number_from_sint32(COLS);
}

Number curses$LINES()
{
    return number_from_sint32(LINES);
}

void curses$addch(Number ch)
{
    addch(number_to_uint32(ch));
}

void curses$waddch(void *win, Number ch)
{
    waddch(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void curses$mvaddch(Number y, Number x, Number ch)
{
    mvaddch(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void curses$mvwaddch(void *win, Number y, Number x, Number ch)
{
    mvwaddch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void curses$addchstr(const std::vector<Number> &chstr)
{
    addchstr(chtypes_from_numbers(chstr).data());
}

void curses$waddchstr(void *win, const std::vector<Number> &chstr)
{
    waddchstr(static_cast<WINDOW *>(win), chtypes_from_numbers(chstr).data());
}

void curses$mvaddchstr(Number y, Number x, const std::vector<Number> &chstr)
{
    mvaddchstr(number_to_sint32(y), number_to_sint32(x), chtypes_from_numbers(chstr).data());
}

void curses$mvwaddchstr(void *win, Number y, Number x, const std::vector<Number> &chstr)
{
    mvwaddchstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), chtypes_from_numbers(chstr).data());
}

void curses$addstr(const std::string &str)
{
    addstr(str.c_str());
}

void curses$waddstr(void *win, const std::string &str)
{
    waddstr(static_cast<WINDOW *>(win), str.c_str());
}

void curses$mvaddstr(Number y, Number x, const std::string &str)
{
    mvaddstr(number_to_sint32(y), number_to_sint32(x), str.c_str());
}

void curses$mvwaddstr(void *win, Number y, Number x, const std::string &str)
{
    mvwaddstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), str.c_str());
}

void curses$attroff(Number attrs)
{
    attroff(number_to_uint32(attrs));
}

void curses$wattroff(void *win, Number attrs)
{
    wattroff(static_cast<WINDOW *>(win), number_to_uint32(attrs));
}

void curses$attron(Number attrs)
{
    attron(number_to_uint32(attrs));
}

void curses$wattron(void *win, Number attrs)
{
    wattron(static_cast<WINDOW *>(win), number_to_uint32(attrs));
}

void curses$attrset(Number attrs)
{
    attrset(number_to_uint32(attrs));
}

void curses$wattrset(void *win, Number attrs)
{
    wattrset(static_cast<WINDOW *>(win), number_to_uint32(attrs));
}

Number curses$baudrate()
{
    return number_from_sint32(baudrate());
}

void curses$beep()
{
    beep();
}

void curses$bkgd(Number ch)
{
    bkgd(number_to_uint32(ch));
}

void curses$wbkgd(void *win, Number ch)
{
    wbkgd(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void curses$bkgdset(Number ch)
{
    bkgdset(number_to_uint32(ch));
}

void curses$wbkgdset(void *win, Number ch)
{
    wbkgdset(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void curses$border(Number ls, Number rs, Number ts, Number bs, Number tl, Number tr, Number bl, Number br)
{
    border(number_to_uint32(ls),
           number_to_uint32(rs),
           number_to_uint32(ts),
           number_to_uint32(bs),
           number_to_uint32(tl),
           number_to_uint32(tr),
           number_to_uint32(bl),
           number_to_uint32(br));
}

void curses$wborder(void *win, Number ls, Number rs, Number ts, Number bs, Number tl, Number tr, Number bl, Number br)
{
    wborder(static_cast<WINDOW *>(win),
            number_to_uint32(ls),
            number_to_uint32(rs),
            number_to_uint32(ts),
            number_to_uint32(bs),
            number_to_uint32(tl),
            number_to_uint32(tr),
            number_to_uint32(bl),
            number_to_uint32(br));
}

void curses$box(void *win, Number verch, Number horch)
{
    box(static_cast<WINDOW *>(win), number_to_uint32(verch), number_to_uint32(horch));
}

bool curses$can_change_color()
{
    return can_change_color() != FALSE;
}

void curses$cbreak()
{
    cbreak();
}

void curses$nocbreak()
{
    nocbreak();
}

void curses$chgat(Number n, Number attr, Number color, void *opts)
{
    chgat(number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void curses$wchgat(void *win, Number n, Number attr, Number color, void *opts)
{
    wchgat(static_cast<WINDOW *>(win), number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void curses$mvchgat(Number y, Number x, Number n, Number attr, Number color, void *opts)
{
    mvchgat(number_to_sint32(y), number_to_sint32(x), number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void curses$mvwchgat(void *win, Number y, Number x, Number n, Number attr, Number color, void *opts)
{
    mvwchgat(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void curses$clear()
{
    clear();
}

void curses$wclear(void *win)
{
    wclear(static_cast<WINDOW *>(win));
}

void curses$clearok(void *win, bool bf)
{
    clearok(static_cast<WINDOW *>(win), bf);
}

void curses$clrtobot()
{
    clrtobot();
}

void curses$wclrtobot(void *win)
{
    wclrtobot(static_cast<WINDOW *>(win));
}

void curses$clrtoeol()
{
    clrtoeol();
}

void curses$wclrtoeol(void *win)
{
    wclrtoeol(static_cast<WINDOW *>(win));
}

std::vector<Number> curses$color_content(Number color)
{
    short r = 0, g = 0, b = 0;
    color_content(static_cast<short>(number_to_uint32(color)), &r, &g, &b);
    std::vector<Number> a;
    a.push_back(number_from_sint32(r));
    a.push_back(number_from_sint32(g));
    a.push_back(number_from_sint32(b));
    return a;
}

void curses$color_set(Number color_pair_number, void *opts)
{
    color_set(static_cast<short>(number_to_uint32(color_pair_number)), opts);
}

void curses$wcolor_set(void *win, Number color_pair_number, void *opts)
{
    wcolor_set(static_cast<WINDOW *>(win), static_cast<short>(number_to_uint32(color_pair_number)), opts);
}

void curses$copywin(void *srcwin, void *dstwin, Number sminrow, Number smincol, Number dminrow, Number dmincol, Number dmaxrow, Number dmaxcol, bool overlay)
{
    copywin(static_cast<WINDOW *>(srcwin),
            static_cast<WINDOW *>(dstwin),
            number_to_sint32(sminrow),
            number_to_sint32(smincol),
            number_to_sint32(dminrow),
            number_to_sint32(dmincol),
            number_to_sint32(dmaxrow),
            number_to_sint32(dmaxcol),
            overlay);
}

void curses$curs_set(Number visibility)
{
    curs_set(number_to_sint32(visibility));
}

std::string curses$curses_version()
{
    return curses_version();
}

void curses$def_prog_mode()
{
    def_prog_mode();
}

void curses$def_shell_mode()
{
    def_shell_mode();
}

void curses$delay_output(Number ms)
{
    delay_output(number_to_sint32(ms));
}

void curses$delch()
{
    delch();
}

void curses$wdelch(void *win)
{
    wdelch(static_cast<WINDOW *>(win));
}

void curses$mvdelch(Number y, Number x)
{
    mvdelch(number_to_sint32(y), number_to_sint32(x));
}

void curses$mvwdelch(void *win, Number y, Number x)
{
    mvwdelch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x));
}

void curses$deleteln()
{
    deleteln();
}

void curses$wdeleteln(void *win)
{
    wdeleteln(static_cast<WINDOW *>(win));
}

void curses$delwin(void *win)
{
    delwin(static_cast<WINDOW *>(win));
}

void *curses$derwin(void *orig, Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return derwin(static_cast<WINDOW *>(orig), number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void curses$doupdate()
{
    doupdate();
}

void *curses$dupwin(void *win)
{
    return dupwin(static_cast<WINDOW *>(win));
}

void curses$echo()
{
    echo();
}

void curses$noecho()
{
    noecho();
}

void curses$echochar(Number ch)
{
    echochar(number_to_uint32(ch));
}

void curses$wechochar(void *win, Number ch)
{
    wechochar(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void curses$endwin()
{
    endwin();
}

void curses$erase()
{
    erase();
}

void curses$werase(void *win)
{
    werase(static_cast<WINDOW *>(win));
}

std::string curses$erasechar()
{
    return std::string(1, erasechar());
}

void curses$filter()
{
    filter();
}

void curses$flash()
{
    flash();
}

void curses$flushinp()
{
    flushinp();
}

Number curses$getbegx(void *win)
{
    return number_from_sint32(getbegx(static_cast<WINDOW *>(win)));
}

Number curses$getbegy(void *win)
{
    return number_from_sint32(getbegy(static_cast<WINDOW *>(win)));
}

std::vector<Number> curses$getbegyx(void *win)
{
    int y = -1, x = -1;
    getbegyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

Number curses$getbkgd(void *win)
{
    return number_from_uint32(getbkgd(static_cast<WINDOW *>(win)));
}

Number curses$getch()
{
    return number_from_sint32(getch());
}

Number curses$wgetch(void *win)
{
    return number_from_sint32(wgetch(static_cast<WINDOW *>(win)));
}

Number curses$mvgetch(Number y, Number x)
{
    return number_from_sint32(mvgetch(number_to_sint32(y), number_to_sint32(x)));
}

Number curses$mvwgetch(void *win, Number y, Number x)
{
    return number_from_sint32(mvwgetch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x)));
}

Number curses$getcurx(void *win)
{
    return number_from_sint32(getcurx(static_cast<WINDOW *>(win)));
}

Number curses$getcury(void *win)
{
    return number_from_sint32(getcury(static_cast<WINDOW *>(win)));
}

Number curses$getmaxx(void *win)
{
    return number_from_sint32(getmaxx(static_cast<WINDOW *>(win)));
}

Number curses$getmaxy(void *win)
{
    return number_from_sint32(getmaxy(static_cast<WINDOW *>(win)));
}

std::vector<Number> curses$getmaxyx(void *win)
{
    int y = -1, x = -1;
    getmaxyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

std::string curses$getnstr(Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    getnstr(str, width);
    std::string r = str;
    delete[] str;
    return r;
}

std::string curses$wgetnstr(void *win, Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    wgetnstr(static_cast<WINDOW *>(win), str, width);
    std::string r = str;
    delete[] str;
    return r;
}

std::string curses$mvgetnstr(Number y, Number x, Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    mvgetnstr(number_to_sint32(y), number_to_sint32(x), str, width);
    std::string r = str;
    delete[] str;
    return r;
}

std::string curses$mvwgetnstr(void *win, Number y, Number x, Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    mvwgetnstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), str, width);
    std::string r = str;
    delete[] str;
    return r;
}

Number curses$getparx(void *win)
{
    return number_from_sint32(getparx(static_cast<WINDOW *>(win)));
}

Number curses$getpary(void *win)
{
    return number_from_sint32(getpary(static_cast<WINDOW *>(win)));
}

std::vector<Number> curses$getparyx(void *win)
{
    int y = -1, x = -1;
    getparyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

std::vector<Number> curses$getsyx()
{
    int y = -1, x = -1;
    getsyx(y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

std::vector<Number> curses$getyx(void *win)
{
    int y = -1, x = -1;
    getyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

void curses$halfdelay(Number tenths)
{
    halfdelay(number_to_sint32(tenths));
}

bool curses$has_colors()
{
    return has_colors() != FALSE;
}

bool curses$has_ic()
{
    return has_ic() != FALSE;
}

bool curses$has_il()
{
    return has_il() != FALSE;
}

bool curses$has_key(Number ch)
{
    return has_key(number_to_sint32(ch)) != FALSE;
}

void curses$hline(Number ch, Number n)
{
    hline(number_to_uint32(ch), number_to_sint32(n));
}

void curses$whline(void *win, Number ch, Number n)
{
    whline(static_cast<WINDOW *>(win), number_to_uint32(ch), number_to_sint32(n));
}

void curses$mvhline(Number y, Number x, Number ch, Number n)
{
    mvhline(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void curses$mvwhline(void *win, Number y, Number x, Number ch, Number n)
{
    mvwhline(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void curses$idcok(void *win, bool bf)
{
    idcok(static_cast<WINDOW *>(win), bf);
}

void curses$idlok(void *win, bool bf)
{
    idlok(static_cast<WINDOW *>(win), bf);
}

void curses$immedok(void *win, bool bf)
{
    immedok(static_cast<WINDOW *>(win), bf);
}

Number curses$inch()
{
    return number_from_uint32(inch());
}

Number curses$winch(void *win)
{
    return number_from_uint32(winch(static_cast<WINDOW *>(win)));
}

Number curses$mvinch(Number y, Number x)
{
    return number_from_uint32(mvinch(number_to_sint32(y), number_to_sint32(x)));
}

Number curses$mvwinch(void *win, Number y, Number x)
{
    return number_from_uint32(mvwinch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x)));
}

void curses$init_color(Number pair, Number r, Number g, Number b)
{
    init_color(static_cast<short>(number_to_uint32(pair)), static_cast<short>(number_to_uint32(r)), static_cast<short>(number_to_uint32(g)), static_cast<short>(number_to_uint32(b)));
}

void curses$init_pair(Number pair, Number f, Number b)
{
    init_pair(static_cast<short>(number_to_uint32(pair)), static_cast<short>(number_to_uint32(f)), static_cast<short>(number_to_uint32(b)));
}

void curses$initscr()
{
    initscr();
}

void curses$intrflush(void *win, bool bf)
{
    intrflush(static_cast<WINDOW *>(win), bf);
}

void curses$insch(Number ch)
{
    insch(number_to_uint32(ch));
}

void curses$winsch(void *win, Number ch)
{
    winsch(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void curses$mvinsch(Number y, Number x, Number ch)
{
    mvinsch(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void curses$mvwinsch(void *win, Number y, Number x, Number ch)
{
    mvwinsch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void curses$insdelln(Number n)
{
    insdelln(number_to_sint32(n));
}

void curses$winsdelln(void *win, Number n)
{
    winsdelln(static_cast<WINDOW *>(win), number_to_sint32(n));
}

void curses$insertln()
{
    insertln();
}

void curses$winsertln(void *win)
{
    winsertln(static_cast<WINDOW *>(win));
}

void curses$insstr(const std::string &str)
{
    insstr(str.c_str());
}

void curses$winsstr(void *win, const std::string &str)
{
    winsstr(static_cast<WINDOW *>(win), str.c_str());
}

void curses$mvinsstr(Number y, Number x, const std::string &str)
{
    mvinsstr(number_to_sint32(y), number_to_sint32(x), str.c_str());
}

void curses$mvwinsstr(void *win, Number y, Number x, const std::string &str)
{
    mvwinsstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), str.c_str());
}

bool curses$is_linetouched(void *win, Number line)
{
    return is_linetouched(static_cast<WINDOW *>(win), number_to_sint32(line)) != FALSE;
}

bool curses$is_wintouched(void *win)
{
    return is_wintouched(static_cast<WINDOW *>(win)) != FALSE;
}

bool curses$isendwin()
{
    return isendwin() != FALSE;
}

std::string curses$keyname(Number c)
{
    return keyname(number_to_sint32(c));
}

void curses$keypad(void *win, bool bf)
{
    keypad(static_cast<WINDOW *>(win), bf);
}

std::string curses$killchar()
{
    return std::string(1, killchar());
}

void curses$leaveok(void *win, bool bf)
{
    leaveok(static_cast<WINDOW *>(win), bf);
}

std::string curses$longname()
{
    return longname();
}

void curses$meta(void *win, bool bf)
{
    meta(static_cast<WINDOW *>(win), bf);
}

void curses$move(Number y, Number x)
{
    move(number_to_sint32(y), number_to_sint32(x));
}

void curses$wmove(void *win, Number y, Number x)
{
    wmove(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x));
}

void curses$mvderwin(void *win, Number par_y, Number par_x)
{
    mvderwin(static_cast<WINDOW *>(win), number_to_sint32(par_y), number_to_sint32(par_x));
}

void curses$mvwin(void *win, Number y, Number x)
{
    mvwin(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x));
}

void curses$napms(Number ms)
{
    napms(number_to_sint32(ms));
}

void *curses$newpad(Number nlines, Number ncols)
{
    return newpad(number_to_sint32(nlines), number_to_sint32(ncols));
}

void *curses$newwin(Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return newwin(number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void curses$nl()
{
    nl();
}

void curses$nonl()
{
    nonl();
}

void curses$nodelay(void *win, bool bf)
{
    nodelay(static_cast<WINDOW *>(win), bf);
}

void curses$noqiflush()
{
    noqiflush();
}

void curses$notimeout(void *win, bool bf)
{
    notimeout(static_cast<WINDOW *>(win), bf);
}

void curses$overlay(void *srcwin, void *dstwin)
{
    overlay(static_cast<WINDOW *>(srcwin), static_cast<WINDOW *>(dstwin));
}

void curses$overwrite(void *srcwin, void *dstwin)
{
    overwrite(static_cast<WINDOW *>(srcwin), static_cast<WINDOW *>(dstwin));
}

std::vector<Number> curses$pair_content(Number color)
{
    short f = 0, b = 0;
    pair_content(static_cast<short>(number_to_uint32(color)), &f, &b);
    std::vector<Number> r;
    r.push_back(number_from_sint32(f));
    r.push_back(number_from_sint32(b));
    return r;
}

void curses$pechochar(void *pad, Number ch)
{
    pechochar(static_cast<WINDOW *>(pad), number_to_uint32(ch));
}

void curses$pnoutrefresh(void *pad, Number pminrow, Number pmincol, Number sminrow, Number smincol, Number smaxrow, Number smaxcol)
{
    pnoutrefresh(static_cast<WINDOW *>(pad),
                 number_to_sint32(pminrow),
                 number_to_sint32(pmincol),
                 number_to_sint32(sminrow),
                 number_to_sint32(smincol),
                 number_to_sint32(smaxrow),
                 number_to_sint32(smaxcol));
}

void curses$prefresh(void *pad, Number pminrow, Number pmincol, Number sminrow, Number smincol, Number smaxrow, Number smaxcol)
{
    prefresh(static_cast<WINDOW *>(pad),
             number_to_sint32(pminrow),
             number_to_sint32(pmincol),
             number_to_sint32(sminrow),
             number_to_sint32(smincol),
             number_to_sint32(smaxrow),
             number_to_sint32(smaxcol));
}

void curses$qiflush()
{
    qiflush();
}

void curses$raw()
{
    raw();
}

void curses$noraw()
{
    noraw();
}

void curses$redrawwin(void *win)
{
    redrawwin(static_cast<WINDOW *>(win));
}

void curses$refresh()
{
    refresh();
}

void curses$wnoutrefresh(void *win)
{
    wnoutrefresh(static_cast<WINDOW *>(win));
}

void curses$wrefresh(void *win)
{
    wrefresh(static_cast<WINDOW *>(win));
}

void curses$reset_prog_mode()
{
    reset_prog_mode();
}

void curses$reset_shell_mode()
{
    reset_shell_mode();
}

void curses$resetty()
{
    resetty();
}

void curses$savetty()
{
    savetty();
}

void curses$scr_dump(const std::string &filename)
{
    scr_dump(filename.c_str());
}

void curses$scr_init(const std::string &filename)
{
    scr_init(filename.c_str());
}

void curses$scr_restore(const std::string &filename)
{
    scr_restore(filename.c_str());
}

void curses$scr_set(const std::string &filename)
{
    scr_set(filename.c_str());
}

void curses$scroll(void *win)
{
    scroll(static_cast<WINDOW *>(win));
}

void curses$scrollok(void *win, bool bf)
{
    scrollok(static_cast<WINDOW *>(win), bf);
}

void curses$scrl(Number n)
{
    scrl(number_to_sint32(n));
}

void curses$wscrl(void *win, Number n)
{
    wscrl(static_cast<WINDOW *>(win), number_to_sint32(n));
}

void curses$setscrreg(Number top, Number bottom)
{
    setscrreg(number_to_sint32(top), number_to_sint32(bottom));
}

void curses$wsetscrreg(void *win, Number top, Number bottom)
{
    wsetscrreg(static_cast<WINDOW *>(win), number_to_sint32(top), number_to_sint32(bottom));
}

void curses$setsyx(Number y, Number x)
{
    setsyx(number_to_sint32(y), number_to_sint32(x));
}

void curses$standend()
{
    standend();
}

void curses$wstandend(void *win)
{
    wstandend(static_cast<WINDOW *>(win));
}

void curses$standout()
{
    standout();
}

void curses$wstandout(void *win)
{
    wstandout(static_cast<WINDOW *>(win));
}

void curses$start_color()
{
    start_color();
}

void *curses$stdscr()
{
    return stdscr;
}

void *curses$subpad(void *orig, Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return subpad(static_cast<WINDOW *>(orig), number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void *curses$subwin(void *orig, Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return subwin(static_cast<WINDOW *>(orig), number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void curses$syncok(void *win, bool bf)
{
    syncok(static_cast<WINDOW *>(win), bf);
}

Number curses$termattrs()
{
    return number_from_uint32(termattrs());
}

std::string curses$termname()
{
    return termname();
}

void curses$timeout(Number delay)
{
    timeout(number_to_sint32(delay));
}

void curses$wtimeout(void *win, Number delay)
{
    wtimeout(static_cast<WINDOW *>(win), number_to_sint32(delay));
}

void curses$touchline(void *win, Number start, Number count)
{
    touchline(static_cast<WINDOW *>(win), number_to_sint32(start), number_to_sint32(count));
}

void curses$touchwin(void *win)
{
    touchwin(static_cast<WINDOW *>(win));
}

void curses$untouchwin(void *win)
{
    untouchwin(static_cast<WINDOW *>(win));
}

std::string curses$unctrl(Number c)
{
    return unctrl(number_to_uint32(c));
}

void curses$ungetch(Number ch)
{
    ungetch(number_to_sint32(ch));
}

void curses$use_env(bool f)
{
    use_env(f);
}

void curses$vline(Number ch, Number n)
{
    vline(number_to_uint32(ch), number_to_sint32(n));
}

void curses$wvline(void *win, Number ch, Number n)
{
    wvline(static_cast<WINDOW *>(win), number_to_uint32(ch), number_to_sint32(n));
}

void curses$mvvline(Number y, Number x, Number ch, Number n)
{
    mvvline(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void curses$mvwvline(void *win, Number y, Number x, Number ch, Number n)
{
    mvwvline(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void curses$wcursyncup(void *win)
{
    wcursyncup(static_cast<WINDOW *>(win));
}

void curses$wredrawln(void *win, Number beg_line, Number num_lines)
{
    wredrawln(static_cast<WINDOW *>(win), number_to_sint32(beg_line), number_to_sint32(num_lines));
}

void curses$wsyncdown(void *win)
{
    wsyncdown(static_cast<WINDOW *>(win));
}

void curses$wsyncup(void *win)
{
    wsyncup(static_cast<WINDOW *>(win));
}

void curses$wtouchln(void *win, Number y, Number n, bool changed)
{
    wtouchln(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(n), changed);
}

} // namespace rtl
