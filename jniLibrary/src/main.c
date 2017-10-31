#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "realHTML_tomcat_connector_JNINatural.h"
#include "natni.h"
#include "rh4n.h"

#define VERSIONSTR "realHTML4Natural Tomcat Connector JNILibrary Version 2.0.1"

int stringHandle(JNIEnv *env, jobject, struct naturalparms*);
int arrayHandle(JNIEnv *env, jobject, struct naturalparms*);

void *stringGetter(JNIEnv *env, struct naturalparms *pparmtarget, int index);
void *arrayGetter(JNIEnv *env, struct naturalparms *pparmtarget, int index);

void printAll(JNIEnv *, struct naturalparms[], int, FILE*);
void freeAll(JNIEnv *, FILE*, struct naturalparms*, int);
jobject createReturnObj(JNIEnv *env, int exit_code, char*, FILE*);

int createNaturalProcess(JNIEnv *env, struct naturalparms *parms, int length, char *error, FILE *logfile);


JNIEXPORT jint JNICALL Java_realHTML_tomcat_connector_JNINatural_jni_1printVersion
  (JNIEnv *env, jobject parent)
{
    printf("%s\n", VERSIONSTR);
    return(0);
}


JNIEXPORT jobject JNICALL Java_realHTML_tomcat_connector_JNINatural_jni_1callNatural
  (JNIEnv *env, jclass in_cls, jobject jnatparams)
{
    const char *reg_type;
    jclass jrh4nparams;
    jfieldID jfid;
    jobject jobj;
    jobjectArray jarr;
    struct naturalparms *target;
    int i, rc = 0;
    char error_msg[2084],
         filename[10+NNI_LEN_LIBRARY+NNI_LEN_MEMBER],
         *natprogram, *natlibrary;

    struct naturalparms params[] = 
    {
        {"keys", "[Ljava/lang/String;", NULL, NULL, -1, arrayHandle, arrayGetter},
        {"vals", "[Ljava/lang/String;", NULL, NULL, -1, arrayHandle, arrayGetter},
        {"reg_type", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter},
        {"nat_library", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter},
        {"nat_program", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter},
        {"natparams", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter},
        {"tmp_file", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter},
        {"settings", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter},
        {"debug", "Ljava/lang/String;", NULL, NULL, -1, stringHandle, stringGetter}
    };

    int parm_length = sizeof(params)/sizeof(struct naturalparms);


    FILE *logfile = NULL;

    //Get reference to the RH4NParams Class
    jrh4nparams = (*env)->GetObjectClass(env, jnatparams);

    for(i=0; i < parm_length; i++)
    {
        jfid = (*env)->GetFieldID(env, jrh4nparams, params[i].jname,
                params[i].jtype);
        if((params[i].target = (*env)->GetObjectField(env, jnatparams, jfid)) == NULL)
        {
            sprintf(error_msg, "Error while proccessing field [%s]: Field is NULL", params[i].jname);
            return(createReturnObj(env, -1, error_msg, logfile));
        }
        if(params[i].handler(env, jrh4nparams, &params[i]) < 0)
        {
            sprintf(error_msg, "Error while proccessing field [%s]: unkown", params[i].jname);
            return(createReturnObj(env, -2, error_msg, logfile));
        }
    }

    target = getParmByName(params, parm_length, "debug");

    if(strcmp(target->getter(env, target, 0), "true") == 0)
    {
        target = getParmByName(params, parm_length, "nat_library");
        natlibrary = target->getter(env, target, 0);

        target = getParmByName(params, parm_length, "nat_program");
        natprogram = target->getter(env, target, 0);

        sprintf(filename, "/tmp/%s_%s.log", natlibrary, natprogram);
        if((logfile = fopen(filename, "w")) == NULL)
        {
            freeAll(env, logfile, params, parm_length);
            fclose(logfile);
            return(createReturnObj(env, -3, strerror(errno), logfile));
        }
    }

    printAll(env, params, parm_length, logfile);
    

    createNaturalProcess(env, params, parm_length, error_msg, logfile);

    debugFileprint(logfile, "Freeing Java res...\n");
    freeAll(env, logfile, params, parm_length);
    debugFileprint(logfile, "...Done\n");

    fclose(logfile);

    return(createReturnObj(env, 0, " ", logfile));
}

int createNaturalProcess(JNIEnv *env, struct naturalparms *parms, int length, 
    char *error, FILE *logfile)
{
    pid_t natpid = 0, ret_pid = 0;
    int status = 0, rc = 0;

    fflush(logfile);

    natpid = fork();
    if(natpid == 0)
    {
        if((rc = callNatural(env, parms, length, error, logfile)) != 0)
        {
            //freeAll(env, logfile);
            //return(createReturnObj(env, rc, error_msg, logfile));
        }
        fflush(logfile);
        fclose(logfile);
        exit(rc);
    }
    debugFileprint(logfile, "Waiting for PID: [%d]....\n", natpid);
    ret_pid = waitpid(natpid, &status, NULL);
    debugFileprint(logfile, "...PID [%d] exited\n", ret_pid);
    return(status);
}

jobject createReturnObj(JNIEnv *env, int exit_code, char *error_msg, FILE* logfile)
{
    jclass jrh4nreturn;
    jmethodID jmid;
    jobject jretobj;
    jfieldID jfid;

    jrh4nreturn = (*env)->FindClass(env, "realHTML/tomcat/connector/RH4NReturn");
    jmid = (*env)->GetMethodID(env, jrh4nreturn, "<init>", "()V");

    jretobj = (*env)->NewObject(env, jrh4nreturn, jmid);

    jfid = (*env)->GetFieldID(env, jrh4nreturn, "natprocess_ret", "I");
    (*env)->SetIntField(env, jretobj, jfid, exit_code);


    jfid = (*env)->GetFieldID(env, jrh4nreturn, "error_msg", "Ljava/lang/String;");
    (*env)->SetObjectField(env, jretobj, jfid, (*env)->NewStringUTF(env, error_msg));

    return(jretobj);
}

void printAll(JNIEnv *env, struct naturalparms params[], int length, FILE *logfile)
{
    int i,x;
    for(i=0; i < length; i++)
    {
        debugFileprint(logfile, "Name: [%s]\n", params[i].jname);
        if(params[i].array_length != -1)
        {
            for(x=0; x < params[i].array_length; x++)
            {
                debugFileprint(logfile, "\t%.2d [%s]\n", x, (char*)params[i].getter(env, &params[i], x));
            }
        }
        else
            debugFileprint(logfile, "\t [%s]\n", (char*)params[i].getter(env, &params[i], 0));
    }
}

void freeAll(JNIEnv *env, FILE *logfile, struct naturalparms *parms, int parms_length)
{
    int i,x;
    jstring jstr;
    jobjectArray jarr;

    for(i=0; i < parms_length; i++)
    {
        debugFileprint(logfile, "\tFreeing [%s]\n", parms[i].jname);
        if(parms[i].array_length != -1)
        {
            jarr = (jobjectArray)parms[i].target;
            for(x=0; x < parms[i].array_length; x++)
            {
                jstr = (jstring)(*env)->GetObjectArrayElement(env, jarr, x);
                (*env)->ReleaseStringUTFChars(env, jstr, ((char**)parms[i].value)[x]);
            }
            //free(params[i].value);
        }
        else
        {
            (*env)->ReleaseStringUTFChars(env, (jstring)parms[i].target, (char*)parms[i].value);
        }
    }
}


int stringHandle(JNIEnv *env, jobject target, struct naturalparms *pparmtarget)
{
    jstring jstr = (jstring)pparmtarget->target;

    if((pparmtarget->value = (*env)->GetStringUTFChars(env, jstr, NULL)) == NULL)
    {
        return(-1);
    }
    return(0);
}

void *stringGetter(JNIEnv *env, struct naturalparms *pparmtarget, int index)
{
    if(pparmtarget->array_length != -1)
        return(NULL);
    return(pparmtarget->value);
}


int arrayHandle(JNIEnv *env, jobject target, struct naturalparms *pparmtarget)
{
    jobjectArray arrtarget = (jobjectArray)pparmtarget->target;
    jstring jstr;
    int i;

    pparmtarget->array_length =  (*env)->GetArrayLength(env, arrtarget);

    pparmtarget->value = malloc(sizeof(char*)*pparmtarget->array_length);

    for(i=0; i < pparmtarget->array_length; i++)
    {
        jstr = (jstring)(*env)->GetObjectArrayElement(env, arrtarget, i);
        ((const char**)pparmtarget->value)[i] = (*env)->GetStringUTFChars(env, jstr, NULL);
    }

    return(0);
}

void *arrayGetter(JNIEnv *env, struct naturalparms *pparmtarget, int index)
{
    if(index >= pparmtarget->array_length || pparmtarget->array_length == -1) 
    {
        return(NULL);
    }

    return(((char**)pparmtarget->value)[index]);
}

void debugFileprint(FILE *logfile, char *format, ...)
{
    va_list args;
    va_start(args, format);

    if(logfile == NULL)
    {
        vprintf(format, args);
    }
    else
    {
        vfprintf(logfile, format, args);
    }
}

char *natErrno2Str(int naterrno)
{
    static char *const natstrerror[] = {
        "Illegal parameter number",
        "Internal Error",
        "Data have been truncated",
        "Parameter is not an array",
        "Parameter is write protcted",
        "Out of memory when dynamic parameter is resized",
        "Invalid version of interface struct",
        "Invalid data format",
        "Invalid length or precision",
        "Invalid dimension count",
        "Invalid x-array bound definition",
        "Array cannot be resized in the way requested",
        "Index for dimension 0 out of range",
        "Index for dimension 1 out of range",
        "Index for dimension 2 out of range"
    };
    static char startuperror[1024];

    if(naterrno < NNI_RC_SERR_OFFSET)
    {
        sprintf(startuperror, "Natural Startup Error [%d]", naterrno-NNI_RC_SERR_OFFSET);
        return(startuperror);
    }

    if(naterrno == NNI_RC_BAD_INDEX_0)
        naterrno = 14;
    else if(naterrno == NNI_RC_BAD_INDEX_1)
        naterrno = 15;
    else if(naterrno == NNI_RC_BAD_INDEX_2)
        naterrno = 16;
    else 
        naterrno = naterrno * -1;

    return(natstrerror[naterrno]);
}
