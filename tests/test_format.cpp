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

    assert(format::parse("4d", spec));
    assert(format::format("0", spec)      == "   0");
    assert(format::format("1", spec)      == "   1");
    assert(format::format("12", spec)     == "  12");
    assert(format::format("123", spec)    == " 123");
    assert(format::format("1234", spec)   == "1234");
    assert(format::format("12345", spec)  == "12345");

    assert(format::parse("04d", spec));
    assert(format::format("0", spec)      == "0000");
    assert(format::format("1", spec)      == "0001");
    assert(format::format("12", spec)     == "0012");
    assert(format::format("123", spec)    == "0123");
    assert(format::format("1234", spec)   == "1234");
    assert(format::format("12345", spec)  == "12345");

    assert(format::parse("4x", spec));
    assert(format::format("0", spec)      == "   0");
    assert(format::format("1", spec)      == "   1");
    assert(format::format("12", spec)     == "   c");
    assert(format::format("123", spec)    == "  7b");
    assert(format::format("1234", spec)   == " 4d2");
    assert(format::format("12345", spec)  == "3039");
    assert(format::format("123456", spec) == "1e240");

    assert(format::parse("04x", spec));
    assert(format::format("0", spec)      == "0000");
    assert(format::format("1", spec)      == "0001");
    assert(format::format("12", spec)     == "000c");
    assert(format::format("123", spec)    == "007b");
    assert(format::format("1234", spec)   == "04d2");
    assert(format::format("12345", spec)  == "3039");
    assert(format::format("123456", spec) == "1e240");

    // TODO: Many more format specs.
}
