import re

AstFromCpp = {
    "void": "TYPE_NOTHING",
    "bool": "TYPE_BOOLEAN",
    "Number": "TYPE_NUMBER",
    "std::string": "TYPE_STRING",
}

CppFromAst = {
    "TYPE_NOTHING": "void",
    "TYPE_BOOLEAN": "bool",
    "TYPE_NUMBER": "Number",
    "TYPE_STRING": "std::string",
}

CppFromAstArg = {
    "TYPE_BOOLEAN": "bool",
    "TYPE_NUMBER": "Number",
    "TYPE_STRING": "const std::string &",
}

CellField = {
    "TYPE_BOOLEAN": "boolean_value",
    "TYPE_NUMBER": "number_value",
    "TYPE_STRING": "string_value",
}

functions = dict()

with open("rtl.cpp") as f:
    for s in f:
        m = re.match("(\S+)\s+([\w$]+)\((.*?)\)$", s)
        if m is not None:
            rtype = m.group(1)
            name = m.group(2)
            paramstr = m.group(3).split(",")
            if name.startswith("rtl_"):
                continue
            assert rtype in ["void", "bool", "Number", "std::string"], rtype
            params = []
            if paramstr[0]:
                for arg in paramstr:
                    for a in arg.split():
                        if a == "const":
                            continue
                        assert a in ["bool", "Number", "std::string"], (arg, a)
                        break
                    params.append(a)
            functions[name] = [name, AstFromCpp[rtype], [AstFromCpp[x] for x in params]]

thunks = set()

for name, rtype, params in functions.values():
    thunks.add((rtype, tuple(params)))

with open("thunks.inc", "w") as inc:
    for rtype, params in thunks:
        print >>inc, "static void thunk_{}_{}(std::stack<Variant> &stack, void *func)".format(rtype, "_".join(params))
        print >>inc, "{"
        for i, a in reversed(list(enumerate(params))):
            print >>inc, "    {} a{} = stack.top().{}; stack.pop();".format(CppFromAst[a], i, CellField[a]);
        if rtype != "TYPE_NOTHING":
            print >>inc, "    stack.push(Variant(reinterpret_cast<{} (*)({})>(func)({})));".format(CppFromAst[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params))))
        else:
            print >>inc, "    reinterpret_cast<{} (*)({})>(func)({});".format(CppFromAst[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params))))
        print >>inc, "}"
        print >>inc, ""

with open("functions.inc", "w") as inc:
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    const Type *returntype;"
    print >>inc, "    const Type *params[10];"
    print >>inc, "    Thunk thunk;"
    print >>inc, "    void *func;"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, params in functions.values():
        print >>inc, "    {{\"{}\", {}, {{{}}}, {}, reinterpret_cast<void *>(rtl::{})}},".format(name, rtype, ",".join(params), "thunk_{}_{}".format(rtype, "_".join(params)), name)
    print >>inc, "};";
