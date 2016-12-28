import realHTML.tomcat.connector.ConfigurationLoader;
import java.util.Map;

public class ConfigTest
{
    public static void main(String args[])
    {
        String default_tags[] = {"route", "templates", "debug", "naterror", "ldaerror", "natparms", "natsourcepath"};

        ConfigurationLoader cl = new ConfigurationLoader("/srv/tomcat_connector/config_files/config.xml", default_tags);

        try
        {
            if(cl.readConfiguration("abve") == false)
            { 
                System.out.printf("Env [abve] was not found");
                return;
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return;
        }
        Map test = cl.getResult();
        System.out.println(test);
    }
}
