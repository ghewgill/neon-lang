package neon;

public class Os {

    public static neon.type.Number system(java.lang.String command) {
        Process p;
        try {
            p = java.lang.Runtime.getRuntime().exec(command);
            try {
                p.waitFor();
            } catch (InterruptedException x) {
                return neon.type.Number.ONE;
            }
            java.io.InputStream in = p.getInputStream();
            byte[] buf = new byte[4096];
            while (true) {
                int n = in.read(buf);
                if (n < 0) {
                    break;
                }
                System.out.write(buf, 0, n);
            }
        } catch (java.io.IOException x) {
            return neon.type.Number.ONE;
        }
        return new neon.type.Number(p.exitValue());
    }

}
