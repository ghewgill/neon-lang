using System.Collections.Generic;
using System.Text;

namespace csnex
{
    public abstract class Object
    {
        public virtual bool getBoolean(ref bool b)
        {
            return false;
        }

        public virtual bool getNumber(ref Number n)
        {
            return false;
        }

        public virtual bool getString(ref string s)
        {
            return false;
        }

        public virtual bool getBytes(ref byte[] b)
        {
            return false;
        }

        public virtual bool getArray(ref List<Object> a)
        {
            return false;
        }

        public virtual bool getDictionary(ref SortedDictionary<string, Object> o)
        {
            return false;
        }

        public virtual bool subscript(Object a, ref Object b)
        {
            b = null;
            return false;
        }

        public virtual string toLiteralString()
        {
            return toString();
        }

        public abstract string toString();
    }

    public class ObjectArray : Object
    {
        public ObjectArray(List<Object> a)
        {
            Value = a;
        }

        public override bool getArray(ref List<Object> r)
        {
            r = Value;
            return true;
        }

        public override bool subscript(Object index, ref Object r)
        {
            Number i = new Number(0);
            if (!index.getNumber(ref i)) {
                throw new NeonRuntimeException("DynamicConversionException", "to Number");
            }
            int ii = Number.number_to_int32(i);
            if (ii >= Value.Count || i.IsNegative()) {
                throw new NeonRuntimeException("PANIC", "Array index exceeds size " + Value.Count + ": " + i.ToString());
            }
            r = Value[ii];
            return true;
        }

        public override string toString()
        {
            StringBuilder r = new StringBuilder("[");
            bool first = true;
            for (int x = 0; x < Value.Count; x++) {
                if (!first) {
                    r.Append(", ");
                } else {
                    first = false;
                }
                r.Append(Value[x] != null ? Value[x].toLiteralString() : "null");
            }
            r.Append("]");
            return r.ToString();
        }

        private List<Object> Value;
    }

    public class ObjectBoolean: Object
    {
        public ObjectBoolean(bool b)
        {
            Value = b;
        }

        public ObjectBoolean(ObjectBoolean o)
        {
            Value = o.getBoolean(ref Value);
        }

        public override bool getBoolean(ref bool r)
        {
            r = Value;
            return true;
        }

        public override string toString()
        {
            return Value ? "TRUE" : "FALSE";
        }

        private bool Value;
    };

    public class ObjectBytes: Object
    {
        public ObjectBytes(byte[] b)
        {
            Value = b;
        }

        public override bool getBytes(ref byte[] b)
        {
            b = Value;
            return true;
        }

        public override string toString()
        {
            StringBuilder r = new StringBuilder("HEXBYTES \"");
            bool first = true;
            foreach (byte x in Value) {
                if (first) {
                    first = false;
                } else {
                    r.Append(' ');
                }
                r.AppendFormat("{0:x2}", x);
            }
            r.Append("\"");
            return r.ToString();
        }

        private byte[] Value;
    }

    public class ObjectDictionary : Object
    {
        public ObjectDictionary(SortedDictionary<string, Object> d)
        {
            Value = d;
        }

        public override bool getDictionary(ref SortedDictionary<string, Object> r)
        {
            r = Value;
            return true;
        }

        public override bool subscript(Object index, ref Object r)
        {
            r = null;
            string s = null;
            if (!index.getString(ref s)) {
                throw new NeonRuntimeException("DynamicConversionException", "to String");
            }
            if (s == null) {
                throw new NeonRuntimeException("DynamicConversionException", "to String");
            }

            if (Value.ContainsKey(s)) {
                r = Value[s];
                return true;
            }
            return false;
        }

        public override string toString()
        {
            StringBuilder r = new StringBuilder("{");
            bool first = true;
            foreach (KeyValuePair<string, Object> e in Value) {
                if (first) {
                    first = false;
                } else {
                    r.Append(", ");
                }
                r.Append(e.Key.Quote());
                r.Append(": ");
                r.Append(e.Value != null ? e.Value.toLiteralString() : "null");
            }
            r.Append("}");
            return r.ToString();
        }

        private SortedDictionary<string, Object> Value;
    }

    public class ObjectNumber: Object
    {
        public ObjectNumber(Number n)
        {
            Value = n;
        }

        public override bool getNumber(ref Number r)
        {
            r = Value;
            return true;
        }

        public override string toString()
        {
            return Value.ToString();
        }

        private Number Value;
    };

    public class ObjectString: Object
    {
        public ObjectString(string s)
        {
            Value = s;
        }

        public override bool getString(ref string r)
        {
            r = Value;
            return true;
        }

        public override string toLiteralString()
        {
            return Value.Quote();
        }

        public override string toString()
        {
            return Value;
        }

        private readonly string Value;
    };
}
