package realHTML.tomcat.connector;

import java.io.*;
import java.util.*;
import java.io.File;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import java.lang.reflect.Method;
import org.xml.sax.SAXException;

public class Router
{
    String filepath = null;
    String program = null;
    String library = null;
    String error_msg; 

    public Router(String filepath)
    {
        this.filepath = filepath;
    }

    public Boolean searchRoute(String route) throws ParserConfigurationException, SAXException, IOException
    {
        File inputFile = new File(this.filepath);

        DocumentBuilderFactory dbFactory; 
        DocumentBuilder dBuilder;
        Document doc;
        NodeList nList, target;
        Node nNode;
        Element eElement;

        Boolean found = false;

        
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
                target = eElement.getElementsByTagName("library");
                if(target.getLength() == 0)
                {
                    this.error_msg = "library tag not found" ;
                    return(false);
                }
                this.library = target.item(0).getTextContent();

                target = eElement.getElementsByTagName("program");
                if(target.getLength() == 0)
                {
                    this.error_msg = "program tag not found" ;
                    return(false);
                }
                this.program = target.item(0).getTextContent();
                return(true);
            }
        }
        this.error_msg = "no route found";
        return(false);
    }

    public String getProgram()
    {
        return(this.program);
    }

    public String getLibrary()
    {
        return(this.library);
    }

    public String getError()
    {
        return(this.error_msg);
    }
}
