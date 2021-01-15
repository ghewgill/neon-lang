using System;

namespace csnex {

    public class Number
    {
        private Decimal val;

#region Constructors
        public Number()
        {
            val = 0;
        }

        public Number(Decimal n)
        {
            val = n;
        }
#endregion

        public static Number FromString(string str)
        {
            return new Number(Decimal.Parse(str, System.Globalization.NumberStyles.Float));
        }

        public override string ToString()
        {
            return val.ToString();
        }
    }
}
