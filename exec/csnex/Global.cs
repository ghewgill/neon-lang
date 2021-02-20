using System.Collections.Generic;

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
            Exec.stack.Push(new Cell(sbuf));
        }
#endregion
#region Array Functions
        public void array__append()
        {
            Cell element = Exec.stack.Pop();
            Cell array = Exec.stack.Pop().Address;
            array.Array.Add(element);
        }

        public void array__concat()
        {
            List<Cell> right = Exec.stack.Pop().Array;
            List<Cell> left = Exec.stack.Pop().Array;
            Cell[] a = new Cell[right.Count + left.Count];

            left.CopyTo(a, 0);
            right.CopyTo(a, left.Count);

            Exec.stack.Push(new Cell(new List<Cell>(a)));
        }

        public void array__extend()
        {
            Cell elements = Exec.stack.Pop();
            Cell array = Exec.stack.Pop().Address;

            foreach (Cell item in elements.Array) {
                array.Array.Add(item);
            }
        }

        public void array__size()
        {
            List<Cell> a = Exec.stack.Pop().Array;

            Exec.stack.Push(new Cell(new Number(a.Count)));
        }

        public void array__toString__number()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(new Cell(s));
        }
#endregion
#region Boolean Functions
        public void boolean__toString()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(new Cell(s));
        }
#endregion
#region Number Functions
        public void number__toString()
        {
            Number n = Exec.stack.Pop().Number;
            Exec.stack.Push(new Cell(n.ToString()));
        }
#endregion
#region Object Functions
        public void object__makeString()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(new Cell(new ObjectString(o.String)));
        }
#endregion
#region String Functions
        public void string__concat()
        {
            Cell b = Exec.stack.Pop();
            Cell a = Exec.stack.Pop();

            Exec.stack.Push(new Cell(a.String + b.String));
        }
#endregion
    }
}
