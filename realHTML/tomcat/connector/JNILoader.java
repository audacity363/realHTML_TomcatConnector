package realHTML.tomcat.connector;

public class JNILoader
{
    static 
    {
        //System.out.println("Loading sharedlib");
        System.loadLibrary("realHTMLconnector");
        //System.out.println("Lib Loaded");
    }

    public int callNatural(String[] keys, String[] vals, String req_type, String[] natinfos, String tmp_file, String settings_str, String nat_parms)
    {
        return(new JNINatural()).jni_callNatural(keys, vals, req_type, natinfos, tmp_file, settings_str, nat_parms);
    }
}
