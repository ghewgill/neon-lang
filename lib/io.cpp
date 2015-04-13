#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iso646.h>
#include <stdio.h>
#include <string>

#include "cell.h"
#include "number.h"

namespace rtl {

void io$close(Cell **pf)
{
    FILE *f = (FILE *)(*pf);
    fclose(f);
    *pf = NULL;
}

void io$fprint(void *pf, const std::string &s)
{
    FILE *f = (FILE *)pf;
    fputs(s.c_str(), f);
    fputs("\n", f);
}

void *io$open(const std::string &name, Cell &/*mode*/)
{
    return fopen(name.c_str(), "r");
}

bool io$readline(void *pf, std::string *s)
{
    FILE *f = (FILE *)pf;
    s->clear();
    for (;;) {
        char buf[1024];
        if (fgets(buf, sizeof(buf), f) == NULL) {
            return not s->empty();
        }
        s->append(buf);
        if (s->at(s->length()-1) == '\n') {
            return true;
        }
    }
}

void *io$stdin()
{
    return stdin;
}

void *io$stdout()
{
    return stdout;
}

void *io$stderr()
{
    return stderr;
}

void io$write(void *pf, const std::string &s)
{
    FILE *f = (FILE *)pf;
    fputs(s.c_str(), f);
}

}
