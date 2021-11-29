using System.Collections.Generic;
using System.Text;

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
                r.Add(Cell.CreateStringCell(value));
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

        public static bool Compare(this SortedDictionary<string, Cell> self, SortedDictionary<string, Cell> rhs)
        {
            if (self.Keys.Count != rhs.Keys.Count) {
                return false;
            }
            foreach (string k in self.Keys) {
                if (!rhs.ContainsKey(k)) {
                    return false;
                }
                if (!self[k].Equals(rhs[k])) {
                    return false;
                }
            }
            return true;
        }

        public static string Quote(this string s)
        {
            StringBuilder r = new StringBuilder("\"");
            for (int i = 0; i < s.Length; i++) {
                char c = s[i];
                int ch = char.ConvertToUtf32(s, i);

                switch (c) {
                    case '\b': r.Append("\\b"); break;
                    case '\f': r.Append("\\f"); break;
                    case '\n': r.Append("\\n"); break;
                    case '\r': r.Append("\\r"); break;
                    case '\t': r.Append("\\t"); break;
                    case '"':
                    case '\\':
                        r.Append('\\');
                        r.Append(c);
                        break;
                    default:
                        if (c >= ' ' && c < 0x7f) {
                            r.Append(c);
                        } else if (ch < 0x10000) {
                            r.AppendFormat("\\u{0}", ch.ToString("x4"));
                        } else {
                            r.AppendFormat("\\u{0}", ch.ToString("x8"));
                        }
                        break;
                }
            }
            r.Append("\"");
            return r.ToString();
        }
    }
}
