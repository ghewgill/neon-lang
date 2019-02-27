#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <map>
#include <vector>
#include <string>

class DebugInfo {
public:
    explicit DebugInfo(const std::string &source_path, const std::string &source);
    const std::string source_path;
    std::vector<std::string> source_lines;
    std::map<size_t, int> line_numbers;
    std::map<size_t, int> stack_depth;
private:
    DebugInfo(const DebugInfo &);
    DebugInfo operator=(const DebugInfo &);
};

#endif
