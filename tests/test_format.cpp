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

    assert(format::parse("04d", spec));
    assert(format::format("", spec)       == "0000");
    assert(format::format("1", spec)      == "0001");
    assert(format::format("12", spec)     == "0012");
    assert(format::format("123", spec)    == "0123");
    assert(format::format("1234", spec)   == "1234");
    assert(format::format("12345", spec)  == "12345");

    // TODO: Many more format specs.
}
