import java.io.File;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;

class ReadXML
{
    static public void main(String[] args)
    {
        File inputFile = new File("routes.xml");

        DocumentBuilderFactory dbFactory; 
        DocumentBuilder dBuilder;
        Document doc;
        NodeList nList;
        Node nNode;
        Element eElement;

        String searchparm = "al3stat";
        Boolean found = false;
        String library = null;
        String program = null;

        
        try
        {
            dbFactory = DocumentBuilderFactory.newInstance();
            dBuilder = dbFactory.newDocumentBuilder();

            doc = dBuilder.parse(inputFile);
            doc.getDocumentElement().normalize();
            System.out.println("Root element :" 
                    + doc.getDocumentElement().getNodeName());

            nList = doc.getElementsByTagName("route");
            for(int i=0; i < nList.getLength(); i++)
            {
                nNode = nList.item(i);
                eElement = (Element)nNode;

                if(eElement.getAttribute("path").equals(searchparm))
                {
                    found = true;
                    library = eElement.getElementsByTagName("library").item(0).getTextContent();
                    program = eElement.getElementsByTagName("programm").item(0).getTextContent();
                    System.out.printf("Lib: [%s]\nProg: [%s]\n", library, program);
                }
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

    }
}
