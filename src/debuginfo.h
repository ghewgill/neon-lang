#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <map>
#include <vector>
#include <string>

class DebugInfo {
public:
    explicit DebugInfo(const std::string &source);
    std::vector<std::string> source_lines;
    std::map<size_t, int> line_numbers;
};

#endif
