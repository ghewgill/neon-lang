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
            Number v = Exec.stack.Pop().Number;
            Exec.stack.Push(new Cell(v.ToString()));
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

    }
}
