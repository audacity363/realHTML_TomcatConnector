#define NATPARMSCOUNT 7

struct naturalparms {
    char *jname;
    char *jtype;
    jobject target;
    void *value;
    int array_length;
    int (*handler)(JNIEnv*, jobject, struct naturalparms*);
    void *(*getter)(JNIEnv*, struct naturalparms*, int);
};

int callNatural(JNIEnv *env, struct naturalparms *parms, int length, char *error, FILE*);
void debugFileprint(FILE *logfile, char *format, ...);
char *natErrno2Str(int naterrno);
struct naturalparms *getParmByName(struct naturalparms *parms, int length, char *searchname);
