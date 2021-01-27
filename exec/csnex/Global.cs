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

                // Boolean
                case "boolean__toString":
                    boolean__toString();
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

        public void boolean__toString()
        {
            string s = Cell.toString(Exec.stack.Pop());
            Exec.stack.Push(new Cell(s));
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
