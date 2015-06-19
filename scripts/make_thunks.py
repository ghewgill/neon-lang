import os
import re
import sys

VALUE = "VALUE"
REF = "REF"

AstFromNeon = {
    None: ("TYPE_NOTHING", VALUE),
    "POINTER": ("TYPE_POINTER", VALUE),
    "Cell": ("TYPE_GENERIC", VALUE),
    "Boolean": ("TYPE_BOOLEAN", VALUE),
    "Number": ("TYPE_NUMBER", VALUE),
    "INOUT Number": ("TYPE_NUMBER", REF),
    "OUT Number": ("TYPE_NUMBER", REF),
    "String": ("TYPE_STRING", VALUE),
    "INOUT String": ("TYPE_STRING", REF),
    "Bytes": ("TYPE_BYTES", VALUE),
    "INOUT Bytes": ("TYPE_BYTES", REF),
    "Array": ("TYPE_GENERIC", VALUE),
    "INOUT Array": ("TYPE_GENERIC", REF),
    "Array<Number>": ("TYPE_ARRAY_NUMBER", VALUE),
    "Array<String>": ("TYPE_ARRAY_STRING", VALUE),
    "INOUT Array<String>": ("TYPE_ARRAY_STRING", REF),
    "Dictionary": ("TYPE_GENERIC", VALUE),
}

CppFromAstParam = {
    ("TYPE_NOTHING", VALUE): "void",
    ("TYPE_GENERIC", VALUE): "Cell",
    ("TYPE_GENERIC", REF): "Cell *",
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_POINTER", REF): "Cell **",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_BOOLEAN", REF): "bool *",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_NUMBER", REF): "Number *",
    ("TYPE_STRING", VALUE): "const std::string &",
    ("TYPE_STRING", REF): "utf8string *",
    ("TYPE_BYTES", VALUE): "const std::string &",
    ("TYPE_BYTES", REF): "utf8string *",
    ("TYPE_ARRAY_NUMBER", VALUE): "std::vector<Number>",
    ("TYPE_ARRAY_STRING", VALUE): "std::vector<utf8string>",
    ("TYPE_ARRAY_STRING", REF): "std::vector<utf8string>",
}

CppFromAstReturn = {
    ("TYPE_NOTHING", VALUE): "void",
    ("TYPE_GENERIC", VALUE): "Cell",
    ("TYPE_GENERIC", REF): "Cell *",
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_POINTER", REF): "Cell **",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_BOOLEAN", REF): "bool *",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_NUMBER", REF): "Number *",
    ("TYPE_STRING", VALUE): "std::string",
    ("TYPE_STRING", REF): "std::string *",
    ("TYPE_BYTES", VALUE): "std::string",
    ("TYPE_BYTES", REF): "std::string *",
    ("TYPE_ARRAY_NUMBER", VALUE): "std::vector<Number>",
    ("TYPE_ARRAY_STRING", VALUE): "std::vector<utf8string>",
    ("TYPE_ARRAY_STRING", REF): "std::vector<utf8string> *",
}

CppFromAstArg = {
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_POINTER", REF): "Cell **",
    ("TYPE_GENERIC", VALUE): "Cell &",
    ("TYPE_GENERIC", REF): "Cell *",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_BOOLEAN", REF): "bool *",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_NUMBER", REF): "Number *",
    ("TYPE_STRING", VALUE): "const std::string &",
    ("TYPE_STRING", REF): "utf8string *",
    ("TYPE_BYTES", VALUE): "const std::string &",
    ("TYPE_BYTES", REF): "utf8string *",
    ("TYPE_ARRAY_NUMBER", VALUE): "const std::vector<Number> &",
    ("TYPE_ARRAY_STRING", VALUE): "const std::vector<utf8string> &",
    ("TYPE_ARRAY_STRING", REF): "std::vector<utf8string> *",
}

CellField = {
    ("TYPE_POINTER", VALUE): "address()",
    ("TYPE_POINTER", REF): "address()->address()",
    ("TYPE_BOOLEAN", VALUE): "boolean()",
    ("TYPE_BOOLEAN", REF): "address()->boolean()",
    ("TYPE_NUMBER", VALUE): "number()",
    ("TYPE_NUMBER", REF): "address()->number()",
    ("TYPE_STRING", VALUE): "string().str()",
    ("TYPE_STRING", REF): "address()->string_for_write()",
    ("TYPE_BYTES", VALUE): "string().str()",
    ("TYPE_BYTES", REF): "address()->string_for_write()",
    ("TYPE_ARRAY_NUMBER", VALUE): "array()",
    ("TYPE_ARRAY_STRING", VALUE): "array()",
    ("TYPE_ARRAY_STRING", REF): "array()",
}

ArrayElementField = {
    ("TYPE_ARRAY_NUMBER", VALUE): "number()",
    ("TYPE_ARRAY_STRING", VALUE): "string()",
    ("TYPE_ARRAY_STRING", REF): "string()",
}

def parse_params(paramstr):
    r = []
    if not paramstr:
        return r
    def next_token(s, i):
        m = re.match(r"\s*([\w<>]+|.)", s[i:])
        if m is None:
            return "", i
        t = m.group(1)
        return t, i + m.end(0)
    i = 0
    while True:
        mode = ""
        n = 0
        while True:
            t, i = next_token(paramstr, i)
            assert re.match(r"[\w_]+$", t), (paramstr, i, t)
            if t in ["OUT", "INOUT"]:
                mode = t
                t, i = next_token(paramstr, i)
            n += 1
            t, i = next_token(paramstr, i)
            if t != ",":
                break
        assert t == ":", (paramstr, i, t)
        t, i = next_token(paramstr, i)
        typename = t
        r.extend(["{} {}".format(mode, typename).strip()] * n)
        t, i = next_token(paramstr, i)
        if t != ",":
            break
    return r

constants = dict()
functions = dict()

for fn in sys.argv[1:]:
    if fn.endswith(".neon"):
        prefix = os.path.basename(fn)[:-5] + "$"
        if prefix == "global$":
            prefix = ""
        with open(fn) as f:
            for s in f:
                a = s.split()
                if a[:1] == ["TYPE"]:
                    m = re.match("TYPE\s+(\w+)\s*:=\s*(\S+)\s*$", s)
                    name = m.group(1)
                    atype = m.group(2)
                    if atype == "POINTER":
                        AstFromNeon[name] = ("TYPE_POINTER", VALUE)
                        AstFromNeon["INOUT "+name] = ("TYPE_POINTER", REF)
                    else:
                        AstFromNeon[name] = ("TYPE_GENERIC", VALUE)
                        AstFromNeon["INOUT "+name] = ("TYPE_GENERIC", REF)
                if a[:3] == ["DECLARE", "NATIVE", "FUNCTION"]:
                    m = re.search(r"(\w+)\((.*?)\)(:\s*(\S+))?\s*$", s)
                    assert m is not None
                    name = prefix + m.group(1)
                    paramstr = m.group(2)
                    rtype = m.group(4)
                    params = parse_params(paramstr)
                    functions[name] = [name, AstFromNeon[rtype], rtype.split()[-1] if rtype is not None else None, [AstFromNeon[x] for x in params], [x.split()[-1] for x in params]]
                if a[:3] == ["DECLARE", "NATIVE", "CONST"]:
                    m = re.search(r"(\w+):\s*(\S+)\s*$", s)
                    assert m is not None
                    name = prefix + m.group(1)
                    ctype = m.group(2)
                    assert ctype in ["Number"]
                    constants[name] = [name, ctype, "new ConstantNumberExpression(rtl::{})".format(name)]
    else:
        print >>sys.stderr, "Unexpected file: {}".format(fn)
        sys.exit(1)

thunks = set()

for name, rtype, rtypename, params, paramtypes in functions.values():
    thunks.add((rtype, tuple(params)))

with open("src/thunks.inc", "w") as inc:
    for rtype, params in thunks:
        print >>inc, "static void thunk_{}_{}(opstack<Cell> &{}, void *func)".format(rtype[0], "_".join("{}_{}".format(p, m) for p, m in params), "stack" if (params or rtype[0] != "TYPE_NOTHING") else "")
        print >>inc, "{"
        for i, a in reversed(list(enumerate(params))):
            d = len(params) - 1 - i
            if a[0].startswith("TYPE_ARRAY_") and a[1] == VALUE:
                print >>inc, "    {} a{};".format(CppFromAstParam[a], i)
                print >>inc, "    for (auto x: stack.peek({}).array()) a{}.push_back(x.{});".format(d, i, ArrayElementField[a])
            elif a[0].startswith("TYPE_ARRAY_") and a[1] == REF:
                print >>inc, "    {} t{};".format(CppFromAstParam[a], i)
                print >>inc, "    for (auto x: stack.peek({}).address()->array()) t{}.push_back(x.{});".format(d, i, ArrayElementField[a])
                print >>inc, "    {} *a{} = &t{};".format(CppFromAstParam[a], i, i)
            elif a == ("TYPE_GENERIC", VALUE):
                print >>inc, "    {} a{} = stack.peek({});".format(CppFromAstParam[a], i, d)
            elif a == ("TYPE_GENERIC", REF):
                print >>inc, "    {} a{} = stack.peek({}).address();".format(CppFromAstParam[a], i, d)
            elif a[1] == REF:
                print >>inc, "    {} a{} = &stack.peek({}).{};".format(CppFromAstParam[a], i, d, CellField[a])
            else:
                print >>inc, "    {} a{} = stack.peek({}).{};".format(CppFromAstParam[a], i, d, CellField[a])
        print >>inc, "    try {"
        print >>inc, "        {}reinterpret_cast<{} (*)({})>(func)({});".format("auto r = " if rtype[0] != "TYPE_NOTHING" else "", CppFromAstReturn[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params))))
        for i, a in reversed(list(enumerate(params))):
            d = len(params) - 1 - i
            if a[0].startswith("TYPE_ARRAY_") and a[1] == REF:
                print >>inc, "        stack.peek({}).address()->array_for_write().clear();".format(d)
                print >>inc, "        for (auto x: t{}) stack.peek({}).address()->array_for_write().push_back(Cell(x));".format(i, d)
        if params:
            print >>inc, "        stack.drop({});".format(len(params))
        if rtype[0] != "TYPE_NOTHING":
            if rtype[0].startswith("TYPE_ARRAY_"):
                print >>inc, "        std::vector<Cell> t;"
                print >>inc, "        for (auto x: r) t.push_back(Cell(x));"
                print >>inc, "        stack.push(Cell(t));"
            elif rtype[0] == "TYPE_POINTER":
                print >>inc, "        stack.push(Cell(static_cast<Cell *>(r)));"
            else:
                print >>inc, "        stack.push(Cell(r));"
        print >>inc, "    } catch (RtlException &) {"
        if params:
            print >>inc, "        stack.drop({});".format(len(params))
        print >>inc, "        throw;"
        print >>inc, "    }"
        print >>inc, "}"
        print >>inc, ""

with open("src/constants_compile.inc", "w") as inc:
    print >>inc, "namespace rtl {"
    for name, ctype, init in constants.values():
        print >>inc, "extern const {} {};".format(ctype, name)
    print >>inc, "}"
    print >>inc, "static void init_builtin_constants(Scope *{})".format("scope" if any("$" not in x[0] for x in constants.values()) else "")
    print >>inc, "{"
    for name, ctype, init in constants.values():
        if "$" not in name:
            print >>inc, "    scope->addName(Token(), \"{}\", new Constant(Token(), \"{}\", {}));".format(name, name, init)
    print >>inc, "}";
    print >>inc, "static void init_builtin_constants(const std::string &{}, Scope *{})".format("module" if any("$" in x[0] for x in constants.values()) else "", "scope" if any("$" in x[0] for x in constants.values()) else "")
    print >>inc, "{"
    for name, ctype, init in constants.values():
        i = name.index("$")
        module = name[:i]
        modname = name[i+1:]
        print >>inc, "    if (module == \"{}\") scope->addName(Token(), \"{}\", new Constant(Token(), \"{}\", {}));".format(module, modname, modname, init)
    print >>inc, "}";

with open("src/functions_compile.inc", "w") as inc:
    print >>inc, "struct PredefinedType {"
    print >>inc, "    const Type *type;"
    print >>inc, "    const char *modtypename;"
    print >>inc, "};"
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    PredefinedType returntype;"
    print >>inc, "    int count;"
    print >>inc, "    struct {ParameterType::Mode m; PredefinedType ptype; } params[10];"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, rtypename, params, paramtypes in functions.values():
        print >>inc, "    {{\"{}\", {{{}, {}}}, {}, {{{}}}}},".format(name, rtype[0] if rtype[0] != "TYPE_GENERIC" else "nullptr", "\"{}\"".format(rtypename) or "nullptr", len(params), ",".join("{{ParameterType::{},{{{},{}}}}}".format("IN" if m == VALUE else "INOUT", p if p != "TYPE_GENERIC" else "nullptr", "\"{}\"".format(t) or "nullptr") for (p, m), t in zip(params, paramtypes)))
    print >>inc, "};";

with open("src/functions_exec.inc", "w") as inc:
    print >>inc, "namespace rtl {"
    for name, rtype, rtypename, params, paramtypes in functions.values():
        if "." in name:
            namespace, name = name.split(".")
            print >>inc, "namespace {} {{ extern {} {}({}); }}".format(namespace, CppFromAstReturn[rtype], name, ", ".join(CppFromAstArg[x] for x in params))
        else:
            print >>inc, "extern {} {}({});".format(CppFromAstReturn[rtype], name, ", ".join(CppFromAstArg[x] for x in params))
    print >>inc, "}"
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    Thunk thunk;"
    print >>inc, "    void *func;"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, rtypename, params, paramtypes in functions.values():
        print >>inc, "    {{\"{}\", {}, reinterpret_cast<void *>(rtl::{})}},".format(name, "thunk_{}_{}".format(rtype[0], "_".join("{}_{}".format(p, m) for p, m in params)), name.replace(".", "::"))
    print >>inc, "};";
