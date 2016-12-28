import java.io.IOException;
import java.io.PrintWriter;
import java.io.File;
import javax.servlet.*;
import javax.servlet.http.*;
import java.util.*;

import realHTML.tomcat.connector.JNILoader;
import realHTML.tomcat.connector.ConfigurationLoader;
import realHTML.tomcat.connector.Router;

public class realHTMLServlet extends HttpServlet {

    private JNILoader bs;
    private Map<String, String> settings;
    //First Entry is the  library and the second the program
    private String natinfos[] = new String[2];
    private List<String> req_keys;
    private List<String> req_vals;
    private String req_type;

    String default_tags[] = {"routes", "templates", "debug", "naterror", "ldaerror", "natparms", "natsourcepath"};

    public void init() throws ServletException
    {
        bs = new JNILoader();
    }

    public void doGet(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        this.req_type = "get";
        handleRequest(request, response);
    }

    public void doPost(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        this.req_type = "post";
        handleRequest(request, response);
    }

    public void doPut(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        this.req_type = "put";
        handleRequest(request, response);
    }

    public void doDelete(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        this.req_type = "delete";
        handleRequest(request, response);
    }


    private void handleRequest(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        String path = request.getRequestURI().substring(request.getContextPath().length());
        path = path.replace("/realHTML4Natural", "");

        int index = path.indexOf("/", 1);
        if(index == -1)
        {
            out.println("No Enviroment was given");
            return;
        }

        String env = path.substring(1, index);
        path = path.replace("/"+env, "");

        if(!checkRequest(out, env, path))
        {
            return;
        }

        saveReqParms(request);

        String[] keys = this.req_keys.toArray(new String[0]);
        String[] vals = this.req_vals.toArray(new String[0]);
    
        String settingsstr = generateSettingsString();
        File temp = File.createTempFile("rH4N", "", new File("/tmp/"));

        String tmp_file = temp.getAbsolutePath();


        out.println(tmp_file);
        out.println("<br>");

        int ret = bs.callNatural(keys, vals, this.req_type, this.natinfos, tmp_file, settingsstr, this.settings.get("natparms"));

        out.println("<h1>"+ret+"</h1>");

    }

    private void saveReqParms(HttpServletRequest req)
    {
        Map m;
        Set s;
        Map.Entry<String, String[]> entry;
        Iterator it;

        this.req_keys = new ArrayList<String>();
        this.req_vals = new ArrayList<String>();

        m = req.getParameterMap();
        s = m.entrySet();
        it = s.iterator();

        while(it.hasNext())
        {
            entry = (Map.Entry<String, String[]>)it.next();
            this.req_keys.add(entry.getKey());
            this.req_vals.add(entry.getValue()[0]);

        }
    }

    private boolean checkRequest(PrintWriter out, String env, String path)
    {
        String configurationfile;    
        ConfigurationLoader cl;
        Router r;

        configurationfile = System.getenv("realHTMLconfiguration");
        if(configurationfile == null)
        {
            out.println("\"realHTMLconfiguration\" ist not set");
            return(false);
        }

        cl = new ConfigurationLoader(configurationfile, this.default_tags);
        try
        {
            if(!cl.readConfiguration(env))
            {
                out.printf("Env [%s] was not found", env);
                return(false);
            }
        }
        catch(Exception e)
        {
            out.println("Something went wrong...(See the logs for more infos)");
            e.printStackTrace();
            return(false);
        }
        this.settings = cl.getResult();

        if(!checkSettings(this.settings, out))
        {
            return(false);
        }

        
        r = new Router(this.settings.get("routes"));
        try
        {
            if(!r.searchRoute(path))
            {
                out.println(r.getError());
                return(false);
            }
        }
        catch(Exception e)
        {
            out.println("Something went wrong...(See the logs for more infos)");
            e.printStackTrace();
            return(false);
        }

        this.natinfos[0] = r.getLibrary();
        this.natinfos[1] = r.getProgram();
        return(true);

    }

    private boolean checkSettings(Map settings, PrintWriter out)
    {
        String must_set_fields[] = {"routes", "templates", "natsourcepath"};

        for(int i=0; i < must_set_fields.length; i++)
        {
            if(settings.get(must_set_fields[i]) == null)
            {
                out.printf("[%s] is not set in configuration", must_set_fields[i]);
                return(false);
            }
        }
        return(true);
    }

    private String generateSettingsString()
    {
        String settings_str;

        settings_str = String.format("templates=%s;lib=%s;natsrc=%s", 
                this.settings.get("templates"), 
                this.natinfos[0],
                this.settings.get("natsourcepath"));
        return(settings_str);
    }


    public void destroy()
    {}
}
