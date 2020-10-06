#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _WIN32
#include <io.h>
#endif
#include <errno.h>
#include <iso646.h>
#include <stdio.h>
#include <string>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "cell.h"
#include "rtl_exec.h"

#include "enums.inc"

class TextFileObject: public Object {
public:
    explicit TextFileObject(FILE *file): file(file) {}
    TextFileObject(const TextFileObject &) = delete;
    TextFileObject &operator=(const TextFileObject &) = delete;
    virtual utf8string toString() const { return utf8string("<TEXTFILE:" + std::to_string(reinterpret_cast<intptr_t>(file)) + ">"); }
    FILE *file;
};

static TextFileObject *check_file(const std::shared_ptr<Object> &pf)
{
    TextFileObject *fo = dynamic_cast<TextFileObject *>(pf.get());
    if (fo == nullptr || fo->file == NULL) {
        throw RtlException(rtl::ne_textio::Exception_TextioException_InvalidFile, utf8string(""));
    }
    return fo;
}

namespace rtl {

namespace ne_textio {

Cell VAR_stdin(std::shared_ptr<Object> { new TextFileObject(stdin) });
Cell VAR_stdout(std::shared_ptr<Object> { new TextFileObject(stdout) });
Cell VAR_stderr(std::shared_ptr<Object> { new TextFileObject(stderr) });

void close(const std::shared_ptr<Object> &pf)
{
    TextFileObject *f = check_file(pf);
    fclose(f->file);
    f->file = NULL;
}

std::shared_ptr<Object> open(const utf8string &name, Cell &mode)
{
    const char *m;
    switch (number_to_uint32(mode.number())) {
        case ENUM_Mode_read:      m = "r"; break;
        case ENUM_Mode_write:     m = "w+"; break;
        case ENUM_Mode_readwrite: m = "r+"; break;
        default:
            return NULL;
    }
    FILE *f = fopen(name.c_str(), m);
    if (f == NULL) {
        throw RtlException(Exception_TextioException_Open, utf8string("open error " + std::to_string(errno)));
    }
    return std::shared_ptr<Object> { new TextFileObject(f) };
}

bool readLine(const std::shared_ptr<Object> &pf, utf8string *s)
{
    TextFileObject *f = check_file(pf);
    s->clear();
    for (;;) {
        char buf[1024];
        if (fgets(buf, sizeof(buf), f->file) == NULL) {
            return not s->empty();
        }
        s->append(buf);
        if (s->at(s->length()-1) == '\n') {
            s->resize(s->length()-1);
            return true;
        }
    }
}

void seekEnd(const std::shared_ptr<Object> &pf)
{
    TextFileObject *f = check_file(pf);
    fseek(f->file, 0, SEEK_END);
}

void seekStart(const std::shared_ptr<Object> &pf)
{
    TextFileObject *f = check_file(pf);
    fseek(f->file, 0, SEEK_SET);
}

void truncate(const std::shared_ptr<Object> &pf)
{
    TextFileObject *f = check_file(pf);
    long ofs = ftell(f->file);
    #ifdef _WIN32
        if (_chsize(_fileno(f->file), ofs) != 0) {
            throw RtlException(Exception_TextioException_Write, utf8string(""));
        }
    #else
        if (ftruncate(fileno(f->file), ofs) != 0) {
            throw RtlException(Exception_TextioException_Write, utf8string(""));
        }
    #endif
}

void writeLine(const std::shared_ptr<Object> &pf, const utf8string &s)
{
    TextFileObject *f = check_file(pf);
    fputs(s.c_str(), f->file);
    fputs("\n", f->file);
}

} // namespace ne_textio

} // namespace rtl
