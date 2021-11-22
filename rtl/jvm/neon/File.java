package neon;

public class File {

    public static neon.lib.file$FileResult copy(java.lang.String filename, java.lang.String destination) {
        try {
            java.nio.file.Files.copy(
                java.nio.file.FileSystems.getDefault().getPath(filename),
                java.nio.file.FileSystems.getDefault().getPath(destination)
            );
        } catch (java.io.IOException x) {
            neon.lib.file$FileResult r = new neon.lib.file$FileResult();
            r._choice = 1; // error
            r.error = "FileException: " + x;
            return r;
        }
        neon.lib.file$FileResult r = new neon.lib.file$FileResult();
        r._choice = 0; // ok
        return r;
    }

    public static neon.lib.file$FileResult copyOverwriteIfExists(java.lang.String filename, java.lang.String destination) {
        try {
            java.nio.file.Files.copy(
                java.nio.file.FileSystems.getDefault().getPath(filename),
                java.nio.file.FileSystems.getDefault().getPath(destination),
                java.nio.file.StandardCopyOption.REPLACE_EXISTING
            );
        } catch (java.io.IOException x) {
            neon.lib.file$FileResult r = new neon.lib.file$FileResult();
            r._choice = 1; // error
            r.error = "FileException: " + x;
            return r;
        }
        neon.lib.file$FileResult r = new neon.lib.file$FileResult();
        r._choice = 0; // ok
        return r;
    }

    public static neon.lib.file$FileResult delete(java.lang.String filename) {
        new java.io.File(filename).delete();
        neon.lib.file$FileResult r = new neon.lib.file$FileResult();
        r._choice = 0; // ok
        return r;
    }

    public static Boolean exists(java.lang.String filename) {
        return Boolean.valueOf(new java.io.File(filename).exists());
    }

    public static void mkdir(java.lang.String path) {
        new java.io.File(path).mkdir();
    }

    public static neon.lib.file$BytesResult readBytes(java.lang.String filename) {
        java.io.InputStream in;
        try {
            in = new java.io.FileInputStream(filename);
        } catch (java.io.FileNotFoundException x) {
            neon.lib.file$BytesResult r = new neon.lib.file$BytesResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        byte[] buf = new byte[4096];
        int i = 0;
        try {
            while (true) {
                int n = in.read(buf, i, buf.length - i);
                if (n < 0) {
                    break;
                }
                i += n;
                if (i >= buf.length) {
                    byte[] tmp = new byte[buf.length * 2];
                    System.arraycopy(buf, 0, tmp, 0, i);
                    buf = tmp;
                }
            }
            in.close();
        } catch (java.io.IOException x) {
            neon.lib.file$BytesResult r = new neon.lib.file$BytesResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        byte[] data = new byte[i];
        System.arraycopy(buf, 0, data, 0, i);
        neon.lib.file$BytesResult r = new neon.lib.file$BytesResult();
        r._choice = 0; // data
        r.data = data;
        return r;
    }

    public static neon.lib.file$LinesResult readLines(java.lang.String filename) {
        java.io.BufferedReader br;
        try {
            br = new java.io.BufferedReader(new java.io.InputStreamReader(new java.io.FileInputStream(filename)));
        } catch (java.io.FileNotFoundException x) {
            neon.lib.file$LinesResult r = new neon.lib.file$LinesResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        neon.type.Array a = new neon.type.Array();
        try {
            while (true) {
                java.lang.String s = br.readLine();
                if (s == null) {
                    break;
                }
                a.add(s);
            }
            br.close();
        } catch (java.io.IOException x) {
            neon.lib.file$LinesResult r = new neon.lib.file$LinesResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        neon.lib.file$LinesResult r = new neon.lib.file$LinesResult();
        r._choice = 0; // lines
        r.lines = a;
        return r;
    }

    public static neon.lib.file$FileResult writeBytes(java.lang.String filename, byte[] data) {
        java.io.OutputStream out;
        try {
            out = new java.io.FileOutputStream(filename);
        } catch (java.io.FileNotFoundException x) {
            neon.lib.file$FileResult r = new neon.lib.file$FileResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        try {
            out.write(data);
            out.close();
        } catch (java.io.IOException x) {
            neon.lib.file$FileResult r = new neon.lib.file$FileResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        neon.lib.file$FileResult r = new neon.lib.file$FileResult();
        r._choice = 0; // ok
        return r;
    }

    public static neon.lib.file$FileResult writeLines(java.lang.String filename, neon.type.Array data) {
        java.io.PrintWriter pw;
        try {
            pw = new java.io.PrintWriter(new java.io.FileOutputStream(filename));
        } catch (java.io.FileNotFoundException x) {
            neon.lib.file$FileResult r = new neon.lib.file$FileResult();
            r._choice = 1; // error
            r.error = x.toString();
            return r;
        }
        for (Object s: data) {
            pw.println(s.toString());
        }
        pw.close();
        neon.lib.file$FileResult r = new neon.lib.file$FileResult();
        r._choice = 0; // ok
        return r;
    }

}
