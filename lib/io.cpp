#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _WIN32
#include <io.h>
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

static FILE *check_file(void *pf)
{
    FILE *f = static_cast<FILE *>(pf);
    if (f == NULL) {
        throw RtlException(rtl::io::Exception_IoException_InvalidFile, utf8string(""));
    }
    return f;
}

namespace rtl {

namespace io {

Cell VAR_stdin(Cell::makeOther(stdin));
Cell VAR_stdout(Cell::makeOther(stdout));
Cell VAR_stderr(Cell::makeOther(stderr));

void close(void **ppf)
{
    FILE *f = check_file(*ppf);
    fclose(f);
    *ppf = NULL;
}

void fprint(void *pf, const utf8string &s)
{
    FILE *f = check_file(pf);
    fputs(s.c_str(), f);
    fputs("\n", f);
}

void *open(const utf8string &name, Cell &mode)
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

std::vector<unsigned char> readBytes(void *pf, Number count)
{
    FILE *f = check_file(pf);
    uint64_t ncount = number_to_uint64(count);
    std::vector<unsigned char> r(ncount);
    size_t n = fread(const_cast<unsigned char *>(r.data()), 1, ncount, f);
    r.resize(n);
    return r;
}

bool readLine(void *pf, utf8string *s)
{
    FILE *f = check_file(pf);
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

void seek(void *pf, Number offset, Cell &whence)
{
    FILE *f = check_file(pf);
    int w;
    switch (number_to_uint32(whence.number())) {
        case ENUM_SeekBase_absolute: w = SEEK_SET; break;
        case ENUM_SeekBase_relative: w = SEEK_CUR; break;
        case ENUM_SeekBase_fromEnd:  w = SEEK_END; break;
        default:
            return;
    }
    fseek(f, static_cast<long>(number_to_sint64(offset)), w);
}

Number tell(void *pf)
{
    FILE *f = check_file(pf);
    return number_from_sint64(ftell(f));
}

void truncate(void *pf)
{
    FILE *f = check_file(pf);
    long ofs = ftell(f);
    #ifdef _WIN32
        if (_chsize(_fileno(f), ofs) != 0) {
            throw RtlException(Exception_IoException_Write, utf8string(""));
        }
    #else
        if (ftruncate(fileno(f), ofs) != 0) {
            throw RtlException(Exception_IoException_Write, utf8string(""));
        }
    #endif
}

void write(void *pf, const utf8string &s)
{
    FILE *f = check_file(pf);
    fputs(s.c_str(), f);
}

void writeBytes(void *pf, const std::vector<unsigned char> &b)
{
    FILE *f = check_file(pf);
    fwrite(b.data(), 1, b.size(), f);
}

} // namespace io

} // namespace rtl
