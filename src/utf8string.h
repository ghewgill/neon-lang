#ifndef UTF8STRING_H
#define UTF8STRING_H

#include <string>
#include <vector>

#include <utf8.h>

class utf8string {
public:
    utf8string(): s(), indexes() {}
    utf8string(const utf8string &s): s(s.s), indexes(s.indexes) {}
    explicit utf8string(const std::string &s): s(s), indexes() {}
    explicit utf8string(const char *s): s(s), indexes() {}
    bool operator==(const utf8string &rhs) const { return s == rhs.s; }
    bool operator!=(const utf8string &rhs) const { return s != rhs.s; }
    bool operator<(const utf8string &rhs) const { return s < rhs.s; }
    bool operator>(const utf8string &rhs) const { return s > rhs.s; }
    bool operator<=(const utf8string &rhs) const { return s <= rhs.s; }
    bool operator>=(const utf8string &rhs) const { return s >= rhs.s; }
    utf8string &operator+=(char c) { push_back(c); return *this; }
    utf8string &operator+=(const char *t) { append(t); return *this; }
    void append(const char *t) { invalidate(); s.append(t); }
    void append(const std::string &t) { invalidate(); s.append(t); }
    void append(const utf8string &t) { invalidate(); s.append(t.s); }
    const char &at(std::string::size_type pos) const { return s.at(pos); }
    const char *c_str() const { return s.c_str(); }
    void clear() { invalidate(); s.clear(); }
    const char *data() const { return s.data(); }
    bool empty() const { return s.empty(); }
    std::string::size_type index(std::string::size_type i) const {
        if (indexes.empty()) {
            for (auto x = s.begin(); x != s.end(); utf8::advance(x, 1, s.end())) {
                indexes.push_back(x - s.begin());
            }
        }
        if (i < indexes.size()) {
            return indexes[i];
        }
        return s.length();
    }
    std::string::size_type length() const { return s.length(); }
    void push_back(std::string::value_type ch) { invalidate(); s.push_back(ch); }
    void reserve(std::string::size_type new_cap) { s.reserve(new_cap); }
    void resize(std::string::size_type count) { s.resize(count); }
    std::string::size_type size() const { return s.size(); }
    const std::string &str() const { return s; }
    std::string substr(std::string::size_type pos, std::string::size_type count) const { return s.substr(pos, count); }
private:
    std::string s;
    mutable std::vector<std::string::size_type> indexes;
    void invalidate() { indexes.clear(); }
};

inline utf8string operator+(utf8string s, const utf8string &t)
{
    s.append(t);
    return s;
}

inline utf8string operator+(const char *t, const utf8string &s) {
    utf8string r {t};
    r.append(s);
    return r;
}

inline utf8string operator+(utf8string s, const char *t) {
    s.append(t);
    return s;
}

#endif
