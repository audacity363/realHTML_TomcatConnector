/*****************************************************************************
**
** File    : natni.h
** Purpose : Natural Native Interface functions and definitions.
**
** (c) Copyright 2004 by Software AG
**
*****************************************************************************/

#ifndef natni_h
#define natni_h

#include <natuser.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* NNI version. */
#define NNI_VERSION_611       611
#define NNI_VERSION_62        62
#define NNI_VERSION_CURR      NNI_VERSION_62

/* Return codes. */

/* Success code. */
#define NNI_RC_OK             IF4_RC_OK

/* Error codes passed through from INTERFACE4. */
#define NNI_RC_ILL_PNUM       IF4_RC_ILL_PNUM
#define NNI_RC_INT_ERROR      IF4_RC_INT_ERROR
#define NNI_RC_DATA_TRUNC     IF4_RC_DATA_TRUNCATED
#define NNI_RC_NOT_ARRAY      IF4_RC_NOT_ARRAY     
#define NNI_RC_WRT_PROT       IF4_RC_WRT_PROTECTED 
#define NNI_RC_NO_MEMORY      IF4_RC_NO_MEMORY     
#define NNI_RC_BAD_FORMAT     IF4_RC_BAD_FORMAT    
#define NNI_RC_BAD_LENGTH     IF4_RC_BAD_LENGTH    
#define NNI_RC_BAD_DIM        IF4_RC_BAD_DIM       
#define NNI_RC_BAD_BOUNDS     IF4_RC_BAD_BOUNDS    
#define NNI_RC_NOT_RESIZABLE  IF4_RC_NOT_RESIZABLE 
#define NNI_RC_BAD_INDEX_0    IF4_RC_BAD_INDEX_0
#define NNI_RC_BAD_INDEX_1    IF4_RC_BAD_INDEX_1
#define NNI_RC_BAD_INDEX_2    IF4_RC_BAD_INDEX_2

/* NNI specific error codes. */
#define NNI_RC_OFFSET         -1000
#define NNI_RC_VERSION_ERROR  NNI_RC_OFFSET - 1
#define NNI_RC_NOT_INIT       NNI_RC_OFFSET - 2
#define NNI_RC_NOT_IMPL       NNI_RC_OFFSET - 3
#define NNI_RC_PARM_ERROR     NNI_RC_OFFSET - 4
#define NNI_RC_LOCKED         NNI_RC_OFFSET - 5

/* Natural startup errors. */
#define NNI_RC_SERR_OFFSET    -2000

/* Flags for Natural parameters. */
#define NNI_FLG_PROTECTED     IF4_FLG_PROTECTED
#define NNI_FLG_DYNAMIC       IF4_FLG_DYNAMIC  
#define NNI_FLG_NOT_CONTIG    IF4_FLG_NOT_CONTIGUOUS
#define NNI_FLG_AIV           IF4_FLG_AIV   
#define NNI_FLG_DYNVAR        IF4_FLG_DYNVAR     
#define NNI_FLG_XARRAY        IF4_FLG_XARRAY    
#define NNI_FLG_LBVAR_0       IF4_FLG_LBVAR_0    
#define NNI_FLG_UBVAR_0       IF4_FLG_UBVAR_0    
#define NNI_FLG_LBVAR_1       IF4_FLG_LBVAR_1      
#define NNI_FLG_UBVAR_1       IF4_FLG_UBVAR_1     
#define NNI_FLG_LBVAR_2       IF4_FLG_LBVAR_2     
#define NNI_FLG_UBVAR_2       IF4_FLG_UBVAR_2     

/* Length definitions. */
#define NNI_LEN_TEXT           250
#define NNI_LEN_LIBRARY          8
#define NNI_LEN_MEMBER           8
#define NNI_LEN_NAME            32

/* Natural data types. */
#define NNI_TYPE_ALPHA         'A'
#define NNI_TYPE_BIN           'B'
#define NNI_TYPE_CV            'C'
#define NNI_TYPE_DATE          'D'
#define NNI_TYPE_FLOAT         'F'
#define NNI_TYPE_INT           'I'
#define NNI_TYPE_LOG           'L'
#define NNI_TYPE_NUM           'N'
#define NNI_TYPE_OBJECT        'O'
#define NNI_TYPE_PACK          'P'
#define NNI_TYPE_TIME          'T'
#define NNI_TYPE_UNICODE       'U'

/* Constants for the type NNI_TYPE_LOG. */
#define NNI_L_TRUE            0x01
#define NNI_L_FALSE           0x00

/* Natural exception structure. */
struct natural_exception
{
	/* Natural message number. */
	int natMessageNumber;
	/* Natural message text with all replacements. */
	char natMessageText[NNI_LEN_TEXT + 1];
	/* Natural library name. */
	char natLibrary[NNI_LEN_LIBRARY + 1] ;
	/* Natural member name. */
	char natMember[NNI_LEN_MEMBER + 1];
	/* Natural function or class name. */
	char natName[NNI_LEN_NAME + 1];
	/* Natural method or property name. */
	char natMethod[NNI_LEN_NAME + 1];
	/* Natural code line. */
	int natLine;
};

/* Function prototypes for static linking. */

/* Initialize a Natural session. */
int nni_initialize( void* pnni_func, const char* szCmdLine, void*, void* );

/* Check if a Natural session is initialized. */
int nni_is_initialized( void* pnni_func, int* piIsInit );

/* Uninitialize the Natural session. */
int nni_uninitialize( void* pnni_func );

/* Free the interface function table. */
int nni_free_interface( void* pnni_func );

/* Current thread waits for exclusive ownership of the NNI. */
int nni_enter( void* pnni_func );

/* Like nni_enter, without waiting. */
int nni_try_enter( void* pnni_func );

/* Current thread releases exclusive ownership of the NNI. */
int nni_leave( void* pnni_func );

/* Logon to a specific library. */
int nni_logon( void* pnni_func, const char* szLibrary, const char* szUser, const char* szPassword );

/* Logoff from the Natural library. */
int nni_logoff( void* pnni_func );

/* Call a subprogram. */
int nni_callnat(  void* pnni_func, const char* szName, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Call a function. */
int nni_function( void* pnni_func, const char* szName, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Create an object. */
int nni_create_object( void* pnni_func, const char* szName, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Send a method. */
int nni_send_method( void* pnni_func, const char* szName, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Get a property. */
int nni_get_property( void* pnni_func, const char* szName, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Set a property. */
int nni_set_property( void* pnni_func, const char* szName, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Delete an object. */
int nni_delete_object( void* pnni_func, int iParm,
	struct parameter_description* rgDesc, struct natural_exception* pExcep );

/* Create a parameter set. */
int nni_create_parm( void* pnni_func, int iParm, void** pparmhandle );

/* Create a parameter set for a specific subprogram, subroutine or function. */
int nni_create_module_parm( void* pnni_func, char chType, const char* szName, void** pparmhandle );

/* Create a parameter set for a specific method. */
int nni_create_method_parm( void* pnni_func, const char* szClass, const char* szMethod, void** pparmhandle );

/* Create a parameter set for a specific property. */
int nni_create_prop_parm( void* pnni_func, const char* szClass, const char* szProp, void** pparmhandle );

/* Get the number of parameters in a parameter set. */
int nni_parm_count( void* pnni_func, void* parmhandle, int* piParm );

/* Initialize a static scalar. */
int nni_init_parm_s( void* pnni_func, int iParm, void* parmhandle,
	char chFormat, int iLength, int iPrecision, int iFlags );

/* Initialize an array of a static data type. */
int nni_init_parm_sa( void* pnni_func, int iParm, void* parmhandle,
	char chFormat, int iLength, int iPrecision, int iDim, int* rgiOcc, int iFlags );

/* Initialize a dynamic scalar. */
int nni_init_parm_d( void* pnni_func, int iParm, void* parmhandle,
	char chFormat, int iFlags );

/* Initialize an array of a dynamic data type. */
int nni_init_parm_da( void* pnni_func, int iParm, void* parmhandle,
	char chFormat, int iDim, int* rgiOcc, int iFlags );

/* Get parameter information. */
int nni_get_parm_info( void* pnni_func, int iParm, void* parmhandle,
	struct parameter_description* pDesc );

/* Get parameter value. */
int nni_get_parm( void* pnni_func, int iParm, void* parmhandle,
	int iBufferLength, void* pBuffer );

/* Get value of an array occurrence. */
int nni_get_parm_array( void* pnni_func, int parmnum, void* parmhandle,
	int iBufferLength, void* pBuffer, int* rgiInd );

/* Get length of an array occurrence. */
int nni_get_parm_array_length( void* pnni_func, int iParm, void* parmhandle,
	int* piLength, int* rgiInd );

/* Put parameter value. */
int nni_put_parm( void* pnni_func, int iParm, void* parmhandle,
	int iBufferLength, const void* pBuffer );

/* Put value of an array occurrence. */
int nni_put_parm_array( void* pnni_func, int iParm, void* parmhandle,
	int iBufferLength, const void* pBuffer, int* rgiInd );

/* Resize an x-array parameter. */
int nni_resize_parm_array( void* pnni_func, int iParm, void* parmhandle, int* rgiOcc );

/* Delete a parameter set. */
int nni_delete_parm( void* pnni_func, void* parmhandle );

/* Convert a string to a P, N, D, or T value. */
int nni_from_string( void* pnni_func, const char* szString, char chFormat,
	int iLength, int iPrecision, int iBufferLength, void* pBuffer );

/* Convert a P, N, D, or T value to a string. */
int nni_to_string( void* pnni_func, int iBufferLength, const void* pBuffer, char chFormat,
	int iLength, int iPrecision, int iStringLength, char* szString );

/* Typedefs for dynamic linking. */
typedef int ( *PF_NNI_INITIALIZE ) ( void*, const char*, void*, void* );
typedef int ( *PF_NNI_IS_INITIALIZED ) ( void*, int* );
typedef int ( *PF_NNI_UNINITIALIZE ) ( void* );
typedef int ( *PF_NNI_FREE_INTERFACE ) ( void* );
typedef int ( *PF_NNI_ENTER ) ( void* );
typedef int ( *PF_NNI_TRY_ENTER ) ( void* );
typedef int ( *PF_NNI_LEAVE ) ( void* );
typedef int ( *PF_NNI_LOGON ) ( void*, const char*, const char*, const char* );
typedef int ( *PF_NNI_LOGOFF ) ( void* );
typedef int ( *PF_NNI_CALLNAT ) ( void*, const char*, int, struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_FUNCTION ) ( void*, const char*, int,	struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_CREATE_OBJECT ) ( void*, const char*, int, struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_SEND_METHOD ) ( void*, const char*, int, struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_GET_PROPERTY ) ( void*, const char*, int,	struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_SET_PROPERTY ) ( void*, const char*, int,	struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_DELETE_OBJECT ) ( void*, int,	struct parameter_description*, struct natural_exception* );
typedef int ( *PF_NNI_CREATE_PARM ) ( void*, int, void** );
typedef int ( *PF_NNI_CREATE_MODULE_PARM ) ( void*, char, const char*, void** );
typedef int ( *PF_NNI_CREATE_METHOD_PARM ) ( void*,	const char*, const char*, void** );
typedef int ( *PF_NNI_CREATE_PROP_PARM ) ( void*, const char*, const char*, void** );
typedef int ( *PF_NNI_PARM_COUNT ) ( void*, void*, int* );
typedef int ( *PF_NNI_INIT_PARM_S ) ( void*, int, void*, char, int, int, int );
typedef int ( *PF_NNI_INIT_PARM_SA ) ( void*, int, void*, char, int, int, int, int*, int );
typedef int ( *PF_NNI_INIT_PARM_D ) ( void*, int, void*, char, int );
typedef int ( *PF_NNI_INIT_PARM_DA ) ( void*, int, void*, char, int, int*, int );
typedef int ( *PF_NNI_GET_PARM_INFO ) (	void*, int, void*, struct parameter_description* descr );
typedef int ( *PF_NNI_GET_PARM ) ( void*, int, void*, int, void* );
typedef int ( *PF_NNI_GET_PARM_ARRAY ) ( void*, int, void*, int, void*, int* );
typedef int ( *PF_NNI_GET_PARM_ARRAY_LENGTH ) (	void*, int, void*, int*, int* );
typedef int ( *PF_NNI_PUT_PARM ) ( void*, int, void*, int, const void* );
typedef int ( *PF_NNI_PUT_PARM_ARRAY ) ( void*, int, void*, int, const void*, int* );
typedef int ( *PF_NNI_RESIZE_PARM_ARRAY ) ( void*, int, void*, int* );
typedef int ( *PF_NNI_DELETE_PARM ) ( void*, void* );
typedef int ( *PF_NNI_FROM_STRING ) ( void*, const char*, char,	int, int, int, void* );
typedef int ( *PF_NNI_TO_STRING ) ( void*, int, const void*, char, int, int, int, char* );

/* Function table for dynamic linking. */
typedef struct _nni_611_functions
{
	PF_NNI_INITIALIZE pf_nni_initialize;
	PF_NNI_IS_INITIALIZED pf_nni_is_initialized;
	PF_NNI_UNINITIALIZE pf_nni_uninitialize;
	PF_NNI_FREE_INTERFACE pf_nni_free_interface;
	PF_NNI_ENTER pf_nni_enter;
	PF_NNI_TRY_ENTER pf_nni_try_enter;
	PF_NNI_LEAVE pf_nni_leave;
	PF_NNI_LOGON pf_nni_logon;
	PF_NNI_LOGOFF pf_nni_logoff;
	PF_NNI_CALLNAT pf_nni_callnat;
	PF_NNI_FUNCTION pf_nni_function;
	PF_NNI_CREATE_OBJECT pf_nni_create_object;
	PF_NNI_SEND_METHOD pf_nni_send_method;
	PF_NNI_GET_PROPERTY pf_nni_get_property;
	PF_NNI_SET_PROPERTY pf_nni_set_property;
	PF_NNI_DELETE_OBJECT pf_nni_delete_object;
	PF_NNI_CREATE_PARM pf_nni_create_parm;
	PF_NNI_CREATE_MODULE_PARM pf_nni_create_module_parm;
	PF_NNI_CREATE_METHOD_PARM pf_nni_create_method_parm;
	PF_NNI_CREATE_PROP_PARM pf_nni_create_prop_parm;
	PF_NNI_PARM_COUNT pf_nni_parm_count;
	PF_NNI_INIT_PARM_S pf_nni_init_parm_s;
	PF_NNI_INIT_PARM_SA pf_nni_init_parm_sa;
	PF_NNI_INIT_PARM_D pf_nni_init_parm_d;
	PF_NNI_INIT_PARM_DA pf_nni_init_parm_da;
	PF_NNI_GET_PARM_INFO pf_nni_get_parm_info;
	PF_NNI_GET_PARM pf_nni_get_parm;
	PF_NNI_GET_PARM_ARRAY pf_nni_get_parm_array;
	PF_NNI_GET_PARM_ARRAY_LENGTH pf_nni_get_parm_array_length;
	PF_NNI_PUT_PARM pf_nni_put_parm;
	PF_NNI_PUT_PARM_ARRAY pf_nni_put_parm_array;
	PF_NNI_RESIZE_PARM_ARRAY pf_nni_resize_parm_array;
	PF_NNI_DELETE_PARM pf_nni_delete_parm;
	PF_NNI_FROM_STRING pf_nni_from_string;
	PF_NNI_TO_STRING pf_nni_to_string;
} nni_611_functions, *pnni_611_functions;

/* Get the interface function table. */
typedef int ( *PF_NNI_GET_INTERFACE ) ( int, void** );
int nni_get_interface( int iVersion, void** ppnni_func );

#ifdef __cplusplus
}
#endif

#endif /* natni_h */

