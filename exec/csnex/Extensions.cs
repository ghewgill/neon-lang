using System.Collections.Generic;

namespace csnex
{
    internal static partial class CodeExtensions
    {
        public static int Compare(this byte[] self, byte[] rhs)
        {
            if (self == null && rhs == null) {
                return 0;
            } else if (self != null && rhs == null) {
                if (self.Length == 0) {
                    return 0;
                }
                return 1;
            } else if (self == null && rhs != null) {
                if (rhs.Length == 0) {
                    return 0;
                }
                return -1;
            }

            int i;
            for (i = 0; i < self.Length && i < rhs.Length; i++) {
                if (self[i] > rhs[i]) {
                    return 1;
                } else if (self[i] < rhs[i]) {
                    return -1;
                }
            }
            if (i == self.Length && i == rhs.Length) {
                return 0; // The lhs and rhs are proven to be equal to one another.
            } else if (i == self.Length) {
                return -1; // rhs still has elements to process, so we must be LESS THAN the rhs.
            } else if (i == rhs.Length) {
                return 1; // lhs still has elements to process, so we must be GREATER than the rhs.
            }
            // Could this ever happen?
            return 0;
        }

        public static List<Cell> ToCellArray(this List<string> self)
        {
            List<Cell> r = new List<Cell>();
            foreach (string value in self) {
                r.Add(new Cell(value));
            }
            return r;
        }

        public static bool Compare(this List<Cell> self, List<Cell> rhs)
        {
            if (self.Count != rhs.Count) {
                return false;
            }
            for (int i = 0; i < self.Count; i++) {
                if (!self[i].Equals(rhs[i])) {
                    return false;
                }
            }
            return true;
        }

        public static string Quote(this string s)
        {
            string r = "\"";
            for (int i = 0; i < s.Length; i++) {
                char c = s[i];
                int ch = char.ConvertToUtf32(s, i);

                switch (c) {
                    case '\b': r += "\\b"; break;
                    case '\f': r += "\\f"; break;
                    case '\n': r += "\\n"; break;
                    case '\r': r += "\\r"; break;
                    case '\t': r += "\\t"; break;
                    case '"':
                    case '\\':
                        r += '\\';
                        r += c;
                        break;
                    default:
                        if (c >= ' ' && c < 0x7f) {
                            r += c;
                        } else if (ch < 0x10000) {
                            r += string.Format("\\u{0}", ch.ToString("x4"));
                        } else {
                            r += string.Format("\\u{0}", ch.ToString("x8"));
                        }
                        break;
                }
            }
            r += "\"";
            return r;
        }
    }
}
