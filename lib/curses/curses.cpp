#include <string>
#include <vector>

#include <curses.h>

#include "neonext.h"

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

const Ne_MethodTable *Ne;

static std::vector<chtype> chtypes_from_ints(const std::vector<int> &chstr)
{
    std::vector<chtype> r;
    for (auto c: chstr) {
        r.push_back(c);
    }
    return r;
}

extern "C" {

Ne_EXPORT int Ne_INIT(const struct Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_CONST_INT(Ne_CURSES_ERR, ERR)
Ne_CONST_INT(Ne_CURSES_OK, OK)

Ne_CONST_INT(Ne_A_CHARTEXT, A_CHARTEXT)
Ne_CONST_INT(Ne_A_NORMAL, A_NORMAL)
Ne_CONST_INT(Ne_A_ALTCHARSET, A_ALTCHARSET)
Ne_CONST_INT(Ne_A_INVIS, A_INVIS)
Ne_CONST_INT(Ne_A_UNDERLINE, A_UNDERLINE)
Ne_CONST_INT(Ne_A_REVERSE, A_REVERSE)
Ne_CONST_INT(Ne_A_BLINK, A_BLINK)
Ne_CONST_INT(Ne_A_BOLD, A_BOLD)
Ne_CONST_INT(Ne_A_ATTRIBUTES, A_ATTRIBUTES)
Ne_CONST_INT(Ne_A_COLOR, A_COLOR)
Ne_CONST_INT(Ne_A_PROTECT, A_PROTECT)

Ne_CONST_INT(Ne_COLOR_BLACK, COLOR_BLACK)
Ne_CONST_INT(Ne_COLOR_RED, COLOR_RED)
Ne_CONST_INT(Ne_COLOR_GREEN, COLOR_GREEN)
Ne_CONST_INT(Ne_COLOR_BLUE, COLOR_BLUE)
Ne_CONST_INT(Ne_COLOR_CYAN, COLOR_CYAN)
Ne_CONST_INT(Ne_COLOR_MAGENTA, COLOR_MAGENTA)
Ne_CONST_INT(Ne_COLOR_YELLOW, COLOR_YELLOW)
Ne_CONST_INT(Ne_COLOR_WHITE, COLOR_WHITE)

Ne_CONST_INT(Ne_KEY_CODE_YES, KEY_CODE_YES)

Ne_CONST_INT(Ne_KEY_BREAK, KEY_BREAK)
Ne_CONST_INT(Ne_KEY_DOWN, KEY_DOWN)
Ne_CONST_INT(Ne_KEY_UP, KEY_UP)
Ne_CONST_INT(Ne_KEY_LEFT, KEY_LEFT)
Ne_CONST_INT(Ne_KEY_RIGHT, KEY_RIGHT)
Ne_CONST_INT(Ne_KEY_HOME, KEY_HOME)
Ne_CONST_INT(Ne_KEY_BACKSPACE, KEY_BACKSPACE)

Ne_CONST_INT(Ne_KEY_F0, KEY_F0)
Ne_CONST_INT(Ne_KEY_DL, KEY_DL)
Ne_CONST_INT(Ne_KEY_IL, KEY_IL)
Ne_CONST_INT(Ne_KEY_DC, KEY_DC)
Ne_CONST_INT(Ne_KEY_IC, KEY_IC)
Ne_CONST_INT(Ne_KEY_EIC, KEY_EIC)
Ne_CONST_INT(Ne_KEY_CLEAR, KEY_CLEAR)
Ne_CONST_INT(Ne_KEY_EOS, KEY_EOS)
Ne_CONST_INT(Ne_KEY_EOL, KEY_EOL)
Ne_CONST_INT(Ne_KEY_SF, KEY_SF)
Ne_CONST_INT(Ne_KEY_SR, KEY_SR)
Ne_CONST_INT(Ne_KEY_NPAGE, KEY_NPAGE)
Ne_CONST_INT(Ne_KEY_PPAGE, KEY_PPAGE)
Ne_CONST_INT(Ne_KEY_STAB, KEY_STAB)
Ne_CONST_INT(Ne_KEY_CTAB, KEY_CTAB)
Ne_CONST_INT(Ne_KEY_CATAB, KEY_CATAB)
Ne_CONST_INT(Ne_KEY_ENTER, KEY_ENTER)
Ne_CONST_INT(Ne_KEY_SRESET, KEY_SRESET)
Ne_CONST_INT(Ne_KEY_RESET, KEY_RESET)
Ne_CONST_INT(Ne_KEY_PRINT, KEY_PRINT)
Ne_CONST_INT(Ne_KEY_LL, KEY_LL)
Ne_CONST_INT(Ne_KEY_SHELP, KEY_SHELP)
Ne_CONST_INT(Ne_KEY_BTAB, KEY_BTAB)
Ne_CONST_INT(Ne_KEY_BEG, KEY_BEG)
Ne_CONST_INT(Ne_KEY_CANCEL, KEY_CANCEL)
Ne_CONST_INT(Ne_KEY_CLOSE, KEY_CLOSE)
Ne_CONST_INT(Ne_KEY_COMMAND, KEY_COMMAND)
Ne_CONST_INT(Ne_KEY_COPY, KEY_COPY)
Ne_CONST_INT(Ne_KEY_CREATE, KEY_CREATE)
Ne_CONST_INT(Ne_KEY_END, KEY_END)
Ne_CONST_INT(Ne_KEY_EXIT, KEY_EXIT)
Ne_CONST_INT(Ne_KEY_FIND, KEY_FIND)
Ne_CONST_INT(Ne_KEY_HELP, KEY_HELP)
Ne_CONST_INT(Ne_KEY_MARK, KEY_MARK)
Ne_CONST_INT(Ne_KEY_MESSAGE, KEY_MESSAGE)
Ne_CONST_INT(Ne_KEY_MOVE, KEY_MOVE)
Ne_CONST_INT(Ne_KEY_NEXT, KEY_NEXT)
Ne_CONST_INT(Ne_KEY_OPEN, KEY_OPEN)
Ne_CONST_INT(Ne_KEY_OPTIONS, KEY_OPTIONS)
Ne_CONST_INT(Ne_KEY_PREVIOUS, KEY_PREVIOUS)
Ne_CONST_INT(Ne_KEY_REDO, KEY_REDO)
Ne_CONST_INT(Ne_KEY_REFERENCE, KEY_REFERENCE)
Ne_CONST_INT(Ne_KEY_REFRESH, KEY_REFRESH)
Ne_CONST_INT(Ne_KEY_REPLACE, KEY_REPLACE)
Ne_CONST_INT(Ne_KEY_RESTART, KEY_RESTART)
Ne_CONST_INT(Ne_KEY_RESUME, KEY_RESUME)
Ne_CONST_INT(Ne_KEY_SAVE, KEY_SAVE)

Ne_CONST_INT(Ne_KEY_SBEG, KEY_SBEG)
Ne_CONST_INT(Ne_KEY_SCANCEL, KEY_SCANCEL)
Ne_CONST_INT(Ne_KEY_SCOMMAND, KEY_SCOMMAND)
Ne_CONST_INT(Ne_KEY_SCOPY, KEY_SCOPY)
Ne_CONST_INT(Ne_KEY_SCREATE, KEY_SCREATE)
Ne_CONST_INT(Ne_KEY_SDC, KEY_SDC)
Ne_CONST_INT(Ne_KEY_SDL, KEY_SDL)
Ne_CONST_INT(Ne_KEY_SELECT, KEY_SELECT)
Ne_CONST_INT(Ne_KEY_SEND, KEY_SEND)
Ne_CONST_INT(Ne_KEY_SEOL, KEY_SEOL)
Ne_CONST_INT(Ne_KEY_SEXIT, KEY_SEXIT)
Ne_CONST_INT(Ne_KEY_SFIND, KEY_SFIND)
Ne_CONST_INT(Ne_KEY_SHOME, KEY_SHOME)
Ne_CONST_INT(Ne_KEY_SIC, KEY_SIC)
Ne_CONST_INT(Ne_KEY_SLEFT, KEY_SLEFT)
Ne_CONST_INT(Ne_KEY_SMESSAGE, KEY_SMESSAGE)
Ne_CONST_INT(Ne_KEY_SMOVE, KEY_SMOVE)
Ne_CONST_INT(Ne_KEY_SNEXT, KEY_SNEXT)
Ne_CONST_INT(Ne_KEY_SOPTIONS, KEY_SOPTIONS)
Ne_CONST_INT(Ne_KEY_SPREVIOUS, KEY_SPREVIOUS)
Ne_CONST_INT(Ne_KEY_SPRINT, KEY_SPRINT)
Ne_CONST_INT(Ne_KEY_SREDO, KEY_SREDO)
Ne_CONST_INT(Ne_KEY_SREPLACE, KEY_SREPLACE)
Ne_CONST_INT(Ne_KEY_SRIGHT, KEY_SRIGHT)
Ne_CONST_INT(Ne_KEY_SRSUME, KEY_SRSUME)
Ne_CONST_INT(Ne_KEY_SSAVE, KEY_SSAVE)
Ne_CONST_INT(Ne_KEY_SSUSPEND, KEY_SSUSPEND)
Ne_CONST_INT(Ne_KEY_SUNDO, KEY_SUNDO)
Ne_CONST_INT(Ne_KEY_SUSPEND, KEY_SUSPEND)
Ne_CONST_INT(Ne_KEY_UNDO, KEY_UNDO)

Ne_CONST_INT(Ne_KEY_MIN, KEY_MIN)
Ne_CONST_INT(Ne_KEY_MAX, KEY_MAX)

Ne_FUNC(Ne_ACS_ULCORNER) { Ne_RETURN_INT(ACS_ULCORNER); }
Ne_FUNC(Ne_ACS_LLCORNER) { Ne_RETURN_INT(ACS_LLCORNER); }
Ne_FUNC(Ne_ACS_URCORNER) { Ne_RETURN_INT(ACS_URCORNER); }
Ne_FUNC(Ne_ACS_LRCORNER) { Ne_RETURN_INT(ACS_LRCORNER); }
Ne_FUNC(Ne_ACS_RTEE) { Ne_RETURN_INT(ACS_RTEE); }
Ne_FUNC(Ne_ACS_LTEE) { Ne_RETURN_INT(ACS_LTEE); }
Ne_FUNC(Ne_ACS_BTEE) { Ne_RETURN_INT(ACS_BTEE); }
Ne_FUNC(Ne_ACS_TTEE) { Ne_RETURN_INT(ACS_TTEE); }
Ne_FUNC(Ne_ACS_HLINE) { Ne_RETURN_INT(ACS_HLINE); }
Ne_FUNC(Ne_ACS_VLINE) { Ne_RETURN_INT(ACS_VLINE); }
Ne_FUNC(Ne_ACS_PLUS) { Ne_RETURN_INT(ACS_PLUS); }

Ne_FUNC(Ne_COLOR_PAIR)
{
    int n = Ne_PARAM_INT(0);
    Ne_RETURN_INT(COLOR_PAIR(n));
}

Ne_FUNC(Ne_Cols)
{
    Ne_RETURN_INT(COLS);
}

Ne_FUNC(Ne_KEY_F)
{
    int n = Ne_PARAM_INT(0);
    Ne_RETURN_INT(KEY_F(n));
}

Ne_FUNC(Ne_Lines)
{
    Ne_RETURN_INT(LINES);
}

Ne_FUNC(Ne_PAIR_NUMBER)
{
    int n = Ne_PARAM_INT(0);
    Ne_RETURN_INT(PAIR_NUMBER(n));
}

Ne_FUNC(Ne_addch)
{
    int ch = Ne_PARAM_INT(0);
    addch(ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_waddch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    waddch(win, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvaddch)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    int ch = Ne_PARAM_INT(2);
    mvaddch(y, x, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwaddch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    int ch = Ne_PARAM_INT(3);
    mvwaddch(win, y, x, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_addchstr)
{
    std::vector<int> chstr = Ne_PARAM_ARRAY_INT(in_params, 0);
    addchstr(chtypes_from_ints(chstr).data());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_waddchstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    std::vector<int> chstr = Ne_PARAM_ARRAY_INT(in_params, 1);
    waddchstr(win, chtypes_from_ints(chstr).data());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvaddchstr)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    std::vector<int> chstr = Ne_PARAM_ARRAY_INT(in_params, 2);
    mvaddchstr(y, x, chtypes_from_ints(chstr).data());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwaddchstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    std::vector<int> chstr = Ne_PARAM_ARRAY_INT(in_params, 3);
    mvwaddchstr(win, y, x, chtypes_from_ints(chstr).data());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_addstr)
{
    std::string str = Ne_PARAM_STRING(0);
    addstr(str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_waddstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    std::string str = Ne_PARAM_STRING(1);
    waddstr(win, str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvaddstr)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    std::string str = Ne_PARAM_STRING(2);
    mvaddstr(y, x, str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwaddstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    std::string str = Ne_PARAM_STRING(3);
    mvwaddstr(win, y, x, str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_attroff)
{
    int attrs = Ne_PARAM_INT(0);
    attroff(attrs);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wattroff)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int attrs = Ne_PARAM_INT(1);
    wattroff(win, attrs);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_attron)
{
    int attrs = Ne_PARAM_INT(0);
    attron(attrs);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wattron)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int attrs = Ne_PARAM_INT(1);
    wattron(win, attrs);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_attrset)
{
    int attrs = Ne_PARAM_INT(0);
    attrset(attrs);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wattrset)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int attrs = Ne_PARAM_INT(1);
    wattrset(win, attrs);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_baudrate)
{
    Ne_RETURN_INT(baudrate());
}

Ne_FUNC(Ne_beep)
{
    beep();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_bkgd)
{
    int ch = Ne_PARAM_INT(0);
    bkgd(ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wbkgd)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    wbkgd(win, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_bkgdset)
{
    int ch = Ne_PARAM_INT(0);
    bkgdset(ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wbkgdset)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    wbkgdset(win, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_border)
{
    int ls = Ne_PARAM_INT(0);
    int rs = Ne_PARAM_INT(1);
    int ts = Ne_PARAM_INT(2);
    int bs = Ne_PARAM_INT(3);
    int tl = Ne_PARAM_INT(4);
    int tr = Ne_PARAM_INT(5);
    int bl = Ne_PARAM_INT(6);
    int br = Ne_PARAM_INT(7);
    border(ls, rs, ts, bs, tl, tr, bl, br);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wborder)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ls = Ne_PARAM_INT(1);
    int rs = Ne_PARAM_INT(2);
    int ts = Ne_PARAM_INT(3);
    int bs = Ne_PARAM_INT(4);
    int tl = Ne_PARAM_INT(5);
    int tr = Ne_PARAM_INT(6);
    int bl = Ne_PARAM_INT(7);
    int br = Ne_PARAM_INT(8);
    wborder(win, ls, rs, ts, bs, tl, tr, bl, br);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_box)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int verch = Ne_PARAM_INT(1);
    int horch = Ne_PARAM_INT(2);
    box(win, verch, horch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_can_change_color)
{
    Ne_RETURN_BOOL(can_change_color());
}

Ne_FUNC(Ne_cbreak)
{
    cbreak();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_nocbreak)
{
    nocbreak();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_chgat)
{
    int n = Ne_PARAM_INT(0);
    int attr = Ne_PARAM_INT(1);
    int color = Ne_PARAM_INT(2);
    void *opts = Ne_PARAM_POINTER(void, 3);
    chgat(n, attr, static_cast<short>(color), opts);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wchgat)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int n = Ne_PARAM_INT(1);
    int attr = Ne_PARAM_INT(2);
    int color = Ne_PARAM_INT(3);
    void *opts = Ne_PARAM_POINTER(void, 4);
    wchgat(win, n, attr, static_cast<short>(color), opts);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvchgat)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    int n = Ne_PARAM_INT(2);
    int attr = Ne_PARAM_INT(3);
    int color = Ne_PARAM_INT(4);
    void *opts = Ne_PARAM_POINTER(void, 5);
    mvchgat(y, x, n, attr, static_cast<short>(color), opts);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwchgat)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    int n = Ne_PARAM_INT(3);
    int attr = Ne_PARAM_INT(4);
    int color = Ne_PARAM_INT(5);
    void *opts = Ne_PARAM_POINTER(void, 6);
    mvwchgat(win, y, x, n, attr, static_cast<short>(color), opts);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_clear)
{
    clear();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wclear)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wclear(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_clearok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    clearok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_clrtobot)
{
    clrtobot();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wclrtobot)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wclrtobot(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_clrtoeol)
{
    clrtoeol();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wclrtoeol)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wclrtoeol(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_color_content)
{
    int color = Ne_PARAM_INT(0);
    short r = 0, g = 0, b = 0;
    color_content(static_cast<short>(color), &r, &g, &b);
    std::vector<int> a {r, g, b};
    Ne_RETURN_ARRAY_INT(a);
}

Ne_FUNC(Ne_color_set)
{
    int color_pair_number = Ne_PARAM_INT(0);
    void *opts = Ne_PARAM_POINTER(void, 1);
    color_set(static_cast<short>(color_pair_number), opts);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wcolor_set)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int color_pair_number = Ne_PARAM_INT(1);
    void *opts = Ne_PARAM_POINTER(void, 2);
    wcolor_set(win, static_cast<short>(color_pair_number), opts);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_copywin)
{
    WINDOW *srcwin = Ne_PARAM_POINTER(WINDOW, 0);
    WINDOW *dstwin = Ne_PARAM_POINTER(WINDOW, 1);
    int sminrow = Ne_PARAM_INT(2);
    int smincol = Ne_PARAM_INT(3);
    int dminrow = Ne_PARAM_INT(4);
    int dmincol = Ne_PARAM_INT(5);
    int dmaxrow = Ne_PARAM_INT(6);
    int dmaxcol = Ne_PARAM_INT(7);
    int overlay = Ne_PARAM_BOOL(8);
    copywin(srcwin, dstwin, sminrow, smincol, dminrow, dmincol, dmaxrow, dmaxcol, overlay);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_curs_set)
{
    int visibility = Ne_PARAM_INT(0);
    curs_set(visibility);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_curscr)
{
    Ne_RETURN_POINTER(curscr);
}

Ne_FUNC(Ne_curses_version)
{
    Ne_RETURN_STRING(curses_version());
}

Ne_FUNC(Ne_def_prog_mode)
{
    def_prog_mode();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_def_shell_mode)
{
    def_shell_mode();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_delay_output)
{
    int ms = Ne_PARAM_INT(0);
    delay_output(ms);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_delch)
{
    delch();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wdelch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wdelch(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvdelch)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    mvdelch(y, x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwdelch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    mvwdelch(win, y, x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_deleteln)
{
    deleteln();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wdeleteln)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wdeleteln(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_delwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    delwin(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_derwin)
{
    WINDOW *orig = Ne_PARAM_POINTER(WINDOW, 0);
    int nlines = Ne_PARAM_INT(1);
    int ncols = Ne_PARAM_INT(2);
    int begin_y = Ne_PARAM_INT(3);
    int begin_x = Ne_PARAM_INT(4);
    Ne_RETURN_POINTER(derwin(orig, nlines, ncols, begin_y, begin_x));
}

Ne_FUNC(Ne_doupdate)
{
    doupdate();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_dupwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_POINTER(dupwin(win));
}

Ne_FUNC(Ne_echo)
{
    echo();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_noecho)
{
    noecho();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_echochar)
{
    int ch = Ne_PARAM_INT(0);
    echochar(ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wechochar)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    wechochar(win, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_endwin)
{
    endwin();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_erase)
{
    erase();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_werase)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    werase(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_erasechar)
{
    Ne_RETURN_STRING(std::string(1, erasechar()).c_str());
}

Ne_FUNC(Ne_filter)
{
    filter();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_flash)
{
    flash();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_flushinp)
{
    flushinp();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_getbegx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getbegx(win));
}

Ne_FUNC(Ne_getbegy)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getbegy(win));
}

Ne_FUNC(Ne_getbegyx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = -1, x = -1;
    getbegyx(win, y, x);
    std::vector<int> r {y, x};
    Ne_RETURN_ARRAY_INT(r);
}

Ne_FUNC(Ne_getbkgd)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getbkgd(win));
}

Ne_FUNC(Ne_getch)
{
    Ne_RETURN_INT(getch());
}

Ne_FUNC(Ne_wgetch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(wgetch(win));
}

Ne_FUNC(Ne_mvgetch)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    Ne_RETURN_INT(mvgetch(y, x));
}

Ne_FUNC(Ne_mvwgetch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    Ne_RETURN_INT(mvwgetch(win, y, x));
}

Ne_FUNC(Ne_getcurx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getcurx(win));
}

Ne_FUNC(Ne_getcury)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getcury(win));
}

Ne_FUNC(Ne_getmaxx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getmaxx(win));
}

Ne_FUNC(Ne_getmaxy)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getmaxy(win));
}

Ne_FUNC(Ne_getmaxyx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = -1, x = -1;
    getmaxyx(win, y, x);
    std::vector<int> r {y, x};
    Ne_RETURN_ARRAY_INT(r);
}

Ne_FUNC(Ne_getnstr)
{
    int width = Ne_PARAM_INT(0);
    char *str = new char[width+1];
    getnstr(str, width);
    std::string r = str;
    delete[] str;
    Ne_RETURN_STRING(r.c_str());
}

Ne_FUNC(Ne_wgetnstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int width = Ne_PARAM_INT(1);
    char *str = new char[width+1];
    wgetnstr(win, str, width);
    std::string r = str;
    delete[] str;
    Ne_RETURN_STRING(r.c_str());
}

Ne_FUNC(Ne_mvgetnstr)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    int width = Ne_PARAM_INT(2);
    char *str = new char[width+1];
    mvgetnstr(y, x, str, width);
    std::string r = str;
    delete[] str;
    Ne_RETURN_STRING(r.c_str());
}

Ne_FUNC(Ne_mvwgetnstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    int width = Ne_PARAM_INT(3);
    char *str = new char[width+1];
    mvwgetnstr(win, y, x, str, width);
    std::string r = str;
    delete[] str;
    Ne_RETURN_STRING(r.c_str());
}

Ne_FUNC(Ne_getparx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getparx(win));
}

Ne_FUNC(Ne_getpary)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(getpary(win));
}

Ne_FUNC(Ne_getparyx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = -1, x = -1;
    getparyx(win, y, x);
    std::vector<int> r {y, x};
    Ne_RETURN_ARRAY_INT(r);
}

Ne_FUNC(Ne_getsyx)
{
    int y = -1, x = -1;
    getsyx(y, x);
    std::vector<int> r {y, x};
    Ne_RETURN_ARRAY_INT(r);
}

Ne_FUNC(Ne_getyx)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = -1, x = -1;
    getyx(win, y, x);
    std::vector<int> r {y, x};
    Ne_RETURN_ARRAY_INT(r);
}

Ne_FUNC(Ne_halfdelay)
{
    int tenths = Ne_PARAM_INT(0);
    halfdelay(tenths);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_has_colors)
{
    Ne_RETURN_BOOL(has_colors());
}

Ne_FUNC(Ne_has_ic)
{
    Ne_RETURN_BOOL(has_ic());
}

Ne_FUNC(Ne_has_il)
{
    Ne_RETURN_BOOL(has_il());
}

Ne_FUNC(Ne_has_key)
{
    int ch = Ne_PARAM_INT(0);
    Ne_RETURN_BOOL(has_key(ch));
}

Ne_FUNC(Ne_hline)
{
    int ch = Ne_PARAM_INT(0);
    int n = Ne_PARAM_INT(1);
    hline(ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_whline)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    int n = Ne_PARAM_INT(2);
    whline(win, ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvhline)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    int ch = Ne_PARAM_INT(2);
    int n = Ne_PARAM_INT(3);
    mvhline(y, x, ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwhline)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    int ch = Ne_PARAM_INT(3);
    int n = Ne_PARAM_INT(4);
    mvwhline(win, y, x, ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_idcok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    idcok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_idlok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    idlok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_immedok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    immedok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_inch)
{
    Ne_RETURN_INT(inch());
}

Ne_FUNC(Ne_winch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_INT(winch(win));
}

Ne_FUNC(Ne_mvinch)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    Ne_RETURN_INT(mvinch(y, x));
}

Ne_FUNC(Ne_mvwinch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    Ne_RETURN_INT(mvwinch(win, y, x));
}

Ne_FUNC(Ne_init_color)
{
    int pair = Ne_PARAM_INT(0);
    int r = Ne_PARAM_INT(1);
    int g = Ne_PARAM_INT(2);
    int b = Ne_PARAM_INT(3);
    init_color(static_cast<short>(pair), static_cast<short>(r), static_cast<short>(g), static_cast<short>(b));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_init_pair)
{
    int pair = Ne_PARAM_INT(0);
    int f = Ne_PARAM_INT(1);
    int b = Ne_PARAM_INT(2);
    init_pair(static_cast<short>(pair), static_cast<short>(f), static_cast<short>(b));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_initscr)
{
    initscr();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_intrflush)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    intrflush(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_insch)
{
    int ch = Ne_PARAM_INT(0);
    insch(ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_winsch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    winsch(win, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvinsch)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    int ch = Ne_PARAM_INT(2);
    mvinsch(y, x, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwinsch)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    int ch = Ne_PARAM_INT(3);
    mvwinsch(win, y, x, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_insdelln)
{
    int n = Ne_PARAM_INT(0);
    insdelln(n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_winsdelln)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int n = Ne_PARAM_INT(1);
    winsdelln(win, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_insertln)
{
    insertln();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_winsertln)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    winsertln(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_insstr)
{
    std::string str = Ne_PARAM_STRING(0);
    insstr(str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_winsstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    std::string str = Ne_PARAM_STRING(1);
    winsstr(win, str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvinsstr)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    std::string str = Ne_PARAM_STRING(2);
    mvinsstr(y, x, str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwinsstr)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    std::string str = Ne_PARAM_STRING(3);
    mvwinsstr(win, y, x, str.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_is_linetouched)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int line = Ne_PARAM_INT(1);
    Ne_RETURN_BOOL(is_linetouched(win, line));
}

Ne_FUNC(Ne_is_wintouched)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    Ne_RETURN_BOOL(is_wintouched(win));
}

Ne_FUNC(Ne_is_endwin)
{
    Ne_RETURN_BOOL(isendwin());
}

Ne_FUNC(Ne_keyname)
{
    int c = Ne_PARAM_INT(0);
    Ne_RETURN_STRING(keyname(c));
}

Ne_FUNC(Ne_keypad)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    keypad(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_killchar)
{
    Ne_RETURN_STRING(std::string(1, killchar()).c_str());
}

Ne_FUNC(Ne_leaveok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    leaveok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_longname)
{
    Ne_RETURN_STRING(longname());
}

Ne_FUNC(Ne_meta)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    meta(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_move)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    move(y, x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wmove)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    wmove(win, y, x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvderwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int par_y = Ne_PARAM_INT(1);
    int par_x = Ne_PARAM_INT(2);
    mvderwin(win, par_y, par_x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    mvwin(win, y, x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_napms)
{
    int ms = Ne_PARAM_INT(0);
    napms(ms);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_newpad)
{
    int nlines = Ne_PARAM_INT(0);
    int ncols = Ne_PARAM_INT(1);
    Ne_RETURN_POINTER(newpad(nlines, ncols));
}

Ne_FUNC(Ne_newwin)
{
    int nlines = Ne_PARAM_INT(0);
    int ncols = Ne_PARAM_INT(1);
    int begin_y = Ne_PARAM_INT(2);
    int begin_x = Ne_PARAM_INT(3);
    Ne_RETURN_POINTER(newwin(nlines, ncols, begin_y, begin_x));
}

Ne_FUNC(Ne_nl)
{
    nl();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_nonl)
{
    nonl();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_nodelay)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    nodelay(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_noqiflush)
{
    noqiflush();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_notimeout)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    notimeout(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_overlay)
{
    WINDOW *srcwin = Ne_PARAM_POINTER(WINDOW, 0);
    WINDOW *dstwin = Ne_PARAM_POINTER(WINDOW, 1);
    overlay(srcwin, dstwin);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_overwrite)
{
    WINDOW *srcwin = Ne_PARAM_POINTER(WINDOW, 0);
    WINDOW *dstwin = Ne_PARAM_POINTER(WINDOW, 1);
    overwrite(srcwin, dstwin);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_pair_content)
{
    int color = Ne_PARAM_INT(0);
    short f = 0, b = 0;
    pair_content(static_cast<short>(color), &f, &b);
    std::vector<int> r {f, b};
    Ne_RETURN_ARRAY_INT(r);
}

Ne_FUNC(Ne_pechochar)
{
    WINDOW *pad = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    pechochar(pad, ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_pnoutrefresh)
{
    WINDOW *pad = Ne_PARAM_POINTER(WINDOW, 0);
    int pminrow = Ne_PARAM_INT(1);
    int pmincol = Ne_PARAM_INT(2);
    int sminrow = Ne_PARAM_INT(3);
    int smincol = Ne_PARAM_INT(4);
    int smaxrow = Ne_PARAM_INT(5);
    int smaxcol = Ne_PARAM_INT(6);
    pnoutrefresh(pad, pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_prefresh)
{
    WINDOW *pad = Ne_PARAM_POINTER(WINDOW, 0);
    int pminrow = Ne_PARAM_INT(1);
    int pmincol = Ne_PARAM_INT(2);
    int sminrow = Ne_PARAM_INT(3);
    int smincol = Ne_PARAM_INT(4);
    int smaxrow = Ne_PARAM_INT(5);
    int smaxcol = Ne_PARAM_INT(6);
    prefresh(pad, pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_qiflush)
{
    qiflush();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_raw)
{
    raw();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_noraw)
{
    noraw();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_redrawwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    redrawwin(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_refresh)
{
    refresh();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wnoutrefresh)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wnoutrefresh(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wrefresh)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wrefresh(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_reset_prog_mode)
{
    reset_prog_mode();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_reset_shell_mode)
{
    reset_shell_mode();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_resetty)
{
    resetty();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_savetty)
{
    savetty();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scr_dump)
{
    std::string filename = Ne_PARAM_STRING(0);
    scr_dump(filename.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scr_init)
{
    std::string filename = Ne_PARAM_STRING(0);
    scr_init(filename.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scr_restore)
{
    std::string filename = Ne_PARAM_STRING(0);
    scr_restore(filename.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scr_set)
{
    std::string filename = Ne_PARAM_STRING(0);
    scr_set(filename.c_str());
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scroll)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    scroll(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scrollok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_BOOL(1);
    scrollok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_scrl)
{
    int n = Ne_PARAM_INT(0);
    scrl(n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wscrl)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int n = Ne_PARAM_INT(1);
    wscrl(win, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_setscrreg)
{
    int top = Ne_PARAM_INT(0);
    int bottom = Ne_PARAM_INT(1);
    setscrreg(top, bottom);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wsetscrreg)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int top = Ne_PARAM_INT(1);
    int bottom = Ne_PARAM_INT(2);
    wsetscrreg(win, top, bottom);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_setsyx)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    setsyx(y, x);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_standend)
{
    standend();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wstandend)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wstandend(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_standout)
{
    standout();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wstandout)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wstandout(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_start_color)
{
    start_color();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_stdscr)
{
    Ne_RETURN_POINTER(stdscr);
}

Ne_FUNC(Ne_subpad)
{
    WINDOW *orig = Ne_PARAM_POINTER(WINDOW, 0);
    int nlines = Ne_PARAM_INT(1);
    int ncols = Ne_PARAM_INT(2);
    int begin_y = Ne_PARAM_INT(3);
    int begin_x = Ne_PARAM_INT(4);
    Ne_RETURN_POINTER(subpad(orig, nlines, ncols, begin_y, begin_x));
}

Ne_FUNC(Ne_subwin)
{
    WINDOW *orig = Ne_PARAM_POINTER(WINDOW, 0);
    int nlines = Ne_PARAM_INT(1);
    int ncols = Ne_PARAM_INT(2);
    int begin_y = Ne_PARAM_INT(3);
    int begin_x = Ne_PARAM_INT(4);
    Ne_RETURN_POINTER(subwin(orig, nlines, ncols, begin_y, begin_x));
}

Ne_FUNC(Ne_syncok)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int bf = Ne_PARAM_INT(1);
    syncok(win, bf != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_termattrs)
{
    Ne_RETURN_INT(termattrs());
}

Ne_FUNC(Ne_termname)
{
    Ne_RETURN_STRING(termname());
}

Ne_FUNC(Ne_timeout)
{
    int delay = Ne_PARAM_INT(0);
    timeout(delay);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wtimeout)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int delay = Ne_PARAM_INT(1);
    wtimeout(win, delay);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_touchline)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int start = Ne_PARAM_INT(1);
    int count = Ne_PARAM_INT(2);
    touchline(win, start, count);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_touchwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    touchwin(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_untouchwin)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    untouchwin(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_unctrl)
{
    int c = Ne_PARAM_INT(0);
    Ne_RETURN_STRING(unctrl(c));
}

Ne_FUNC(Ne_ungetch)
{
    int ch = Ne_PARAM_INT(0);
    ungetch(ch);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_use_env)
{
    int f = Ne_PARAM_BOOL(0);
    use_env(f != 0);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_vline)
{
    int ch = Ne_PARAM_INT(0);
    int n = Ne_PARAM_INT(1);
    vline(ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wvline)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int ch = Ne_PARAM_INT(1);
    int n = Ne_PARAM_INT(2);
    wvline(win, ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvvline)
{
    int y = Ne_PARAM_INT(0);
    int x = Ne_PARAM_INT(1);
    int ch = Ne_PARAM_INT(2);
    int n = Ne_PARAM_INT(3);
    mvvline(y, x, ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_mvwvline)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int x = Ne_PARAM_INT(2);
    int ch = Ne_PARAM_INT(3);
    int n = Ne_PARAM_INT(4);
    mvwvline(win, y, x, ch, n);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wcursyncup)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wcursyncup(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wredrawln)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int beg_line = Ne_PARAM_INT(1);
    int num_lines = Ne_PARAM_INT(2);
    wredrawln(win, beg_line, num_lines);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wsyncdown)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wsyncdown(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wsyncup)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    wsyncup(win);
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_wtouchln)
{
    WINDOW *win = Ne_PARAM_POINTER(WINDOW, 0);
    int y = Ne_PARAM_INT(1);
    int n = Ne_PARAM_INT(2);
    int changed = Ne_PARAM_BOOL(3);
    wtouchln(win, y, n, changed != 0);
    return Ne_SUCCESS;
}

} // extern "C"
