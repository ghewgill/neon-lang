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

namespace curses {

Number COLOR_PAIR_(Number n)
{
    return number_from_uint32(COLOR_PAIR(number_to_uint32(n)));
}

Number Cols_()
{
    return number_from_sint32(COLS);
}

Number KEY_F_(Number c)
{
    return number_from_uint32(KEY_F(number_to_uint32(c)));
}

Number Lines_()
{
    return number_from_sint32(LINES);
}

Number PAIR_NUMBER_(Number n)
{
    return number_from_uint32(PAIR_NUMBER(number_to_uint32(n)));
}

void addch_(Number ch)
{
    addch(number_to_uint32(ch));
}

void waddch_(void *win, Number ch)
{
    waddch(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void mvaddch_(Number y, Number x, Number ch)
{
    mvaddch(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void mvwaddch_(void *win, Number y, Number x, Number ch)
{
    mvwaddch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void addchstr_(const std::vector<Number> &chstr)
{
    addchstr(chtypes_from_numbers(chstr).data());
}

void waddchstr_(void *win, const std::vector<Number> &chstr)
{
    waddchstr(static_cast<WINDOW *>(win), chtypes_from_numbers(chstr).data());
}

void mvaddchstr_(Number y, Number x, const std::vector<Number> &chstr)
{
    mvaddchstr(number_to_sint32(y), number_to_sint32(x), chtypes_from_numbers(chstr).data());
}

void mvwaddchstr_(void *win, Number y, Number x, const std::vector<Number> &chstr)
{
    mvwaddchstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), chtypes_from_numbers(chstr).data());
}

void addstr_(const std::string &str)
{
    addstr(str.c_str());
}

void waddstr_(void *win, const std::string &str)
{
    waddstr(static_cast<WINDOW *>(win), str.c_str());
}

void mvaddstr_(Number y, Number x, const std::string &str)
{
    mvaddstr(number_to_sint32(y), number_to_sint32(x), str.c_str());
}

void mvwaddstr_(void *win, Number y, Number x, const std::string &str)
{
    mvwaddstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), str.c_str());
}

void attroff_(Number attrs)
{
    attroff(number_to_uint32(attrs));
}

void wattroff_(void *win, Number attrs)
{
    wattroff(static_cast<WINDOW *>(win), number_to_uint32(attrs));
}

void attron_(Number attrs)
{
    attron(number_to_uint32(attrs));
}

void wattron_(void *win, Number attrs)
{
    wattron(static_cast<WINDOW *>(win), number_to_uint32(attrs));
}

void attrset_(Number attrs)
{
    attrset(number_to_uint32(attrs));
}

void wattrset_(void *win, Number attrs)
{
    wattrset(static_cast<WINDOW *>(win), number_to_uint32(attrs));
}

Number baudrate_()
{
    return number_from_sint32(baudrate());
}

void beep_()
{
    beep();
}

void bkgd_(Number ch)
{
    bkgd(number_to_uint32(ch));
}

void wbkgd_(void *win, Number ch)
{
    wbkgd(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void bkgdset_(Number ch)
{
    bkgdset(number_to_uint32(ch));
}

void wbkgdset_(void *win, Number ch)
{
    wbkgdset(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void border_(Number ls, Number rs, Number ts, Number bs, Number tl, Number tr, Number bl, Number br)
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

void wborder_(void *win, Number ls, Number rs, Number ts, Number bs, Number tl, Number tr, Number bl, Number br)
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

void box_(void *win, Number verch, Number horch)
{
    box(static_cast<WINDOW *>(win), number_to_uint32(verch), number_to_uint32(horch));
}

bool can_change_color_()
{
    return can_change_color() != FALSE;
}

void cbreak_()
{
    cbreak();
}

void nocbreak_()
{
    nocbreak();
}

void chgat_(Number n, Number attr, Number color, void *opts)
{
    chgat(number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void wchgat_(void *win, Number n, Number attr, Number color, void *opts)
{
    wchgat(static_cast<WINDOW *>(win), number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void mvchgat_(Number y, Number x, Number n, Number attr, Number color, void *opts)
{
    mvchgat(number_to_sint32(y), number_to_sint32(x), number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void mvwchgat_(void *win, Number y, Number x, Number n, Number attr, Number color, void *opts)
{
    mvwchgat(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_sint32(n), number_to_uint32(attr), static_cast<short>(number_to_uint32(color)), opts);
}

void clear_()
{
    clear();
}

void wclear_(void *win)
{
    wclear(static_cast<WINDOW *>(win));
}

void clearok_(void *win, bool bf)
{
    clearok(static_cast<WINDOW *>(win), bf);
}

void clrtobot_()
{
    clrtobot();
}

void wclrtobot_(void *win)
{
    wclrtobot(static_cast<WINDOW *>(win));
}

void clrtoeol_()
{
    clrtoeol();
}

void wclrtoeol_(void *win)
{
    wclrtoeol(static_cast<WINDOW *>(win));
}

std::vector<Number> color_content_(Number color)
{
    short r = 0, g = 0, b = 0;
    color_content(static_cast<short>(number_to_uint32(color)), &r, &g, &b);
    std::vector<Number> a;
    a.push_back(number_from_sint32(r));
    a.push_back(number_from_sint32(g));
    a.push_back(number_from_sint32(b));
    return a;
}

void color_set_(Number color_pair_number, void *opts)
{
    color_set(static_cast<short>(number_to_uint32(color_pair_number)), opts);
}

void wcolor_set_(void *win, Number color_pair_number, void *opts)
{
    wcolor_set(static_cast<WINDOW *>(win), static_cast<short>(number_to_uint32(color_pair_number)), opts);
}

void copywin_(void *srcwin, void *dstwin, Number sminrow, Number smincol, Number dminrow, Number dmincol, Number dmaxrow, Number dmaxcol, bool overlay)
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

void curs_set_(Number visibility)
{
    curs_set(number_to_sint32(visibility));
}

void *curscr_()
{
    return curscr;
}

std::string curses_version_()
{
    return curses_version();
}

void def_prog_mode_()
{
    def_prog_mode();
}

void def_shell_mode_()
{
    def_shell_mode();
}

void delay_output_(Number ms)
{
    delay_output(number_to_sint32(ms));
}

void delch_()
{
    delch();
}

void wdelch_(void *win)
{
    wdelch(static_cast<WINDOW *>(win));
}

void mvdelch_(Number y, Number x)
{
    mvdelch(number_to_sint32(y), number_to_sint32(x));
}

void mvwdelch_(void *win, Number y, Number x)
{
    mvwdelch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x));
}

void deleteln_()
{
    deleteln();
}

void wdeleteln_(void *win)
{
    wdeleteln(static_cast<WINDOW *>(win));
}

void delwin_(void *win)
{
    delwin(static_cast<WINDOW *>(win));
}

void *derwin_(void *orig, Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return derwin(static_cast<WINDOW *>(orig), number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void doupdate_()
{
    doupdate();
}

void *dupwin_(void *win)
{
    return dupwin(static_cast<WINDOW *>(win));
}

void echo_()
{
    echo();
}

void noecho_()
{
    noecho();
}

void echochar_(Number ch)
{
    echochar(number_to_uint32(ch));
}

void wechochar_(void *win, Number ch)
{
    wechochar(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void endwin_()
{
    endwin();
}

void erase_()
{
    erase();
}

void werase_(void *win)
{
    werase(static_cast<WINDOW *>(win));
}

std::string erasechar_()
{
    return std::string(1, erasechar());
}

void filter_()
{
    filter();
}

void flash_()
{
    flash();
}

void flushinp_()
{
    flushinp();
}

Number getbegx_(void *win)
{
    return number_from_sint32(getbegx(static_cast<WINDOW *>(win)));
}

Number getbegy_(void *win)
{
    return number_from_sint32(getbegy(static_cast<WINDOW *>(win)));
}

std::vector<Number> getbegyx_(void *win)
{
    int y = -1, x = -1;
    getbegyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

Number getbkgd_(void *win)
{
    return number_from_uint32(getbkgd(static_cast<WINDOW *>(win)));
}

Number getch_()
{
    return number_from_sint32(getch());
}

Number wgetch_(void *win)
{
    return number_from_sint32(wgetch(static_cast<WINDOW *>(win)));
}

Number mvgetch_(Number y, Number x)
{
    return number_from_sint32(mvgetch(number_to_sint32(y), number_to_sint32(x)));
}

Number mvwgetch_(void *win, Number y, Number x)
{
    return number_from_sint32(mvwgetch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x)));
}

Number getcurx_(void *win)
{
    return number_from_sint32(getcurx(static_cast<WINDOW *>(win)));
}

Number getcury_(void *win)
{
    return number_from_sint32(getcury(static_cast<WINDOW *>(win)));
}

Number getmaxx_(void *win)
{
    return number_from_sint32(getmaxx(static_cast<WINDOW *>(win)));
}

Number getmaxy_(void *win)
{
    return number_from_sint32(getmaxy(static_cast<WINDOW *>(win)));
}

std::vector<Number> getmaxyx_(void *win)
{
    int y = -1, x = -1;
    getmaxyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

std::string getnstr_(Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    getnstr(str, width);
    std::string r = str;
    delete[] str;
    return r;
}

std::string wgetnstr_(void *win, Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    wgetnstr(static_cast<WINDOW *>(win), str, width);
    std::string r = str;
    delete[] str;
    return r;
}

std::string mvgetnstr_(Number y, Number x, Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    mvgetnstr(number_to_sint32(y), number_to_sint32(x), str, width);
    std::string r = str;
    delete[] str;
    return r;
}

std::string mvwgetnstr_(void *win, Number y, Number x, Number n)
{
    int width = number_to_sint32(n);
    char *str = new char[width+1];
    mvwgetnstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), str, width);
    std::string r = str;
    delete[] str;
    return r;
}

Number getparx_(void *win)
{
    return number_from_sint32(getparx(static_cast<WINDOW *>(win)));
}

Number getpary_(void *win)
{
    return number_from_sint32(getpary(static_cast<WINDOW *>(win)));
}

std::vector<Number> getparyx_(void *win)
{
    int y = -1, x = -1;
    getparyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

std::vector<Number> getsyx_()
{
    int y = -1, x = -1;
    getsyx(y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

std::vector<Number> getyx_(void *win)
{
    int y = -1, x = -1;
    getyx(static_cast<WINDOW *>(win), y, x);
    std::vector<Number> r;
    r.push_back(number_from_sint32(y));
    r.push_back(number_from_sint32(x));
    return r;
}

void halfdelay_(Number tenths)
{
    halfdelay(number_to_sint32(tenths));
}

bool has_colors_()
{
    return has_colors() != FALSE;
}

bool has_ic_()
{
    return has_ic() != FALSE;
}

bool has_il_()
{
    return has_il() != FALSE;
}

bool has_key_(Number ch)
{
    return has_key(number_to_sint32(ch)) != FALSE;
}

void hline_(Number ch, Number n)
{
    hline(number_to_uint32(ch), number_to_sint32(n));
}

void whline_(void *win, Number ch, Number n)
{
    whline(static_cast<WINDOW *>(win), number_to_uint32(ch), number_to_sint32(n));
}

void mvhline_(Number y, Number x, Number ch, Number n)
{
    mvhline(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void mvwhline_(void *win, Number y, Number x, Number ch, Number n)
{
    mvwhline(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void idcok_(void *win, bool bf)
{
    idcok(static_cast<WINDOW *>(win), bf);
}

void idlok_(void *win, bool bf)
{
    idlok(static_cast<WINDOW *>(win), bf);
}

void immedok_(void *win, bool bf)
{
    immedok(static_cast<WINDOW *>(win), bf);
}

Number inch_()
{
    return number_from_uint32(inch());
}

Number winch_(void *win)
{
    return number_from_uint32(winch(static_cast<WINDOW *>(win)));
}

Number mvinch_(Number y, Number x)
{
    return number_from_uint32(mvinch(number_to_sint32(y), number_to_sint32(x)));
}

Number mvwinch_(void *win, Number y, Number x)
{
    return number_from_uint32(mvwinch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x)));
}

void init_color_(Number pair, Number r, Number g, Number b)
{
    init_color(static_cast<short>(number_to_uint32(pair)), static_cast<short>(number_to_uint32(r)), static_cast<short>(number_to_uint32(g)), static_cast<short>(number_to_uint32(b)));
}

void init_pair_(Number pair, Number f, Number b)
{
    init_pair(static_cast<short>(number_to_uint32(pair)), static_cast<short>(number_to_uint32(f)), static_cast<short>(number_to_uint32(b)));
}

void initscr_()
{
    initscr();
}

void intrflush_(void *win, bool bf)
{
    intrflush(static_cast<WINDOW *>(win), bf);
}

void insch_(Number ch)
{
    insch(number_to_uint32(ch));
}

void winsch_(void *win, Number ch)
{
    winsch(static_cast<WINDOW *>(win), number_to_uint32(ch));
}

void mvinsch_(Number y, Number x, Number ch)
{
    mvinsch(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void mvwinsch_(void *win, Number y, Number x, Number ch)
{
    mvwinsch(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch));
}

void insdelln_(Number n)
{
    insdelln(number_to_sint32(n));
}

void winsdelln_(void *win, Number n)
{
    winsdelln(static_cast<WINDOW *>(win), number_to_sint32(n));
}

void insertln_()
{
    insertln();
}

void winsertln_(void *win)
{
    winsertln(static_cast<WINDOW *>(win));
}

void insstr_(const std::string &str)
{
    insstr(str.c_str());
}

void winsstr_(void *win, const std::string &str)
{
    winsstr(static_cast<WINDOW *>(win), str.c_str());
}

void mvinsstr_(Number y, Number x, const std::string &str)
{
    mvinsstr(number_to_sint32(y), number_to_sint32(x), str.c_str());
}

void mvwinsstr_(void *win, Number y, Number x, const std::string &str)
{
    mvwinsstr(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), str.c_str());
}

bool is_linetouched_(void *win, Number line)
{
    return is_linetouched(static_cast<WINDOW *>(win), number_to_sint32(line)) != FALSE;
}

bool is_wintouched_(void *win)
{
    return is_wintouched(static_cast<WINDOW *>(win)) != FALSE;
}

bool isendwin_()
{
    return isendwin() != FALSE;
}

std::string keyname_(Number c)
{
    return keyname(number_to_sint32(c));
}

void keypad_(void *win, bool bf)
{
    keypad(static_cast<WINDOW *>(win), bf);
}

std::string killchar_()
{
    return std::string(1, killchar());
}

void leaveok_(void *win, bool bf)
{
    leaveok(static_cast<WINDOW *>(win), bf);
}

std::string longname_()
{
    return longname();
}

void meta_(void *win, bool bf)
{
    meta(static_cast<WINDOW *>(win), bf);
}

void move_(Number y, Number x)
{
    move(number_to_sint32(y), number_to_sint32(x));
}

void wmove_(void *win, Number y, Number x)
{
    wmove(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x));
}

void mvderwin_(void *win, Number par_y, Number par_x)
{
    mvderwin(static_cast<WINDOW *>(win), number_to_sint32(par_y), number_to_sint32(par_x));
}

void mvwin_(void *win, Number y, Number x)
{
    mvwin(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x));
}

void napms_(Number ms)
{
    napms(number_to_sint32(ms));
}

void *newpad_(Number nlines, Number ncols)
{
    return newpad(number_to_sint32(nlines), number_to_sint32(ncols));
}

void *newwin_(Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return newwin(number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void nl_()
{
    nl();
}

void nonl_()
{
    nonl();
}

void nodelay_(void *win, bool bf)
{
    nodelay(static_cast<WINDOW *>(win), bf);
}

void noqiflush_()
{
    noqiflush();
}

void notimeout_(void *win, bool bf)
{
    notimeout(static_cast<WINDOW *>(win), bf);
}

void overlay_(void *srcwin, void *dstwin)
{
    overlay(static_cast<WINDOW *>(srcwin), static_cast<WINDOW *>(dstwin));
}

void overwrite_(void *srcwin, void *dstwin)
{
    overwrite(static_cast<WINDOW *>(srcwin), static_cast<WINDOW *>(dstwin));
}

std::vector<Number> pair_content_(Number color)
{
    short f = 0, b = 0;
    pair_content(static_cast<short>(number_to_uint32(color)), &f, &b);
    std::vector<Number> r;
    r.push_back(number_from_sint32(f));
    r.push_back(number_from_sint32(b));
    return r;
}

void pechochar_(void *pad, Number ch)
{
    pechochar(static_cast<WINDOW *>(pad), number_to_uint32(ch));
}

void pnoutrefresh_(void *pad, Number pminrow, Number pmincol, Number sminrow, Number smincol, Number smaxrow, Number smaxcol)
{
    pnoutrefresh(static_cast<WINDOW *>(pad),
                 number_to_sint32(pminrow),
                 number_to_sint32(pmincol),
                 number_to_sint32(sminrow),
                 number_to_sint32(smincol),
                 number_to_sint32(smaxrow),
                 number_to_sint32(smaxcol));
}

void prefresh_(void *pad, Number pminrow, Number pmincol, Number sminrow, Number smincol, Number smaxrow, Number smaxcol)
{
    prefresh(static_cast<WINDOW *>(pad),
             number_to_sint32(pminrow),
             number_to_sint32(pmincol),
             number_to_sint32(sminrow),
             number_to_sint32(smincol),
             number_to_sint32(smaxrow),
             number_to_sint32(smaxcol));
}

void qiflush_()
{
    qiflush();
}

void raw_()
{
    raw();
}

void noraw_()
{
    noraw();
}

void redrawwin_(void *win)
{
    redrawwin(static_cast<WINDOW *>(win));
}

void refresh_()
{
    refresh();
}

void wnoutrefresh_(void *win)
{
    wnoutrefresh(static_cast<WINDOW *>(win));
}

void wrefresh_(void *win)
{
    wrefresh(static_cast<WINDOW *>(win));
}

void reset_prog_mode_()
{
    reset_prog_mode();
}

void reset_shell_mode_()
{
    reset_shell_mode();
}

void resetty_()
{
    resetty();
}

void savetty_()
{
    savetty();
}

void scr_dump_(const std::string &filename)
{
    scr_dump(filename.c_str());
}

void scr_init_(const std::string &filename)
{
    scr_init(filename.c_str());
}

void scr_restore_(const std::string &filename)
{
    scr_restore(filename.c_str());
}

void scr_set_(const std::string &filename)
{
    scr_set(filename.c_str());
}

void scroll_(void *win)
{
    scroll(static_cast<WINDOW *>(win));
}

void scrollok_(void *win, bool bf)
{
    scrollok(static_cast<WINDOW *>(win), bf);
}

void scrl_(Number n)
{
    scrl(number_to_sint32(n));
}

void wscrl_(void *win, Number n)
{
    wscrl(static_cast<WINDOW *>(win), number_to_sint32(n));
}

void setscrreg_(Number top, Number bottom)
{
    setscrreg(number_to_sint32(top), number_to_sint32(bottom));
}

void wsetscrreg_(void *win, Number top, Number bottom)
{
    wsetscrreg(static_cast<WINDOW *>(win), number_to_sint32(top), number_to_sint32(bottom));
}

void setsyx_(Number y, Number x)
{
    setsyx(number_to_sint32(y), number_to_sint32(x));
}

void standend_()
{
    standend();
}

void wstandend_(void *win)
{
    wstandend(static_cast<WINDOW *>(win));
}

void standout_()
{
    standout();
}

void wstandout_(void *win)
{
    wstandout(static_cast<WINDOW *>(win));
}

void start_color_()
{
    start_color();
}

void *stdscr_()
{
    return stdscr;
}

void *subpad_(void *orig, Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return subpad(static_cast<WINDOW *>(orig), number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void *subwin_(void *orig, Number nlines, Number ncols, Number begin_y, Number begin_x)
{
    return subwin(static_cast<WINDOW *>(orig), number_to_sint32(nlines), number_to_sint32(ncols), number_to_sint32(begin_y), number_to_sint32(begin_x));
}

void syncok_(void *win, bool bf)
{
    syncok(static_cast<WINDOW *>(win), bf);
}

Number termattrs_()
{
    return number_from_uint32(termattrs());
}

std::string termname_()
{
    return termname();
}

void timeout_(Number delay)
{
    timeout(number_to_sint32(delay));
}

void wtimeout_(void *win, Number delay)
{
    wtimeout(static_cast<WINDOW *>(win), number_to_sint32(delay));
}

void touchline_(void *win, Number start, Number count)
{
    touchline(static_cast<WINDOW *>(win), number_to_sint32(start), number_to_sint32(count));
}

void touchwin_(void *win)
{
    touchwin(static_cast<WINDOW *>(win));
}

void untouchwin_(void *win)
{
    untouchwin(static_cast<WINDOW *>(win));
}

std::string unctrl_(Number c)
{
    return unctrl(number_to_uint32(c));
}

void ungetch_(Number ch)
{
    ungetch(number_to_sint32(ch));
}

void use_env_(bool f)
{
    use_env(f);
}

void vline_(Number ch, Number n)
{
    vline(number_to_uint32(ch), number_to_sint32(n));
}

void wvline_(void *win, Number ch, Number n)
{
    wvline(static_cast<WINDOW *>(win), number_to_uint32(ch), number_to_sint32(n));
}

void mvvline_(Number y, Number x, Number ch, Number n)
{
    mvvline(number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void mvwvline_(void *win, Number y, Number x, Number ch, Number n)
{
    mvwvline(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(x), number_to_uint32(ch), number_to_sint32(n));
}

void wcursyncup_(void *win)
{
    wcursyncup(static_cast<WINDOW *>(win));
}

void wredrawln_(void *win, Number beg_line, Number num_lines)
{
    wredrawln(static_cast<WINDOW *>(win), number_to_sint32(beg_line), number_to_sint32(num_lines));
}

void wsyncdown_(void *win)
{
    wsyncdown(static_cast<WINDOW *>(win));
}

void wsyncup_(void *win)
{
    wsyncup(static_cast<WINDOW *>(win));
}

void wtouchln_(void *win, Number y, Number n, bool changed)
{
    wtouchln(static_cast<WINDOW *>(win), number_to_sint32(y), number_to_sint32(n), changed);
}

} // namespace curses

} // namespace rtl
