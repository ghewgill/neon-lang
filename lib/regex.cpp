#include <string>
#include <vector>

#include <cell.h>
#include <number.h>

#define PCRE2_STATIC
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

namespace rtl {

bool regex$search(const std::string &pattern, const std::string &subject, Cell *match)
{
    std::vector<Cell> matches;
    int errorcode;
    PCRE2_SIZE erroroffset;
    pcre2_code *code = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern.data()), pattern.length(), 0, &errorcode, &erroroffset, NULL);
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(code, NULL);
    int r = pcre2_match(code, reinterpret_cast<PCRE2_SPTR>(subject.data()), subject.length(), 0, 0, md, NULL);
    if (r <= 0) {
        return false;
    }
    uint32_t n = pcre2_get_ovector_count(md);
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(md);
    for (uint32_t i = 0; i < n*2; i += 2) {
        std::vector<Cell> g;
        g.push_back(Cell(number_from_uint32(static_cast<uint32_t>(ovector[i]))));
        g.push_back(Cell(number_from_uint32(static_cast<uint32_t>(ovector[i+1]))));
        g.push_back(Cell(subject.substr(ovector[i], ovector[i+1]-ovector[i])));
        matches.push_back(Cell(g));
    }
    pcre2_match_data_free(md);
    *match = Cell(matches);
    return true;
}

} // namespace rtl
