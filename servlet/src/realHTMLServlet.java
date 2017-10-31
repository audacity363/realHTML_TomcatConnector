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
import realHTML.tomcat.connector.RH4NCallParms;


public class realHTMLServlet extends HttpServlet {

    private JNILoader bs;
    private String version = "realHTML4Natural Tomcat Connector Servlet Version 1.0 (test1)";

    static String default_tags[] = {"routes", "templates", "debug", "naterror", "ldaerror", "natparms", "natsourcepath"};

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
        handleRequest(request, response, "get");
    }

    public void doPost(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        handleRequest(request, response, "post");
    }

    public void doPut(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        handleRequest(request, response, "put");
    }

    public void doDelete(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
    {
        handleRequest(request, response, "delete");
    }


    private void handleRequest(HttpServletRequest request,
                    HttpServletResponse response, String req_type)
            throws ServletException, IOException
    {
        RH4NParams parms = new RH4NParams();
        RH4NReturn returncode;
        RH4NCallParms call_parms = new RH4NCallParms();

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

        call_parms = checkRequest(out, env, path, call_parms);

        if(!call_parms.request_successful)
        {
            return;
        }

        call_parms = saveReqParms(request, call_parms);

        parms.keys = call_parms.req_keys.toArray(new String[0]);
        parms.vals = call_parms.req_vals.toArray(new String[0]);
    
        parms.settings = generateSettingsString(call_parms);

        File temp = File.createTempFile("rH4N", "", new File("/tmp/"));

        parms.tmp_file = temp.getAbsolutePath();
        parms.reg_type = req_type;
        parms.nat_library = call_parms.natinfos[0];
        parms.nat_program = call_parms.natinfos[1];
        parms.natparams = call_parms.settings.get("natparms");
        parms.debug = String.valueOf(call_parms.debug);

        returncode = bs.callNatural(parms);

        if(returncode.natprocess_ret < 0)
        {
            out.println(returncode.error_msg);
            return;
        }

        switch(deliverFile(out, parms.tmp_file, call_parms))
        {
            case -1:
                out.println("File Not found");
                break;
            case -2:
                out.println("IOException");
                break;
        }
    }

    private RH4NCallParms saveReqParms(HttpServletRequest req, RH4NCallParms parms)
    {
        Map m;
        Set s;
        Map.Entry<String, String[]> entry;
        Iterator it;

        parms.req_keys = new ArrayList<String>();
        parms.req_vals = new ArrayList<String>();

        m = req.getParameterMap();
        s = m.entrySet();
        it = s.iterator();

        while(it.hasNext())
        {
            entry = (Map.Entry<String, String[]>)it.next();
            parms.req_keys.add(entry.getKey());
            parms.req_vals.add(entry.getValue()[0]);

        }

        return parms;
    }

    private RH4NCallParms checkRequest(PrintWriter out, String env, String path, RH4NCallParms parms)
    {
        String configurationfile;    
        ConfigurationLoader cl;
        Router r;

        configurationfile = System.getenv("realHTMLconfiguration");
        if(configurationfile == null)
        {
            out.println("\"realHTMLconfiguration\" ist not set");
            parms.request_successful = false;
            return(parms);
        }

        cl = new ConfigurationLoader(configurationfile, this.default_tags);
        try
        {
            if(!cl.readConfiguration(env))
            {
                out.printf("Env [%s] was not found", env);
                parms.request_successful = false;
                return(parms);
            }
        }
        catch(Exception e)
        {
            out.println("Something went wrong...(See the logs for more infos)");
            e.printStackTrace();
            parms.request_successful = false;
            return(parms);
        }

        parms.settings = cl.getResult();

        if(!checkSettings(parms.settings, out))
        {
            parms.request_successful = false;
            return(parms);
        }

        
        r = new Router(parms.settings.get("routes"));
        try
        {
            if(!r.searchRoute(path))
            {
                out.println(r.getError());
                parms.request_successful = false;
                return(parms);
            }
        }
        catch(Exception e)
        {
            out.println("Something went wrong...(See the logs for more infos)");
            e.printStackTrace();
            parms.request_successful = false;
            return(parms);
        }

        parms.natinfos[0] = r.getLibrary();
        parms.natinfos[1] = r.getProgram();
        parms.debug = r.getDebug();
        parms.deleteFile = r.getDeleteFile();
        parms.request_successful = true;
        return(parms);

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

    private String generateSettingsString(RH4NCallParms parms)
    {
        String settings_str;

        settings_str = String.format("templates=%s;lib=%s;natsrc=%s;debug=%b", 
                parms.settings.get("templates"), 
                parms.natinfos[0],
                parms.settings.get("natsourcepath"),
                parms.debug);
        return(settings_str);
    }
     
    private int deliverFile(PrintWriter out, String filename, RH4NCallParms parms)
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
        System.out.printf("File_size of [%s]: [%d]\nDelete File: [%b]\n", filename, file.length(), parms.deleteFile);

        if(parms.deleteFile)
        {
            file.delete();
        }

        return(0);
    }


    public void destroy()
    {
       System.out.printf("Shutdown %s...\n", this.version);
    }
}
