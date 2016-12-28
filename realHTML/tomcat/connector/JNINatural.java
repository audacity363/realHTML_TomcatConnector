package realHTML.tomcat.connector;

public class JNINatural{
    public native int jni_callNatural(String[] keys, String[] vals, String req_type, String[] natinfos, String tmp_file, String settings_str, String nat_parms);
}
