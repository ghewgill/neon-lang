using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace csnex
{
    public class Bytecode
    {
        private const int BYTECODE_VERSION = 3;
        private byte[] obj;
        public string source_path;
        public List<String> strtable;
        public byte[] code;

        public List<FunctionInfo> functions;
        public List<Cell> globals;

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

        public List<String> GetStrTable(byte[] obj, int size, ref int i)
        {
            List<String> r = new List<string>();

            while (i < size) {
                int len = Get_VInt(obj, ref i);
                string ts = new string(System.Text.Encoding.UTF8.GetChars(obj, i, len));
                r.Add(ts);
                i += len;
            }
            return r;
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

            /* String Table */
            int strtablesize = Get_VInt(obj, ref i);
            strtable = GetStrTable(obj, strtablesize + i, ref i);

            /* Types */
            int typesize = Get_VInt(obj, ref i);
            Debug.Assert(typesize == 0);

            /* Constants */
            int constantsize = Get_VInt(obj, ref i);
            Debug.Assert(constantsize == 0);

            /* Exported Variabes */
            int variablesize = Get_VInt(obj, ref i);
            Debug.Assert(variablesize == 0);

            /* Exported Functions */
            int functionsize = Get_VInt(obj, ref i);
            Debug.Assert(functionsize == 0);

            /* Exported Exceptions */
            int exceptionexportsize = Get_VInt(obj, ref i);
            Debug.Assert(exceptionexportsize == 0);

            /* Exported Interfaces */
            int interfaceexportsize = Get_VInt(obj, ref i);
            Debug.Assert(interfaceexportsize == 0);

            /* Imported Modules */
            int importsize = Get_VInt(obj, ref i);
            Debug.Assert(importsize == 0);

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
            Debug.Assert(exceptionsize == 0);

            /* Classes */
            int classsize = Get_VInt(obj, ref i);
            Debug.Assert(classsize == 0);

            code = new byte[obj.Length - i];
            Array.Copy(obj, i, code, 0, obj.Length - i);
        }

        public struct FunctionInfo
        {
            public int name;
            public int nest;
            public int args;
            public int locals;
            public int entry;
        }
    }
}
