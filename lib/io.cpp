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
#include "number.h"
#include "rtl_exec.h"

#include "enums.inc"

class FileObject: public Object {
public:
    explicit FileObject(FILE *file): file(file) {}
    FileObject(const FileObject &) = delete;
    FileObject &operator=(const FileObject &) = delete;
    virtual utf8string toString() const { return utf8string("<FILE:" + std::to_string(reinterpret_cast<intptr_t>(file)) + ">"); }
    FILE *file;
};

static FileObject *check_file(const std::shared_ptr<Object> &pf)
{
    FileObject *fo = dynamic_cast<FileObject *>(pf.get());
    if (fo == nullptr || fo->file == NULL) {
        throw RtlException(rtl::ne_io::Exception_IoException_InvalidFile, utf8string(""));
    }
    return fo;
}

namespace rtl {

namespace ne_io {

Cell VAR_stdin(std::shared_ptr<Object> { new FileObject(stdin) });
Cell VAR_stdout(std::shared_ptr<Object> { new FileObject(stdout) });
Cell VAR_stderr(std::shared_ptr<Object> { new FileObject(stderr) });

void close(const std::shared_ptr<Object> &pf)
{
    FileObject *f = check_file(pf);
    fclose(f->file);
    f->file = NULL;
}

void flush(const std::shared_ptr<Object> &pf)
{
    FileObject *f = check_file(pf);
    fflush(f->file);
}

std::shared_ptr<Object> open(const utf8string &name, Cell &mode)
{
    const char *m;
    switch (number_to_uint32(mode.number())) {
        case ENUM_Mode_read:  m = "rb"; break;
        case ENUM_Mode_write: m = "w+b"; break;
        default:
            return NULL;
    }
    FILE *f = fopen(name.c_str(), m);
    if (f == NULL) {
        throw RtlException(Exception_IoException_Open, utf8string("open error " + std::to_string(errno)));
    }
    return std::shared_ptr<Object> { new FileObject(f) };
}

std::vector<unsigned char> readBytes(const std::shared_ptr<Object> &pf, Number count)
{
    FileObject *f = check_file(pf);
    uint64_t ncount = number_to_uint64(count);
    std::vector<unsigned char> r(ncount);
    size_t n = fread(const_cast<unsigned char *>(r.data()), 1, ncount, f->file);
    r.resize(n);
    return r;
}

bool readLine(const std::shared_ptr<Object> &pf, utf8string *s)
{
    FileObject *f = check_file(pf);
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

void seek(const std::shared_ptr<Object> &pf, Number offset, Cell &whence)
{
    FileObject *f = check_file(pf);
    int w;
    switch (number_to_uint32(whence.number())) {
        case ENUM_SeekBase_absolute: w = SEEK_SET; break;
        case ENUM_SeekBase_relative: w = SEEK_CUR; break;
        case ENUM_SeekBase_fromEnd:  w = SEEK_END; break;
        default:
            return;
    }
    fseek(f->file, static_cast<long>(number_to_sint64(offset)), w);
}

Number tell(const std::shared_ptr<Object> &pf)
{
    FileObject *f = check_file(pf);
    return number_from_sint64(ftell(f->file));
}

void truncate(const std::shared_ptr<Object> &pf)
{
    FileObject *f = check_file(pf);
    long ofs = ftell(f->file);
    #ifdef _WIN32
        if (_chsize(_fileno(f->file), ofs) != 0) {
            throw RtlException(Exception_IoException_Write, utf8string(""));
        }
    #else
        if (ftruncate(fileno(f->file), ofs) != 0) {
            throw RtlException(Exception_IoException_Write, utf8string(""));
        }
    #endif
}

void write(const std::shared_ptr<Object> &pf, const utf8string &s)
{
    FileObject *f = check_file(pf);
    fputs(s.c_str(), f->file);
}

void writeBytes(const std::shared_ptr<Object> &pf, const std::vector<unsigned char> &b)
{
    FileObject *f = check_file(pf);
    fwrite(b.data(), 1, b.size(), f->file);
}

} // namespace ne_io

} // namespace rtl
