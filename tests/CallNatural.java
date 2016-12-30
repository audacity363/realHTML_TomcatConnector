import realHTML.tomcat.connector.JNILoader;

public class CallNatural
{
    public static void main(String args[])
    {
        String keys[] = {"key1", "key2", "key3"};
        String vals[] = {"vals1", "vals2", "vals3"};
        String req_type = "GET";

        String natinfos[] = {"TGAP0734", "TOMTEST"};
        
        String tmp_file = "/u/it/a140734/C/realHTML_TomcatConnector/tests/test.out";

        String settings_str = "templates=/u/it/a140734/C/realHtml4Natural/web_server/templates/;lib=TGAP0734;natsrc=/VAW/natural/abve/fuser63/";
        String nat_parms = "parm=abveparm etid=$$";

        JNILoader bs = new JNILoader(); 

        int ret = bs.callNatural(keys, vals, req_type, natinfos, tmp_file, settings_str, nat_parms);
        System.out.printf("Ret: [%d]\n", ret);

    }
}
