package org.neon_lang.jnex;

import java.io.ByteArrayInputStream;
import java.io.DataInput;
import java.io.DataInputStream;
import java.util.ArrayList;

class Bytecode {
    class ExceptionInfo {
        int start;
        int end;
        int excid;
        int handler;
        int stack_depth;
    }

    class ClassInfo {
        int name;
        int[][] interfaces;
    }

    Bytecode(DataInput in)
    {
        int i;
        try {
            in.readFully(new byte[32]);
            global_size = readVint(in);
            int strtablesize = readVint(in);
            byte[] buf = new byte[strtablesize];
            in.readFully(buf);
            bytetable = getStringTable(buf);
            strtable = new String[bytetable.length];
            for (i = 0; i < bytetable.length; i++) {
                strtable[i] = new String(bytetable[i]);
            }

            int typesize = readVint(in);
            assert typesize == 0 : "types";

            int constantsize = readVint(in);
            assert constantsize == 0 : "constants";

            int variablesize = readVint(in);
            assert variablesize == 0 : "variables";

            int functionsize = readVint(in);
            assert functionsize == 0 : "function_export";

            int exceptionexportsize = readVint(in);
            assert exceptionexportsize == 0 : "exception_export";

            int interfaceexportsize = readVint(in);
            assert interfaceexportsize == 0 : "interface_export";

            int importsize = readVint(in);
            while (importsize > 0) {
                int name = readVint(in);
                byte[] hash = new byte[32];
                in.readFully(hash);
                importsize--;
            }

            functionsize = readVint(in);
            while (functionsize > 0) {
                int name = readVint(in);
                int entry = readVint(in);
                // TODO
                functionsize--;
            }

            int exceptionsize = readVint(in);
            exceptions = new ExceptionInfo[exceptionsize];
            i = 0;
            while (exceptionsize > 0) {
                ExceptionInfo ei = new ExceptionInfo();
                ei.start = readVint(in);
                ei.end = readVint(in);
                ei.excid = readVint(in);
                ei.handler = readVint(in);
                ei.stack_depth = readVint(in);
                exceptions[i] = ei;
                i++;
                exceptionsize--;
            }

            int classsize = readVint(in);
            classes = new ClassInfo[classsize];
            i = 0;
            while (classsize > 0) {
                ClassInfo ci = new ClassInfo();
                ci.name = readVint(in);
                int interfacecount = readVint(in);
                ci.interfaces = new int[interfacecount][];
                int ii = 0;
                while (interfacecount > 0) {
                    int methodcount = readVint(in);
                    ci.interfaces[ii]= new int[methodcount];
                    int mi = 0;
                    while (methodcount > 0) {
                        ci.interfaces[ii][mi] = readVint(in);
                        mi++;
                        methodcount--;
                    }
                    ii++;
                    interfacecount--;
                }
                classes[i] = ci;
                i++;
                classsize--;
            }

            code = new ArrayList<Byte>();
            while (true) {
                try {
                    code.add(in.readByte());
                } catch (java.io.EOFException x) {
                    break;
                }
            }

        } catch (java.io.IOException x) {
            System.err.println(x);
            System.exit(1);
        }
    }

    static byte[][] getStringTable(byte[] buf)
    {
        DataInput in = new DataInputStream(new ByteArrayInputStream(buf));
        ArrayList<byte[]> r = new ArrayList<byte[]>();
        while (true) {
            try {
                int len = readVint(in);
                byte[] b = new byte[len];
                in.readFully(b);
                r.add(b);
            } catch (java.io.EOFException x) {
                break;
            } catch (java.io.IOException x) {
                System.err.println(x);
                System.exit(1);
            }
        }
        return r.toArray(new byte[][] {});
    }

    static int readVint(DataInput in) throws java.io.IOException
    {
        int r = 0;
        while (true) {
            byte x = in.readByte();
            if ((r & 0xff000000) != 0) {
                System.err.println("Integer value exceeds maximum");
                System.exit(1);
            }
            r = (r << 7) | (x & 0x7f);
            if ((x & 0x80) == 0) {
                break;
            }
        }
        return r;
    }

    int global_size;
    byte[][] bytetable;
    String[] strtable;
    ExceptionInfo[] exceptions;
    ClassInfo[] classes;
    ArrayList<Byte> code;
}
