using System;

namespace csnex.rtl
{
    public class math
    {
        private Executor Exec;
        public math(Executor exe)
        {
            Exec = exe;
        }

        public void acos()
        {
            Number x = Exec.stack.Pop().Number;
            double r = Math.Acos(x.ToDouble());
            if (Double.IsNaN(r)) {
                Exec.Raise("PANIC", "Number invalid error: acos");
                return;
            }
            Exec.stack.Push(Cell.CreateNumberCell(Number.FromDouble(r)));
        }

        public void log()
        {
            Number x = Exec.stack.Pop().Number;
            double r = Math.Log(x.ToDouble());
            if (Double.IsInfinity(r)) {
                Exec.Raise("PANIC", "Number divide by zero error: log");
                return;
            }
            if (Double.IsNaN(r)) {
                Exec.Raise("PANIC", "Number invalid error: log");
                return;
            }
            Exec.stack.Push(Cell.CreateNumberCell(Number.FromDouble(r)));
        }

        public void sqrt()
        {
            Number x = Exec.stack.Pop().Number;
            double r = Math.Sqrt(x.ToDouble());
            if (Double.IsNaN(r)) {
                Exec.Raise("PANIC", "Number invalid error: sqrt");
                return;
            }
            Exec.stack.Push(Cell.CreateNumberCell(Number.FromDouble(r)));
        }
    }
}
