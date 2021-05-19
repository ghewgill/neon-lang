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
                Exec.Raise("ValueRangeException", "num() argument not a number");
                return;
            }

            if (n == null || n.IsNaN()) {
                Exec.Raise("ValueRangeException", "num() argument not a number");
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

            Exec.stack.Push(Cell.CreateArrayCell(new List<Cell>(a)));
        }

        public void array__extend()
        {
            Cell elements = Exec.stack.Pop();
            Cell array = Exec.stack.Pop().Address;

            foreach (Cell item in elements.Array) {
                array.Array.Add(item);
            }
        }

        public void array__find()
        {
            Cell e = Exec.stack.Pop();
            List<Cell> a = Exec.stack.Pop().Array;

            Number r = new Number(-1);

            for (int i = 0; i < a.Count; i++) {
                if (a[i].Equals(e)) {
                    r = new Number(i);
                    break;
                }
            }

            if (r.IsNegative()) {
                Exec.Raise("ArrayIndexException", "value not found in array");
                return;
            }

            Exec.stack.Push(Cell.CreateNumberCell(r));
        }

        public void array__range()
        {
            Number step = Exec.stack.Pop().Number;
            Number last = Exec.stack.Pop().Number;
            Number first = Exec.stack.Pop().Number;
            List<Cell> r = new List<Cell>();

            if (step.IsZero()) {
                Exec.Raise("ValueRangeException", step.ToString());
                return;
            }

            if (step.IsNegative()) {
                for (Number i = first; Number.IsGreaterOrEqual(i, last); i = Number.Add(i, step)) {
                    r.Add(Cell.CreateNumberCell(i));
                }
            } else {
                for (Number i = first; Number.IsLessOrEqual(i, last); i = Number.Add(i, step)) {
                    r.Add(Cell.CreateNumberCell(i));
                }
            }

            Exec.stack.Push(Cell.CreateArrayCell(r));
        }

        public void array__remove()
        {
            Number index = Exec.stack.Pop().Number;
            List<Cell> array = Exec.stack.Pop().Address.Array;

            if (!index.IsInteger() || index.IsNegative()) {
                Exec.Raise("ArrayIndexException", index.ToString());
                return;
            }
            if (Number.number_to_int32(index) > array.Count) {
                Exec.Raise("ArrayIndexException", index.ToString());
                return;
            }

            int i = Number.number_to_int32(index);
            array.Remove(array[i]);
        }

        public void array__resize()
        {
            Number new_size = Exec.stack.Pop().Number;
            List<Cell> array = Exec.stack.Pop().Address.Array;

            if (!new_size.IsInteger()) {
                Exec.Raise("ArrayIndexException", new_size.ToString());
                return;
            }

            if (Number.number_to_int32(new_size) < array.Count) {
                array.RemoveRange(Number.number_to_int32(new_size), array.Count - Number.number_to_int32(new_size));
            } else {
                int array_size = array.Count;
                for (int i = array_size; i < Number.number_to_int32(new_size); i++) {
                    array.Add(new Cell(Cell.Type.None));
                }
            }
        }

        public void array__reversed()
        {
            List<Cell> a = Exec.stack.Peek().Array;

            a.Reverse();
        }

        public void array__size()
        {
            List<Cell> a = Exec.stack.Pop().Array;

            Exec.stack.Push(Cell.CreateNumberCell(new Number(a.Count)));
        }

        public void array__slice()
        {
            bool last_from_end = Exec.stack.Pop().Boolean;
            Number last = Exec.stack.Pop().Number;
            bool first_from_end = Exec.stack.Pop().Boolean;
            Number first  = Exec.stack.Pop().Number;
            Cell array = Exec.stack.Pop();

            if (!first.IsInteger()) {
                Exec.Raise("ArrayIndexException", first.ToString());
                return;
            }
            if (!last.IsInteger()) {
                Exec.Raise("ArrayIndexException", last.ToString());
                return;
            }

            Int32 fst = Number.number_to_int32(first);
            Int32 lst = Number.number_to_int32(last);
            if (first_from_end) {
                fst += array.Array.Count - 1;
            }
            if (fst < 0) {
                fst = 0;
            }
            if (fst > array.Array.Count) {
                fst = array.Array.Count;
            }
            if (last_from_end) {
                lst += array.Array.Count - 1;
            }
            if (lst < -1) {
                lst = -1;
            }
            if (lst >= array.Array.Count) {
                lst = array.Array.Count - 1;
            }
            if (lst < fst) {
                lst = fst - 1;
            }
            List<Cell> r = new List<Cell>(lst - fst + 1);

            for (int i = fst; i < lst + 1; i++) {
                r.Add(Cell.FromCell(array.Array[i]));
            }

            Exec.stack.Push(Cell.CreateArrayCell(r));
        }

        public void array__splice()
        {
            bool last_from_end = Exec.stack.Pop().Boolean;
            Number last = Exec.stack.Pop().Number;
            bool first_from_end = Exec.stack.Pop().Boolean;
            Number first  = Exec.stack.Pop().Number;
            Cell a = Exec.stack.Pop();
            Cell b = Exec.stack.Pop();

            Int32 fst = Number.number_to_int32(first);
            Int32 lst = Number.number_to_int32(last);
            if (first_from_end) {
                fst += a.Array.Count - 1;
            }
            if (fst < 0) {
                fst = 0;
            }
            if (fst > a.Array.Count) {
                fst = a.Array.Count;
            }
            if (last_from_end) {
                lst += a.Array.Count - 1;
            }
            if (lst < -1) {
                lst = -1;
            }
            if (lst >= a.Array.Count) {
                lst = a.Array.Count - 1;
            }

            List<Cell> r = new List<Cell>();
            for (int i = 0; i < fst; i++) {
                r.Add(Cell.FromCell(a.Array[i]));
            }
            for (int i = 0; i < b.Array.Count; i++) {
                r.Add(Cell.FromCell(b.Array[i]));
            }
            for (int i = lst + 1; i < a.Array.Count; i++) {
                r.Add(Cell.FromCell(a.Array[i]));
            }

            Exec.stack.Push(Cell.CreateArrayCell(r));
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
            Exec.stack.Push(Cell.CreateStringCell(s));
        }

        public void array__toString__string()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(Cell.CreateStringCell(s));
        }

        public void array__toString__object()
        {
            Cell a = Exec.stack.Pop();

            StringBuilder r = new StringBuilder("[");
            bool first = true;
            foreach (Cell x in a.Array) {
                if (first) {
                    first = false;
                } else {
                    r.Append(", ");
                }
                r.Append(x.Object.toLiteralString());
            }
            r.Append("]");

            Exec.stack.Push(Cell.CreateStringCell(r.ToString()));
        }
#endregion
#region Boolean Functions
        public void boolean__toString()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(Cell.CreateStringCell(s));
        }
#endregion
#region Bytes Functions
        public void bytes__concat()
        {
            Cell b = Exec.stack.Pop();
            Cell a = Exec.stack.Pop();
            byte[] r = new byte[b.Bytes.Length + a.Bytes.Length];
            Array.Copy(a.Bytes, r, a.Bytes.Length);
            Array.Copy(b.Bytes, 0, r, a.Bytes.Length, b.Bytes.Length);
            Exec.stack.Push(Cell.CreateBytesCell(r));
        }

        public void bytes__decodeToString()
        {
            Cell s = Exec.stack.Pop();

            Exec.stack.Push(Cell.CreateStringCell(new string(System.Text.Encoding.UTF8.GetChars(s.Bytes, 0, s.Bytes.Length))));
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
            Exec.stack.Push(Cell.CreateNumberCell(new Number(c)));
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

            Exec.stack.Push(Cell.CreateNumberCell(new Number(b.Bytes.Length)));
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
                a.Add(Cell.CreateNumberCell(new Number(s.Bytes[i])));
            }

            Exec.stack.Push(Cell.CreateArrayCell(a));
        }

        public void bytes__toString()
        {
            // ToDo: Convert this function to use the StringBuilder class!
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

            Exec.stack.Push(Cell.CreateStringCell(r));
        }
#endregion
#region Exception Functions
        public void exceptiontype__toString()
        {
            Cell ex = Exec.stack.Pop();
            System.Diagnostics.Debug.Assert(ex.Array.Count == 3);

            StringBuilder sb = new StringBuilder("<ExceptionType:");
            sb.AppendFormat("{0},{1},{2}>", ex.Array[0].String, ex.Array[1].Object.toString(), ex.Array[2].Number.ToString());
            Exec.stack.Push(Cell.CreateStringCell(sb.ToString()));
        }
#endregion
#region Dictionary Functions
        public void dictionary__keys()
        {
            List<Cell> keys = Exec.stack.Pop().Dictionary.Keys.ToList().ToCellArray();
            Exec.stack.Push(Cell.CreateArrayCell(keys));
        }

        public void dictionary__remove()
        {
            string key = Exec.stack.Pop().String;
            Cell addr = Exec.stack.Pop().Address;
            addr.Dictionary.Remove(key);
        }

        public void dictionary__toString__object()
        {
            Cell d = Exec.stack.Pop();

            StringBuilder r = new StringBuilder("{");
            foreach (KeyValuePair<string, Cell> e in d.Dictionary) {
                if (r.Length > 1) {
                    r.Append(", ");
                }
                r.Append(e.Key.Quote());
                r.Append(": ");
                r.Append(e.Value.Object.toLiteralString());
            }
            r.Append("}");
            Exec.stack.Push(Cell.CreateStringCell(r.ToString()));
        }

        public void dictionary__toString__string()
        {
            Cell s = Exec.stack.Pop();
            Exec.stack.Push(Cell.CreateStringCell(Cell.toString(s)));
        }
#endregion
#region Number Functions
        public void number__toString()
        {
            Number n = Exec.stack.Pop().Number;
            Exec.stack.Push(Cell.CreateStringCell(n.ToString()));
        }
#endregion
#region Object Functions
        #region Object Getters
        public void object__getArray()
        {
            Object obj = Exec.stack.Pop().Object;

            List<Object> a = null;
            if (obj == null || !obj.getArray(ref a)) {
                Exec.Raise("DynamicConversionException", "to Array");
                return;
            }
            if (a == null) {
                Exec.Raise("DynamicConversionException", "to Array");
                return;
            }
            List<Cell> r = new List<Cell>();
            foreach (Object x in a) {
                r.Add(Cell.CreateObjectCell(x));
            }
            Exec.stack.Push(Cell.CreateArrayCell(r));
        }

        public void object__getBoolean()
        {
            Cell obj = Exec.stack.Pop();

            bool r = false;
            if (obj.Object == null || !obj.Object.getBoolean(ref r)) {
                Exec.Raise("DynamicConversionException", "to Boolean");
                return;
            }
            Exec.stack.Push(Cell.CreateBooleanCell(r));
        }

        public void object__getBytes()
        {
            Cell obj = Exec.stack.Pop();

            if (obj.Object == null) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            byte[] b = null;
            if (!obj.Object.getBytes(ref b)) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            if (b == null) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            Exec.stack.Push(Cell.CreateBytesCell(b));
        }

        public void object__getDictionary()
        {
            Object o = Exec.stack.Pop().Object;
            if (o == null) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            SortedDictionary<string, Object> d = null;
            if (!o.getDictionary(ref d)) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            if (d == null) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }

            SortedDictionary<string, Cell> r = new SortedDictionary<string, Cell>();
            foreach (KeyValuePair<string, Object> e in d) {
                r.Add(e.Key, Cell.CreateObjectCell(e.Value));
            }

            Exec.stack.Push(Cell.CreateDictionaryCell(r));
        }

        public void object__getNumber()
        {
            Cell obj = Exec.stack.Pop();

            Number n = null;
            if (obj.Object == null || !obj.Object.getNumber(ref n)) {
                Exec.Raise("DynamicConversionException", "to Number");
                return;
            }
            Exec.stack.Push(Cell.CreateNumberCell(n));
        }

        public void object__getString()
        {
            Cell obj = Exec.stack.Pop();

            if (obj.Object == null) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            String s = null;
            if (!obj.Object.getString(ref s)) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            if (s == null) {
                Exec.Raise("DynamicConversionException", "");
                return;
            }
            Exec.stack.Push(Cell.CreateStringCell(s));
        }
        #endregion
        #region Object Makers
        public void object__makeArray()
        {
            List<Cell> a = Exec.stack.Pop().Array;

            List<Object> oa = new List<Object>();
            foreach (Cell c in a) {
                oa.Add(c.Object);
            }

            Exec.stack.Push(Cell.CreateObjectCell(new ObjectArray(oa)));
        }

        public void object__makeBoolean()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(Cell.CreateObjectCell(new ObjectBoolean(o.Boolean)));
        }

        public void object__makeBytes()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(Cell.CreateObjectCell(new ObjectBytes(o.Bytes)));
        }

        public void object__makeDictionary()
        {
            SortedDictionary<string, Cell> d = Exec.stack.Pop().Dictionary;

            SortedDictionary<string, Object> r = new SortedDictionary<string, Object>();
            foreach (KeyValuePair<string, Cell> e in d) {
                r.Add(e.Key, e.Value.Object);
            }

            Exec.stack.Push(Cell.CreateObjectCell(new ObjectDictionary(r)));
        }

        public void object__makeNull()
        {
            Exec.stack.Push(Cell.CreateObjectCell(null));
        }

        public void object__makeNumber()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(Cell.CreateObjectCell(new ObjectNumber(o.Number)));
        }

        public void object__makeString()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(Cell.CreateObjectCell(new ObjectString(o.String)));
        }
        #endregion
        public void object__isNull()
        {
            Object o = Exec.stack.Pop().Object;
            Exec.stack.Push(Cell.CreateBooleanCell(o == null));
        }

        public void object__subscript()
        {
            Object i = Exec.stack.Pop().Object;
            Object o = Exec.stack.Pop().Object;

            if (o == null) {
                Exec.Raise("DynamicConversionException", "object is null");
                return;
            }
            if (i == null) {
                Exec.Raise("DynamicConversionException", "index is null");
                return;
            }

            Object r = null;
            if (!o.subscript(i, ref r)) {
                Exec.Raise("ObjectSubscriptException", i.toString());
                return;
            }

            Exec.stack.Push(Cell.CreateObjectCell(r));
        }

        public void object__toString()
        {
            Object o = Exec.stack.Pop().Object;

            Exec.stack.Push(Cell.CreateStringCell(o != null ? o.toString() : "null"));
        }
#endregion
#region Pointer Functions
        public void pointer__toString()
        {
            Cell p = Exec.stack.Pop().Address;
            Module m = (Module)p.Array[0].Array[0].Other;
            Bytecode.ClassInfo ci = (Bytecode.ClassInfo)p.Array[0].Array[1].Other;
            Exec.stack.Push(Cell.CreateStringCell(string.Format("<p:{0}-{1}>", m.Bytecode.strtable[ci.name], p.AllocNum)));
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

            Exec.stack.Push(Cell.CreateStringCell(a.String + b.String));
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

            Exec.stack.Push(Cell.CreateStringCell(a.String.Substring(i, 1)));
        }

        public void string__length()
        {
            Number n = new Number(Exec.stack.Pop().String.Length);
            Exec.stack.Push(Cell.CreateNumberCell(n));
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

            Exec.stack.Push(Cell.CreateStringCell(sub));
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
                Exec.stack.Push(Cell.CreateStringCell(""));
                return;
            }

            string sub = a.String.Substring(f, l+1-f);
            Exec.stack.Push(Cell.CreateStringCell(sub));
        }

        public void string__toBytes()
        {
            string s = Exec.stack.Pop().String;
            byte[] r = System.Text.Encoding.UTF8.GetBytes(s);
            Exec.stack.Push(Cell.CreateBytesCell(r));
        }

        public void string__toString()
        {
            // String is already on the stack, so we don't have to modify it.
        }
#endregion
    }
}
