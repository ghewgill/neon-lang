namespace csnex
{
    public abstract class Object
    {
        public virtual bool getString(ref string s)
        {
            return false;
        }

        public virtual string toLiteralString()
        {
            return toString();
        }

        public abstract string toString();
    }

    public class ObjectString: Object
    {
        public ObjectString(string s)
        {
            this.s = s;
        }

        public override bool getString(ref string r)
        {
            r = s;
            return true;
        }

        // TODO: Use quoting function to quote the value properly.
        public override string toLiteralString()
        {
            return "\"" + s + "\"";
        }

        public override string toString()
        {
            return s;
        }

        private readonly string s;
    };
}
