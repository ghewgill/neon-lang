#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "support.h"

#include <fstream>
#include <iso646.h>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define PATHSEP ';'
#else
#include <sys/types.h>
#include <dirent.h>
#define PATHSEP ':'
#endif

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
#ifdef _WIN32
        WIN32_FIND_DATA fd;
        HANDLE ff = FindFirstFile((p + module_name + ".neon*").c_str(), &fd);
        if (ff != INVALID_HANDLE_VALUE) {
            do {
                std::string fn = fd.cFileName;
                if (fn == source_name) {
                    r.first = p + source_name;
                } else if (fn == object_name) {
                    r.second = p + object_name;
                }
            } while (FindNextFile(ff, &fd));
            FindClose(ff);
        }
#else
        DIR *d = opendir(p.c_str());
        if (d != NULL) {
            for (;;) {
                struct dirent *de = readdir(d);
                if (de == NULL) {
                    break;
                }
                std::string fn = de->d_name;
                if (fn == source_name) {
                    r.first = p + source_name;
                } else if (fn == object_name) {
                    r.second = p + object_name;
                }
            }
            closedir(d);
        }
#endif
        if (not r.first.empty() || not r.second.empty()) {
            break;
        }
    }
    return r;
}

bool RuntimeSupport::loadBytecode(const std::string &name, Bytecode &object)
{
    std::pair<std::string, std::string> names = findModule(name);
    std::ifstream inf(names.second, std::ios::binary);
    if (not inf.good()) {
        return false;
    }
    std::stringstream buf;
    buf << inf.rdbuf();
    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    if (not object.load(bytecode)) {
        return false;
    }
    return true;
}
