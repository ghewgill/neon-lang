using System;
using System.IO;

namespace csnex.rtl
{
    public class TextFileObject: Object
    {
        public TextFileObject(Stream fs)
        {
            file = fs;
        }

        public override string toString()
        {
            return string.Format("<TEXTFILE:{0}>", file.ToString());
        }

        public Stream file;
    };

    public class textio
    {
        private Executor exec;
        public Cell stdin { get; private set; }
        public Cell stdout { get; private set; }
        public Cell stderr { get; private set; }

        public textio(Executor exe)
        {
            exec = exe;
            stdin = Cell.CreateObjectCell(new TextFileObject(Console.OpenStandardInput()));
            stdout = Cell.CreateObjectCell(new TextFileObject(Console.OpenStandardOutput()));
            stderr = Cell.CreateObjectCell(new TextFileObject(Console.OpenStandardError()));
        }

        private Stream check_file(object pf)
        {
            TextFileObject f = (TextFileObject)pf;
            if (f == null) {
                // ToDo: decide if SEH would be a better fit here.
                // throw new NeonRuntimeException("TextioException.InvalidFile", "");
                exec.Raise("TextioException.InvalidFile", "");
                return null;
            }
            return f.file;
        }

        public void close()
        {
            object ppf = exec.stack.Pop().Object;

            Stream f = check_file(ppf);

            if (f == null) {
                return;
            }
            f.Close();
            ppf = null;
        }

        public void open()
        {
            Number mode = exec.stack.Pop().Number;
            string name = exec.stack.Pop().String;

            FileAccess fa;
            FileMode fm;
            switch ((Mode)Number.number_to_uint32(mode)) {
                case Mode.read:
                    fa = FileAccess.Read;
                    fm = FileMode.Open;
                    break;
                case Mode.write:
                    fa = FileAccess.Write;
                    fm = FileMode.OpenOrCreate;
                    break;
            default:
                exec.stack.Push(Cell.CreateObjectCell(new TextFileObject(null)));
                return;
            }

            try {
                FileStream f = new FileStream(name, fm, fa);
                exec.stack.Push(Cell.CreateObjectCell(new TextFileObject(f)));
            } catch (IOException iox) {
                exec.Raise("TextioException.Open", iox.HResult.ToString());
            }
        }

        public void readLine()
        {
            object pf = exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            try {
                StreamReader sr = new StreamReader(f);
                Cell r = Cell.CreateStringCell(sr.ReadLine());
                Boolean ret = r.String != null;

                exec.stack.Push(Cell.CreateBooleanCell(ret));
                exec.stack.Push(r);
            } catch (IOException iox) {
                exec.Raise("TextioException.Read", iox.HResult.ToString());
            }
        }

        public void seekEnd()
        {
            object pf = exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            f.Seek(0, SeekOrigin.End);
        }

        public void seekStart()
        {
            object pf = exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            f.Seek(0, SeekOrigin.Begin);
        }

        public void truncate()
        {
            object pf = exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            try {
                long ofs = f.Seek(0, SeekOrigin.Current);
                f.SetLength(ofs);
            } catch {
                exec.Raise("TextioException.Write", "");
            }
        }

        public void writeLine()
        {
            string s = exec.stack.Pop().String;
            object pf = exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            try {
                StreamWriter sw = new StreamWriter(f);
                sw.WriteLine(s);
                sw.Flush();
            } catch (IOException iox) {
                exec.Raise("TextioException.Write", iox.HResult.ToString());
            }
        }
    }
}
