#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iso646.h>
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#include <utf8.h>

#include "cell.h"
#include "intrinsic.h"
#include "number.h"
#include "rtl_exec.h"

#include "choices.inc"

namespace rtl {

namespace ne_global {

Cell parseNumber(const utf8string &s)
{
    Number n = number_from_string(s.str());
    if (number_is_nan(n)) {
        return Cell(std::vector<Cell> {
            Cell(number_from_uint32(CHOICE_ParseNumberResult_error)),
            Cell(utf8string("parseNumber() argument not a number"))
        });
    }
    return Cell(std::vector<Cell> {Cell(number_from_uint32(CHOICE_ParseNumberResult_number)), Cell(n)});
}

void print(const std::shared_ptr<Object> &x)
{
    if (x == nullptr) {
        std::cout << "NIL\n";
        return;
    }
    std::cout << x->toString().str() << "\n";
}

Cell Bytes__decodeUTF8(const std::vector<unsigned char> &self)
{
    auto inv = utf8::find_invalid(self.begin(), self.end());
    if (inv != self.end()) {
        return Cell(std::vector<Cell> {
            Cell(number_from_uint32(CHOICE_DecodeResult_error)),
            Cell(std::vector<Cell> {
                Cell(number_from_uint32(std::distance(self.begin(), inv)))
            })
        });
    }
    return Cell(std::vector<Cell> {Cell(number_from_uint32(CHOICE_DecodeResult_string)), Cell(utf8string(std::string(self.begin(), self.end())))});
}

} // namespace ne_global

} // namespace rtl
