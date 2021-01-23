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

        public Number(Int32 n)
        {
            val = n;
        }
#endregion

        public static Number FromString(string str)
        {
            return new Number(Decimal.Parse(str, System.Globalization.NumberStyles.Float));
        }

#region Static Arithmetic Functions
        public static Number Add(Number x, Number y)
        {
            return new Number(x.val + y.val);
        }

        public static Number Subtract(Number x, Number y)
        {
            return new Number(x.val - y.val);
        }

        public static Number Multiply(Number x, Number y)
        {
            return new Number(x.val * y.val);
        }

        public static Number Divide(Number x, Number y)
        {
            return new Number(x.val / y.val);
        }

        public static Number Pow(Number x, Number y)
        {
            if (y.IsInteger() && !y.IsNegative()) {
                UInt32 iy = number_to_uint32(y);
                Number r = new Number(1);
                while (iy != 0) {
                    if ((iy & 1) == 1) {
                        r = Multiply(r, x);
                    }
                    x = Multiply(x, x);
                    iy >>= 1;
                }
                return r;
            }
            return new Number(Decimal.ToInt64(x.val) ^ Decimal.ToInt64(y.val));
        }

        public static Number Modulo(Number x, Number y)
        {
            return new Number(Decimal.Remainder(x.val, y.val));
        }

        public static Number Negate(Number x)
        {
            return new Number(Decimal.Negate(x.val));
        }
#endregion
#region Static "Is" Functions
        public static bool IsGreaterThan(Number x, Number y)
        {
            return Decimal.Compare(x.val, y.val) > 0;
        }

        public static bool IsLessThan(Number x, Number y)
        {
            return Decimal.Compare(x.val, y.val) < 0;
        }

        public static bool IsLessOrEqual(Number x, Number y)
        {
            return Decimal.Compare(x.val, y.val) <= 0;
        }

        public static bool IsGreaterOrEqual(Number x, Number y)
        {
            return Decimal.Compare(x.val, y.val) >= 0;
        }

        public static bool IsEqual(Number x, Number y)
        {
            return Decimal.Equals(x.val, y.val);
        }
#endregion
#region "Is" Tests
        public bool IsInteger()
        {
            Decimal i = Decimal.Truncate(val);
            return Decimal.Compare(val, i) == 0;
        }

        public bool IsNegative()
        {
            return Math.Sign(val) == -1;
        }

        public bool IsZero()
        {
            return val == Decimal.Zero;
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

        internal static Int64 number_to_int64(Number index)
        {
            throw new System.NotImplementedException();
        }
    }
#endregion
}
