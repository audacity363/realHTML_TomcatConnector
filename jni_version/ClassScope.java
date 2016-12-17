import java.util.*;

public class ClassScope {
    private static java.lang.reflect.Field LIBRARIES = null;

    static 
    {
        try
        {
            LIBRARIES = ClassLoader.class.getDeclaredField("loadedLibraryNames");
            LIBRARIES.setAccessible(true);
        } 
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public static String[] getLoadedLibraries(final ClassLoader loader) {
        try
        {
            final Vector<String> libraries = (Vector<String>) LIBRARIES.get(loader);
            return libraries.toArray(new String[] {});
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        return(null);
    }
}
