using System;
using System.Collections.Generic;
using System.Linq;

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

        public void array__toBytes__number()
        {
            Cell a = Exec.stack.Pop();

            Cell r = new Cell(Cell.Type.Bytes);
            r.Bytes = new byte[a.Array.Count];
            for (int x = 0, i = 0; x < a.Array.Count; x++) {
                uint b = Number.number_to_uint32(a.Array[x].Number);
                if (b >= 256) {
                    Exec.Raise("BytesOutOfRangeException", b.ToString());
                }
                r.Bytes[i++] = (byte)b;
            }

            Exec.stack.Push(r);
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
#region Bytes Functions
        public void bytes__decodeToString()
        {
            Cell s = Exec.stack.Pop();

            Exec.stack.Push(new Cell(new string(System.Text.Encoding.UTF8.GetChars(s.Bytes, 0, s.Bytes.Length))));
        }

        public void bytes__index()
        {
            Number index = Exec.stack.Pop().Number;
            Cell t = Exec.stack.Pop();

            if (!index.IsInteger()) {
                Exec.Raise("BytesIndexException", index.ToString());
                return;
            }

            Int64 i = Number.number_to_int64(index);
            if (i < 0 || i >= t.Bytes.LongLength) {
                Exec.Raise("BytesIndexException", i.ToString());
                return;
            }

            byte c = t.Bytes[i];
            Exec.stack.Push(new Cell(new Number(c)));
        }

        public void bytes__range()
        {
            bool last_from_end = Exec.stack.Pop().Boolean;
            Number last = Exec.stack.Pop().Number;
            bool first_from_end = Exec.stack.Pop().Boolean;
            Number first  = Exec.stack.Pop().Number;
            Cell t = Exec.stack.Pop();

            if (!first.IsInteger()) {
                Exec.Raise("BytesIndexException", first.ToString());
                return;
            }
            if (!last.IsInteger()) {
                Exec.Raise("BytesIndexException", last.ToString());
                return;
            }

            Int64 fst = Number.number_to_int64(first);
            Int64 lst = Number.number_to_int64(last);
            if (first_from_end) {
                fst += t.Bytes.LongLength - 1;
            }
            if (last_from_end) {
                lst += t.Bytes.LongLength - 1;
            }
            if (fst < 0) {
                fst = 0;
            }
            if (fst > t.Bytes.LongLength - 1) {
                fst = t.Bytes.LongLength;
            }
            if (lst > t.Bytes.LongLength - 1) {
                lst = t.Bytes.LongLength - 1;
            }
            if (lst < 0) {
                lst = -1;
            }

            Cell sub = new Cell(Cell.Type.Bytes);
            if (lst < fst) {
                Exec.stack.Push(sub);
                return;
            }
            sub.Bytes = new byte[lst + 1 - fst];
            Array.Copy(t.Bytes, fst, sub.Bytes, 0, lst + 1 - fst);

            Exec.stack.Push(sub);
        }

        public void bytes__size()
        {
            Cell b = Exec.stack.Pop();

            Exec.stack.Push(new Cell(new Number(b.Bytes.Length)));
        }

        public void bytes__splice()
        {
            bool last_from_end = Exec.stack.Pop().Boolean;
            Number last = Exec.stack.Pop().Number;
            bool first_from_end = Exec.stack.Pop().Boolean;
            Number first  = Exec.stack.Pop().Number;
            Cell s = Exec.stack.Pop();
            Cell t = Exec.stack.Pop();

            Int64 fst = Number.number_to_int64(first);
            Int64 lst = Number.number_to_int64(last);
            if (first_from_end) {
                fst += t.Bytes.Length - 1;
            }
            if (last_from_end) {
                lst += t.Bytes.Length - 1;
            }

            Cell sub = new Cell(Cell.Type.Bytes);
            sub.Bytes = new byte[t.Bytes.Length + (((fst - 1) + s.Bytes.Length) - lst)];
            Array.Copy(s.Bytes, sub.Bytes, fst);
            Array.Copy(t.Bytes, 0, sub.Bytes, fst, (t.Bytes.Length - fst));
            Array.Copy(s.Bytes, lst + 1, sub.Bytes, t.Bytes.Length, s.Bytes.Length - (lst + 1));

            Exec.stack.Push(sub);
        }

        public void bytes__toArray()
        {
            Cell s = Exec.stack.Pop();
            List<Cell> a = new List<Cell>(s.Bytes.Length);

            for (int i = 0; i < s.Bytes.Length; i++) {
                a.Add(new Cell(new Number(s.Bytes[i])));
            }

            Exec.stack.Push(new Cell(a));
        }

        public void bytes__toString()
        {
            bool first;
            int i;
            Cell s = Exec.stack.Pop();
            string r = "HEXBYTES \"";

            first = true;
            for (i = 0; i < s.Bytes.Length; i++) {
                if (first) {
                    first = false;
                } else {
                    r += " ";
                }
                const string hex = @"0123456789abcdef";
                r += string.Format("{0}{1}", hex.ToCharArray()[(s.Bytes[i] >> 4) & 0xf], hex.ToCharArray()[s.Bytes[i] & 0xf]);
            }
            r += "\"";
            Cell ret = new Cell(r);

            Exec.stack.Push(ret);
        }
#endregion
#region Dictionary Functions
        public void dictionary__keys()
        {
            List<Cell> keys = Exec.stack.Pop().Dictionary.Keys.ToList().ToCellArray();
            Exec.stack.Push(new Cell(keys));
        }

        public void dictionary__remove()
        {
            string key = Exec.stack.Pop().String;
            Cell addr = Exec.stack.Pop().Address;
            addr.Dictionary.Remove(key);
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
        public void string__append()
        {
            Cell b = Exec.stack.Pop();
            Cell addr = Exec.stack.Pop().Address;

            addr.String += b.String;
        }

        public void string__concat()
        {
            Cell b = Exec.stack.Pop();
            Cell a = Exec.stack.Pop();

            Exec.stack.Push(new Cell(a.String + b.String));
        }

        public void string__index()
        {
            Number index = Exec.stack.Pop().Number;
            Cell a = Exec.stack.Pop();

            if (!index.IsInteger()) {
                Exec.Raise("StringIndexException", index.ToString());
                return;
            }
            // String index in C# / .NET are limited to 32 bit values.
            Int32 i = Number.number_to_int32(index);
            if (i < 0) {
                Exec.Raise("StringIndexException", i.ToString());
                return;
            }
            if (i >= a.String.Length) {
                Exec.Raise("StringIndexException", i.ToString());
                return;
            }

            string r = a.String.Substring(i, 1);
            Exec.stack.Push(new Cell(r));
        }

        public void string__length()
        {
            Number n = new Number(Exec.stack.Pop().String.Length);
            Exec.stack.Push(new Cell(n));
        }

        public void string__splice()
        {
            bool last_from_end = Exec.stack.Pop().Boolean;
            Number last = Exec.stack.Pop().Number;
            bool first_from_end = Exec.stack.Pop().Boolean;
            Number first  = Exec.stack.Pop().Number;
            Cell s = Exec.stack.Pop();
            Cell t = Exec.stack.Pop();

            // C# only supports 32 bit string lengths.
            Int32 f = Number.number_to_int32(first);
            Int32 l = Number.number_to_int32(last);
            if (first_from_end) {
                f += s.String.Length - 1;
            }
            if (last_from_end) {
                l += s.String.Length - 1;
            }

            string sub;
            sub = s.String.Substring(0, f);
            sub += t.String;
            sub += s.String.Substring(l+1, s.String.Length - (l + 1));

            Exec.stack.Push(new Cell(sub));
        }

        public void string__substring()
        {
            bool last_from_end = Exec.stack.Pop().Boolean;
            Number last = Exec.stack.Pop().Number;
            bool first_from_end = Exec.stack.Pop().Boolean;
            Number first  = Exec.stack.Pop().Number;
            Cell a = Exec.stack.Pop();

            if (!first.IsInteger()) {
                Exec.Raise("StringIndexException", first.ToString());
                return;
            }
            if (!last.IsInteger()) {
                Exec.Raise("StringIndexException", last.ToString());
                return;
            }

            Int32 f = Number.number_to_int32(first);
            Int32 l = Number.number_to_int32(last);
            if (first_from_end) {
                f += a.String.Length - 1;
            }
            if (last_from_end) {
                l += a.String.Length - 1;
            }
            if (f < 0) {
                f = 0;
            }
            if (f > a.String.Length) {
                f = a.String.Length;
            }
            if (l >= a.String.Length) {
                l = a.String.Length - 1;
            }
            if (l < 0) {
                l = -1;
            }

            if ((((l + 1) - f) + 1) <= 0) {
                Exec.stack.Push(new Cell(""));
                return;
            }

            string sub = a.String.Substring(f, l+1-f);
            Exec.stack.Push(new Cell(sub));
        }

        public void string__toBytes()
        {
            string s = Exec.stack.Pop().String;
            byte[] r = System.Text.Encoding.UTF8.GetBytes(s);
            Exec.stack.Push(new Cell(r));
        }

        public void string__toString()
        {
            // String is already on the stack, so we don't have to modify it.
        }
#endregion
    }
}
