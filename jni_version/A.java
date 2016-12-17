public class A implements IA
{
    public A() {}

    static {

        System.load("./realHTMLjni.so");
    }
    public native int sayHello(String[] keys, String[] val, String req_type,
            String library, String program, String filepath);
}
