using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace csnex
{
    public class Global
    {
        private Executor Exec;

        public Global(Executor exec)
        {
            Exec = exec;
        }

#region Global Functions
        public void num()
        {
            string s = Exec.stack.Pop().String;

            Number n = null;
            try {
                n = Number.FromString(s);
            } catch {
                Exec.Raise("PANIC", "num() argument not a number");
                return;
            }

            if (n == null || n.IsNaN()) {
                Exec.Raise("PANIC", "num() argument not a number");
                return;
            }

            Exec.stack.Push(Cell.CreateNumberCell(n));
        }

        public void print()
        {
            Object o = Exec.stack.Pop().Object;
            if (o == null) {
                System.Console.Out.WriteLine("NIL");
                return;
            }
            System.Console.Out.WriteLine(o.toString());
        }

        public void str()
        {
            string sbuf = Exec.stack.Pop().Number.ToString();
            if (sbuf.IndexOf('.') >= 0) {
                while (sbuf.Length > 1 && sbuf[sbuf.Length-1] == '0') {
                    sbuf = sbuf.Substring(0, sbuf.Length-1);
                }
                if (sbuf[sbuf.Length-1] == '.') {
                    sbuf = sbuf.Substring(0, sbuf.Length-1);
                }
            }
            Exec.stack.Push(Cell.CreateStringCell(sbuf));
        }
#endregion
#region Bytes Functions
        public void Bytes__decodeToString()
        {
            Cell s = Exec.stack.Pop();

            Exec.stack.Push(Cell.CreateArrayCell(new List<Cell> {Cell.CreateNumberCell(new Number(0)), Cell.CreateStringCell(new string(System.Text.Encoding.UTF8.GetChars(s.Bytes, 0, s.Bytes.Length)))}));
        }
#endregion
    }
}
