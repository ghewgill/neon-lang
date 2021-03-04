using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;

namespace csnex
{
    public class Executor
    {
        sealed class ActivationFrame
        {
            public List<Cell> Locals;
            public readonly int NestingDepth;
            public readonly ActivationFrame Outer;
            public readonly int OpstackDepth;

            public ActivationFrame(int nest, ActivationFrame outer, int size, int stack_depth)
            {
                Locals = new List<Cell>();
                for (int i = 0; i < size; i++) {
                    Locals.Add(new Cell(Cell.Type.None));
                }
                NestingDepth = nest;
                OpstackDepth = stack_depth;
                Outer = outer;
            }
        }

        public Executor(Bytecode bc)
        {
            exit_code = 0;
            stack = new Stack<Cell>();
            callstack = new Stack<int>();
            global = new Global(this);
            bytecode = bc;
            param_recursion_limit = 1000;
            library = new List<KeyValuePair<string, object>>();
            library.Add(new KeyValuePair<string, object>("random", new rtl.random(this)));
            library.Add(new KeyValuePair<string, object>("runtime", new rtl.runtime(this)));
            library.Add(new KeyValuePair<string, object>("string", new rtl.@string(this)));
            library.Add(new KeyValuePair<string, object>("sys", new rtl.sys(this)));
            library.Add(new KeyValuePair<string, object>("textio", new rtl.textio(this)));
        }

        private List<KeyValuePair<string, object>> library;

        private int exit_code;
        private readonly Bytecode bytecode;
        public Stack<Cell> stack;
        private Stack<int> callstack;
        public bool enable_assert;
        private int ip;
        private Global global;
        public Int32 param_recursion_limit;
        private Stack<ActivationFrame> frames;
        public int max_frames;

        public int Run(bool EnableAssertions)
        {
            frames = new Stack<ActivationFrame>();
            ip = bytecode.code.Length;
            Invoke(0);

            for (int g = 0; g < bytecode.globals.Capacity; g++) {
                bytecode.globals.Add(new Cell(Cell.Type.None));
            }

            exit_code = Loop(0);

            if (exit_code == 0) {
                Debug.Assert(stack.Count == 0);
            }
            return exit_code;
        }

        private void RaiseLiteral(string name, Cell info)
        {
            List<Cell> exceptionvar = new List<Cell>();
            exceptionvar.Add(new Cell(name));
            exceptionvar.Add(info);
            exceptionvar.Add(new Cell(new Number(ip)));
            int tip = ip;
            int sp = callstack.Count;
            for (;;) {
                int i;
                for (i = 0; i < bytecode.exceptions.Count; i++) {
                    if ((bytecode.exceptions[i].start <= tip) && (tip < bytecode.exceptions[i].end)) {
                        string handler = bytecode.strtable[bytecode.exceptions[i].exid];
                        if ((string.Compare(name, handler) == 0) || (name.Length > handler.Length && name.StartsWith(handler) && name[handler.Length] == '.')) {
                            ip = bytecode.exceptions[i].handler;
                            while (stack.Count > (((frames.Count == 0 ? 0 : frames.Peek().OpstackDepth) + bytecode.exceptions[i].stack_depth))) {
                                stack.Pop();
                            }
                            stack.Push(new Cell(exceptionvar));
                            return;
                        }
                    }
                }
                if (sp == 0) {
                    break;
                }
                if (frames.Count != 0) {
                    frames.Pop();
                }
                tip = callstack.Pop();
                sp -= 1;
            }
            Console.Error.WriteLine(string.Format("Unhandled exception {0} ({1})\n", name, info.ToString()));
            // Destroy the stack if there are no exception handlers available.
            while (stack.Count > 0) {
                stack.Pop();
            }
            // Setting exit_code here will cause the executor Loop() to terminate and return this exit code.
            exit_code = 1;
        }

        public void Raise(string name, string info)
        {
            RaiseLiteral(name, new Cell(new ObjectString(info)));
        }

#region Opcode Handlers
#region PUSHx Opcodes
        void PUSHB()
        {
            Boolean val = bytecode.code[ip + 1] != 0;
            ip += 2;
            stack.Push(new Cell(val));
        }

        void PUSHN()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            stack.Push(new Cell(Number.FromString(bytecode.strtable[val])));
        }

        void PUSHS()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            stack.Push(new Cell(bytecode.strtable[val]));
        }

        void PUSHY()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            stack.Push(new Cell(bytecode.bytetable[val]));
        }

        void PUSHPG()
        {
            ip++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref ip);
            Debug.Assert(addr < bytecode.globals.Count);
            stack.Push(new Cell(bytecode.globals[addr]));
        }

        void PUSHPPG()
        {
            ip++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref ip);
            string var = bytecode.strtable[addr];
            try {
                object obj = library.Find(a => a.Key == var.Substring(0, var.IndexOf('$'))).Value;
                PropertyInfo pi = obj.GetType().GetProperty(var.Substring(var.IndexOf('$')+1));
                stack.Push(new Cell((Cell)pi.GetValue(obj)));
            } catch {
                throw new NeonException(string.Format("\"{0}\" - invalid or unsupported predefined variable.", var));
            }
        }

        void PUSHPMG()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHPL()
        {
            ip++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref ip);
            stack.Push(new Cell(frames.Peek().Locals[addr]));
        }

        void PUSHPOL()
        {
            ip++;
            int back = Bytecode.Get_VInt(bytecode.code, ref ip);
            int addr = Bytecode.Get_VInt(bytecode.code, ref ip);
            dump_frames();
            ActivationFrame frame = frames.Peek();
            while (back > 0) {
                frame = frame.Outer;
                back--;
            }
            stack.Push(new Cell(frame.Locals[addr]));
        }

        void PUSHI()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHNIL()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHFP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHCI()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHPEG()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region LOADx Opcodes
        void LOADB()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.Boolean));
        }

        void LOADN()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.Number));
        }

        void LOADS()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.String));
        }

        void LOADY()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.Bytes));
        }

        void LOADA()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.Array));
        }

        void LOADD()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.Dictionary));
        }

        void LOADP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LOADJ()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            stack.Push(new Cell(addr.Object));
        }

        void LOADV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region STOREx Opcodes
        void STOREB()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            Boolean b = stack.Pop().Boolean;
            addr.Set(b);
        }

        void STOREN()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            Number num = stack.Pop().Number;
            addr.Set(num);
        }

        void STORES()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            string str = stack.Pop().String;
            addr.Set(str);
        }

        void STOREY()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            byte[] b = stack.Pop().Bytes;
            addr.Set(b);
        }

        void STOREA()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            List<Cell> arr = stack.Pop().Array;
            addr.Set(arr);
        }

        void STORED()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            SortedDictionary<string, Cell> dict = stack.Pop().Dictionary;
            addr.Set(dict);
        }

        void STOREP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void STOREJ()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            Object obj = stack.Pop().Object;
            addr.Set(obj);
        }

        void STOREV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region Arithmetic Opcodes
        void NEGN()
        {
            ip++;
            Number x = stack.Pop().Number;
            stack.Push(new Cell(Number.Negate(x)));
        }

        void ADDN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.Add(a, b)));
        }

        void SUBN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.Subtract(a, b)));
        }

        void MULN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.Multiply(a, b)));
        }

        void DIVN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            if (b.IsZero()) {
                Raise("DivideByZeroException", "");
                return;
            }
            stack.Push(new Cell(Number.Divide(a, b)));
        }

        void MODN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            if (b.IsZero()) {
                Raise("DivideByZeroException", "");
                return;
            }
            stack.Push(new Cell(Number.Modulo(a, b)));
        }

        void EXPN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            if (b.IsZero()) {
                Raise("DivideByZeroException", "");
                return;
            }
            stack.Push(new Cell(Number.Pow(a, b)));
        }
#endregion
#region Comparison Opcodes
        void EQB()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NEB()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void EQN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.IsEqual(a, b)));
        }

        void NEN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(!Number.IsEqual(a, b)));
        }

        void LTN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.IsLessThan(a, b)));
        }

        void GTN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.IsGreaterThan(a, b)));
        }

        void LEN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.IsLessOrEqual(a, b)));
        }

        void GEN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            stack.Push(new Cell(Number.IsGreaterOrEqual(a, b)));
        }

        void EQS()
        {
            ip++;
            string b = stack.Pop().String;
            string a = stack.Pop().String;
            stack.Push(new Cell(string.Compare(a, b) == 0));
        }

        void NES()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LTS()
        {
            ip++;
            string b = stack.Pop().String;
            string a = stack.Pop().String;
            stack.Push(new Cell(string.Compare(a, b) < 0));
        }

        void GTS()
        {
            ip++;
            string b = stack.Pop().String;
            string a = stack.Pop().String;
            stack.Push(new Cell(string.Compare(a, b) > 0));
        }

        void LES()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void GES()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void EQY()
        {
            ip++;
            Cell b = stack.Pop();
            Cell a = stack.Pop();
            stack.Push(new Cell(a.Bytes.Compare(b.Bytes) == 0));
        }

        void NEY()
        {
            ip++;
            Cell b = stack.Pop();
            Cell a = stack.Pop();
            stack.Push(new Cell(a.Bytes.Compare(b.Bytes) != 0));
        }

        void LTY()
        {
            ip++;
            Cell b = stack.Pop();
            Cell a = stack.Pop();
            stack.Push(new Cell(a.Bytes.Compare(b.Bytes) < 0));
        }

        void GTY()
        {
            ip++;
            Cell b = stack.Pop();
            Cell a = stack.Pop();
            stack.Push(new Cell(a.Bytes.Compare(b.Bytes) > 0));
        }

        void LEY()
        {
            ip++;
            Cell b = stack.Pop();
            Cell a = stack.Pop();
            stack.Push(new Cell(a.Bytes.Compare(b.Bytes) <= 0));
        }

        void GEY()
        {
            ip++;
            Cell b = stack.Pop();
            Cell a = stack.Pop();
            stack.Push(new Cell(a.Bytes.Compare(b.Bytes) >= 0));
        }

        void EQA()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NEA()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void EQD()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NED()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void EQP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NEP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void EQV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NEV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region Logic Opcodes
        void ANDB()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void ORB()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NOTB()
        {
            ip++;
            bool x = stack.Pop().Boolean;
            stack.Push(new Cell(!x));
        }
#endregion
#region Index Opcodes
        void INDEXAR()
        {
            ip++;
            Number index = stack.Pop().Number;
            Cell addr = stack.Pop().Address;

            if (!index.IsInteger()) {
                Raise("ArrayIndexException", index.ToString());
                return;
            }
            int i = Number.number_to_int32(index);
            if (i < 0) {
                Raise("ArrayIndexException", index.ToString());
                return;
            }

            if (i >= addr.Array.Count) {
                Raise("ArrayIndexException", index.ToString());
                return;
            }
            stack.Push(new Cell(addr.ArrayIndexForRead(i)));
        }

        void INDEXAW()
        {
            ip++;
            Number index = stack.Pop().Number;
            Cell addr = stack.Pop().Address;

            if (!index.IsInteger()) {
                Raise("ArrayIndexException", index.ToString());
                return;
            }
            int i = Number.number_to_int32(index);
            if (i < 0) {
                Raise("ArrayIndexException", index.ToString());
                return;
            }
            stack.Push(new Cell(addr.ArrayIndexForWrite(i)));
        }

        void INDEXAV()
        {
            ip++;
            Number index = stack.Pop().Number;
            Cell array = stack.Pop();

            if (!index.IsInteger()) {
                RaiseLiteral("ArrayIndexException", new Cell(index.ToString()));
                return;
            }
            int i = Number.number_to_int32(index);
            if (i < 0) {
                RaiseLiteral("ArrayIndexException", new Cell(new Number(i).ToString()));
                return;
            }
            uint j = (uint)i;
            if (j >= array.Array.Count) {
                RaiseLiteral("ArrayIndexException", new Cell(new Number(j).ToString()));
                return;
            }
            Debug.Assert(j < array.Array.Count);
            stack.Push(array.Array[(int)j]);
        }

        void INDEXAN()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXDR()
        {
            ip++;
            string index = stack.Pop().String;
            Cell addr = stack.Pop().Address;
            if (!addr.Dictionary.ContainsKey(index)) {
                Raise("DictionaryIndexException", index);
                return;
            }
            stack.Push(new Cell(addr.DictionaryIndexForRead(index)));
        }

        void INDEXDW()
        {
            ip++;
            string index = stack.Pop().String;
            Cell dict = stack.Pop().Address;
            stack.Push(new Cell(dict.DictionaryIndexForWrite(index)));
        }

        void INDEXDV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region INx Opcdes
        void INA()
        {
            ip++;
            Cell array = stack.Pop();
            Cell val = stack.Pop();

            bool v = array.Array.Exists(x => x.Equals(val));

            stack.Push(new Cell(v));
        }

        void IND()
        {
            ip++;
            SortedDictionary<string, Cell> dictionary = stack.Pop().Dictionary;
            string key = stack.Pop().String;

            bool v = dictionary.ContainsKey(key);

            stack.Push(new Cell(v));
        }
#endregion
#region CALLx Opcodes
        void CALLP()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            string func = bytecode.strtable[val];
            try {
                if (func.IndexOf('$') > 0) {
                    // Call a module function from our csnex.rtl namspace
                    object lib = library.Find(a => a.Key == func.Substring(0, func.IndexOf('$'))).Value;
                    MethodInfo mi = lib.GetType().GetMethod(func.Substring(func.IndexOf('$')+1));
                    mi.Invoke(lib, null);
                } else {
                    // Call a global function
                    MethodInfo mi = global.GetType().GetMethod(func);
                    mi.Invoke(global, null);
                }
            } catch {
                throw new NeonException(string.Format("\"{0}\" - invalid or unsupported predefined function call.", func));
            }
        }

        void CALLF()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            if (callstack.Count >= param_recursion_limit) {
                Raise("StackOverflowException", "");
                return;
            }
            Invoke(val);
        }

        void CALLMF()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void CALLI()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void CALLE()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void CALLX()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void CALLV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region JUMP Opcodes
        void JUMP()
        {
            ip++;
            int target = Bytecode.Get_VInt(bytecode.code, ref ip);
            ip = target;
        }

        void JF()
        {
            ip++;
            int target = Bytecode.Get_VInt(bytecode.code, ref ip);
            bool a = stack.Pop().Boolean;
            if (!a) {
                ip = target;
            }
        }

        void JT()
        {
            ip++;
            int target = Bytecode.Get_VInt(bytecode.code, ref ip);
            bool a = stack.Pop().Boolean;
            if (a) {
                ip = target;
            }
        }

        void JFCHAIN()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void JUMPTBL()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            Number n = stack.Pop().Number;
            if (n.IsInteger() && !n.IsNegative()) {
                int i = Number.number_to_int32(n);
                if (i < val) {
                    ip += 6 * i;
                } else {
                    ip += 6 * val;
                }
            } else {
                ip += 6 * val;
            }
        }

        void JNASSERT()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void RET()
        {
            frames.Pop();
            ip = callstack.Pop();
        }
#endregion
#region Stack Handler Opcodes
        void DUP()
        {
            ip++;
            stack.Push(stack.Peek());
        }

        void DUPX1()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void DROP()
        {
            ip++;
            stack.Pop();
        }

        void DROPN()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void SWAP()
        {
            ip++;
            Cell a = stack.Pop();
            Cell b = stack.Pop();
            stack.Push(a);
            stack.Push(b);
        }
#endregion
#region Construct Opcodes
        void CONSA()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            List<Cell> a = new List<Cell>();

            while (val > 0) {
                a.Add(stack.Pop());
                val--;
            }
            stack.Push(new Cell(a));
        }

        void CONSD()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            SortedDictionary<string, Cell> d = new SortedDictionary<string, Cell>();

            while (val > 0) {
                Cell value = stack.Pop();
                string key = stack.Pop().String;
                d.Add(key, value);
                val--;
            }
            stack.Push(new Cell(d));
        }
#endregion
#region Exception Opcodes
        void EXCEPT()
        {
            int start_ip = ip;
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            ip = start_ip;
            Cell info = stack.Pop();
            RaiseLiteral(bytecode.strtable[val], info);
        }
#endregion
#region Memory Opcodes
        void ALLOC()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void RESETC()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            addr.ResetCell();
        }
#endregion
#endregion
#region Debug / Diagnostic functions
#pragma warning disable 0162
        void dump_frames()
        {
            if (false) {
                Console.Out.Write("Frames:\n");
                for (int i = 0; i < frames.Count; i++) {
                    ActivationFrame f = frames.ToArray()[i];
                    Console.Out.Write("\t {0}\t{{ locals={1} }}\n", i, f.Locals.Count.ToString());
                    for (int l = 0; 0 < f.Locals.Count; l++) {
                        string x = f.Locals[l].ToString();
                        Console.Out.Write("{0}: {1} {2}", l, f.Locals[l].type.ToString(), x);
                    }
                }
            }
        }
#pragma warning restore 0162
#endregion

        private void Invoke(int index)
        {
            callstack.Push(ip);
            ActivationFrame outer = null;
            int nest = bytecode.functions[index].nest;
            int args = bytecode.functions[index].args;
            int locals = bytecode.functions[index].locals;
            if (frames.Count > 0) {
                Debug.Assert(nest <= frames.Peek().NestingDepth+1);
                outer = frames.Peek();
                while (outer != null && nest <= outer.NestingDepth) {
                    Debug.Assert(outer.Outer == null || outer.NestingDepth == outer.Outer.NestingDepth+1);
                    outer = outer.Outer;
                }
            }
            ActivationFrame frame = new ActivationFrame(nest, outer, locals, stack.Count - args);
            frames.Push(frame);
            dump_frames();
            if (frames.Count > max_frames) {
                max_frames = frames.Count;
            }
            ip = bytecode.functions[index].entry;
        }

        private int Loop(Int64 min_callstack_depth)
        {
            while (callstack.Count > min_callstack_depth && ip < bytecode.code.Length && exit_code == 0) {
                switch ((Opcode)bytecode.code[ip]) {
                    case Opcode.PUSHB: PUSHB(); break;                // push boolean immediate
                    case Opcode.PUSHN: PUSHN(); break;                // push number immediate
                    case Opcode.PUSHS: PUSHS(); break;                // push string immediate
                    case Opcode.PUSHY: PUSHY(); break;                // push bytes immediate
                    case Opcode.PUSHPG: PUSHPG(); break;              // push pointer to global
                    case Opcode.PUSHPPG: PUSHPPG(); break;            // push pointer to predefined global
                    case Opcode.PUSHPMG: PUSHPMG(); break;            // push pointer to module global
                    case Opcode.PUSHPL: PUSHPL(); break;              // push pointer to local
                    case Opcode.PUSHPOL: PUSHPOL(); break;            // push pointer to outer local
                    case Opcode.PUSHI: PUSHI(); break;                // push 32-bit integer immediate
                    case Opcode.LOADB: LOADB(); break;                // load boolean
                    case Opcode.LOADN: LOADN(); break;                // load number
                    case Opcode.LOADS: LOADS(); break;                // load string
                    case Opcode.LOADY: LOADY(); break;                // load bytes
                    case Opcode.LOADA: LOADA(); break;                // load array
                    case Opcode.LOADD: LOADD(); break;                // load dictionary
                    case Opcode.LOADP: LOADP(); break;                // load pointer
                    case Opcode.LOADJ: LOADJ(); break;                // load object
                    case Opcode.LOADV: LOADV(); break;                // load voidptr
                    case Opcode.STOREB: STOREB(); break;              // store boolean
                    case Opcode.STOREN: STOREN(); break;              // store number
                    case Opcode.STORES: STORES(); break;              // store string
                    case Opcode.STOREY: STOREY(); break;              // store bytes
                    case Opcode.STOREA: STOREA(); break;              // store array
                    case Opcode.STORED: STORED(); break;              // store dictionary
                    case Opcode.STOREP: STOREP(); break;              // store pointer
                    case Opcode.STOREJ: STOREJ(); break;              // store object
                    case Opcode.STOREV: STOREV(); break;              // store voidptr
                    case Opcode.NEGN: NEGN(); break;                  // negate number
                    case Opcode.ADDN: ADDN(); break;                  // add number
                    case Opcode.SUBN: SUBN(); break;                  // subtract number
                    case Opcode.MULN: MULN(); break;                  // multiply number
                    case Opcode.DIVN: DIVN(); break;                  // divide number
                    case Opcode.MODN: MODN(); break;                  // modulo number
                    case Opcode.EXPN: EXPN(); break;                  // exponentiate number
                    case Opcode.EQB: EQB(); break;                    // compare equal boolean
                    case Opcode.NEB: NEB(); break;                    // compare unequal boolean
                    case Opcode.EQN: EQN(); break;                    // compare equal number
                    case Opcode.NEN: NEN(); break;                    // compare unequal number
                    case Opcode.LTN: LTN(); break;                    // compare less number
                    case Opcode.GTN: GTN(); break;                    // compare greater number
                    case Opcode.LEN: LEN(); break;                    // compare less equal number
                    case Opcode.GEN: GEN(); break;                    // compare greater equal number
                    case Opcode.EQS: EQS(); break;                    // compare equal string
                    case Opcode.NES: NES(); break;                    // compare unequal string
                    case Opcode.LTS: LTS(); break;                    // compare less string
                    case Opcode.GTS: GTS(); break;                    // compare greater string
                    case Opcode.LES: LES(); break;                    // compare less equal string
                    case Opcode.GES: GES(); break;                    // compare greater equal string
                    case Opcode.EQY: EQY(); break;                    // compare equal bytes
                    case Opcode.NEY: NEY(); break;                    // compare unequal bytes
                    case Opcode.LTY: LTY(); break;                    // compare less bytes
                    case Opcode.GTY: GTY(); break;                    // compare greater bytes
                    case Opcode.LEY: LEY(); break;                    // compare less equal bytes
                    case Opcode.GEY: GEY(); break;                    // compare greater equal bytes
                    case Opcode.EQA: EQA(); break;                    // compare equal array
                    case Opcode.NEA: NEA(); break;                    // compare unequal array
                    case Opcode.EQD: EQD(); break;                    // compare equal dictionary
                    case Opcode.NED: NED(); break;                    // compare unequal dictionary
                    case Opcode.EQP: EQP(); break;                    // compare equal pointer
                    case Opcode.NEP: NEP(); break;                    // compare unequal pointer
                    case Opcode.EQV: EQV(); break;                    // compare equal voidptr
                    case Opcode.NEV: NEV(); break;                    // compare unequal voidptr
                    case Opcode.ANDB: ANDB(); break;                  // and boolean
                    case Opcode.ORB: ORB(); break;                    // or boolean
                    case Opcode.NOTB: NOTB(); break;                  // not boolean
                    case Opcode.INDEXAR: INDEXAR(); break;            // index array for read
                    case Opcode.INDEXAW: INDEXAW(); break;            // index array for write
                    case Opcode.INDEXAV: INDEXAV(); break;            // index array value
                    case Opcode.INDEXAN: INDEXAN(); break;            // index array value, no exception
                    case Opcode.INDEXDR: INDEXDR(); break;            // index dictionary for read
                    case Opcode.INDEXDW: INDEXDW(); break;            // index dictionary for write
                    case Opcode.INDEXDV: INDEXDV(); break;            // index dictionary value
                    case Opcode.INA: INA(); break;                    // in array
                    case Opcode.IND: IND(); break;                    // in dictionary
                    case Opcode.CALLP: CALLP(); break;                // call predefined
                    case Opcode.CALLF: CALLF(); break;                // call function
                    case Opcode.CALLMF: CALLMF(); break;              // call module function
                    case Opcode.CALLI: CALLI(); break;                // call indirect
                    case Opcode.JUMP: JUMP(); break;                  // unconditional jump
                    case Opcode.JF: JF(); break;                      // jump if false
                    case Opcode.JT: JT(); break;                      // jump if true
                    case Opcode.DUP: DUP(); break;                    // duplicate
                    case Opcode.DUPX1: DUPX1(); break;                // duplicate under second value
                    case Opcode.DROP: DROP(); break;                  // drop
                    case Opcode.RET: RET(); break;                    // return
                    case Opcode.CONSA: CONSA(); break;                // construct array
                    case Opcode.CONSD: CONSD(); break;                // construct dictionary
                    case Opcode.EXCEPT: EXCEPT(); break;              // throw exception
                    case Opcode.ALLOC: ALLOC(); break;                // allocate record
                    case Opcode.PUSHNIL: PUSHNIL(); break;            // push nil pointer
                    case Opcode.RESETC: RESETC(); break;              // reset cell
                    case Opcode.PUSHPEG: PUSHPEG(); break;            // push pointer to external global
                    case Opcode.JUMPTBL: JUMPTBL(); break;            // jump table
                    case Opcode.CALLX: CALLX(); break;                // call extension
                    case Opcode.SWAP: SWAP(); break;                  // swap two top stack elements
                    case Opcode.DROPN: DROPN(); break;                // drop element n
                    case Opcode.PUSHFP: PUSHFP(); break;              // push function pointer
                    case Opcode.CALLV: CALLV(); break;                // call virtual
                    case Opcode.PUSHCI: PUSHCI(); break;              // push class info
                    default:
                        throw new InvalidOpcodeException(string.Format("Invalid opcode ({0}) in bytecode file.", bytecode.code[ip]));
                }
            }
            return exit_code;
        }
    }
}
