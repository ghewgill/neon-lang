namespace csnex.rtl
{
    public class runtime
    {
        private Executor exec;

        public runtime(Executor ex)
        {
            exec = ex;
        }

        public void assertionsEnabled()
        {
            exec.stack.Push(Cell.CreateBooleanCell(exec.enable_assert));
        }

        public void executorName()
        {
            exec.stack.Push(Cell.CreateStringCell("csnex"));
        }

        public void moduleIsMain()
        {
            exec.stack.Push(Cell.CreateBooleanCell(exec.ModuleIsMain()));
        }

        public void setRecursionLimit()
        {
            exec.param_recursion_limit = Number.number_to_int32(exec.stack.Pop().Number);
        }
    }
}
