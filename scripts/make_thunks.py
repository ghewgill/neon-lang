#!/usr/bin/env python3

import os
import re
import sys

VALUE = "VALUE"
REF = "REF"
OUT = "OUT"

AstFromNeon = {
    None: ("TYPE_NOTHING", VALUE),
    "POINTER": ("TYPE_POINTER", VALUE),
    "Cell": ("TYPE_GENERIC", VALUE),
    "Boolean": ("TYPE_BOOLEAN", VALUE),
    "Number": ("TYPE_NUMBER", VALUE),
    "INOUT Number": ("TYPE_NUMBER", REF),
    "OUT Number": ("TYPE_NUMBER", OUT),
    "String": ("TYPE_STRING", VALUE),
    "INOUT String": ("TYPE_STRING", REF),
    "OUT String": ("TYPE_STRING", OUT),
    "Bytes": ("TYPE_BYTES", VALUE),
    "INOUT Bytes": ("TYPE_BYTES", REF),
    "OUT Bytes": ("TYPE_BYTES", OUT),
    "Object": ("TYPE_OBJECT", VALUE),
    "Array": ("TYPE_ARRAY", VALUE),
    "INOUT Array": ("TYPE_ARRAY", REF),
    "Array<Number>": ("TYPE_ARRAY_NUMBER", VALUE),
    "Array<String>": ("TYPE_ARRAY_STRING", VALUE),
    "INOUT Array<String>": ("TYPE_ARRAY_STRING", REF),
    "OUT Array<String>": ("TYPE_ARRAY_STRING", OUT),
    "Array<Object>": ("TYPE_ARRAY_OBJECT", VALUE),
    "Dictionary": ("TYPE_DICTIONARY", VALUE),
    "INOUT Dictionary": ("TYPE_DICTIONARY", REF),
    "Dictionary<Number>": ("TYPE_DICTIONARY_NUMBER", VALUE),
    "Dictionary<String>": ("TYPE_DICTIONARY_STRING", VALUE),
    "Dictionary<Object>": ("TYPE_DICTIONARY_OBJECT", VALUE),
}

CppFromAstParam = {
    ("TYPE_NOTHING", VALUE): "void",
    ("TYPE_GENERIC", VALUE): "Cell",
    ("TYPE_GENERIC", REF): "Cell *",
    ("TYPE_GENERIC", OUT): "Cell",
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_POINTER", REF): "void **",
    ("TYPE_POINTER", OUT): "void *",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_BOOLEAN", REF): "bool *",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_NUMBER", REF): "Number *",
    ("TYPE_NUMBER", OUT): "Number",
    ("TYPE_STRING", VALUE): "const utf8string &",
    ("TYPE_STRING", REF): "utf8string *",
    ("TYPE_STRING", OUT): "utf8string",
    ("TYPE_BYTES", VALUE): "const std::vector<unsigned char> &",
    ("TYPE_BYTES", REF): "std::vector<unsigned char> *",
    ("TYPE_BYTES", OUT): "std::vector<unsigned char>",
    ("TYPE_OBJECT", VALUE): "std::shared_ptr<Object>",
    ("TYPE_ARRAY", VALUE): "Cell",
    ("TYPE_ARRAY", REF): "Cell *",
    ("TYPE_ARRAY_NUMBER", VALUE): "std::vector<Number>",
    ("TYPE_ARRAY_STRING", VALUE): "std::vector<utf8string>",
    ("TYPE_ARRAY_STRING", REF): "std::vector<utf8string>",
    ("TYPE_ARRAY_STRING", OUT): "std::vector<utf8string>",
    ("TYPE_ARRAY_OBJECT", VALUE): "std::vector<std::shared_ptr<Object>>",
    ("TYPE_DICTIONARY", VALUE): "Cell",
    ("TYPE_DICTIONARY", REF): "Cell *",
    ("TYPE_DICTIONARY_NUMBER", VALUE): "std::map<utf8string, Number>",
    ("TYPE_DICTIONARY_STRING", VALUE): "std::map<utf8string, utf8string>",
    ("TYPE_DICTIONARY_OBJECT", VALUE): "std::map<utf8string, std::shared_ptr<Object>>",
}

CppFromAstReturn = {
    ("TYPE_NOTHING", VALUE): "void",
    ("TYPE_GENERIC", VALUE): "Cell",
    ("TYPE_GENERIC", REF): "Cell *",
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_POINTER", REF): "void **",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_BOOLEAN", REF): "bool *",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_NUMBER", REF): "Number *",
    ("TYPE_STRING", VALUE): "utf8string",
    ("TYPE_STRING", REF): "utf8string *",
    ("TYPE_BYTES", VALUE): "std::vector<unsigned char>",
    ("TYPE_BYTES", REF): "std::vector<unsigned char> *",
    ("TYPE_OBJECT", VALUE): "std::shared_ptr<Object>",
    ("TYPE_ARRAY", VALUE): "Cell",
    ("TYPE_ARRAY_NUMBER", VALUE): "std::vector<Number>",
    ("TYPE_ARRAY_STRING", VALUE): "std::vector<utf8string>",
    ("TYPE_ARRAY_STRING", REF): "std::vector<utf8string> *",
    ("TYPE_ARRAY_OBJECT", VALUE): "std::vector<std::shared_ptr<Object>>",
    ("TYPE_DICTIONARY_OBJECT", VALUE): "std::map<utf8string, std::shared_ptr<Object>>",
}

CppFromAstArg = {
    ("TYPE_POINTER", VALUE): "void *",
    ("TYPE_POINTER", REF): "void **",
    ("TYPE_POINTER", OUT): "void **",
    ("TYPE_GENERIC", VALUE): "Cell &",
    ("TYPE_GENERIC", REF): "Cell *",
    ("TYPE_GENERIC", OUT): "Cell *",
    ("TYPE_BOOLEAN", VALUE): "bool",
    ("TYPE_BOOLEAN", REF): "bool *",
    ("TYPE_NUMBER", VALUE): "Number",
    ("TYPE_NUMBER", REF): "Number *",
    ("TYPE_NUMBER", OUT): "Number *",
    ("TYPE_STRING", VALUE): "const utf8string &",
    ("TYPE_STRING", REF): "utf8string *",
    ("TYPE_STRING", OUT): "utf8string *",
    ("TYPE_BYTES", VALUE): "const std::vector<unsigned char> &",
    ("TYPE_BYTES", REF): "std::vector<unsigned char> *",
    ("TYPE_BYTES", OUT): "std::vector<unsigned char> *",
    ("TYPE_OBJECT", VALUE): "const std::shared_ptr<Object> &",
    ("TYPE_ARRAY", VALUE): "Cell &",
    ("TYPE_ARRAY", REF): "Cell *",
    ("TYPE_ARRAY_NUMBER", VALUE): "const std::vector<Number> &",
    ("TYPE_ARRAY_STRING", VALUE): "const std::vector<utf8string> &",
    ("TYPE_ARRAY_STRING", REF): "std::vector<utf8string> *",
    ("TYPE_ARRAY_STRING", OUT): "std::vector<utf8string> *",
    ("TYPE_ARRAY_OBJECT", VALUE): "std::vector<std::shared_ptr<Object>>",
    ("TYPE_DICTIONARY", VALUE): "Cell &",
    ("TYPE_DICTIONARY", REF): "Cell *",
    ("TYPE_DICTIONARY_NUMBER", VALUE): "const std::map<utf8string, Number> &",
    ("TYPE_DICTIONARY_STRING", VALUE): "const std::map<utf8string, utf8string> &",
    ("TYPE_DICTIONARY_OBJECT", VALUE): "std::map<utf8string, std::shared_ptr<Object>>",
}

JvmFromAst = {
    ("TYPE_NOTHING", VALUE): "V",
    ("TYPE_POINTER", VALUE): "Ljava/lang/Object;",
    ("TYPE_POINTER", REF): "Ljava/lang/Object;",
    ("TYPE_GENERIC", VALUE): "Ljava/lang/Object;",
    ("TYPE_GENERIC", REF): "Ljava/lang/Object;",
    ("TYPE_GENERIC", OUT): "Ljava/lang/Object;", # TODO
    ("TYPE_BOOLEAN", VALUE): "Ljava/lang/Boolean;",
    ("TYPE_NUMBER", VALUE): "Lneon/type/Number;",
    ("TYPE_NUMBER", REF): "Lneon/type/Number;",
    ("TYPE_NUMBER", OUT): "Lneon/type/Number;", # TODO
    ("TYPE_STRING", VALUE): "Ljava/lang/String;",
    ("TYPE_STRING", REF): "Ljava/lang/String;",
    ("TYPE_STRING", OUT): "Ljava/lang/String;", # TODO
    ("TYPE_BYTES", VALUE): "[B",
    ("TYPE_BYTES", REF): "[B",
    ("TYPE_BYTES", OUT): "Ljava/lang/Object;", # TODO
    ("TYPE_OBJECT", VALUE): "Ljava/lang/Object;",
    ("TYPE_ARRAY", VALUE): "Lneon/type/Array;",
    ("TYPE_ARRAY", REF): "Lneon/type/Array;",
    ("TYPE_ARRAY_NUMBER", VALUE): "Lneon/type/Array;",
    ("TYPE_ARRAY_STRING", VALUE): "Lneon/type/Array;",
    ("TYPE_ARRAY_STRING", OUT): "Lneon/type/Array;", # TODO
    ("TYPE_ARRAY_OBJECT", VALUE): "Lneon/type/Array;",
    ("TYPE_DICTIONARY", VALUE): "Ljava/util/Map;",
    ("TYPE_DICTIONARY", REF): "Ljava/util/Map;",
    ("TYPE_DICTIONARY_NUMBER", VALUE): "Ljava/util/Map;",
    ("TYPE_DICTIONARY_STRING", VALUE): "Ljava/util/Map;",
    ("TYPE_DICTIONARY_OBJECT", VALUE): "Ljava/util/Map;",
}

CellField = {
    ("TYPE_POINTER", VALUE): "address()",
    ("TYPE_POINTER", REF): "address()->address()",
    ("TYPE_BOOLEAN", VALUE): "boolean()",
    ("TYPE_BOOLEAN", REF): "address()->boolean()",
    ("TYPE_NUMBER", VALUE): "number()",
    ("TYPE_NUMBER", REF): "address()->number()",
    ("TYPE_STRING", VALUE): "string()",
    ("TYPE_STRING", REF): "address()->string_for_write()",
    ("TYPE_STRING", OUT): "address()->string_for_write()",
    ("TYPE_BYTES", VALUE): "bytes()",
    ("TYPE_BYTES", REF): "address()->bytes_for_write()",
    ("TYPE_BYTES", OUT): "address()->bytes()",
    ("TYPE_OBJECT", VALUE): "object()",
    ("TYPE_ARRAY_NUMBER", VALUE): "array()",
    ("TYPE_ARRAY_STRING", VALUE): "array()",
    ("TYPE_ARRAY_STRING", REF): "array()",
    ("TYPE_ARRAY_STRING", OUT): "array()",
    ("TYPE_DICTIONARY_STRING", VALUE): "dictionary()",
}

ArrayElementField = {
    ("TYPE_ARRAY_NUMBER", VALUE): "number()",
    ("TYPE_ARRAY_STRING", VALUE): "string()",
    ("TYPE_ARRAY_STRING", REF): "string()",
    ("TYPE_ARRAY_OBJECT", VALUE): "object()",
    ("TYPE_DICTIONARY_NUMBER", VALUE): "number()",
    ("TYPE_DICTIONARY_STRING", VALUE): "string()",
    ("TYPE_DICTIONARY_OBJECT", VALUE): "object()",
}

def parse_params(paramstr):
    r = []
    if not paramstr:
        return r, False
    def next_token(s, i):
        m = re.match(r"\s*([\w<>]+|.)", s[i:])
        if m is None:
            return "", i
        t = m.group(1)
        return t, i + m.end(0)
    i = 0
    while True:
        mode = ""
        names = []
        while True:
            t, i = next_token(paramstr, i)
            assert re.match(r"[\w_]+$", t), (paramstr, i, t)
            if t in ["OUT", "INOUT"]:
                mode = t
                t, i = next_token(paramstr, i)
            names.append(t)
            t, i = next_token(paramstr, i)
            if t != ",":
                break
        assert t == ":", (paramstr, i, t)
        t, i = next_token(paramstr, i)
        typename = t
        m = re.match(r"Array<(.*)>$", typename)
        if m is not None and m.group(1) not in ["Number", "String", "Object"]:
            typename = "Array"
        r.extend(zip(["{} {}".format(mode, typename).strip()] * len(names), names))
        t, i = next_token(paramstr, i)
        if t != ",":
            break
    variadic = False
    if t == "." and paramstr[i:i+2] == "..":
        variadic = True
        r[-1] = ("Array<" + r[-1][0] + ">", r[-1][1])
        t, i = next_token(paramstr, i)
        t, i = next_token(paramstr, i)
        t, i = next_token(paramstr, i)
    assert t in ["", "DEFAULT"], (paramstr, i, t)
    return r, variadic

enums = dict()
variables = dict()
functions = dict()
exceptions = []
exported = set()

for fn in sys.argv[1:]:
    if fn.endswith(".neon"):
        prefix = os.path.basename(fn)[:-5] + "$"
        with open(fn) as f:
            in_enum = None
            for s in f:
                a = s.split()
                if a[:1] == ["TYPE"]:
                    m = re.match("TYPE\s+(\w+)\s+IS\s+(\S+)\s*(\s+IMPLEMENTS\s+\w+)?$", s)
                    assert m, s
                    name = m.group(1)
                    atype = m.group(2)
                    if atype == "POINTER":
                        AstFromNeon[name] = ("TYPE_POINTER", VALUE)
                        AstFromNeon["INOUT "+name] = ("TYPE_POINTER", REF)
                        AstFromNeon["OUT "+name] = ("TYPE_POINTER", OUT)
                    elif atype == "ENUM":
                        # TODO: Why is this TYPE_GENERIC? It should be TYPE_NUMBER.
                        AstFromNeon[name] = ("TYPE_GENERIC", VALUE)
                        AstFromNeon["INOUT "+name] = ("TYPE_GENERIC", REF)
                        enums[name] = []
                        in_enum = name
                    elif atype == "Object":
                        AstFromNeon[name] = ("TYPE_OBJECT", VALUE)
                        AstFromNeon["INOUT "+name] = ("TYPE_OBJECT", REF)
                        AstFromNeon["OUT "+name] = ("TYPE_OBJECT", OUT)
                    else:
                        AstFromNeon[name] = ("TYPE_GENERIC", VALUE)
                        AstFromNeon["INOUT "+name] = ("TYPE_GENERIC", REF)
                        AstFromNeon["OUT "+name] = ("TYPE_GENERIC", OUT)
                elif a[:3] == ["DECLARE", "NATIVE", "FUNCTION"]:
                    m = re.search(r"(\w+)\((.*?)\)(:\s*(\S+))?\s*$", s)
                    assert m is not None
                    name = prefix + m.group(1)
                    paramstr = m.group(2)
                    rtype = m.group(4)
                    params, variadic = parse_params(paramstr)
                    functions[name] = [
                        name,
                        AstFromNeon[rtype],
                        rtype.split()[-1] if rtype is not None else None,
                        name in exported or prefix == "builtin$",
                        [AstFromNeon[x[0]] for x in params],
                        [x[0].split()[-1] for x in params],
                        [x[1] for x in params],
                        variadic
                    ]
                elif a[:3] == ["DECLARE", "NATIVE", "CONSTANT"]:
                    m = re.search(r"(\w+)\s*:\s*(\S+)", s)
                    assert m is not None
                    name = prefix + "_CONSTANT_" + m.group(1)
                    ntype = m.group(2)
                    assert ntype in ["Number", "String"]
                    ctype = ntype
                    if ctype == "String":
                        ctype = "utf8string"
                    a = name.split("$")
                    functions[name] = [
                        name,
                        AstFromNeon[ntype],
                        ntype,
                        name in exported,
                        [],
                        [],
                        [],
                        False
                    ]
                elif a[:3] == ["DECLARE", "NATIVE", "VAR"]:
                    m = re.search(r"(\w+)\s*:\s*(\S+)", s)
                    assert m is not None
                    name = prefix + m.group(1)
                    ntype = m.group(2)
                    atype = {
                        "Array<String>": "ast::TYPE_ARRAY_STRING",
                        "File": "dynamic_cast<const ast::Type *>(scope->lookupName(\"File\"))",
                        "Number": "ast::TYPE_NUMBER",
                        "TextFile": "dynamic_cast<const ast::Type *>(scope->lookupName(\"TextFile\"))",
                    }[ntype]
                    variables[name] = [name, atype]
                elif a[:1] == ["EXCEPTION"]:
                    exceptions.append((prefix, a[1]))
                elif a[:1] == ["EXPORT"]:
                    exported.add(prefix + a[1])
                elif in_enum:
                    if a[:2] == ["END", "ENUM"]:
                        in_enum = None
                    else:
                        enums[in_enum].append(a[0])
    else:
        print("Unexpected file: {}".format(fn), file=sys.stderr)
        sys.exit(1)

thunks = set()

for name, rtype, rtypename, exported, params, paramtypes, paramnames, variadic in functions.values():
    thunks.add((rtype, tuple(params)))

with open("gen/thunks.inc", "w") as inc:
    for rtype, params in thunks:
        print("static void thunk_{}_{}(opstack<Cell> &{}, void *func)".format(rtype[0], "_".join("{}_{}".format(p, m) for p, m in params), "stack" if (params or rtype[0] != "TYPE_NOTHING") else ""), file=inc)
        print("{", file=inc)
        d = 0
        for i, a in reversed(list(enumerate(params))):
            from_stack = True
            if a[0].startswith("TYPE_ARRAY_") and a[1] == VALUE:
                print("    {} a{};".format(CppFromAstParam[a], i), file=inc)
                print("    for (auto x: stack.peek({}).array()) a{}.push_back(x.{});".format(d, i, ArrayElementField[a]), file=inc)
            elif a[0].startswith("TYPE_ARRAY_") and a[1] == REF:
                print("    {} t{};".format(CppFromAstParam[a], i), file=inc)
                print("    for (auto x: stack.peek({}).address()->array()) t{}.push_back(x.{});".format(d, i, ArrayElementField[a]), file=inc)
                print("    {} *a{} = &t{};".format(CppFromAstParam[a], i, i), file=inc)
            elif a[0].startswith("TYPE_ARRAY_") and a[1] == OUT:
                print("    {} t{};".format(CppFromAstParam[a], i), file=inc)
                print("    {} *a{} = &t{};".format(CppFromAstParam[a], i, i), file=inc)
                from_stack = False
            elif a[0].startswith("TYPE_DICTIONARY_") and a[1] == VALUE:
                print("    {} a{};".format(CppFromAstParam[a], i), file=inc)
                print("    for (auto x: stack.peek({}).dictionary()) a{}[x.first] = x.second.{};".format(d, i, ArrayElementField[a]), file=inc)
            elif a in [("TYPE_GENERIC", VALUE), ("TYPE_ARRAY", VALUE), ("TYPE_DICTIONARY", VALUE)]:
                print("    {} a{} = stack.peek({});".format(CppFromAstParam[a], i, d), file=inc)
            elif a in [("TYPE_GENERIC", REF), ("TYPE_ARRAY", REF), ("TYPE_DICTIONARY", REF)]:
                print("    {} a{} = stack.peek({}).address();".format(CppFromAstParam[a], i, d), file=inc)
            elif a[1] == REF:
                print("    {} a{} = &stack.peek({}).{};".format(CppFromAstParam[a], i, d, CellField[a]), file=inc)
            elif a[1] == OUT:
                print("    {} t{};".format(CppFromAstParam[a], i), file=inc)
                print("    {} *a{} = &t{};".format(CppFromAstParam[a], i, i), file=inc)
                from_stack = False
            else:
                print("    {} a{} = stack.peek({}).{};".format(CppFromAstParam[a], i, d, CellField[a]), file=inc)
            if from_stack:
                d += 1
        stack_count = sum(1 for x in params if x[1] != OUT)
        assert d == stack_count
        print("    try {", file=inc)
        print("        {}reinterpret_cast<{} (*)({})>(func)({});".format("auto r = " if rtype[0] != "TYPE_NOTHING" else "", CppFromAstReturn[rtype], ",".join(CppFromAstArg[x] for x in params), ",".join("a{}".format(x) for x in range(len(params)))), file=inc)
        for i, a in reversed(list(enumerate(params))):
            d = len(params) - 1 - i
            if a[0].startswith("TYPE_ARRAY_") and a[1] == REF:
                print("        stack.peek({}).address()->array_for_write().clear();".format(d), file=inc)
                print("        for (auto x: t{}) stack.peek({}).address()->array_for_write().push_back(Cell(x));".format(i, d), file=inc)
        if params:
            print("        stack.drop({});".format(stack_count), file=inc)
        if rtype[0] != "TYPE_NOTHING":
            if rtype[0].startswith("TYPE_ARRAY_"):
                print("        std::vector<Cell> t;", file=inc)
                print("        for (auto x: r) t.push_back(Cell(x));", file=inc)
                print("        stack.push(Cell(t));", file=inc)
            elif rtype[0].startswith("TYPE_DICTIONARY_"):
                print("        std::map<utf8string, Cell> t;", file=inc)
                print("        for (auto x: r) t[x.first] = Cell(x.second);", file=inc)
                print("        stack.push(Cell(t));", file=inc)
            elif rtype[0] == "TYPE_POINTER":
                print("        stack.push(Cell::makeAddress(r));", file=inc)
            else:
                print("        stack.push(Cell(r));", file=inc)
        for i, a in reversed(list(enumerate(params))):
            if a[1] == OUT:
                if a[0].startswith("TYPE_ARRAY_"):
                    print("        std::vector<Cell> o{};".format(i), file=inc)
                    print("        for (auto x: t{}) o{}.push_back(Cell(x));".format(i, i), file=inc)
                    print("        stack.push(Cell(o{}));".format(i), file=inc)
                else:
                    print("        stack.push(Cell(t{}));".format(i), file=inc)
        print("    } catch (RtlException &) {", file=inc)
        if params:
            print("        stack.drop({});".format(stack_count), file=inc)
        print("        throw;", file=inc)
        print("    }", file=inc)
        print("}", file=inc)
        print("", file=inc)

with open("gen/variables_compile.inc", "w") as inc:
    print("static void init_builtin_variables(const std::string &module, ast::Scope *scope)", file=inc)
    print("{", file=inc)
    for name, atype in variables.values():
        i = name.index("$")
        module = name[:i]
        modname = name[i+1:]
        print("    if (module == \"{}\") scope->addName(Token(IDENTIFIER, \"{}\"), \"{}\", new ast::PredefinedVariable(\"{}\", {}));".format(module, modname, modname, name, atype), file=inc)
    print("}", file=inc)

with open("gen/variables_exec.inc", "w") as inc:
    print("namespace rtl {", file=inc)
    for name, atype in variables.values():
        a = name.split("$")
        print("namespace ne_{} {{ extern Cell VAR_{}; }}".format(a[0], a[1]), file=inc)
    print("}", file=inc)
    print("static struct {", file=inc)
    print("    const char *name;", file=inc)
    print("    Cell *value;", file=inc)
    print("} BuiltinVariables[] = {", file=inc)
    for name, atype, in variables.values():
        a = name.split("$")
        print("    {{\"{}\", &rtl::ne_{}::VAR_{}}},".format(name, a[0], a[1]), file=inc)
    print("};", file=inc)

with open("gen/functions_compile.inc", "w") as inc:
    print("struct PredefinedType {", file=inc)
    print("    const ast::Type *type;", file=inc)
    print("    const char *modtypename;", file=inc)
    print("};", file=inc)
    print("static struct {", file=inc)
    print("    const char *name;", file=inc)
    print("    PredefinedType returntype;", file=inc)
    print("    bool exported;", file=inc)
    print("    int count;", file=inc)
    print("    struct {ast::ParameterType::Mode mode; const char *name; PredefinedType ptype; } params[10];", file=inc)
    print("    bool variadic;", file=inc)
    print("}} BuiltinFunctions[{}];".format(len(functions)), file=inc)
    print("void init_builtin_functions() {", file=inc)
    bfi = 0
    for name, rtype, rtypename, exported, params, paramtypes, paramnames, variadic in functions.values():
        print("    BuiltinFunctions[{}] = {{\"{}\", {{{}, {}}}, {}, {}, {{{}}}, {}}};".format(
            bfi,
            name,
            "ast::"+rtype[0] if rtype[0] not in ["TYPE_GENERIC","TYPE_POINTER","TYPE_ARRAY","TYPE_DICTIONARY"] else "nullptr",
            "\"{}\"".format(rtypename) or "nullptr",
            "true" if exported else "false",
            len(params),
            ",".join("{{ast::ParameterType::Mode::{},\"{}\",{{{},{}}}}}".format("IN" if m == VALUE else "INOUT" if m == REF else "OUT", n, "ast::"+p if p not in ["TYPE_GENERIC","TYPE_POINTER","TYPE_ARRAY","TYPE_DICTIONARY"] else "nullptr", "\"{}\"".format(t) or "nullptr") for (p, m), t, n in zip(params, paramtypes, paramnames)),
            "true" if variadic else "false"
        ), file=inc)
        bfi += 1
    print("}", file=inc)

with open("gen/functions_compile_jvm.inc", "w") as inc:
    print("static struct {", file=inc)
    print("    const char *name;", file=inc)
    print("    const char *module;", file=inc)
    print("    const char *function;", file=inc)
    print("    const char *signature;", file=inc)
    print("} FunctionSignatures[] = {", file=inc)
    for name, rtype, rtypename, exported, params, paramtypes, paramnames, variadic in functions.values():
        module, function = name.split("$")
        if function in [
            "int",
        ]:
            function += "_"
        print("    {{\"{}\", \"{}\", \"{}\", \"{}\"}},".format(
            name,
            "neon/{}".format(module.title()),
            function,
            "(" + "".join(JvmFromAst[(p, m)] for (p, m), t, n in zip(params, paramtypes, paramnames)) + ")" + ("[Ljava/lang/Object;" if any(x[1] == REF for x in params) else JvmFromAst[rtype])
        ), file=inc)
    print("};", file=inc)

with open("gen/functions_exec.inc", "w") as inc:
    print("namespace rtl {", file=inc)
    cpp_name = {}
    for name, rtype, rtypename, exported, params, paramtypes, paramnames, variadic in functions.values():
        assert "." not in name
        a = name.split("$")
        if a[1] in [
            "int", "delete",
            "timegm",
        ] or a[0] in "curses":
            a[1] += "_"
        cpp_name[name] = "{}::{}".format(*a)
        print("namespace ne_{} {{ extern {} {}({}); }}".format(a[0], CppFromAstReturn[rtype], a[1], ", ".join(CppFromAstArg[x] for x in params)), file=inc)
    print("}", file=inc)
    print("static struct {", file=inc)
    print("    const char *name;", file=inc)
    print("    Thunk thunk;", file=inc)
    print("    void *func;", file=inc)
    print("} BuiltinFunctions[] = {", file=inc)
    for name, rtype, rtypename, exported, params, paramtypes, paramnames, variadic in functions.values():
        print("    {{\"{}\", {}, reinterpret_cast<void *>(rtl::ne_{})}},".format(name, "thunk_{}_{}".format(rtype[0], "_".join("{}_{}".format(p, m) for p, m in params)), cpp_name[name]), file=inc)
    print("};", file=inc)

with open("gen/enums.inc", "w") as inc:
    for name, values in enums.items():
        for i, v in enumerate(values):
            print("static const uint32_t ENUM_{}_{} = {};".format(name, v, i), file=inc)

with open("gen/exceptions.inc", "w") as inc:
    print("struct ExceptionName {", file=inc)
    print("    const char *name;", file=inc)
    print("    const char *sub[10];", file=inc)
    print("};", file=inc)
    print("namespace rtl {", file=inc)
    for prefix, name in exceptions:
        print("namespace ne_{} {{ static const ExceptionName Exception_{} = {{\"{}\", {{{}NULL}}}}; }}".format(prefix[:-1], name.replace(".", "_"), name, "".join("\"{}\", ".format(e[1].split(".")[-1]) for e in exceptions if e[1].startswith(name+"."))), file=inc)
    print("", file=inc)
    print("}", file=inc)
