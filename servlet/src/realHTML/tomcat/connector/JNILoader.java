package realHTML.tomcat.connector;

public class JNILoader
{
    static 
    {
        //System.out.println("Loading sharedlib");
        System.loadLibrary("realHTMLconnector");
        //System.load("/u/it/a140734/C/realHTML_TomcatConnector/librealHTMLconnector.so");
        //System.out.println("Lib Loaded");
    }

    public RH4NReturn callNatural(RH4NParams parms)
    {
        return(new JNINatural()).jni_callNatural(parms);
    }
    public int printVersion()
    {
        return(new JNINatural()).jni_printVersion();
    }
}
