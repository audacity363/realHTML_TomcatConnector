package realHTML.tomcat.connector;

import java.io.*;
import java.util.*;

public class RH4NCallParms {
    public Map<String, String> settings;

    public String natinfos[] = new String[2];
    public List<String> req_keys;
    public List<String> req_vals;
    public String req_type;
    public Boolean debug = false;
    public Boolean deleteFile = false;

    public Boolean request_successful = false;


}

