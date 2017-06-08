#include <string>
#include <vector>

#define PCRE2_STATIC
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include "neonext.h"

const Ne_MethodTable *Ne;

extern "C" {

int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_search)
{
    std::string pattern = Ne_PARAM_STRING(0);
    std::string subject = Ne_PARAM_STRING(1);
    Ne_Cell *match = Ne->parameterlist_set_cell(out_params, 0);

    int errorcode;
    PCRE2_SIZE erroroffset;
    pcre2_code *code = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern.data()), pattern.length(), 0, &errorcode, &erroroffset, NULL);
    pcre2_match_data *md = pcre2_match_data_create_from_pattern(code, NULL);
    int r = pcre2_match(code, reinterpret_cast<PCRE2_SPTR>(subject.data()), subject.length(), 0, 0, md, NULL);
    if (r <= 0) {
        Ne_RETURN_BOOL(false);
    }
    uint32_t n = pcre2_get_ovector_count(md);
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(md);
    for (uint32_t i = 0; i < n*2; i += 2) {
        Ne_Cell *g = Ne->cell_set_array_cell(match, i/2);
        if (ovector[i] != PCRE2_UNSET) {
            Ne->cell_set_boolean(Ne->cell_set_array_cell(g, 0), true);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(g, 1), ovector[i]);
            Ne->cell_set_number_int(Ne->cell_set_array_cell(g, 2), ovector[i+1]);
            Ne->cell_set_string(Ne->cell_set_array_cell(g, 3), subject.substr(ovector[i], ovector[i+1]-ovector[i]).c_str());
        } else {
            Ne->cell_set_boolean(Ne->cell_set_array_cell(g, 0), false);
        }
    }
    pcre2_match_data_free(md);
    Ne_RETURN_BOOL(true);
}

} // extern "C"
