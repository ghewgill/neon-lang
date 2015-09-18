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

Number curses$COLOR_PAIR(Number n)
{
    return number_from_uint32(COLOR_PAIR(number_to_uint32(n)));
}

Number curses$COLS()
{
    return number_from_sint32(COLS);
}

Number curses$KEY_F(Number c)
{
    return number_from_uint32(KEY_F(number_to_uint32(c)));
}

Number curses$LINES()
{
    return number_from_sint32(LINES);
}

Number curses$PAIR_NUMBER(Number n)
{
    return number_from_uint32(PAIR_NUMBER(number_to_uint32(n)));
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

void *curses$curscr()
{
    return curscr;
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
