using System;
using System.Diagnostics;

namespace csnex
{
    public struct CommandLineOptions
    {
        public Boolean EnableAssertions;
        public string Filename;
        public string ExecutableName;
    }

    static class csnex
    {
        private static CommandLineOptions gOptions;

        private static void ShowUsage()
        {
            Console.Error.Write("Usage:\n\n");
            Console.Error.Write("   {0} [options] program.neonx\n", gOptions.ExecutableName);
            Console.Error.Write("\n Where [options] is one or more of the following:\n");
            Console.Error.Write("     -h       Display this help screen.\n");
            Console.Error.Write("     -n       No Assertions\n");
        }

        private static Boolean ParseOptions(string[] args)
        {
            Boolean Retval = false;
            for (int nIndex = 0; nIndex < args.Length; nIndex++) {
                if (args[nIndex][0] == '-') {
                    if (args[nIndex][1] == 'h' || args[nIndex][1] == '?' || ((args[nIndex][1] == '-' && args[nIndex][2] != '\0') && (args[nIndex][2] == 'h'))) {
                        ShowUsage();
                        Environment.Exit(1);
                    } else if (args[nIndex][1] == 'n') {
                        gOptions.EnableAssertions = false;
                    } else {
                        Console.Error.WriteLine(string.Format("Unknown option {0}\n", args[nIndex]));
                        return false;
                    }
                } else {
                    Retval = true;
                    gOptions.Filename = args[nIndex];
                }
            }
            if (gOptions.Filename.Length == 0) {
                Console.Error.WriteLine("You must provide a Neon binary file to execute.\n");
                Retval = false;
            }
            return Retval;
        }

        static string GetApplicationName()
        {
            return Process.GetCurrentProcess().ProcessName;
        }

        static int Main(string[] args)
        {
            int retval = 0;
            gOptions.ExecutableName = GetApplicationName();

            if(!ParseOptions(args)) {
                return 3;
            }

            System.IO.FileStream fs;
            try {
                fs = new System.IO.FileStream(gOptions.Filename, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
            } catch(Exception ex) {
                Console.Error.Write("Could not open Neon executable: {0}\nError: {1} - {2}.\n", gOptions.Filename, ex.HResult & 0xffff, ex.Message);
                return 2;
            }

            long nSize = fs.Length;
            byte[] code = new byte[nSize];
            fs.Read(code, 0, (int)nSize);
            fs.Close();

            Bytecode bc = new Bytecode();
            bc.LoadBytecode(gOptions.Filename, code);

            Executor exec = new Executor(bc);
            retval = exec.Run(gOptions.EnableAssertions);

            return retval;
        }
    }
}
