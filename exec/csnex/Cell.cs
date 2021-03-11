using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

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
#endregion
#region Constructors
        public Cell()
        {
            type = Type.None;
        }

        public Cell(Type t)
        {
            type = t;
        }

        public Cell(List<Cell> a)
        {
            type = Type.Array;
            m_Array = a;
        }

        public Cell(Boolean b)
        {
            type = Type.Boolean;
            m_Boolean = b;
        }

        public Cell(byte[] b)
        {
            type = Type.Bytes;
            m_Bytes = new byte[b.Length];
            b.CopyTo(m_Bytes, 0);
        }

        public Cell(Cell c)
        {
            type = Type.Address;
            m_Address = c;
        }

        public Cell(SortedDictionary<string, Cell> d)
        {
            type = Type.Dictionary;
            m_Dictionary = d;
        }

        public Cell(Number d)
        {
            type = Type.Number;
            m_Number = d;
        }

        public Cell(Object o)
        {
            type = Type.Object;
            m_Object = o;
        }

        public Cell(String s)
        {
            type = Type.String;
            m_String = s;
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
            m_String = null;
            type = Type.None;
        }

        public static string toString(Cell c)
        {
            string r;
            switch (c.type) {
                case Type.Array:
                {
                    int x;
                    r = "[";
                    for (x = 0; x < c.Array.Count; x++) {
                        if (r.Length > 1) {
                            r += ", ";
                        }
                        if (c.Array[x].type == Type.String) {
                            throw new NeonException("Array<String>.toString() not implemented.");
                        } else {
                            r += toString(c.Array[x]);
                        }
                    }
                    r += "]";
                    return r;
                }
                case Type.Boolean:
                    if (c.Boolean) {
                        return "TRUE";
                    }
                    return "FALSE";
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
                case Type.Boolean:
                    if (Boolean) {
                        return "TRUE";
                    }
                    return "FALSE";
                case Type.Object:
                    return Object.toString();
                case Type.String:
                    return String;
            }
            return toString(this);
        }

        public Boolean Equals(Cell x)
        {
            Debug.Assert(type == x.type);
            if (type != x.type) {
                return false;
            }
            switch (type) {
                case Type.Address:
                case Type.Array:
                case Type.Boolean:
                case Type.Bytes:
                case Type.Dictionary:
                    // Not implemenetd yet.
                    return false;
                case Type.Number:
                    return Number.IsEqual(Number, x.Number);
                case Type.Object:
                    // Not Implemented yet.
                    return false;
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

#region Set functions
        public void Set(List<Cell> a)
        {
            if (type == Type.None) {
                type = Type.Array;
            }
            Debug.Assert(type == Type.Array);
            Array = a;
        }

        public void Set(Boolean b)
        {
            if (type == Type.None) {
                type = Type.Boolean;
            }
            Debug.Assert(type == Type.Boolean);
            Boolean = b;
        }

        public void Set(byte[] b)
        {
            if (type == Type.None) {
                type = Type.Bytes;
            }
            Debug.Assert(type == Type.Bytes);
            m_Bytes = new byte[b.Length];
            b.CopyTo(m_Bytes, 0);
        }

        public void Set(Cell c)
        {
            if (type == Type.None) {
                type = Type.Address;
            }
            Debug.Assert(type == Type.Address);
            Address = c;
        }

        public void Set(SortedDictionary<string, Cell> d)
        {
            if (type == Type.None) {
                type = Type.Dictionary;
            }
            Debug.Assert(type == Type.Dictionary);
            Dictionary = d;
        }

        public void Set(Number n)
        {
            if (type == Type.None) {
                type = Type.Number;
            }
            Debug.Assert(type == Type.Number);
            Number = n;
        }

        public void Set(Object o)
        {
            if (type == Type.None) {
                type = Type.Object;
            }
            Debug.Assert(type == Type.Object);
            Object = o;
        }

        public void Set(string s)
        {
            if (type == Type.None) {
                type = Type.String;
            }
            Debug.Assert(type == Type.String);
            String = s;
        }
#endregion
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
