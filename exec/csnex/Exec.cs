using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;

namespace csnex
{
    public class Executor
    {
        public Executor(Bytecode bc)
        {
            exit_code = 0;
            stack = new Stack<Cell>();
            global = new Global(this);
            bytecode = bc;
        }

        private int exit_code;
        private readonly Bytecode bytecode;
        public Stack<Cell> stack;
        public bool enable_assert;
        private int ip;
        private Global global;

        public int Run(bool EnableAssertions)
        {
            ip = 0;

            for (int g = 0; g < bytecode.globals.Capacity; g++)
            {
                bytecode.globals.Add(new Cell(Cell.Type.None));
            }

            exit_code = Loop();

            if (exit_code == 0) {
                Debug.Assert(stack.Count == 0);
            }
            return exit_code;
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHPMG()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHPL()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void PUSHPOL()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LOADA()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LOADD()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LOADP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LOADJ()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void STOREA()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void STORED()
        {
            ip++;
            Cell addr = stack.Pop().Address;
            Dictionary<string, Cell> dict = stack.Pop().Dictionary;
            addr.Set(dict);
        }

        void STOREP()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void STOREJ()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
                throw new NeonDivideByZeroException("DivideByZeroException", "");
            }
            stack.Push(new Cell(Number.Divide(a, b)));
        }

        void MODN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            if (b.IsZero()) {
                throw new NeonDivideByZeroException("DivideByZeroException", "");
            }
            stack.Push(new Cell(Number.Modulo(a, b)));
        }

        void EXPN()
        {
            ip++;
            Number b = stack.Pop().Number;
            Number a = stack.Pop().Number;
            if (b.IsZero()) {
                throw new NeonDivideByZeroException("DivideByZeroException", "");
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void GEN()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void EQS()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NES()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LTS()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void GTS()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void NEY()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LTY()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void GTY()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void LEY()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void GEY()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXAW()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXAV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXAN()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXDR()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXDW()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void INDEXDV()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region INx Opcdes
        void INA()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void IND()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }
#endregion
#region CALLx Opcodes
        void CALLP()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            string func = bytecode.strtable[val];
            global.Dispatch(func);
        }

        void CALLF()
        {
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            ip++;
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
        }

        void CONSD()
        {
            ip++;
            int val = Bytecode.Get_VInt(bytecode.code, ref ip);
            Dictionary<string, Cell> d = new Dictionary<string, Cell>();

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
            throw new NotImplementedException(string.Format("{0} not implemented.", MethodBase.GetCurrentMethod().Name));
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

        private int Loop()
        {
            while (ip < bytecode.code.Length && exit_code == 0) {
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
