using System;
using System.IO;

namespace csnex.rtl
{
    public class FileObject: Object
    {
        public FileObject(Stream fs)
        {
            file = fs;
        }

        public override string toString()
        {
            return string.Format("<FILE:{0}>", file.ToString());
        }

        public Stream file;
    };

    public class io
    {
        private readonly Executor Exec;
        public Cell stdin { get; private set; }
        public Cell stdout { get; private set; }
        public Cell stderr { get; private set; }

        public io(Executor exe)
        {
            Exec = exe;
            stdin = new Cell(new FileObject(Console.OpenStandardInput()));
            stdout = new Cell(new FileObject(Console.OpenStandardOutput()));
            stderr = new Cell(new FileObject(Console.OpenStandardError()));
        }

        private Stream check_file(object pf)
        {
            Stream f = ((FileObject)pf).file;
            if (f == null) {
                Exec.Raise("IoException.InvalidFile", "");
            }
            return f;
        }


        public void close()
        {
            object ppf = Exec.stack.Pop().Object;

            Stream f = check_file(ppf);
            if (f == null) {
                return;
            }
            f.Close();
            f = null;
        }

        public void flush()
        {
            object ppf = Exec.stack.Pop().Object;

            Stream f = check_file(ppf);
            if (f == null) {
                return;
            }
            f.Flush();
        }

        public void open()
        {
            Number mode = Exec.stack.Pop().Number;
            string name = Exec.stack.Pop().String;

            FileAccess fa;
            FileMode fm;
            switch ((Mode)Number.number_to_uint32(mode)) {
                case Mode.read:
                    fa = FileAccess.Read;
                    fm = FileMode.Open;
                    break;
                case Mode.write:
                    fa = FileAccess.Write;
                    fm = FileMode.Append;
                    break;
                default:
                    Exec.stack.Push(new Cell(new FileObject(null)));
                    return;
            }

            try {
                FileStream f = new FileStream(name, fm, fa);
                Exec.stack.Push(new Cell(new FileObject(f)));
            } catch (IOException iox) {
                Exec.Raise("IoException.Open", iox.HResult.ToString());
            }
        }

        public void readBytes()
        {
            Number count = Exec.stack.Pop().Number;
            object pf = Exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            try {
                int ncount = Number.number_to_int32(count);

                byte[] r = new byte[ncount];
                int n = f.Read(r, 0, ncount);

                if (n != ncount) {
                    Array.Resize(ref r, n);
                }
                Exec.stack.Push(new Cell(r));
            } catch {
                Exec.Raise("IoException.Read", "");
            }
        }

        public void seek()
        {
            Number whence = Exec.stack.Pop().Number;
            Number offset = Exec.stack.Pop().Number;
            object pf = Exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            SeekOrigin w;
            switch ((SeekBase)Number.number_to_int32(whence)) {
                case SeekBase.absolute: w = SeekOrigin.Begin; break;
                case SeekBase.relative: w = SeekOrigin.Current; break;
                case SeekBase.fromEnd:  w = SeekOrigin.End; break;
                default:
                    return;
            }
            f.Seek(Number.number_to_int64(offset), w);
        }

        public void tell()
        {
            object pf = Exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            Exec.stack.Push(new Cell(new Number(f.Seek(0, SeekOrigin.Current))));
        }

        public void truncate()
        {
            object pf = Exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            long ofs = f.Seek(0, SeekOrigin.Current);
            try {
                f.SetLength(ofs);
            } catch {
                Exec.Raise("IoException.Write", "");
            }
        }

        public void write()
        {
            string buf = Exec.stack.Pop().String;
            object pf = Exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            try {
                StreamWriter sw = new StreamWriter(f);
                sw.Write(buf);
                sw.Flush();
            } catch {
                Exec.Raise("IoException.Write", "");
            }
        }

        public void writeBytes()
        {
            byte[] b = Exec.stack.Pop().Bytes;
            object pf = Exec.stack.Pop().Object;

            Stream f = check_file(pf);
            if (f == null) {
                return;
            }

            try {
                f.Write(b, 0, b.Length);
            } catch {
                Exec.Raise("IoException.Write", "");
            }
        }
    }
}
