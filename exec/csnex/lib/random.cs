using System;
using System.Security.Cryptography;

namespace csnex.rtl
{
    public class random
    {
        private Executor Exec;
        private RandomNumberGenerator rng;

        public random(Executor exe)
        {
            rng = RandomNumberGenerator.Create();
            Exec = exe;
        }

        public void uint32()
        {
            byte[] r = new byte[4];
            rng.GetBytes(r);
            uint val = BitConverter.ToUInt32(r, 0);

            Exec.stack.Push(new Cell(new Number(val)));
        }

        public void bytes()
        {
            uint count = Number.number_to_uint32(Exec.stack.Pop().Number);

            byte[] r = new byte[count];
            rng.GetBytes(r);

            Exec.stack.Push(new Cell(r));
        }
    }
}
