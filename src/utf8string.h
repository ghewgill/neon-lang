#ifndef UTF8STRING_H
#define UTF8STRING_H

#include <string>
#include <vector>

#include <utf8.h>

class UTF8StringBuilder;

class utf8string {
public:
    utf8string(): s(), indexes(), character_length(std::string::npos) {}
    utf8string(const utf8string &s): s(s.s), indexes(s.indexes), character_length(s.character_length) {}
    explicit utf8string(const std::string &s): s(s), indexes(), character_length(std::string::npos) {}
    explicit utf8string(const char *s): s(s), indexes(), character_length(std::string::npos) {}
    explicit utf8string(const UTF8StringBuilder &s);
    utf8string &operator=(const utf8string &s) { if (this == &s) return *this; invalidate(); this->s = s.str(); return *this; }
    bool operator==(const utf8string &rhs) const { return s == rhs.s; }
    bool operator!=(const utf8string &rhs) const { return s != rhs.s; }
    bool operator<(const utf8string &rhs) const { return s < rhs.s; }
    bool operator>(const utf8string &rhs) const { return s > rhs.s; }
    bool operator<=(const utf8string &rhs) const { return s <= rhs.s; }
    bool operator>=(const utf8string &rhs) const { return s >= rhs.s; }
    char at(std::string::size_type pos) const { return s.at(pos); }
    const char *c_str() const { return s.c_str(); }
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
    std::string::size_type length() const {
        if (character_length == std::string::npos) {
            character_length = 0;
            for (auto x = s.begin(); x != s.end(); utf8::advance(x, 1, s.end())) {
                character_length++;
            }
        }
        return character_length;
    }
    std::string::size_type size() const { return s.size(); }
    const std::string &str() const { return s; }
    std::string substr(std::string::size_type pos, std::string::size_type count) const { return s.substr(pos, count); }
private:
    std::string s;
    mutable std::vector<std::string::size_type> indexes;
    mutable std::string::size_type character_length;
    void invalidate() {
        indexes.clear();
        character_length = std::string::npos;
    }
    friend utf8string operator+(const utf8string &s, const utf8string &t);
    friend utf8string operator+(const char *t, const utf8string &s);
    friend utf8string operator+(const utf8string &s, const char *t);
};

inline utf8string operator+(const utf8string &s, const utf8string &t)
{
    return utf8string(s.s + t.s);
}

inline utf8string operator+(const char *t, const utf8string &s) {
    return utf8string(t + s.s);
}

inline utf8string operator+(const utf8string &s, const char *t) {
    return utf8string(s.s + t);
}

class UTF8StringBuilder {
public:
    UTF8StringBuilder() {}
    UTF8StringBuilder(const char *s): s(s) {}
    void append(char c) { this->s.push_back(c); }
    void append(const char *s) { this->s.append(s); }
    void append(const std::string &s) { this->s.append(s); }
    void append(const utf8string &s) { this->s.append(s.str()); }
    size_t length() const { return s.length(); }
    std::string str() const { return s; }
private:
    std::string s;
};

inline utf8string::utf8string(const UTF8StringBuilder &s): s(s.str()), indexes(), character_length(s.length()) {}

#endif
