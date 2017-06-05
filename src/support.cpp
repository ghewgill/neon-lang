#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "support.h"

#include <cstdlib>
#include <fstream>
#include <iso646.h>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#define access _access
#define R_OK 04
#define PATHSEP ';'
#else
#include <unistd.h>
#define PATHSEP ':'
#endif

#include "bytecode.h"

namespace {

std::vector<std::string> split(const std::string &s, char d)
{
    std::vector<std::string> r;
    std::string::size_type i = 0;
    auto start = i;
    while (i < s.length()) {
        if (s.at(i) == d) {
            r.push_back(s.substr(start, i-start));
            start = i + 1;
        }
        i++;
    }
    if (i > start) {
        r.push_back(s.substr(start, i-start));
    }
    return r;
}

} // namespace

PathSupport::PathSupport(const std::string &source_path)
  : paths()
{
    if (not source_path.empty()) {
        paths.push_back(source_path);
    }
    paths.push_back("./");
    const char *neonpath = std::getenv("NEONPATH");
    if (neonpath != NULL) {
        std::vector<std::string> p = split(neonpath, PATHSEP);
        std::copy(p.begin(), p.end(), std::back_inserter(paths));
    }
    std::ifstream inf(".neonpath");
    if (inf.good()) {
        std::string s;
        while (std::getline(inf, s)) {
            paths.push_back(s);
        }
    }
}

std::pair<std::string, std::string> PathSupport::findModule(const std::string &name)
{
    std::string module_name = name;
    std::vector<std::string> use_paths = paths;
    std::string::size_type slash = name.find_last_of("/\\");
    if (slash != std::string::npos) {
        use_paths.clear();
        use_paths.push_back(name.substr(0, slash+1));
        module_name = name.substr(slash+1);
    }
    const std::string source_name = module_name + ".neon";
    const std::string object_name = module_name + ".neonx";
    std::pair<std::string, std::string> r;
    for (auto p: use_paths) {
        if (access((p + source_name).c_str(), R_OK) == 0) {
            r.first = p + source_name;
        }
        if (access((p + object_name).c_str(), R_OK) == 0) {
            r.second = p + object_name;
        }
        if (r.first.empty() && r.second.empty()) {
            if (access((p + module_name + "/" + source_name).c_str(), R_OK) == 0) {
                r.first = p + module_name + "/" + source_name;
            }
            if (access((p + module_name + "/" + object_name).c_str(), R_OK) == 0) {
                r.second = p + module_name + "/" + object_name;
            }
        }
        if (not r.first.empty() || not r.second.empty()) {
            break;
        }
    }
    return r;
}
