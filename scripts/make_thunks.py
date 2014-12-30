import re
import sys

VALUE = "VALUE"
REF = "REF"

AstFromCpp = {
    "void": ("TYPE_NOTHING", VALUE),
    "void*": ("TYPE_POINTER", VALUE),
    "bool": ("TYPE_BOOLEAN", VALUE),
    "Number": ("TYPE_NUMBER", VALUE),
    "std::string": ("TYPE_STRING", VALUE),
    "std::string&": ("TYPE_STRING", VALUE),
    "std::string*": ("TYPE_STRING", REF),
    "std::vector<Number>": ("TYPE_ARRAY_NUMBER", VALUE),
    "std::vector<Number>&": ("TYPE_ARRAY_NUMBER", VALUE),
    "std::vector<std::string>": ("TYPE_ARRAY_STRING", VALUE),
    "std::vector<std::string>&": ("TYPE_ARRAY_STRING", VALUE),
}

CppFromAst = {
    ("TYPE_NOTHING", VALUE): "void",
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_STRING", VALUE): "std::string",
    ("TYPE_STRING", REF): "std::string *",
    ("TYPE_ARRAY_NUMBER", VALUE): "std::vector<Number>",
    ("TYPE_ARRAY_STRING", VALUE): "std::vector<std::string>",
}

CppFromAstArg = {
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_STRING", VALUE): "const std::string &",
    ("TYPE_STRING", REF): "std::string *",
    ("TYPE_ARRAY_NUMBER", VALUE): "const std::vector<Number> &",
    ("TYPE_ARRAY_STRING", VALUE): "const std::vector<std::string> &",
}

CellField = {
    ("TYPE_POINTER", VALUE): "address()",
    ("TYPE_BOOLEAN", VALUE): "boolean()",
    ("TYPE_NUMBER", VALUE): "number()",
    ("TYPE_STRING", VALUE): "string()",
    ("TYPE_STRING", REF): "address()->string()",
    ("TYPE_ARRAY_NUMBER", VALUE): "array()",
    ("TYPE_ARRAY_STRING", VALUE): "array()",
}

ArrayElementField = {
    ("TYPE_ARRAY_NUMBER", VALUE): "number()",
    ("TYPE_ARRAY_STRING", VALUE): "string()",
}

constants = dict()
functions = dict()

for fn in sys.argv[1:]:
    with open(fn) as f:
        prefix = ""
        for s in f:
            if s.startswith("//") or s.startswith("static "):
                continue
            m = re.match(r"namespace (\w+) {", s)
            if m is not None and m.group(1) != "rtl":
                prefix = m.group(1) + "."
            if s.startswith("} // namespace"):
                prefix = ""
            m = re.match(r"extern\s+const\s+(\w+)\s+([\w$]+)\s*=", s)
            if m is not None:
                ctype = m.group(1)
                name = m.group(2)
                assert ctype in ["Number"]
                constants[name] = [name, ctype, "new ConstantNumberExpression(rtl::{})".format(name)]
            m = re.match(r"([\S\* ]+?)\s*([\w$]+)\((.*?)\)$", s)
            if m is not None:
                rtype = m.group(1).replace(" ", "")
                name = prefix + m.group(2)
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
        print >>inc, "static void thunk_{}_{}(std::stack<Cell> &{}, void *func)".format(rtype[0], "_".join("{}_{}".format(p, m) for p, m in params), "stack" if (params or rtype[0] != "TYPE_NOTHING") else "")
        print >>inc, "{"
        for i, a in reversed(list(enumerate(params))):
            if a[0].startswith("TYPE_ARRAY_"):
                print >>inc, "    {} a{};".format(CppFromAst[a], i)
                print >>inc, "    for (auto x: stack.top().array()) a{}.push_back(x.{});".format(i, ArrayElementField[a])
                print >>inc, "    stack.pop();"
            elif a[1] == REF:
                print >>inc, "    {} a{} = &stack.top().{}; stack.pop();".format(CppFromAst[a], i, CellField[a]);
            else:
                print >>inc, "    {} a{} = stack.top().{}; stack.pop();".format(CppFromAst[a], i, CellField[a]);
        if rtype[0] != "TYPE_NOTHING":
            print >>inc, "    auto r = reinterpret_cast<{} (*)({})>(func)({});".format(CppFromAst[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params))))
            if rtype[0].startswith("TYPE_ARRAY_"):
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

with open("src/constants_compile.inc", "w") as inc:
    print >>inc, "namespace rtl {"
    for name, ctype, init in constants.values():
        print >>inc, "extern const {} {};".format(ctype, name)
    print >>inc, "}"
    print >>inc, "static void init_builtin_constants(Scope *{})".format("scope" if any("$" not in x[0] for x in constants.values()) else "")
    print >>inc, "{"
    for name, ctype, init in constants.values():
        if "$" not in name:
            print >>inc, "    scope->addName(\"{}\", new Constant(\"{}\", {}));".format(name, name, init)
    print >>inc, "}";
    print >>inc, "static void init_builtin_constants(const std::string &module, Scope *scope)"
    print >>inc, "{"
    for name, ctype, init in constants.values():
        i = name.index("$")
        module = name[:i]
        modname = name[i+1:]
        print >>inc, "    if (module == \"{}\") scope->addName(\"{}\", new Constant(\"{}\", {}));".format(module, modname, modname, init)
    print >>inc, "}";

with open("src/functions_compile.inc", "w") as inc:
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    const Type *returntype;"
    print >>inc, "    struct {ParameterType::Mode m; const Type *p; } params[10];"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, params in functions.values():
        print >>inc, "    {{\"{}\", {}, {{{}}}}},".format(name, rtype[0], ",".join("{{ParameterType::{},{}}}".format("IN" if m == VALUE else "INOUT", p) for p, m in params))
    print >>inc, "};";

with open("src/functions_exec.inc", "w") as inc:
    print >>inc, "namespace rtl {"
    for name, rtype, params in functions.values():
        if "." in name:
            namespace, name = name.split(".")
            print >>inc, "namespace {} {{ extern {} {}({}); }}".format(namespace, CppFromAst[rtype], name, ", ".join(CppFromAstArg[x] for x in params))
        else:
            print >>inc, "extern {} {}({});".format(CppFromAst[rtype], name, ", ".join(CppFromAstArg[x] for x in params))
    print >>inc, "}"
    print >>inc, "static struct {"
    print >>inc, "    const char *name;"
    print >>inc, "    Thunk thunk;"
    print >>inc, "    void *func;"
    print >>inc, "} BuiltinFunctions[] = {"
    for name, rtype, params in functions.values():
        print >>inc, "    {{\"{}\", {}, reinterpret_cast<void *>(rtl::{})}},".format(name, "thunk_{}_{}".format(rtype[0], "_".join("{}_{}".format(p, m) for p, m in params)), name.replace(".", "::"))
    print >>inc, "};";
