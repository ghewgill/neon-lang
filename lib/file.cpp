#include <fstream>
#include <iso646.h>
#include <string>
#include <vector>

#include "rtl_exec.h"

#include "enums.inc"
#include "choices.inc"

namespace rtl {

namespace ne_file {

Cell error_result(int error, const utf8string &path)
{
    int type = ENUM_Error_other;
    switch (error) {
        case EACCES:
            type = ENUM_Error_permissionDenied;
            break;
        case EEXIST:
            type = ENUM_Error_alreadyExists;
            break;
        case ENOENT:
            type = ENUM_Error_notFound;
            break;
    }
    return Cell(std::vector<Cell> {
        Cell(number_from_uint32(CHOICE_FileResult_error)),
        Cell(std::vector<Cell> {
            /* type    */ Cell(number_from_sint32(type)),
            /* os_code */ Cell(number_from_sint32(error)),
            /* message */ Cell(utf8string(strerror(error))),
            /* path    */ Cell(utf8string(path))
        })
    });
}

Cell readBytes(const utf8string &filename)
{
    std::vector<unsigned char> r;
    std::ifstream f(filename.str(), std::ios::binary);
    if (not f.is_open()) {
        return error_result(errno, filename);
    }
    for (;;) {
        char buf[16384];
        f.read(buf, sizeof(buf));
        std::streamsize n = f.gcount();
        if (n == 0) {
            break;
        }
        std::copy(buf, buf+n, std::back_inserter(r));
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_BytesResult_data)), Cell(r) });
}

Cell readLines(const utf8string &filename)
{
    std::vector<Cell> r;
    std::ifstream f(filename.str());
    if (not f.is_open()) {
        return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_LinesResult_error)), Cell(filename) });
    }
    std::string s;
    while (std::getline(f, s)) {
        r.push_back(Cell(s.c_str()));
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_LinesResult_lines)), Cell(r) });
}

Cell writeBytes(const utf8string &filename, const std::vector<unsigned char> &data)
{
    std::ofstream f(filename.str(), std::ios::binary);
    if (not f.is_open()) {
        return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_error)), Cell(filename) });
    }
    if (not f.write(reinterpret_cast<const char *>(data.data()), data.size())) {
        return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_error)), Cell(filename) });
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_ok)) });
}

Cell writeLines(const utf8string &filename, const std::vector<utf8string> &lines)
{
    std::ofstream f(filename.str(), std::ios::out | std::ios::trunc); // Truncate the file every time we open it to write lines to it.
    if (not f.is_open()) {
        return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_error)), Cell(filename) });
    }
    for (auto s: lines) {
        f << s.str() << "\n";   // Write line, and line-ending for each element in the array.
        if (f.fail()) {
            // If the write fails for any reason, consider that a FileException.Write exception.
            return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_error)), Cell(filename) });
        }
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_ok)) });
}

} // namespace ne_file

} // namespace rtl
