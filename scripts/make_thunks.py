import re
import sys

AstFromCpp = {
    "void": "TYPE_NOTHING",
    "void*": "TYPE_POINTER",
    "bool": "TYPE_BOOLEAN",
    "Number": "TYPE_NUMBER",
    "std::string": "TYPE_STRING",
    "std::string&": "TYPE_STRING",
    "std::vector<Number>": "TYPE_ARRAY_NUMBER",
    "std::vector<Number>&": "TYPE_ARRAY_NUMBER",
    "std::vector<std::string>": "TYPE_ARRAY_STRING",
    "std::vector<std::string>&": "TYPE_ARRAY_STRING",
}

CppFromAst = {
    "TYPE_NOTHING": "void",
    "TYPE_POINTER": "void *",
    "TYPE_BOOLEAN": "bool",
    "TYPE_NUMBER": "Number",
    "TYPE_STRING": "std::string",
    "TYPE_ARRAY_NUMBER": "std::vector<Number>",
    "TYPE_ARRAY_STRING": "std::vector<std::string>",
}

CppFromAstArg = {
    "TYPE_POINTER": "void *",
    "TYPE_BOOLEAN": "bool",
    "TYPE_NUMBER": "Number",
    "TYPE_STRING": "const std::string &",
    "TYPE_ARRAY_NUMBER": "const std::vector<Number> &",
    "TYPE_ARRAY_STRING": "const std::vector<std::string> &",
}

CellField = {
    "TYPE_POINTER": "address()",
    "TYPE_BOOLEAN": "boolean()",
    "TYPE_NUMBER": "number()",
    "TYPE_STRING": "string()",
    "TYPE_ARRAY_NUMBER": "array()",
    "TYPE_ARRAY_STRING": "array()",
}

ArrayElementField = {
    "TYPE_ARRAY_NUMBER": "number()",
    "TYPE_ARRAY_STRING": "string()",
}

functions = dict()

for fn in sys.argv[1:]:
    with open(fn) as f:
        for s in f:
            if s.startswith("//") or s.startswith("static "):
                continue
            m = re.match(r"([\S\* ]+?)\s*([\w$]+)\((.*?)\)$", s)
            if m is not None:
                rtype = m.group(1).replace(" ", "")
                name = m.group(2)
                paramstr = m.group(3).split(",")
                if name.startswith("rtl_"):
                    continue
                assert rtype in ["void", "bool", "Number", "std::string", "std::vector<Number>", "std::vector<std::string>", "void*"], rtype
                params = []
                if paramstr[0]:
                    for arg in paramstr:
                        arg = arg.strip()
                        if arg.startswith("const"):
                            arg = arg[5:]
                        arg = re.sub(r"\w+$", "", arg)
                        arg = arg.replace(" ", "")
                        params.append(arg)
                functions[name] = [name, AstFromCpp[rtype], [AstFromCpp[x] for x in params]]

thunks = set()

for name, rtype, params in functions.values():
    thunks.add((rtype, tuple(params)))

with open("src/thunks.inc", "w") as inc:
    for rtype, params in thunks:
        print >>inc, "static void thunk_{}_{}(std::stack<Cell> &{}, void *func)".format(rtype, "_".join(params), "stack" if (params or rtype != "TYPE_NOTHING") else "")
        print >>inc, "{"
        for i, a in reversed(list(enumerate(params))):
            if a.startswith("TYPE_ARRAY_"):
                print >>inc, "    {} a{};".format(CppFromAst[a], i)
                print >>inc, "    for (auto x: stack.top().array()) a{}.push_back(x.{});".format(i, ArrayElementField[a])
                print >>inc, "    stack.pop();"
            else:
                print >>inc, "    {} a{} = stack.top().{}; stack.pop();".format(CppFromAst[a], i, CellField[a]);
        if rtype != "TYPE_NOTHING":
            print >>inc, "    auto r = reinterpret_cast<{} (*)({})>(func)({});".format(CppFromAst[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params))))
            if rtype.startswith("TYPE_ARRAY_"):
                print >>inc, "    Cell t;"
                print >>inc, "    for (auto x: r) t.array().push_back(Cell(x));"
                print >>inc, "    stack.push(t);"
            elif rtype == "TYPE_POINTER":
                print >>inc, "    stack.push(Cell(static_cast<Cell *>(r)));"
            else:
                print >>inc, "    stack.push(Cell(r));"
        else:
            print >>inc, "    reinterpret_cast<{} (*)({})>(func)({});".format(CppFromAst[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params))))
        print >>inc, "}"
        print >>inc, ""

with open("src/functions_compile.inc", "w") as inc:
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    const Type *returntype;"
    print >>inc, "    const Type *params[10];"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, params in functions.values():
        print >>inc, "    {{\"{}\", {}, {{{}}}}},".format(name, rtype, ",".join(params))
    print >>inc, "};";

with open("src/functions_exec.inc", "w") as inc:
    print >>inc, "namespace rtl {"
    for name, rtype, params in functions.values():
        print >>inc, "extern {} {}({});".format(CppFromAst[rtype], name, ", ".join(CppFromAstArg[x] for x in params))
    print >>inc, "}"
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    Thunk thunk;"
    print >>inc, "    void *func;"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, params in functions.values():
        print >>inc, "    {{\"{}\", {}, reinterpret_cast<void *>(rtl::{})}},".format(name, "thunk_{}_{}".format(rtype, "_".join(params)), name)
    print >>inc, "};";
