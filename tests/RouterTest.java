import realHTML.tomcat.connector.Router;
import java.util.Map;

public class RouterTest
{
    public static void main(String args[])
    {
        Router test = new Router("/develop/tom/abfo/routes.xml");

        try
        {
            test.searchRoute("/test");
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return;
        }


        Boolean debug = test.getDebug();

        System.out.printf("Debug result: [%b]\n", debug);
    }
}
