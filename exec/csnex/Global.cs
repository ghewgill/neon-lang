using System.Collections.Generic;

namespace csnex
{
    public class Global
    {
        private Executor Exec;

        public Global(Executor exec)
        {
            Exec = exec;
        }

        public void Dispatch(string name)
        {
            // ToDo: Use System.Reflection to call functions out of
            //       the assembly.
            // Globals
            switch (name) {
                case "print":
                    print();
                    break;
                case "str":
                    str();
                    break;

                // Array
                case "array__append":
                    array__append();
                    break;
                case "array__concat":
                    array__concat();
                    break;
                case "array__extend":
                    array__extend();
                    break;
                case "array__size":
                    array__size();
                    break;
                case "array__toString__number":
                    array__toString__number();
                    break;

                // Boolean
                case "boolean__toString":
                    boolean__toString();
                    break;

                // Number
                case "number__toString":
                    number__toString();
                    break;

                // Object
                case "object__makeString":
                    object__makeString();
                    break;

                // String
                case "string__concat":
                    string__concat();
                    break;

                default:
                    throw new NotImplementedException(string.Format("Global::Dispatch(\"{0}\"); - invalid or unsupported predefined function call.", name));
            }
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
            Exec.stack.Push(new Cell(sbuf));
        }

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

        public void array__toString__number()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(new Cell(s));
        }

        public void boolean__toString()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(new Cell(s));
        }

        public void number__toString()
        {
            Number n = Exec.stack.Pop().Number;
            Exec.stack.Push(new Cell(n.ToString()));
        }

        public void object__makeString()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(new Cell(new ObjectString(o.String)));
        }

        public void string__concat()
        {
            Cell b = Exec.stack.Pop();
            Cell a = Exec.stack.Pop();

            Exec.stack.Push(new Cell(a.String + b.String));
        }

    }
}
