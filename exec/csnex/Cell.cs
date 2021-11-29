using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace csnex
{
    [DebuggerDisplay("{DebugType,nq}:{ToString(),nq}")]
    public class Cell
    {
        private string DebugType { get { return Enum.GetName(typeof(Type), type); } }
        public enum Type
        {
            None,
            Address,
            Array,
            Boolean,
            Bytes,
            Dictionary,
            Number,
            Object,
            Other,
            String,
        }

        private Cell m_Address;
        private List<Cell> m_Array;
        private Boolean m_Boolean;
        private byte[] m_Bytes;
        private SortedDictionary<string, Cell> m_Dictionary;
        private Number m_Number;
        private Object m_Object;
        private String m_String;
        private object m_Other;

#region Cell Property Accessors
        public Cell Address {
            get {
                if (type == Type.None) {
                    type = Type.Address;
                }
                Debug.Assert(type == Type.Address);
                return m_Address;
            }
            set {
                type = Type.Address;
                m_Address = value;
            }
        }

        public List<Cell> Array {
            get {
                if (type == Type.None) {
                    type = Type.Array;
                    if (m_Array == null) {
                        m_Array = new List<Cell>();
                    }
                }
                Debug.Assert(type == Type.Array);
                return m_Array;
            }
            set {
                type = Type.Array;
                m_Array = value;
            }
        }

        public Boolean Boolean {
            get {
                if (type == Type.None) {
                    type = Type.Boolean;
                }
                Debug.Assert(type == Type.Boolean);
                return m_Boolean;
            }
            set {
                type = Type.Boolean;
                m_Boolean = value;
            }
        }

        public byte[] Bytes {
            get {
                if (type == Type.None) {
                    type = Type.Bytes;
                    if (m_Bytes == null) {
                        m_Bytes = new byte[0];
                    }
                }
                Debug.Assert(type == Type.Bytes);
                return m_Bytes;
            }
            set {
                type = Type.Bytes;
                if (value == null) {
                    m_Bytes = value;
                    return;
                }
                m_Bytes = new byte[value.Length];
                value.CopyTo(m_Bytes, 0);
            }
        }

        public SortedDictionary<string, Cell> Dictionary {
            get {
                if (type == Type.None) {
                    type = Type.Dictionary;
                    if (m_Dictionary == null) {
                        m_Dictionary = new SortedDictionary<string, Cell>();
                    }
                }
                Debug.Assert(type == Type.Dictionary);
                return m_Dictionary;
            }
            set {
                type = Type.Dictionary;
                m_Dictionary = value;
            }
        }

        public Number Number {
            get {
                if (type == Type.None) {
                    type = Type.Number;
                    if (m_Number == null) {
                        m_Number = new Number();
                    }
                }
                Debug.Assert(type == Type.Number);
                return m_Number;
            }
            set {
                type = Type.Number;
                m_Number = value;
            }
        }

        public Object Object {
            get {
                if (type == Type.None) {
                    type = Type.Object;
                }
                Debug.Assert(type == Type.Object);
                return m_Object;
            }
            set {
                type = Type.Object;
                m_Object = value;
            }
        }

        public object Other {
            get {
                if (type == Type.Other) {
                    type = Type.Other;
                }
                Debug.Assert(type == Type.Other);
                return m_Other;
            }
            set {
                type = Type.Other;
                m_Other = value;
            }
        }

        public String String {
            get {
                if (type == Type.None) {
                    type = Type.String;
                    if (m_String == null) {
                        m_String = "";
                    }
                }
                Debug.Assert(type == Type.String);
                return m_String;
            }
            set {
                type = Type.String;
                m_String = value;
            }
        }

        public Type type { get; private set; }

        public UInt64 AllocNum { get; }
#endregion
#region Constructors
        public Cell(UInt64 alloc = 0)
        {
            type = Type.None;
            AllocNum = alloc;
        }

        public Cell(Type t, UInt64 alloc = 0)
        {
            type = t;
            AllocNum = alloc;
        }
#endregion
#region Static creation functions
        public static Cell CreateAddressCell(Cell c)
        {
            Cell r = new Cell();
            r.type = Type.Address;
            r.m_Address = c;
            return r;
        }

        public static Cell CreateArrayCell(List<Cell> a)
        {
            Cell r = new Cell();
            r.type = Type.Array;
            r.m_Array = a;
            return r;
        }

        public static Cell CreateBooleanCell(Boolean b)
        {
            Cell r = new Cell();
            r.type = Type.Boolean;
            r.m_Boolean = b;
            return r;
        }

        public static Cell CreateBytesCell(byte[] b)
        {
            Cell r = new Cell();
            r.type = Type.Bytes;
            r.m_Bytes = new byte[b.Length];
            b.CopyTo(r.m_Bytes, 0);
            return r;
        }

        public static Cell CreateDictionaryCell(SortedDictionary<string, Cell> d)
        {
            Cell r = new Cell();
            r.type = Type.Dictionary;
            r.m_Dictionary = d;
            return r;
        }

        public static Cell CreateNumberCell(Number d)
        {
            Cell r = new Cell();
            r.type = Type.Number;
            r.m_Number = d;
            return r;
        }

        public static Cell CreateObjectCell(Object o)
        {
            Cell r = new Cell();
            r.type = Type.Object;
            r.m_Object = o;
            return r;
        }

        public static Cell CreateOtherCell(object p)
        {
            Cell r = new Cell();
            r.type = Type.Other;
            r.m_Other = p;
            return r;
        }

        public static Cell CreateStringCell(String s)
        {
            Cell r = new Cell();
            r.type = Type.String;
            r.m_String = s;
            return r;
        }
#endregion
        public void ResetCell()
        {
            m_Address = null;
            m_Array = null;
            m_Boolean = false;
            m_Bytes = null;
            m_Dictionary = null;
            m_Number = null;
            m_Object = null;
            m_Other = null;
            m_String = null;
            type = Type.None;
        }

        public static string toString(Cell c)
        {
            StringBuilder r = new StringBuilder();
            switch (c.type) {
                case Type.Array:
                {
                    r.Append("[");
                    int x;
                    for (x = 0; x < c.Array.Count; x++) {
                        if (r.Length > 1) {
                            r.Append(", ");
                        }
                        if (c.Array[x].type == Type.String) {
                            r.Append(c.Array[x].String.Quote());
                        } else {
                            r.Append(toString(c.Array[x]));
                        }
                    }
                    r.Append("]");
                    return r.ToString();
                }
                case Type.Boolean:
                    if (c.Boolean) {
                        return "TRUE";
                    }
                    return "FALSE";
                case Type.Dictionary:
                    r.Append("{");
                    bool first = true;
                    foreach (KeyValuePair<string, Cell> e in c.Dictionary) {
                        if (first) {
                            first = false;
                        } else {
                            r.Append(", ");
                        }
                        r.Append(e.Key.Quote());
                        r.Append(": ");
                        r.Append(e.Value != null ? e.Value.ToString().Quote() : "null");
                    }
                    r.Append("}");
                    return r.ToString();
                case Type.Number:
                    return c.Number.ToString();
                case Type.String:
                    return c.String;
            }
            throw new NotImplementedException(string.Format("ToString not implemented for the {0} Cell.", c.type.ToString()));
        }

        public override String ToString()
        {
            switch (type) {
                case Type.Address:
                    if (Address != null) {
                        return Address.ToString();
                    }
                    return "NIL";
                case Type.Boolean:
                    if (Boolean) {
                        return "TRUE";
                    }
                    return "FALSE";
                case Type.Object:
                    return Object.toString();
                case Type.Other:
                    return Other.ToString();
                case Type.String:
                    return String;
            }
            return toString(this);
        }

        public Boolean Equals(Cell x)
        {
            if (type != x.type) {
                return false;
            }
            switch (type) {
                case Type.Address:
                case Type.Array:
                case Type.Boolean:
                case Type.Bytes:
                case Type.Dictionary:
                    // Not implemented yet.
                    return false;
                case Type.Number:
                    return Number.IsEqual(Number, x.Number);
                case Type.Object:
                    // Not implemented yet.
                    return false;
                case Type.Other:
                    return m_Other == x.m_Other;
                case Type.String:
                    return String.Compare(String, x.String) == 0;
            }
            return false;
        }

        public static void CopyCell(Cell dest, Cell source)
        {
            Debug.Assert(source != null);
            Debug.Assert(dest != null);

            dest.type = source.type;
            switch (source.type) {
                case Type.Address:
                    dest.m_Address = source.Address;
                    break;
                case Type.Array:
                    dest.Array = new List<Cell>(source.Array.Count);
                    for (int i = 0; i < source.Array.Count; i++) {
                        dest.Array.Add(FromCell(source.Array[i]));
                    }
                    break;
                case Type.Boolean:
                    dest.m_Boolean = source.Boolean;
                    break;
                case Type.Bytes:
                    dest.Bytes = new byte[source.Bytes.Length];
                    System.Array.Copy(source.Bytes, 0, dest.Bytes, 0, source.Bytes.Length);
                    break;
                case Type.Dictionary:
                    dest.m_Dictionary = new SortedDictionary<string, Cell>();
                    List<string> keys = source.Dictionary.Keys.ToList();
                    foreach (string k in keys) {
                        dest.Dictionary.Add(k, FromCell(source.Dictionary[k]));
                    }
                    break;
                case Type.Number:
                    dest.Number = new Number(source.Number);
                    break;
                case Type.Object:
                    dest.m_Object = source.Object;
                    break;
                case Type.Other:
                    dest.m_Other = source.Other;
                    break;
                case Type.String:
                    dest.String = source.String;
                    break;
                case Type.None:
                    dest.ResetCell();
                    break;
            }
        }

        public static Cell FromCell(Cell c)
        {
            Debug.Assert(c != null);
            Cell x = new Cell(Type.None);
            CopyCell(x, c);
            return x;
        }
#region Index functions
        public Cell ArrayIndexForWrite(int i)
        {
            if (type == Type.None) {
                type = Type.Array;
            }
            Debug.Assert(type == Type.Array);
            if (Array == null) {
                Array = new List<Cell>(i);
            }
            if (i >= Array.Count) {
                int s = Array.Count;
                for (int n = s; n < i+1; n++) {
                    Array.Insert(n, new Cell(Type.None));
                }
            }
            return Array.ElementAt(i);
        }

        public Cell ArrayIndexForRead(int i)
        {
            if (type == Type.None) {
                type = Type.Array;
            }
            Debug.Assert(type == Type.Array);
            if (m_Array == null) {
                m_Array = new List<Cell>(i+1);
            }
            return Array.ElementAt(i);
        }

        public Cell DictionaryIndexForWrite(string index)
        {
            if (type == Type.None) {
                type = Type.Dictionary;
            }
            Debug.Assert(type == Type.Dictionary);
            if (m_Dictionary == null) {
                m_Dictionary = new SortedDictionary<string, Cell>();
            }
            if (m_Dictionary.ContainsKey(index)) {
                return m_Dictionary[index];
            }
            m_Dictionary.Add(index, new Cell(Type.None));
            return m_Dictionary[index];
        }

        public Cell DictionaryIndexForRead(string index)
        {
            if (type == Type.None) {
                type = Type.Dictionary;
            }
            Debug.Assert(type == Type.Dictionary);
            if (m_Dictionary == null) {
                m_Dictionary = new SortedDictionary<string, Cell>();
                m_Dictionary.Add(index, new Cell(Type.None));
            }
            return m_Dictionary[index];
        }
#endregion
    }
}
