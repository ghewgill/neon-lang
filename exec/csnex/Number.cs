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

#region Static Arithmetic Functions
        public static Number Subtract(Number x, Number y)
        {
            return new Number(x.val - y.val);
        }
#endregion
#region Static "Is" Functions
        public static bool IsEqual(Number x, Number y)
        {
            return Decimal.Equals(x.val, y.val);
        }

        public bool IsInteger()
        {
            Decimal i = Decimal.Truncate(val);
            return Decimal.Compare(val, i) == 0;
        }

        public bool IsNegative()
        {
            return Math.Sign(val) == -1;
        }
#endregion
#region Static Number Conversions
        public static UInt32 number_to_uint32(Number n)
        {
            return Decimal.ToUInt32(n.val);
        }

        public static Int32 number_to_int32(Number n)
        {
            return Decimal.ToInt32(n.val);
        }
#endregion
#region Overrides
        public override string ToString()
        {
            return val.ToString();
        }
    }
#endregion
}
