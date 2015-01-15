#include <assert.h>

#include <format.h>

int main()
{
    format::Spec spec;

    assert(format::parse("5", spec));
    assert(format::format("", spec)       == "     ");
    assert(format::format("a", spec)      == "a    ");
    assert(format::format("ab", spec)     == "ab   ");
    assert(format::format("abc", spec)    == "abc  ");
    assert(format::format("abcd", spec)   == "abcd ");
    assert(format::format("abcde", spec)  == "abcde");
    assert(format::format("abcdef", spec) == "abcdef");

    assert(format::parse(">5", spec));
    assert(format::format("", spec)       == "     ");
    assert(format::format("a", spec)      == "    a");
    assert(format::format("ab", spec)     == "   ab");
    assert(format::format("abc", spec)    == "  abc");
    assert(format::format("abcd", spec)   == " abcd");
    assert(format::format("abcde", spec)  == "abcde");
    assert(format::format("abcdef", spec) == "abcdef");

    assert(format::parse("^5", spec));
    assert(format::format("", spec)       == "     ");
    assert(format::format("a", spec)      == "  a  ");
    assert(format::format("ab", spec)     == " ab  ");
    assert(format::format("abc", spec)    == " abc ");
    assert(format::format("abcd", spec)   == "abcd ");
    assert(format::format("abcde", spec)  == "abcde");
    assert(format::format("abcdef", spec) == "abcdef");

    assert(format::parse("5.3", spec));
    assert(format::format("", spec)       == "     ");
    assert(format::format("a", spec)      == "a    ");
    assert(format::format("ab", spec)     == "ab   ");
    assert(format::format("abc", spec)    == "abc  ");
    assert(format::format("abcd", spec)   == "abc  ");
    assert(format::format("abcde", spec)  == "abc  ");
    assert(format::format("abcdef", spec) == "abc  ");

    // TODO: Many more format specs.
}
