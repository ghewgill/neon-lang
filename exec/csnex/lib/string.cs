using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace csnex.rtl
{
    public class @string
    {
        private Executor Exec;
        public @string(Executor exe)
        {
            Exec = exe;
        }

        public void find()
        {
            string t = Exec.stack.Pop().String;
            string s = Exec.stack.Pop().String;

            int r = s.IndexOf(t, 0);
            if (r < 0) {
                Exec.stack.Push(Cell.CreateArrayCell(new List<Cell> {
                    Cell.CreateNumberCell(new Number(0))
                }));
                return;
            }
            Exec.stack.Push(Cell.CreateArrayCell(new List<Cell> {
                Cell.CreateNumberCell(new Number(1)),
                Cell.CreateNumberCell(new Number(r))
            }));
        }

        public void fromCodePoint()
        {
            Number x = Exec.stack.Pop().Number;

            if (!x.IsInteger()) {
                Exec.Raise("PANIC", "fromCodePoint() argument not an integer");
                return;
            }
            if (x.IsNegative() || Number.number_to_int32(x) > 0x10ffff) {
                Exec.Raise("PANIC", "fromCodePoint() argument out of range 0-0x10ffff");
                return;
            }

            Exec.stack.Push(Cell.CreateStringCell(new string((char)Number.number_to_uint32(x), 1)));
        }

        public void toCodePoint()
        {
            string s = Exec.stack.Pop().String;

            if (s.Length != 1) {
                Exec.Raise("PANIC", "toCodePoint() requires string of length 1");
                return;
            }

            Exec.stack.Push(Cell.CreateNumberCell(new Number(s[0])));
        }
    }
}
