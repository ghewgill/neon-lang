using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace csnex
{
    public class Cell
    {
        public enum Type
        {
            None,
            Address,
            Array,
            Boolean,
            Dictionary,
            Number,
            Object,
            String,
        }

        private Cell m_Address;
        private List<Cell> m_Array;
        private Boolean m_Boolean;
        private Dictionary<string, Cell> m_Dictionary;
        private Number m_Number;
        private Object m_Object;
        private String m_String;

#region Cell Property Accessors
        public Cell Address {
            get {
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
                Debug.Assert(type == Type.Boolean);
                return m_Boolean;
            }
            set {
                type = Type.Boolean;
                m_Boolean = value;
            }
        }

        public Dictionary<string, Cell> Dictionary {
            get {
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

        public Cell(Cell c)
        {
            type = Type.Address;
            m_Address = c;
        }

        public Cell(Dictionary<string, Cell> d)
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
                case Type.String:
                    return String;
            }
            return null;
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

        public void Set(Cell c)
        {
            if (type == Type.None) {
                type = Type.Address;
            }
            Debug.Assert(type == Type.Address);
            Address = c;
        }

        public void Set(Dictionary<string, Cell> d)
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
#endregion
    }
}
