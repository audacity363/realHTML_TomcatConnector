import java.io.File;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;


class XMLLoader
{
    String filepath = null;
    String program = null;
    String library = null;

    public XMLLoader(String filepath)
    {
        this.filepath = filepath;
    }

    public Boolean searchRoute(String route)
    {
        File inputFile = new File(this.filepath);

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

    public String getProgram()
    {
        return(this.program);
    }

    public String getLibrary()
    {
        return(this.library);
    }
}

class Start
{
    static
    {
        System.load("./realHTMLjni.so");
    }

    static native int sayHello(String[] keys, String[] val, String req_type,
                                String library, String program, String filepath);

    public static void main(String[] args)
    {
        String[] keys = {"key1", "key2", "key3"};
        String[] vals = {"val1", "val2", "val3"};
        String filename;
        File out_file;
        String route = "al3stat";
        String[] settings_res = new String[2];
    
        XMLLoader settings = new XMLLoader("routes.xml");
        if(!settings.searchRoute(route))
        {
            System.out.println("Route not found");
            return;
        }

        settings_res[0] = settings.getProgram();
        settings_res[1] = settings.getLibrary();

        System.out.printf("Found Settings:\nLib: [%s]\nProg: [%s]\n", settings_res[0], settings_res[1]);


        try
        {
            out_file = File.createTempFile("rH4N", "", new File("/tmp/"));
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return;
        }

        filename = out_file.getAbsolutePath();


        System.out.printf("Tmp Filename: [%s]\n", filename);

        sayHello(keys, vals, "POST", settings_res[1], settings_res[0], filename);

        out_file.deleteOnExit();
    }
}
