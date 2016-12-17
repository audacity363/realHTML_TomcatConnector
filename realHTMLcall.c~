#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>

#include "natuser.h"
#include "natni.h"

#define NAT_LIB_NAME "libnatural.so"
#define GET_INTERFACE_FUNC "nni_get_interface"
#define NAT_PARMS "parm=abveparm etid=$$"


void printNaturalException( struct natural_exception *pnatexcep);
int OpenLib(void **shLib, char *name);
void CloseLib(void **shLib);
int setupNatParms(pnni_611_functions s_func, char **keys,
                  char **vals, int *val_length, int length,
                  char *tmp_file, char *parms, struct parameter_description *natparms);

int main(int argc, char *argv[])
{

    if(argc != 7)
    {
        fprintf(stderr, "parm error\n");
        return(-1);
    }
    int key_length = 0,
        val_length = 0,
        i = 0;

    char **key_array = NULL,
         **val_array = NULL,
         *library = NULL, *program = NULL,
         *sav_ptr = NULL, *tmp_file = NULL,
         *parms = NULL; 


    int *parm_length = NULL,
        rc = 0;

    void *shlib = NULL;
    struct natural_exception nat_ex;
    PF_NNI_GET_INTERFACE pf_nni_get_interface = 0;
    pnni_611_functions s_func;
    struct parameter_description params[7];

    if(!(key_array = malloc(sizeof(char*))))
    {
        fprintf(stderr, "%s", strerror(errno));
        return(-2);
    }
    if(!(val_array = malloc(sizeof(char*))))
    {
        fprintf(stderr, "%s", strerror(errno));
        return(-2);
    }

    sav_ptr = strtok(argv[1], ";");
    while(sav_ptr != NULL)
    {
        key_array[key_length] = sav_ptr;
        key_length++;
        key_array = realloc(key_array, sizeof(char*)*key_length);
        sav_ptr = strtok(NULL, ";");
    }

    sav_ptr = strtok(argv[2], ";");
    while(sav_ptr != NULL)
    {
        val_array[val_length] = sav_ptr;
        val_length++;
        val_array = realloc(val_array, sizeof(char*)*val_length);
        sav_ptr = strtok(NULL, ";");
    }


    if(key_length != val_length)
        return(-2);

    if(!(key_array = malloc(key_length*sizeof(const char*))))
        return(-3);

    if(!(val_array = malloc(val_length*sizeof(const char*))))
    {
        free(key_array);
        return(-4);
    }

    if(!(parm_length = malloc(val_length*sizeof(int))))
    {
        free(key_array);
        free(val_array);
        return(-5);
    }

    library = argv[3];
    program = argv[4];
    tmp_file = argv[5];
    parms = argv[6];


    for(i=0; i < key_length; i++)
    {
        printf("[%s] = [%s] (%d)\n", key_array[i], val_array[i], parm_length[i]);
    }

    if(OpenLib(&shlib, NAT_LIB_NAME) < 0)
    {
        free(key_array);
        free(val_array);
        free(parm_length);
        return(-5);
    }

    printf("Done Loading. Pointer: %p\n", shlib);


    pf_nni_get_interface = (PF_NNI_GET_INTERFACE)dlsym(shlib,
        GET_INTERFACE_FUNC);
    printf("Get Function Table...");
    if(((pf_nni_get_interface)(NNI_VERSION_CURR, (void**)&s_func)) != NNI_RC_OK)
    {
        printf("...Error while gettings Functrion Table\n");
        CloseLib(&shlib);
        free(key_array);
        free(val_array);
        free(parm_length);
        return(-3);
    }
    printf("...Done\n");

    //Initialize Natural session
    printf("Init Natural Session with parms: [%s]...", NAT_PARMS);
    if(s_func->pf_nni_initialize(s_func, NAT_PARMS, 0, 0) != NNI_RC_OK)
    {
        printf("...Error\n");
        CloseLib(&shlib);
        free(key_array);
        free(val_array);
        free(parm_length);
        return(-4);
    }
    printf("...Done\n");

    printf("Logon into [%s]...", library);
    if((rc = s_func->pf_nni_logon(s_func, library, 0, 0)) != NNI_RC_OK)
    {
        printf("...Error. Nr: [%d]\n", rc);
        CloseLib(&shlib);
        free(key_array);
        free(val_array);
        free(parm_length);
        return(-5);
    }
    printf("...Done\n");


    setupNatParms(s_func, key_array, val_array, parm_length, key_length, tmp_file, parms, params);

    printf("Call subprogramm [%s]...", program);
    if(s_func->pf_nni_callnat(s_func, program, 7, params, &nat_ex) != NNI_RC_OK)
    {
        printf("...Error\n");
        printNaturalException(&nat_ex);
        CloseLib(&shlib);
        free(key_array);
        free(val_array);
        free(parm_length);
        return(-6);
    }
    printf("...Done\n");


    printf( "Uninitializing Natural..." );
    s_func->pf_nni_uninitialize(s_func);
    printf("...Done\n");

    printf("Freeing interface...");
    s_func->pf_nni_free_interface(s_func);
    printf("...Done\n");

    CloseLib(&shlib);

    free(key_array);
    free(val_array);
    free(parm_length);

    return(0);
}


int setupNatParms(pnni_611_functions s_func, char **keys,
                  char **vals, int *val_length, int length,
                  char *tmp_file, char *parms, struct parameter_description *natparms)
{
    //TODO: Switch to XArrays with Dynamic Unicode Variables
    int occ[3] = {20, 0, 0},
        ind[3] = {0, 0, 0},
        i = 0;
    char val_buff[2024], key_buff[100], file_buff[100];
    void *phandle = NULL;

    s_func->pf_nni_create_parm(s_func, 7, &phandle);

    //Values
    s_func->pf_nni_init_parm_sa(s_func, 0, phandle, 'A', 2024, 0, 1, occ, 0);
    //Value Length
    s_func->pf_nni_init_parm_sa(s_func, 1, phandle, 'I', 4, 0, 1, occ, 0);
    //Keys
    s_func->pf_nni_init_parm_sa(s_func, 2, phandle, 'A', 100, 0, 1, occ, 0);
    //Parameter count
    s_func->pf_nni_init_parm_s(s_func, 3, phandle, 'I', 4, 0, 0);
    //Type
    s_func->pf_nni_init_parm_s(s_func, 4, phandle, 'A', 5, 0, 0);
    //File
    s_func->pf_nni_init_parm_s(s_func, 5, phandle, 'A', 100, 0, 0);
    //Settings
    s_func->pf_nni_init_parm_d(s_func, 6, phandle, 'A', 0);

    
    for(; ind[0] < length; ind[0]++)
    {
        memset(val_buff, 0x20, sizeof(val_buff));
        memcpy(val_buff, vals[i], strlen(vals[i]));
        s_func->pf_nni_put_parm_array(s_func, 0, phandle,
                sizeof(val_buff), val_buff, ind);

        s_func->pf_nni_put_parm_array(s_func, 1, phandle, 4, &(val_length[i]),
                                      ind);
        
        memset(key_buff, 0x20, sizeof(key_buff));
        memcpy(key_buff, keys[i], strlen(keys[i]));
        s_func->pf_nni_put_parm_array(s_func, 2, phandle,
                sizeof(key_buff), key_buff, ind);
    }

    s_func->pf_nni_put_parm(s_func, 3, phandle, sizeof(int), &length);

    memset(file_buff, 0x20, sizeof(file_buff));
    memcpy(file_buff, tmp_file, strlen(tmp_file)+1);
    s_func->pf_nni_put_parm(s_func, 5, phandle, sizeof(file_buff), file_buff);

    strcpy(val_buff, parms);
    s_func->pf_nni_put_parm(s_func, 6, phandle, strlen(val_buff)+1, val_buff);

    for(;i < 7; i++)
    {
        s_func->pf_nni_get_parm_info(s_func, i, phandle, &(natparms[i]));
    }

    return(0);
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
void printNaturalException( struct natural_exception *pnatexcep) 
{
    if (pnatexcep)
    {
        printf( "MessageNumber: %d.\n",
            pnatexcep->natMessageNumber );
        printf( "MessageText:   %s\n",
            pnatexcep->natMessageText ? pnatexcep->natMessageText : "0" );
        printf( "Library:       %s.\n",
            pnatexcep->natLibrary ? pnatexcep->natLibrary : "0" );
        printf( "Member:        %s.\n",
            pnatexcep->natMember ? pnatexcep->natMember : "0" );
        printf( "Name:          %s.\n",
            pnatexcep->natName ? pnatexcep->natName : "0" );
        printf( "Method:        %s.\n",
            pnatexcep->natMethod ? pnatexcep->natMethod : "0" );
        printf( "Line:          %d.\n",
            pnatexcep->natLine );
    }
}
