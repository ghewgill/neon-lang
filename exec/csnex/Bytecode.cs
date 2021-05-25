using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;

namespace csnex
{
    public class Bytecode
    {
        public const int BYTECODE_VERSION = 3;
        private byte[] obj;
        public string source_path;
        public List<string> strtable;
        public List<byte[]> bytetable;
        public byte[] code;

        public List<ModuleImport> imports;
        public List<FunctionInfo> functions;
        public List<Function> exports;
        public List<Type> types;
        public List<ExceptionInfo> exceptions;
        public List<ExceptionExport> export_exceptions;
        public List<Cell> globals;
        public List<ClassInfo> classes;

        public Bytecode()
        {
        }

        public static int Get_VInt(byte[] pobj, ref int i)
        {
            int r = 0;
            while (i < pobj.Length) {
                byte x = pobj[i];
                i++;
                if ((r & ~(int.MaxValue >> 7)) != 0) {
                    throw new BytecodeException(string.Format("Integer value exceeds maximum ({0})", int.MaxValue));
                }
                r = (r << 7) | (x & 0x7f);
                if ((x & 0x80) == 0) {
                    break;
                }
            }
            return r;
        }

        public void GetStringBytesTable(byte[] obj, int size, ref int i)
        {
            bytetable = new List<byte[]>();
            strtable = new List<string>();
            while (i < size) {
                int len = Get_VInt(obj, ref i);
                byte[] ts = new byte[len];
                Array.Copy(obj, i, ts, 0, len);
                bytetable.Add(ts);
                // Yea, we're keeping two copies of the data here.  This could probably be optimized.
                strtable.Add(new string(System.Text.Encoding.UTF8.GetChars(obj, i, len)));
                i += len;
            }
        }

        public void LoadBytecode(string a_source_path, byte[] bytes)
        {
            source_path = a_source_path;
            obj = bytes;
            int i = 0;

            if (i + 4 > obj.Length) {
                throw new BytecodeException("unexpected end of bytecode");
            }
            string sig = new string(System.Text.Encoding.ASCII.GetChars(obj, i, 4));
            if (sig != "Ne\0n") {
                throw new BytecodeException("bytecode signature missing");
            }
            i += 4;

            int version = Get_VInt(obj, ref i);

            if (version != BYTECODE_VERSION) {
                throw new BytecodeException("bytecode version mismatch");
            }

            if (i + 32 > obj.Length) {
                throw new BytecodeException("unexpected end of bytecode");
            }

            byte[] source_hash = new byte[32];
            Array.Copy(obj, i, source_hash, 0, 32);
            i += 32;

            /* Globals */
            int global_size = Get_VInt(obj, ref i);
            globals = new List<Cell>(global_size);

            /* Byte / String Table */
            int strtablesize = Get_VInt(obj, ref i);
            GetStringBytesTable(obj, strtablesize + i, ref i);

            /* Types */
            int typesize = Get_VInt(obj, ref i);
            types = new List<Type>();
            while (typesize > 0) {
                Type t = new Type();
                t.name = Get_VInt(obj, ref i);
                t.descriptor = Get_VInt(obj, ref i);
                types.Add(t);
                typesize--;
            }

            /* Constants */
            int constantsize = Get_VInt(obj, ref i);
            Debug.Assert(constantsize == 0);

            /* Exported Variabes */
            int variablesize = Get_VInt(obj, ref i);
            Debug.Assert(variablesize == 0);

            /* Exported Functions */
            int functionsize = Get_VInt(obj, ref i);
            exports = new List<Function>();
            while (functionsize > 0) {
                Function ef = new Function();
                ef.name = Get_VInt(obj, ref i);
                ef.descriptor = Get_VInt(obj, ref i);
                ef.index = Get_VInt(obj, ref i);
                exports.Add(ef);
                functionsize--;
            }

            /* Exported Exceptions */
            int exceptionexportsize = Get_VInt(obj, ref i);
            export_exceptions = new List<ExceptionExport>();
            while (exceptionexportsize > 0) {
                ExceptionExport e = new ExceptionExport();
                e.name = Get_VInt(obj, ref i);
                export_exceptions.Add(e);
                exceptionexportsize--;
            }

            /* Exported Interfaces */
            int interfaceexportsize = Get_VInt(obj, ref i);
            Debug.Assert(interfaceexportsize == 0);

            /* Imported Modules */
            int importsize = Get_VInt(obj, ref i);
            imports = new List<ModuleImport>();
            while (importsize > 0) {
                ModuleImport imp = new ModuleImport();
                imp.name = Get_VInt(obj, ref i);
                imp.optional = Get_VInt(obj, ref i) != 0;
                if (i + 32 > obj.Length) {
                    throw new BytecodeException("unexpected end of bytecode");
                }
                imp.hash = new byte[32];
                Array.Copy(obj, i, imp.hash, 0, 32);
                i += 32;
                imports.Add(imp);
                importsize--;
            }

            /* Functions */
            functionsize = Get_VInt(obj, ref i);
            functions = new List<FunctionInfo>();
            while(functionsize > 0) {
                FunctionInfo fi = new FunctionInfo();
                fi.name = Get_VInt(obj, ref i);
                fi.nest = Get_VInt(obj, ref i);
                fi.args = Get_VInt(obj, ref i);
                fi.locals = Get_VInt(obj, ref i);
                fi.entry = Get_VInt(obj, ref i);
                functions.Add(fi);
                functionsize--;
            }

            /* Exception Handlers */
            int exceptionsize = Get_VInt(obj, ref i);
            exceptions = new List<ExceptionInfo>();
            while (exceptionsize > 0) {
                ExceptionInfo ex = new ExceptionInfo();
                ex.start = Get_VInt(obj, ref i);
                ex.end = Get_VInt(obj, ref i);
                ex.exid = Get_VInt(obj, ref i);
                ex.handler = Get_VInt(obj, ref i);
                ex.stack_depth = Get_VInt(obj, ref i);
                exceptions.Add(ex);
                exceptionsize--;
            }

            /* Classes */
            int classsize = Get_VInt(obj, ref i);
            classes = new List<ClassInfo>();
            while (classsize > 0) {
                ClassInfo cls = new ClassInfo();
                cls.interfaces = new List<Interface>();
                cls.name = Get_VInt(obj, ref i);
                int interfacesize = Get_VInt(obj, ref i);
                while (interfacesize > 0) {
                    Interface inf = new Interface();
                    int methodsize = Get_VInt(obj, ref i);
                    inf.methods = new List<int>();
                    while (methodsize > 0) {
                        inf.methods.Add(Get_VInt(obj, ref i));
                        methodsize--;
                    }
                    cls.interfaces.Add(inf);
                    interfacesize--;
                }
                classes.Add(cls);
                classsize--;
            }

            code = new byte[obj.Length - i];
            Array.Copy(obj, i, code, 0, obj.Length - i);
        }

        public struct Type
        {
            public int name;
            public int descriptor;
        }

        public struct ModuleImport
        {
            public int name;
            public bool optional;
            public byte[] hash;
        }

        public struct Function
        {
            public int name;
            public int descriptor;
            public int index;
        }

        public struct FunctionInfo
        {
            public int name;
            public int nest;
            public int args;
            public int locals;
            public int entry;
        }

        public struct ExceptionInfo
        {
            public int start;
            public int end;
            public int exid;
            public int handler;
            public int stack_depth;
        }

        public struct ExceptionExport
        {
            public int name;
            public int descriptor;
            public int index;
        }

        public struct Interface
        {
            public int name;
            public List<int> methods;
        }

        public struct ClassInfo
        {
            public int name;
            public int interfacesize;
            public List<Interface> interfaces;
        }
    }

    public class Module
    {
        public Module()
        {
            Bytecode = new Bytecode();
            Globals = new List<Cell>();
            Name = null;
            SourcePath = null;
            Code = null;
        }
        public string      Name;
        public string      SourcePath;
        public byte[]      Code;
        public Bytecode    Bytecode;
        public List<Cell>  Globals;
    }
}
