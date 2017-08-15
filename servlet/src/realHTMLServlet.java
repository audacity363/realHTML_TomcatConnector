import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.util.*;
import java.nio.charset.Charset;

import realHTML.tomcat.connector.JNILoader;
import realHTML.tomcat.connector.ConfigurationLoader;
import realHTML.tomcat.connector.Router;
import realHTML.tomcat.connector.RH4NReturn;
import realHTML.tomcat.connector.RH4NParams;


public class realHTMLServlet extends HttpServlet {

    private JNILoader bs;
    private Map<String, String> settings;
    //First Entry is the  library and the second the program
    private String natinfos[] = new String[2];
    private List<String> req_keys;
    private List<String> req_vals;
    private String req_type;
    private Boolean debug = false;
    private String version = "realHTML4Natural Tomcat Connector Servlet Version 1.0 (test)";

    String default_tags[] = {"routes", "templates", "debug", "naterror", "ldaerror", "natparms", "natsourcepath"};

    public void init() throws ServletException
    {
        System.out.printf("Start %s...\n", this.version);
        bs = new JNILoader();
        bs.printVersion();
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
        RH4NParams parms = new RH4NParams();
        RH4NReturn returncode;

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

        parms.keys = this.req_keys.toArray(new String[0]);
        parms.vals = this.req_vals.toArray(new String[0]);
    
        parms.settings = generateSettingsString();

        File temp = File.createTempFile("rH4N", "", new File("/tmp/"));

        parms.tmp_file = temp.getAbsolutePath();
        parms.reg_type = this.req_type;
        parms.nat_library = this.natinfos[0];
        parms.nat_program = this.natinfos[1];
        parms.natparams = this.settings.get("natparms");
        parms.debug = String.valueOf(this.debug);

        returncode = bs.callNatural(parms);

        if(returncode.natprocess_ret < 0)
        {
            out.println(returncode.error_msg);
            return;
        }

        switch(deliverFile(out, parms.tmp_file))
        {
            case -1:
                out.println("File Not found");
                break;
            case -2:
                out.println("IOException");
                break;
        }
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
        this.debug = r.getDebug();
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

        settings_str = String.format("templates=%s;lib=%s;natsrc=%s;debug=%b", 
                this.settings.get("templates"), 
                this.natinfos[0],
                this.settings.get("natsourcepath"),
                this.debug);
        return(settings_str);
    }
     
    private int deliverFile(PrintWriter out, String filename)
    {
        String curLine = null;

        try
        {
            InputStreamReader in = new InputStreamReader(new FileInputStream(filename), Charset.forName("ISO-8859-15"));
            BufferedReader br = new BufferedReader(in);

            while((curLine = br.readLine()) != null)
            {
                if(curLine.length() < 1)
                {
                    continue;
                }
                if(curLine.charAt(0) == '\n')
                {
                    continue;
                }
                out.write(curLine);
            }

            br.close();
            out.flush();
        }
        catch(FileNotFoundException e)
        {
            return(-1);
        }
        catch(IOException e)
        {
            return(-2);
        }

        File file = new File(filename);
        System.out.printf("File_size of [%s]: [%d]\n", filename, file.length());

        file.delete();


        return(0);
    }


    public void destroy()
    {
       System.out.printf("Shutdown %s...\n", this.version);
    }
}
