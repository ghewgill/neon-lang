#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <fstream>
#include <iso646.h>
#include <iterator>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <vector>

#ifdef _MSC_VER
#define unlink _unlink
#else
#include <unistd.h>
#endif

#include <zip.h>

#include "bytecode.h"
#include "support.h"

#ifdef _WIN32
#define DEFAULT_STUB_NAME "neonstub.exe"
#else
#define DEFAULT_STUB_NAME "neonstub"
#endif

RuntimeSupport support(".");

void get_modules(const Bytecode &obj, std::map<std::string, Bytecode> &modules)
{
    for (auto x: obj.imports) {
        std::string name = obj.strtable[x.name];
        if (modules.find(name) == modules.end()) {
            Bytecode bytecode;
            if (support.loadBytecode(name, bytecode)) {
                modules[name] = bytecode;
                get_modules(bytecode, modules);
            } else {
                fprintf(stderr, "could not load module: %s\n", name.c_str());
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    bool executable = false;
    std::string stub_name = DEFAULT_STUB_NAME;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-e] bundle module\n", argv[0]);
        exit(1);
    }

    int a = 1;
    while (a < argc) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-e") == 0) {
                executable = true;
            } else if (strcmp(argv[a], "-stub") == 0) {
                a++;
                stub_name = argv[a];
            } else {
                fprintf(stderr, "Unknown option: %s\n", argv[a]);
            }
        } else {
            break;
        }
        a++;
    }

    const char *bundle = argv[a];
    const char *module = argv[a+1];

    std::map<std::string, Bytecode> modules;
    {
        std::ifstream inf(module, std::ios::binary);
        if (not inf.good()) {
            fprintf(stderr, "could not find: %s\n", module);
            exit(1);
        }
        std::stringstream buf;
        buf << inf.rdbuf();
        std::vector<unsigned char> bytecode;
        std::string s = buf.str();
        std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
        Bytecode b;
        if (not b.load(module, bytecode)) {
            fprintf(stderr, "could not load bytecode\n");
            exit(1);
        }
        modules[""] = b;
    }
    get_modules(modules[""], modules);

    const std::string zipname = executable ? std::string(bundle) + ".zip" : bundle;
    {
        zipFile zip = zipOpen(zipname.c_str(), 0);
        if (zip == NULL) {
            fprintf(stderr, "zip open error\n");
            exit(1);
        }
        for (auto m: modules) {
            int r = zipOpenNewFileInZip(zip, (m.first+".neonx").c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
            if (r != ZIP_OK) {
                fprintf(stderr, "zip open error\n");
                exit(1);
            }
            r = zipWriteInFileInZip(zip, m.second.obj.data(), static_cast<unsigned int>(m.second.obj.size()));
            zipCloseFileInZip(zip);
        }
        zipClose(zip, NULL);
    }

    if (executable) {
        FILE *stub = fopen(("bin/"+stub_name).c_str(), "rb");
        if (stub == NULL) {
            fprintf(stderr, "stub open (%s)\n", stub_name.c_str());
            exit(1);
        }
        FILE *zip = fopen(zipname.c_str(), "rb");
        if (zip == NULL) {
            fprintf(stderr, "zip open\n");
            exit(1);
        }
        FILE *exe = fopen(bundle, "wb");
        if (exe == NULL) {
            fprintf(stderr, "exe open\n");
            exit(1);
        }
        for (;;) {
            char buf[4096];
            size_t n = fread(buf, 1, sizeof(buf), stub);
            if (n == 0) {
                break;
            }
            fwrite(buf, 1, n, exe);
        }
        for (;;) {
            char buf[4096];
            size_t n = fread(buf, 1, sizeof(buf), zip);
            if (n == 0) {
                break;
            }
            fwrite(buf, 1, n, exe);
        }
        fclose(stub);
        fclose(zip);
        #ifndef _WIN32
            fchmod(fileno(exe), 0755);
        #endif
        fclose(exe);
        unlink(zipname.c_str());
    }
}
