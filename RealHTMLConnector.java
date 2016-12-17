import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.util.*;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import java.lang.reflect.Method;


// Extend HttpServlet class
public class RealHTMLConnector extends HttpServlet {

    String filepath = null;
    String program = null;
    String library = null;

    String natsrc = null;
    String routes = null;
    String templates = null;

    public void init() throws ServletException
    {
    }

    public void doGet(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		writeParms(out, request, response);
	}

	public void doPost(HttpServletRequest request,
                    HttpServletResponse response)
            throws ServletException, IOException
	{
		PrintWriter out = response.getWriter();
		writeParms(out, request, response);
	}

	void writeParms(PrintWriter out, HttpServletRequest req, HttpServletResponse response)
	{
		response.setContentType("text/html");

        HttpServletRequest request = (HttpServletRequest) req;
        String path = request.getRequestURI().substring(request.getContextPath().length());
        path = path.replace("/realHTML4Natural", "");

        File out_file;
        List<String> args_val = new ArrayList<String>();
        List<String> args_key = new ArrayList<String>();
        String argstr_key = ";"; 
        String argstr_val = ";";
        String settingsstr = "";

		Map m = req.getParameterMap();	
		Set s = m.entrySet();
        Iterator it = s.iterator();

        if(!searchSettings("/tmp/tomcat/config.xml", "test"))
        {
            try
            {
                response.sendError(500);
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
            return;
        }

        if(!searchRoute(this.routes, path))
        {
            out.println("<h1>Route not found</h1>");
            try
            {
                response.sendError(HttpServletResponse.SC_NOT_FOUND);
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
            return;
        }

        settingsstr = String.format("natsrc=%s;lib=%s;templates=%s", this.natsrc, this.library, this.templates);

        try
        {
            out_file = File.createTempFile("realHTML", "", new File("/tmp/"));
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return;
        }

        out.println(System.getenv("NATUSER"));

        out.println(settingsstr);
        out.println("<br>");


        out.println("<ul>");

		while(it.hasNext())
		{
			Map.Entry<String,String[]> entry = (Map.Entry<String,String[]>)it.next();
			String key = entry.getKey();
            String[] value = entry.getValue();

            out.printf("<li>[%s] = [%s]</li>\n", key, value[0]);

            argstr_key += key+";";
            argstr_val += value[0]+";";
		}
        out.println("</ul>");
        out.println(argstr_key);
        out.println(argstr_val);
        out.println("<br>");
        out.println(path);
        out.println("<br>");
        out.println(out_file.getAbsolutePath());
        
        try
        {
            ProcessBuilder proc = new ProcessBuilder("/tmp/tomcat/realHTMLcall", argstr_key, argstr_val,
                this.library, this.program, out_file.getAbsolutePath(), settingsstr, "2> /tmp/realHTMLError.log");
            Map<String, String> env = proc.environment();
            env.put("NATUSER", "/develop/sharedlibs/v63140/general_v2.0.9.so:/tmp/tomcat/jinja2.so");
            Process p = proc.start();
            p.waitFor();
            //

            Integer rc = p.exitValue();
            out.println("<br>");
            out.print("Exitval:");
            out.println(rc);
        }
        catch(Exception e)
        {
            out.println("Exception");
            out.println("<br>");
            e.printStackTrace();
        }
	}

    public Boolean searchSettings(String filepath, String envtype)
    {
        File inputFile = new File(filepath);

        DocumentBuilderFactory dbFactory; 
        DocumentBuilder dBuilder;
        Document doc;
        NodeList nList;
        Node nNode;
        Element eElement;

        Boolean found = false;

        
        try
        {
            dbFactory = DocumentBuilderFactory.newInstance();
            dBuilder = dbFactory.newDocumentBuilder();

            doc = dBuilder.parse(inputFile);
            doc.getDocumentElement().normalize();

            nList = doc.getElementsByTagName("environment");
            for(int i=0; i < nList.getLength(); i++)
            {
                nNode = nList.item(i);
                eElement = (Element)nNode;

                if(eElement.getAttribute("type").equals(envtype))
                {
                    found = true;
                    this.routes = eElement.getElementsByTagName("routes").item(0).getTextContent();
                    this.templates = eElement.getElementsByTagName("templates").item(0).getTextContent();
                    this.natsrc = eElement.getElementsByTagName("natsourcepath").item(0).getTextContent();
                }
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        return(found);

    }

    public Boolean searchRoute(String filename, String route)
    {
        File inputFile = new File(filename);

        DocumentBuilderFactory dbFactory; 
        DocumentBuilder dBuilder;
        Document doc;
        NodeList nList;
        Node nNode;
        Element eElement;

        Boolean found = false;

        
        try
        {
            dbFactory = DocumentBuilderFactory.newInstance();
            dBuilder = dbFactory.newDocumentBuilder();

            doc = dBuilder.parse(inputFile);
            doc.getDocumentElement().normalize();

            nList = doc.getElementsByTagName("route");
            for(int i=0; i < nList.getLength(); i++)
            {
                nNode = nList.item(i);
                eElement = (Element)nNode;

                if(eElement.getAttribute("path").equals(route))
                {
                    found = true;
                    this.library = eElement.getElementsByTagName("library").item(0).getTextContent();
                    this.program = eElement.getElementsByTagName("programm").item(0).getTextContent();
                }
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        return(found);
    }
	
  public void destroy()
  {
      // do nothing.
  }
}
