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
            Number,
            Object,
            String,
        }

        private Cell m_Address;
        private Number m_Number;
        private Object m_Object;
        private String m_String;

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

#region Constructors
        public Cell()
        {
            type = Type.None;
        }

        public Cell(Cell c)
        {
            type = Type.Address;
            Address = c;
        }

        public Cell(Type t)
        {
            type = t;
        }

        public Cell(Object o)
        {
            Object = o;
            type = Type.Object;
        }

        public Cell(String s)
        {
            String = s;
            type = Type.String;
        }

        public Cell(Number d)
        {
            Number = d;
            type = Type.Number;
        }
#endregion

        public void ResetCell()
        {
            Address = null;
            Number = null;
            Object = null;
            String = null;
            type = Type.None;
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
    }
}
