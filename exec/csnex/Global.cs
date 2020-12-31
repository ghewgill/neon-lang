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

                // Object
                case "object__makeString":
                    object__makeString();
                    break;

                default:
                    throw new NotImplementedException(string.Format("global_callFunction(): \"{0}\" - invalid or unsupported predefined function call.", name));
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

        public void object__makeString()
        {
            Cell o = Exec.stack.Pop();
            Exec.stack.Push(new Cell(new ObjectString(o.String)));
        }

    }
}
