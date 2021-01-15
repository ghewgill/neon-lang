using System;
using System.Diagnostics;

namespace csnex
{
    public class Cell
    {
        public enum Type
        {
            None,
            Address,
            Boolean,
            Number,
            Object,
            String,
        }

        private Cell m_Address;
        private Boolean m_Boolean;
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
            m_Boolean = false;
            m_Number = null;
            m_Object = null;
            m_String = null;
            type = Type.None;
        }

        public static string toString(Cell c)
        {
            switch (c.type) {
                case Type.Boolean:
                    if (c.Boolean) {
                        return "TRUE";
                    }
                    return "FALSE";
            }
            throw new NotImplementedException(string.Format("ToString not implemented for the {0} Cell.", c.type.ToString()));
        }

#region Set functions
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

        public void Set(Number n)
        {
            if (type == Type.None) {
                type = Type.Number;
            }
            Debug.Assert(type == Type.Number);
            Number = n;
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
    }
}
