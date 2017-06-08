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

public class ConfigurationLoader
{
    String filepath = null;
    String env = null;
    Map<String, String> results = new HashMap();
    String tags[] = {};

    public ConfigurationLoader(String filepath, String searchtags[])
    {
        this.filepath = filepath;
        this.tags = searchtags;
    }

    public Boolean readConfiguration(String env) throws ParserConfigurationException, SAXException, IOException
    {
        DocumentBuilderFactory dbFactory; 
        DocumentBuilder dBuilder;
        Document doc;
        Element env_element;

        File inputFile = new File(this.filepath);

        dbFactory = DocumentBuilderFactory.newInstance();
        dBuilder = dbFactory.newDocumentBuilder();

        doc = dBuilder.parse(inputFile);
        doc.getDocumentElement().normalize();

        env_element = findEnv(env, doc);
        if(env_element == null)
        {
            return(false);
        }
        readSettings(env_element);
        return(true);
        
    }

    private Element findEnv(String env, Document doc)
    {
        Element eElement;
        NodeList nList;
        Node nNode;

        nList = doc.getElementsByTagName("environment");
        for(int i=0; i < nList.getLength(); i++)
        {
            nNode = nList.item(i);
            eElement = (Element)nNode;

            if(eElement.getAttribute("type").equals(env))
            {
                return(eElement);
            }
        }
        return(null);
    }

    private void readSettings(Element target)
    {
        NodeList result;

        for(int i=0; i < this.tags.length; i++)
        {
            result = target.getElementsByTagName(this.tags[i]);
            if(result.getLength() == 0)
            {
                continue;
            }
            this.results.put(this.tags[i], result.item(0).getTextContent());
        }
    }

    public Map getResult() 
    {
        return(this.results);
    }
}
