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

        public void abs()
        {
            Number x = Exec.stack.Pop().Number;
            Exec.stack.Push(Cell.CreateNumberCell(Number.Abs(x)));
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

        public void exp()
        {
            Number x = Exec.stack.Pop().Number;
            double r = Math.Exp(x.ToDouble());
            if (Double.IsInfinity(r)) {
                Exec.Raise("PANIC", "Number overflow error: exp");
                return;
            }
            Exec.stack.Push(Cell.CreateNumberCell(Number.FromDouble(r)));
        }

        public void floor()
        {
            Number x = Exec.stack.Pop().Number;
            Exec.stack.Push(Cell.CreateNumberCell(Number.Floor(x)));
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

        public void min()
        {
            Number b = Exec.stack.Pop().Number;
            Number a = Exec.stack.Pop().Number;
            if (Number.IsGreaterThan(a, b)) {
                Exec.stack.Push(Cell.CreateNumberCell(b));
            } else {
                Exec.stack.Push(Cell.CreateNumberCell(a));
            }
        }

        public void sign()
        {
            Number x = Exec.stack.Pop().Number;
            Exec.stack.Push(Cell.CreateNumberCell(Number.Sign(x)));
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
