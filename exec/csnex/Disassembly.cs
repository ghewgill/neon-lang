using System.Text;

namespace csnex
{
    public class InstructionDisassembler
    {
        private Bytecode bytecode;
        private int index;
        private StringBuilder stream;

        public InstructionDisassembler(Bytecode bc, int ip)
        {
            bytecode = bc;
            index = ip;
            stream = new StringBuilder();
        }

#region PUSHx Opcodes
        private void PUSHB()
        {
            bool val = bytecode.code[index + 1] != 0;
            index += 2;
            stream.AppendFormat("PUSHB {0}", val);
        }

        void PUSHN()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHN {0}", bytecode.strtable[val]);
        }

        void PUSHS()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHS \"{0}\"", bytecode.strtable[val]);
        }

        void PUSHY()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHY {0}", val);
        }

        void PUSHPG()
        {
            index++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHPG {0}", addr);
        }

        void PUSHPPG()
        {
            index++;
            int name = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHPPG {0}", bytecode.strtable[name]);
        }

        void PUSHPMG()
        {
            index++;
            int module = Bytecode.Get_VInt(bytecode.code, ref index);
            int name = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHPMG {0}, {1}", module, name);
        }

        void PUSHPL()
        {
            index++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHPL {0}", addr);
        }

        void PUSHPOL()
        {
            index++;
            int enclosing = Bytecode.Get_VInt(bytecode.code, ref index);
            int addr = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHPOL {0},{1}", enclosing, addr);
        }

        void PUSHI()
        {
            index++;
            int x = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHI {0}", x);
        }

        void PUSHNIL()
        {
            stream.AppendFormat("PUSHNIL");
            index++;
        }

        void PUSHPEG()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHPEG \"{0}\"", bytecode.strtable[val]);
        }

        void PUSHFP()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHFP {0}", val);
        }

        void PUSHCI()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("PUSHCI \"{0}\"", bytecode.strtable[val]);
        }
#endregion
#region LOADx Opcodes
        void LOADB()
        {
            stream.AppendFormat("LOADB");
            index++;
        }

        void LOADN()
        {
            stream.AppendFormat("LOADN");
            index++;
        }

        void LOADS()
        {
            stream.AppendFormat("LOADS");
            index++;
        }

        void LOADY()
        {
            stream.AppendFormat("LOADY");
            index++;
        }

        void LOADA()
        {
            stream.AppendFormat("LOADA");
            index++;
        }

        void LOADD()
        {
            stream.AppendFormat("LOADD");
            index++;
        }

        void LOADP()
        {
            stream.AppendFormat("LOADP");
            index++;
        }

        void LOADJ()
        {
            stream.AppendFormat("LOADJ");
            index++;
        }
#endregion
#region STOREx Handlers
        void STOREB()
        {
            stream.AppendFormat("STOREB");
            index++;
        }

        void STOREN()
        {
            stream.AppendFormat("STOREN");
            index++;
        }

        void STORES()
        {
            stream.AppendFormat("STORES");
            index++;
        }

        void STOREY()
        {
            stream.AppendFormat("STOREY");
            index++;
        }

        void STOREA()
        {
            stream.AppendFormat("STOREA");
            index++;
        }

        void STORED()
        {
            stream.AppendFormat("STORED");
            index++;
        }

        void STOREP()
        {
            stream.AppendFormat("STOREP");
            index++;
        }

        void STOREJ()
        {
            stream.AppendFormat("STOREJ");
            index++;
        }
#endregion
#region Arithmetic Handlers
        void NEGN()
        {
            stream.AppendFormat("NEGN");
            index++;
        }

        void ADDN()
        {
            stream.AppendFormat("ADDN");
            index++;
        }

        void SUBN()
        {
            stream.AppendFormat("SUBN");
            index++;
        }

        void MULN()
        {
            stream.AppendFormat("MULN");
            index++;
        }

        void DIVN()
        {
            stream.AppendFormat("DIVN");
            index++;
        }

        void MODN()
        {
            stream.AppendFormat("MODN");
            index++;
        }

        void EXPN()
        {
            stream.AppendFormat("EXPN");
            index++;
        }
#endregion
#region Comparrison Handlers
        void EQB()
        {
            stream.AppendFormat("EQB");
            index++;
        }

        void NEB()
        {
            stream.AppendFormat("NEB");
            index++;
        }

        void EQN()
        {
            stream.AppendFormat("EQN");
            index++;
        }

        void NEN()
        {
            stream.AppendFormat("NEN");
            index++;
        }

        void LTN()
        {
            stream.AppendFormat("LTN");
            index++;
        }

        void GTN()
        {
            stream.AppendFormat("GTN");
            index++;
        }

        void LEN()
        {
            stream.AppendFormat("LEN");
            index++;
        }

        void GEN()
        {
            stream.AppendFormat("GEN");
            index++;
        }

        void EQS()
        {
            stream.AppendFormat("EQS");
            index++;
        }

        void NES()
        {
            stream.AppendFormat("NES");
            index++;
        }

        void LTS()
        {
            stream.AppendFormat("LTS");
            index++;
        }

        void GTS()
        {
            stream.AppendFormat("GTS");
            index++;
        }

        void LES()
        {
            stream.AppendFormat("LES");
            index++;
        }

        void GES()
        {
            stream.AppendFormat("GES");
            index++;
        }

        void EQY()
        {
            stream.AppendFormat("EQY");
            index++;
        }

        void NEY()
        {
            stream.AppendFormat("NEY");
            index++;
        }

        void LTY()
        {
            stream.AppendFormat("LTY");
            index++;
        }

        void GTY()
        {
            stream.AppendFormat("GTY");
            index++;
        }

        void LEY()
        {
            stream.AppendFormat("LEY");
            index++;
        }

        void GEY()
        {
            stream.AppendFormat("GEY");
            index++;
        }

        void EQA()
        {
            stream.AppendFormat("EQA");
            index++;
        }

        void NEA()
        {
            stream.AppendFormat("NEA");
            index++;
        }

        void EQD()
        {
            stream.AppendFormat("EQD");
            index++;
        }

        void NED()
        {
            stream.AppendFormat("NED");
            index++;
        }

        void EQP()
        {
            stream.AppendFormat("EQP");
            index++;
        }

        void NEP()
        {
            stream.AppendFormat("NEP");
            index++;
        }
#endregion
#region Logic Handlers
        void ANDB()
        {
            stream.AppendFormat("ANDB");
            index++;
        }

        void ORB()
        {
            stream.AppendFormat("ORB");
            index++;
        }

        void NOTB()
        {
            stream.AppendFormat("NOTB");
            index++;
        }
#endregion
#region INDEXx Handlers
        void INDEXAR()
        {
            stream.AppendFormat("INDEXAR");
            index++;
        }

        void INDEXAW()
        {
            stream.AppendFormat("INDEXAW");
            index++;
        }

        void INDEXAV()
        {
            stream.AppendFormat("INDEXAV");
            index++;
        }

        void INDEXAN()
        {
            stream.AppendFormat("INDEXAN");
            index++;
        }

        void INDEXDR()
        {
            stream.AppendFormat("INDEXDR");
            index++;
        }

        void INDEXDW()
        {
            stream.AppendFormat("INDEXDW");
            index++;
        }

        void INDEXDV()
        {
            stream.AppendFormat("INDEXDV");
            index++;
        }
#endregion
#region INx Handlers
        void INA()
        {
            stream.AppendFormat("INA");
            index++;
        }

        void IND()
        {
            stream.AppendFormat("IND");
            index++;
        }
#endregion
#region CALLx Handlers
        void CALLP()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CALLP {0} {1}",val, val < bytecode.strtable.Count ? bytecode.strtable[val] : "(invalid)");
        }

        void CALLF()
        {
            index++;
            int findex = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CALLF {0} {1}", bytecode.functions[findex].name, bytecode.strtable[bytecode.functions[findex].name]);
        }

        void CALLMF()
        {
            index++;
            int mod = Bytecode.Get_VInt(bytecode.code, ref index);
            int func = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CALLMF {0},{1}", bytecode.strtable[mod], bytecode.strtable[func]);
        }

        void CALLI()
        {
            stream.AppendFormat("CALLI");
            index++;
        }

        void CALLE()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CALLE %s", bytecode.strtable[val]);
        }

        void CALLV()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CALLV {0}", val);
        }

        void CALLX()
        {
            index++;
            int module = Bytecode.Get_VInt(bytecode.code, ref index);
            int name = Bytecode.Get_VInt(bytecode.code, ref index);
            int out_param_count = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CALLX {0}.{1},{2}", bytecode.strtable[module], bytecode.strtable[name], out_param_count);
        }
#endregion
#region JUMPx Handlers
        void JUMP()
        {
            index++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("JUMP {0}", addr);
        }

        void JF()
        {
            index++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("JF {0}", addr);
        }

        void JT()
        {
            index++;
            int addr = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("JT {0}", addr);
        }

        void JUMPTBL()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("JUMPTBL {0}", val);
        }

        void RET()
        {
            stream.AppendFormat("RET");
            index++;
        }
        #endregion
#region Stack Handlers
        void DUP()
        {
            stream.AppendFormat("DUP");
            index++;
        }

        void DUPX1()
        {
            stream.AppendFormat("DUPX1");
            index++;
        }

        void DROP()
        {
            stream.AppendFormat("DROP");
            index++;
        }

        void SWAP()
        {
            stream.AppendFormat("SWAP");
            index++;
        }

        void DROPN()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("DROPN {0}", val);
        }
#endregion
#region Constructor Handlers
        void CONSA()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CONSA {0}", val);
        }

        void CONSD()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("CONSD {0}", val);
        }
#endregion
#region Exception Handler
        void EXCEPT()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("EXCEPT \"{0}\"", bytecode.strtable[val]);
        }
#endregion
#region Memory Handlers
        void ALLOC()
        {
            index++;
            int val = Bytecode.Get_VInt(bytecode.code, ref index);
            stream.AppendFormat("ALLOC {0}", val);
        }
#endregion

        private void Disassemble()
        {
            switch ((Opcode)bytecode.code[index]) {
                case Opcode.PUSHB: PUSHB(); break;
                case Opcode.PUSHN: PUSHN(); break;
                case Opcode.PUSHS: PUSHS(); break;
                case Opcode.PUSHY: PUSHY(); break;
                case Opcode.PUSHPG: PUSHPG(); break;
                case Opcode.PUSHPPG: PUSHPPG(); break;
                case Opcode.PUSHPMG: PUSHPMG(); break;
                case Opcode.PUSHPL: PUSHPL(); break;
                case Opcode.PUSHPOL: PUSHPOL(); break;
                case Opcode.PUSHI: PUSHI(); break;
                case Opcode.LOADB: LOADB(); break;
                case Opcode.LOADN: LOADN(); break;
                case Opcode.LOADS: LOADS(); break;
                case Opcode.LOADY: LOADY(); break;
                case Opcode.LOADA: LOADA(); break;
                case Opcode.LOADD: LOADD(); break;
                case Opcode.LOADP: LOADP(); break;
                case Opcode.LOADJ: LOADJ(); break;
                case Opcode.STOREB: STOREB(); break;
                case Opcode.STOREN: STOREN(); break;
                case Opcode.STORES: STORES(); break;
                case Opcode.STOREY: STOREY(); break;
                case Opcode.STOREA: STOREA(); break;
                case Opcode.STORED: STORED(); break;
                case Opcode.STOREP: STOREP(); break;
                case Opcode.STOREJ: STOREJ(); break;
                case Opcode.NEGN: NEGN(); break;
                case Opcode.ADDN: ADDN(); break;
                case Opcode.SUBN: SUBN(); break;
                case Opcode.MULN: MULN(); break;
                case Opcode.DIVN: DIVN(); break;
                case Opcode.MODN: MODN(); break;
                case Opcode.EXPN: EXPN(); break;
                case Opcode.EQB: EQB(); break;
                case Opcode.NEB: NEB(); break;
                case Opcode.EQN: EQN(); break;
                case Opcode.NEN: NEN(); break;
                case Opcode.LTN: LTN(); break;
                case Opcode.GTN: GTN(); break;
                case Opcode.LEN: LEN(); break;
                case Opcode.GEN: GEN(); break;
                case Opcode.EQS: EQS(); break;
                case Opcode.NES: NES(); break;
                case Opcode.LTS: LTS(); break;
                case Opcode.GTS: GTS(); break;
                case Opcode.LES: LES(); break;
                case Opcode.GES: GES(); break;
                case Opcode.EQY: EQY(); break;
                case Opcode.NEY: NEY(); break;
                case Opcode.LTY: LTY(); break;
                case Opcode.GTY: GTY(); break;
                case Opcode.LEY: LEY(); break;
                case Opcode.GEY: GEY(); break;
                case Opcode.EQA: EQA(); break;
                case Opcode.NEA: NEA(); break;
                case Opcode.EQD: EQD(); break;
                case Opcode.NED: NED(); break;
                case Opcode.EQP: EQP(); break;
                case Opcode.NEP: NEP(); break;
                case Opcode.ANDB: ANDB(); break;
                case Opcode.ORB: ORB(); break;
                case Opcode.NOTB: NOTB(); break;
                case Opcode.INDEXAR: INDEXAR(); break;
                case Opcode.INDEXAW: INDEXAW(); break;
                case Opcode.INDEXAV: INDEXAV(); break;
                case Opcode.INDEXAN: INDEXAN(); break;
                case Opcode.INDEXDR: INDEXDR(); break;
                case Opcode.INDEXDW: INDEXDW(); break;
                case Opcode.INDEXDV: INDEXDV(); break;
                case Opcode.INA: INA(); break;
                case Opcode.IND: IND(); break;
                case Opcode.CALLP: CALLP(); break;
                case Opcode.CALLF: CALLF(); break;
                case Opcode.CALLMF: CALLMF(); break;
                case Opcode.CALLI: CALLI(); break;
                case Opcode.JUMP: JUMP(); break;
                case Opcode.JF: JF(); break;
                case Opcode.JT: JT(); break;
                case Opcode.DUP: DUP(); break;
                case Opcode.DUPX1: DUPX1(); break;
                case Opcode.DROP: DROP(); break;
                case Opcode.RET: RET(); break;
                case Opcode.CONSA: CONSA(); break;
                case Opcode.CONSD: CONSD(); break;
                case Opcode.EXCEPT: EXCEPT(); break;
                case Opcode.ALLOC: ALLOC(); break;
                case Opcode.PUSHNIL: PUSHNIL(); break;
                case Opcode.PUSHPEG: PUSHPEG(); break;
                case Opcode.JUMPTBL: JUMPTBL(); break;
                case Opcode.CALLX: CALLX(); break;
                case Opcode.SWAP: SWAP(); break;
                case Opcode.DROPN: DROPN(); break;
                case Opcode.PUSHFP: PUSHFP(); break;
                case Opcode.CALLV: CALLV(); break;
                case Opcode.PUSHCI: PUSHCI(); break;
                default:
                    stream.AppendFormat("Unknown opcode: {0}", bytecode.code[index]);
                    index++;
                    break;
            }
        }

        public static string DisassembleInstruction(Bytecode obj, int ip)
        {
            InstructionDisassembler disassembler = new InstructionDisassembler(obj, ip);
            disassembler.Disassemble();
            return disassembler.stream.ToString();
        }
    }
}
