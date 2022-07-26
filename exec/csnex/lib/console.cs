using System;
using System.IO;
using System.Text;

namespace csnex.rtl
{
    public class console
    {
        private Executor Exec;

        public console(Executor exe)
        {
            Exec = exe;
        }

        public void input_internal()
        {
            string prompt = Exec.stack.Pop().String;
            string r;
            try {
                Console.InputEncoding = Encoding.UTF8;
                Console.OutputEncoding = Encoding.UTF8;
                Console.Out.Write(prompt);
                r = Console.In.ReadLine();
                Exec.stack.Push(Cell.CreateBooleanCell(true));
                Exec.stack.Push(Cell.CreateStringCell(r));
            } catch (EndOfStreamException) {
                Exec.stack.Push(Cell.CreateBooleanCell(false));
                Exec.stack.Push(Cell.CreateStringCell(""));
            }
        }

        public void output()
        {
            string data = Exec.stack.Pop().String;

            Console.OutputEncoding = Encoding.UTF8;
            Console.Out.Write(data);
        }
    }
}
