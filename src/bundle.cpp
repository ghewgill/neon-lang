#include "bundle.h"

#include <fstream>
#include <iostream>
#include <iso646.h>
#include <map>
#include <sstream>
#include <vector>

#include <unzip.h>

#include "bytecode.h"
#include "exec.h"
#include "support.h"

std::map<std::string, std::vector<unsigned char>> g_Contents;

class ZipSupport: public ICompilerSupport {
public:
    virtual bool loadBytecode(const std::string &module, Bytecode &bytecode) override;
    virtual void writeOutput(const std::string &, const std::vector<unsigned char> &) override {}
};

bool ZipSupport::loadBytecode(const std::string &module, Bytecode &bytecode)
{
    const std::string module_name = module + ".neonx";
    auto i = g_Contents.find(module_name);
    if (i == g_Contents.end()) {
        return false;
    }
    if (not bytecode.load(module_name, i->second)) {
        return false;
    }
    return true;
}

void run_from_bundle(const std::string &name, bool enable_assert, unsigned short debug_port, int argc, char *argv[])
{
    unzFile zip = unzOpen(name.c_str());
    if (zip == NULL) {
        fprintf(stderr, "zip open error\n");
        exit(1);
    }
    int r = unzGoToFirstFile(zip);
    if (r != UNZ_OK) {
        fprintf(stderr, "zip first\n");
        exit(1);
    }
    for (;;) {
        unz_file_info file_info;
        char name[256];
        r = unzGetCurrentFileInfo(zip, &file_info, name, sizeof(name), NULL, 0, NULL, 0);
        if (r != UNZ_OK) {
            fprintf(stderr, "zip file info\n");
            exit(1);
        }
        r = unzOpenCurrentFile(zip);
        if (r != UNZ_OK) {
            fprintf(stderr, "zip file open\n");
            exit(1);
        }
        std::vector<unsigned char> bytecode;
        for (;;) {
            char buf[4096];
            int n = unzReadCurrentFile(zip, buf, sizeof(buf));
            if (n == 0) {
                break;
            } else if (n < 0) {
                fprintf(stderr, "zip read\n");
                exit(1);
            }
            for (auto i = 0; i < n; i++) {
                bytecode.push_back(buf[i]);
            }
        }
        unzCloseCurrentFile(zip);
        g_Contents[name] = bytecode;
        r = unzGoToNextFile(zip);
        if (r != UNZ_OK) {
            break;
        }
    }
    unzClose(zip);

    ZipSupport zip_support;

    exec(name, g_Contents[".neonx"], nullptr, &zip_support, enable_assert, debug_port, argc, argv);
}
