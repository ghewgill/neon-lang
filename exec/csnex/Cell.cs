using System;

namespace csnex
{
    public class Cell
    {
        public enum Type
        {
            None,
            Address,
            Object,
            String,
        }

        public Object Object;
        public String String;

        public Type type { get; private set; }

        public Cell()
        {
            type = Type.None;
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
    }
}
