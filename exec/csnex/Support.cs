using System;
using System.Collections.Generic;
using System.IO;

namespace csnex
{
    public class Support
    {
        static public List<string> Paths;

        public Support(string source_path)
        {
            Paths = new List<string>();

            if (source_path != null && source_path.Length > 0) {
                // Check the Neon executable path, first.
                Paths.Add(string.Format("{0}{1}", source_path, Path.DirectorySeparatorChar));
            }
            // Then, check current directory;
            Paths.Add(string.Format(".{0}", Path.DirectorySeparatorChar));

            // Get paths from %NEONPATH% environment variable, if there is one.
            string ne = Environment.GetEnvironmentVariable("NEONPATH");
            if (ne != null) {
                string[] evpaths = ne.Split(Path.PathSeparator);
                foreach (string s in evpaths) {
                    // Don't bother pushing empty paths.
                    if (s.Length > 0) {
                        Paths.Add(s);
                    }
                }
            }
            // Next, check for .neonpath, and process any paths that might exist in it.
            try {
                StreamReader sr = new StreamReader(".neonpath");
                string line = null;
                for (;;) {
                    line = sr.ReadLine();
                    if (line == null) {
                        break;
                    }
                    // Don't bother pushing empty paths.
                    if (line.Length > 0) {
                        Paths.Add(line);
                    }
                }
            } catch (IOException) {
            }
        }

        public Module ReadModule(string name)
        {
            Module m = new Module();
            Stream s = OpenModule(name, out m.SourcePath);
            if (s == null) {
                throw new NeonException(string.Format("Could not find Neon module \"{0}\"", m.Name));
            }
            m.Name = name;
            m.Code = new byte[s.Length];
            s.Read(m.Code, 0, m.Code.Length);
            s.Close();
            return m;
        }

        private static Stream OpenModule(string name, out string actualPath)
        {
            string filename;
            foreach (String path in Paths) {
                // ToDo: Locate .neon (source) and .neond (debug) files, and return full paths and handles to those as well.
                filename = String.Format("{0}{1}{2}.neonx", path, Path.DirectorySeparatorChar, name);
                // ToDo: Look for path/name/name.neonx for extension modules.
                /*filename = String.Format("{0}{1}{2}{3}{4}.neonx",
                                            path,
                                            Path.DirectorySeparatorChar,
                                            name,
                                            Path.DirectorySeparatorChar,
                                            name
                );*/
                if (File.Exists(filename)) {
                    FileStream r = new FileStream(filename, FileMode.Open, FileAccess.Read);
                    // Make sure we return the actual path we found the module at.
                    actualPath = path;
                    return r;
                }
            }
            actualPath = null;
            return null;
        }
    }
}
