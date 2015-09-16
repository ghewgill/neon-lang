#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iso646.h>
#include <stdio.h>
#include <string>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "cell.h"
#include "number.h"
#include "rtl_exec.h"

#include "enums.inc"

namespace rtl {

void io$close(Cell **pf)
{
    FILE *f = (FILE *)(*pf);
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    fclose(f);
    *pf = NULL;
}

void io$fprint(void *pf, const std::string &s)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    fputs(s.c_str(), f);
    fputs("\n", f);
}

void *io$open(const std::string &name, Cell &mode)
{
    const char *m;
    switch (number_to_uint32(mode.number())) {
        case ENUM_Mode_read:  m = "r"; break;
        case ENUM_Mode_write: m = "w+"; break;
        default:
            return NULL;
    }
    return fopen(name.c_str(), m);
}

std::string io$read_bytes(void *pf, Number count)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    uint64_t ncount = number_to_uint64(count);
    std::string r(ncount, 0);
    size_t n = fread(const_cast<char *>(r.data()), 1, ncount, f);
    r.resize(n);
    return r;
}

bool io$readline(void *pf, utf8string *s)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    s->clear();
    for (;;) {
        char buf[1024];
        if (fgets(buf, sizeof(buf), f) == NULL) {
            return not s->empty();
        }
        s->append(buf);
        if (s->at(s->length()-1) == '\n') {
            s->resize(s->length()-1);
            return true;
        }
    }
}

void io$seek(void *pf, Number offset, Cell &whence)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    int w;
    switch (number_to_uint32(whence.number())) {
        case ENUM_SeekBase_absolute: w = SEEK_SET; break;
        case ENUM_SeekBase_relative: w = SEEK_CUR; break;
        case ENUM_SeekBase_from_end: w = SEEK_END; break;
        default:
            return;
    }
    fseek(f, number_to_sint64(offset), w);
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

Number io$tell(void *pf)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    return number_from_sint64(ftell(f));
}

void io$truncate(void *pf)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    long ofs = ftell(f);
    #ifdef _WIN32
        _chsize(_fileno(f), ofs);
    #else
        ftruncate(fileno(f), ofs);
    #endif
}

void io$write(void *pf, const std::string &s)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    fputs(s.c_str(), f);
}

void io$write_bytes(void *pf, const std::string &b)
{
    FILE *f = (FILE *)pf;
    if (f == NULL) {
        throw RtlException(Exception_io$InvalidFile, "");
    }
    fwrite(b.data(), 1, b.size(), f);
}

}
