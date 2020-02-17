package neon;

public class Hash {

    public static java.lang.String md5(byte[] data) {
        return hash("MD5", data);
    }

    public static java.lang.String sha1(byte[] data) {
        return hash("SHA-1", data);
    }

    public static java.lang.String sha256(byte[] data) {
        return hash("SHA-256", data);
    }

    public static java.lang.String sha3(byte[] data) {
        return hash("SHA-3", data);
    }

    private static java.lang.String hash(java.lang.String algorithm, byte[] data) {
        java.security.MessageDigest md;
        try {
            md = java.security.MessageDigest.getInstance(algorithm);
        } catch (java.security.NoSuchAlgorithmException x) {
            throw new neon.type.NeonException("HashException.AlgorithmNotSupported");
        }
        byte[] d = md.digest(data);
        StringBuilder r = new StringBuilder();
        for (int i = 0; i < d.length; i++) {
            r.append(java.lang.String.format("%02x", d[i] & 0xff));
        }
        return r.toString();
    }

}
