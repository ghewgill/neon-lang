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
            exec.stack.Push(new Cell(exec.enable_assert));
        }

        public void executorName()
        {
            exec.stack.Push(new Cell("csnex"));
        }

        public void setRecursionLimit()
        {
            exec.param_recursion_limit = Number.number_to_int32(exec.stack.Pop().Number);
        }
    }
}
