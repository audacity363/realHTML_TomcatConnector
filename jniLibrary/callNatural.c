#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "natni.h"
#include "natuser.h"
#include "realHTML_tomcat_connector_JNINatural.h"

#define NAT_LIB_NAME "libnatural.so"
#define GET_INTERFACE_FUNC "nni_get_interface"

#define HTML_ERROR_HEAD "<html><head><title>Natural Error: [%d]</title></head><body>\n"

#define DEBUG 1

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while(0);


typedef struct {
    char **http_keys;
    char **http_vals;
    int http_parms_length;
    char *http_reqtype;

    char *nat_library;
    char *nat_program;
    char *nat_parms;
    
    char *tmp_file;
    char *settings_str;
} realHTMLinfos;

int OpenLib(void **shLib, char *name);
void CloseLib(void **shLib);
void printNaturalException( struct natural_exception *pnatexcep);
pnni_611_functions initNatural(void *lib, char *natparms);
int initNatParms(realHTMLinfos infos, struct parameter_description *parms, pnni_611_functions s_funcs);
int printErrortoFile(struct natural_exception ext, char *outfile);
    
JNIEXPORT jint JNICALL Java_realHTML_tomcat_connector_JNINatural_jni_1callNatural
  (JNIEnv *env, jobject obj, jobjectArray keys, jobjectArray vals, 
   jstring req_type, jobjectArray natinfos, jstring tmp_file,
   jstring settings_str, jstring nat_parms)
{
#ifdef FIILE_DEBUG
    logfile = fopen("/tmp/jnilog.log", "w");
#endif

    int key_length = 0,
        val_length = 0,
        i = 0, ret = 0;

    pid_t child;

    realHTMLinfos infos = {NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL};

    const char *tmp_buff = NULL;

    jstring j_string;

    //First copy all information that comes from java.
    key_length = (*env)->GetArrayLength(env, keys);
    val_length = (*env)->GetArrayLength(env, vals);

    infos.http_keys = malloc(sizeof(char*)*key_length);
    infos.http_vals = malloc(sizeof(char*)*key_length);

    //Copy all HTTP parms
    for(i=0; i < key_length; i++)
    {
        j_string = (jstring)(*env)->GetObjectArrayElement(env, keys, i);
        tmp_buff = (*env)->GetStringUTFChars(env, j_string, 0);
        infos.http_keys[i] = malloc(sizeof(char)*(strlen(tmp_buff)+1));
        strcpy(infos.http_keys[i], tmp_buff);
        (*env)->ReleaseStringUTFChars(env, j_string, tmp_buff);

        j_string = (jstring)(*env)->GetObjectArrayElement(env, vals, i);
        tmp_buff = (*env)->GetStringUTFChars(env, j_string, 0);
        infos.http_vals[i] = malloc(sizeof(char)*(strlen(tmp_buff)+1));
        strcpy(infos.http_vals[i], tmp_buff);
        (*env)->ReleaseStringUTFChars(env, j_string, tmp_buff);
    }
    infos.http_parms_length = key_length;

    //Copy HTTP request type
    tmp_buff = (*env)->GetStringUTFChars(env, req_type, 0);
    infos.http_reqtype = malloc(sizeof(char)*(strlen(tmp_buff)+1));
    strcpy(infos.http_reqtype, tmp_buff);
    (*env)->ReleaseStringUTFChars(env, req_type, tmp_buff);

    //Copy natural library and programm that should get called
    //First index is the library
    //Second index is the program
    j_string = (jstring)(*env)->GetObjectArrayElement(env, natinfos, 0);
    tmp_buff = (*env)->GetStringUTFChars(env, j_string, 0);
    infos.nat_library = malloc(sizeof(char)*(strlen(tmp_buff)+1));
    strcpy(infos.nat_library, tmp_buff);
    (*env)->ReleaseStringUTFChars(env, j_string, tmp_buff);

    j_string = (jstring)(*env)->GetObjectArrayElement(env, natinfos, 1);
    tmp_buff = (*env)->GetStringUTFChars(env, j_string, 0);
    infos.nat_program = malloc(sizeof(char)*(strlen(tmp_buff)+1));
    strcpy(infos.nat_program, tmp_buff);
    (*env)->ReleaseStringUTFChars(env, j_string, tmp_buff);

    //Copy the natural parms for the natural session
    tmp_buff = (*env)->GetStringUTFChars(env, nat_parms, 0);
    infos.nat_parms = malloc(sizeof(char)*(strlen(tmp_buff)+1));
    strcpy(infos.nat_parms, tmp_buff);
    (*env)->ReleaseStringUTFChars(env, nat_parms, tmp_buff);

    //Copy the path to the output file
    tmp_buff = (*env)->GetStringUTFChars(env, tmp_file, 0);
    infos.tmp_file = malloc(sizeof(char)*(strlen(tmp_buff)+1));
    strcpy(infos.tmp_file , tmp_buff);
    (*env)->ReleaseStringUTFChars(env, tmp_file, tmp_buff);

    //Copy the settings string for the lda and html parser
    tmp_buff = (*env)->GetStringUTFChars(env, settings_str, 0);
    infos.settings_str = malloc(sizeof(char)*(strlen(tmp_buff)+1));
    strcpy(infos.settings_str, tmp_buff);
    (*env)->ReleaseStringUTFChars(env, settings_str, tmp_buff);

    if((child = fork()) == -1)
    {
        fprintf(stderr, "Can not create natural child\n");
        ret = -2;
        goto exit;
    }

    if(child == 0)
    {
        ret = callNatural(infos);
        exit(0);
    }

    fprintf(stderr, "Wait for pid[%d]...", child);

    //TODO: Add error Handling! (Write to the output file when the child exited abnormally)
    waitpid(child, NULL, 0);

    fprintf(stderr, "...OK[%d]\n", child);


exit:
    //Free all allocated memory
    if(infos.http_keys != NULL)
            free(infos.http_keys[i]);

    if(infos.http_vals != NULL)
        for(i=0; i < key_length; i++)
            free(infos.http_vals[i]);
    
    if(infos.http_keys != NULL)
        free(infos.http_keys);

    if(infos.http_vals != NULL)
        free(infos.http_vals);

    if(infos.http_reqtype != NULL)
        free(infos.http_reqtype);

    if(infos.nat_library != NULL)
        free(infos.nat_library);

    if(infos.nat_program != NULL)
        free(infos.nat_program);

    if(infos.nat_parms != NULL)
        free(infos.nat_parms);


    if(infos.tmp_file != NULL)
        free(infos.tmp_file);

    if(infos.settings_str != NULL)
        free(infos.settings_str);

#ifdef FIILE_DEBUG
    fclose(test);
#endif

    return(ret);
}


int callNatural(realHTMLinfos infos)
{
    void *shlib = NULL;
    pnni_611_functions s_funcs;
    struct parameter_description parms[7];
    struct natural_exception nat_ex;
    int rc = 0;

    if(OpenLib(&shlib, NAT_LIB_NAME) < 0)
    {
        return(-1);
    }

    debug_print("Done loading SO. Pointer: [%p]\n", shlib);

    if((s_funcs = initNatural(shlib, infos.nat_parms)) == NULL)
    {
        CloseLib(&shlib);
        return(-1);
    }

    initNatParms(infos, parms, s_funcs);

    printf("Logon into [%s]...", infos.nat_library);
    if((rc = s_funcs->pf_nni_logon(s_funcs, infos.nat_library , 0, 0)) != NNI_RC_OK)
    {
        printf("...Error. Nr: [%d]\n", rc);
        CloseLib(&shlib);
        return(-5);
    }
    printf("...Done\n");


    printf("Tmp_file: [%s]\n", infos.tmp_file);
    if(s_funcs->pf_nni_callnat(s_funcs, infos.nat_program , 7, parms, &nat_ex) != NNI_RC_OK)
    {
        printNaturalException(&nat_ex);
        printErrortoFile(nat_ex, infos.tmp_file);
    }

    printf( "Uninitializing Natural..." );
    s_funcs->pf_nni_uninitialize(s_funcs);
    printf("...Done\n");

    printf("Freeing interface...");
    s_funcs->pf_nni_free_interface(s_funcs);
    printf("...Done\n");

    

    CloseLib(&shlib);
    return(0);
}

int initNatParms(realHTMLinfos infos, struct parameter_description *parms, pnni_611_functions s_funcs) 
{
    void *phandle = NULL;
    int rc = 0, i = 0,
        occ[3] = {0, 0, 0},
        flags = 0,
        i_buff = 0;

    s_funcs->pf_nni_create_parm(s_funcs, 7, &phandle);

    occ[0] = infos.http_parms_length;

    //Init the array for the keys
    flags = flags |  NNI_FLG_UBVAR_0;
    s_funcs->pf_nni_init_parm_da(s_funcs, 0, phandle, NNI_TYPE_ALPHA, 1, occ, flags);

    //Init the array for the vals
    s_funcs->pf_nni_init_parm_da(s_funcs, 1, phandle, NNI_TYPE_ALPHA, 1, occ, flags);

    //Init array for the value length array
    s_funcs->pf_nni_init_parm_sa(s_funcs, 2, phandle, NNI_TYPE_INT, 4, 0, 1, occ, flags);

    //Init variable for the parameter count
    s_funcs->pf_nni_init_parm_s(s_funcs, 3, phandle, NNI_TYPE_INT, 4, 0, 0);

    //Init variable for the http request type
    s_funcs->pf_nni_init_parm_d(s_funcs, 4, phandle, NNI_TYPE_ALPHA, 0);

    //Init variable for the tmp file
    s_funcs->pf_nni_init_parm_d(s_funcs, 5, phandle, NNI_TYPE_ALPHA, 0);

    //Init variable for the settings string
    s_funcs->pf_nni_init_parm_d(s_funcs, 6, phandle, NNI_TYPE_ALPHA, 0);


    //Set the http parm key, values and value lengths
    for(i=0; i < infos.http_parms_length; i++)
    {
        occ[0] = i;

        s_funcs->pf_nni_put_parm_array(s_funcs, 0, phandle, strlen(infos.http_keys[i]), infos.http_keys[i], occ);

        i_buff = strlen(infos.http_vals[i]);
        s_funcs->pf_nni_put_parm_array(s_funcs, 1, phandle, i_buff, infos.http_vals[i], occ);
        s_funcs->pf_nni_put_parm_array(s_funcs, 2, phandle, sizeof(int), &i_buff, occ);
    }

    //Set the number of parms
    s_funcs->pf_nni_put_parm(s_funcs, 3, phandle, sizeof(int), &infos.http_parms_length);

    //Set the http request type
    s_funcs->pf_nni_put_parm(s_funcs, 4, phandle, strlen(infos.http_reqtype), infos.http_reqtype);

    //Set the tmp file
    s_funcs->pf_nni_put_parm(s_funcs, 5, phandle, strlen(infos.tmp_file), infos.tmp_file);

    //Set the settings string for the parsers
    s_funcs->pf_nni_put_parm(s_funcs, 6, phandle, strlen(infos.settings_str), infos.settings_str);

    for(i=0; i < 7; i++)
    {
        s_funcs->pf_nni_get_parm_info(s_funcs, i, phandle, &parms[i]);
    }

    return(0);

}


pnni_611_functions initNatural(void *lib, char *natparms)
{
    pnni_611_functions s_funcs;
    PF_NNI_GET_INTERFACE pf_nni_get_interface = 0;
    int rc = 0;
    char *error = NULL;

    if(!(pf_nni_get_interface = (PF_NNI_GET_INTERFACE)dlsym(lib, GET_INTERFACE_FUNC)))
    {
        error = dlerror();
        debug_print("Error while loading Function [%s]: [%s]\n", GET_INTERFACE_FUNC,
            error);
        return(NULL);
    }

    if(((pf_nni_get_interface)(NNI_VERSION_CURR, (void**)&s_funcs)) != NNI_RC_OK)
    {
        printf("...Error while gettings Function Table\n");
        return(NULL);
    }

    
    debug_print("Init Natural Session with parms: [%s]...", natparms);
    if((rc = s_funcs->pf_nni_initialize(s_funcs, natparms, 0, 0)) != NNI_RC_OK)
    {
        debug_print("...Error [%d]\n", rc)
        return(NULL);
    }
    printf("..Done\n");

    return(s_funcs);

}

int OpenLib(void **shLib, char *name)
{
    char *error;

    *shLib = dlopen(name, RTLD_NOW);
    if(!*shLib)
    {
        error = dlerror();
        printf("Error while loading Module [%s]: [%s]\n", name, error);
        return(-1);
    }
    return(0);
}

void CloseLib(void **shLib)
{
    dlclose(*shLib);
}



/* Format and print a Natural exception. */
void printNaturalException(struct natural_exception *pnatexcep)
{
    char error_str[2024];
    int error_line;

    if (pnatexcep)
    {
        debug_print( "MessageNumber: %d.\n",
            pnatexcep->natMessageNumber );
        debug_print( "MessageText:   %s\n",
            pnatexcep->natMessageText ? pnatexcep->natMessageText : "0" );
        debug_print( "Library:       %s.\n",
            pnatexcep->natLibrary ? pnatexcep->natLibrary : "0" );
        debug_print( "Member:        %s.\n",
            pnatexcep->natMember ? pnatexcep->natMember : "0" );
        debug_print( "Name:          %s.\n",
            pnatexcep->natName ? pnatexcep->natName : "0" );
        debug_print( "Method:        %s.\n",
            pnatexcep->natMethod ? pnatexcep->natMethod : "0" );
        debug_print( "Line:          %d.\n",
            pnatexcep->natLine );
    }
}

int printErrortoFile(struct natural_exception ext, char *outfile)
{
    FILE *output;

    if((output = fopen(outfile, "w")) == NULL)
    {
        fprintf(stderr, "Can not open file\n");
        return(-1);
    }

    fprintf(output, HTML_ERROR_HEAD, ext.natMessageNumber);
    fprintf(output, "<h1>A Natural Runtime Error occurred</h1>\n");
    fprintf(output, "<p><span>Message Text:</span>%s</p>\n", ext.natMessageText);
    fprintf(output, "<p><span>Error Number:</span>%d</p>\n", ext.natMessageNumber);
    fprintf(output, "<p><span>Library:</span>%s</p>\n", ext.natLibrary);
    fprintf(output, "</body></html>");

    fclose(output);
    return(0);
}
