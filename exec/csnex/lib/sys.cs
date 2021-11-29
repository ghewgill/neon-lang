using System;
using System.Collections.Generic;

namespace csnex.rtl
{
    public class sys
    {
        private Executor exec;
        public Cell args { get; private set; }

        public sys(Executor exe)
        {
            exec = exe;
            List<Cell> arr = new List<Cell>();
            // Note that Environment.GetCommandLineArgs() returns the path to csnex as element 0.  We don't want that,
            // we want the .neonx file to be the first argument.
            for (int x = csnex.gOptions.ArgStart + 1; x < Environment.GetCommandLineArgs().Length; x++) {
                arr.Add(Cell.CreateStringCell(Environment.GetCommandLineArgs()[x]));
            }
            args = Cell.CreateArrayCell(arr);
        }

        public void exit()
        {
            Number x = exec.stack.Pop().Number;

            if (!x.IsInteger()) {
                exec.Raise("InvalidValueException", string.Format("sys.exit invalid parameter: {0}", x.ToString()));
                return;
            }
            int r = Number.number_to_int32(x);
            if (r < 0 || r > 255) {
                exec.Raise("InvalidValueException", string.Format("sys.exit invalid parameter: {0}", x.ToString()));
                return;
            }
            Environment.Exit(r);
        }
    }
}
