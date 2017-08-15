
/*****************************************************************************
**
** file    : natuser.h
** purpose : NATURAL interface to user supplied C functions
**
** (c) Copyright 1991-2004 by Software AG
**
*****************************************************************************/

#ifndef DEF_NATUSER
#define DEF_NATUSER     1

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef WORD
#define WORD  unsigned short 
#endif

#ifndef BYTE
#define BYTE  unsigned char
#endif

/*----------------------------------------------------------------------------
** NATURAL parameter field information
**--------------------------------------------------------------------------*/
typedef struct {
  char typevar;                      /* type of variable           */
  BYTE pb2;                          /* if type == (N or P) ==>    */
                                     /*    total num of digits     */
                                     /* else                       */
  union {                            /*    unused                  */
    BYTE pb[2];                      /* if type == (N or P) ==>    */
    WORD lfield;                     /*  pb[0] = #dig bef.dec.point*/
  } flen;                            /*  pb[1] = #dig aft.dec.point*/
                                     /* else                       */
                                     /*    lfield = length of      */
                                     /*             field          */
} FINFO;


/*----------------------------------------------------------------------------
** defines for platform independent user exits
**--------------------------------------------------------------------------*/
#if OS_WIN_32
/* WINDOWS 32-bit compiler ---------------------------------------*/
# define NATTYP_I1      char                    /* 1 byte integer */
# define NATTYP_I2      short                   /* 2 byte integer */
# define NATTYP_I4      int                     /* 4 byte integer */
# define NATTYP_FCT     long                    /* return type    */
# define NATFCT         long                    /* return type    */
# define NATARGDEF(count,pdat,pinf) (WORD count, \
                                    BYTE ** pdat, \
                                    FINFO * pinf)
# define NATARGDCL(count,pdat,pinf) NATARGDEF(count,pdat,pinf)

extern int ncxr_callnat(char *, WORD, BYTE **, FINFO *);

#else
#if OS_VMS
/* openvms 32-bit compiler ------------------------------------------*/
# define NATTYP_I1      char                    /* 1 byte integer */
# define NATTYP_I2      short                   /* 2 byte integer */
# define NATTYP_I4      int                     /* 4 byte integer */
# define NATTYP_FCT     long                    /* return type    */
# define NATFCT         long                    /* return type    */
# define NATARGDEF(count,pdat,pinf) (WORD count, \
                                     BYTE ** pdat, \
                                     FINFO * pinf)
# define NATARGDCL(count,pdat,pinf) NATARGDEF(count,pdat,pinf)

extern int ncxr_callnat();

#else
/* UNIX ----------------------------------------------------------*/
# define NATTYP_I1      char                    /* 1 byte integer */
# define NATTYP_I2      short                   /* 2 byte integer */
# define NATTYP_I4      int                     /* 4 byte integer */
# define NATTYP_FCT     long                    /* return type    */
# define NATFCT         long                    /* return type    */
# define NATARGDEF(count,pdat,pinf) ( \
                                    WORD count, \
                                    BYTE ** pdat, \
                                    FINFO * pinf)
# define NATARGDCL(count,pdat,pinf) NATARGDEF(count,pdat,pinf)

extern int ncxr_callnat(char *, WORD, BYTE **, FINFO *);

#endif
#endif

/*-------------- INTERFACE4 ------------------------------------------*/

# define NATARGDEF4(numparm,parmhandle,traditional)  \
          ( WORD numparm, void *parmhandle, void *traditional )
# define NATARGDCL4(numparm,parmhandle,traditional)  \
          NATARGDEF4(numparm,parmhandle,traditional)


/*
** defines for struct parameter_description
*/

#define IF4_MAX_DIM    3

/* defines for flags */
#define IF4_FLG_PROTECTED      0x00000001    /* parameter is protected, cannot be returned */
#define IF4_FLG_DYNAMIC        0x00000002    /* parameter is dynamic (variable or x-array) */
#define IF4_FLG_NOT_CONTIGUOUS 0x00000004    /* array is not contiguous                    */
#define IF4_FLG_AIV            0x00000008    /* is an AIV/INDEPENDENT variable (+ variable)*/
#define IF4_FLG_DYNVAR         0x00000010    /* parameter is a dynamic variable            */
#define IF4_FLG_XARRAY         0x00000020    /* parameter is an x-array                    */
#define IF4_FLG_LBVAR_0        0x00000040    /* lower bound of dimension 0 is variable     */
#define IF4_FLG_UBVAR_0        0x00000080    /* upper bound of dimension 0 is variable     */
#define IF4_FLG_LBVAR_1        0x00000100    /* lower bound of dimension 1 is variable     */
#define IF4_FLG_UBVAR_1        0x00000200    /* upper bound of dimension 1 is variable     */
#define IF4_FLG_LBVAR_2        0x00000400    /* lower bound of dimension 2 is variable     */
#define IF4_FLG_UBVAR_2        0x00000800    /* upper bound of dimension 2 is variable     */

struct parameter_description
{
    void *address;          /* not aligned, realloc()/free() is not allowed */
    int   format;           /* field type: ALPHA -> 'A', INT -> 'I', etc. */
    int   length;           /* length (before decimal point)   */
    int   precision;        /* length after decimal point (PACKED, NUMERIC) */
    int   byte_length;      /* length of field in bytes  */
    int   dimensions;       /* number of dimensions: 0 ... IF4_MAX_DIM */
    int   length_all;       /* total length of array in bytes */
    int   flags;            /* IF4_PROTECTED, IF4_DYNAMIC, IF4_FLG_NOT_CONTIGUOUS */

    int   occurrences[10];  /* array occurrences in each dimension */
                            /* currently, the max. dimension is IF4_MAX_DIM  (0...2) */

    int   indexfactors[10]; /* array indexfactors in each dimension */
                            /* currently, the max. dimension is IF4_MAX_DIM  (0...2) */

    void *dynp;             /* internal use only (for dynamic variables) */

    void *pops;             /* internal use only (for dynamic variables and x-arrays) */
    void *reservedp2;       /* for future use */
    void *reservedp1;       /* for future use */

    int   reservedi8;       /* for future use */
    int   reservedi7;       /* for future use */
    int   reservedi6;       /* for future use */
    int   reservedi5;       /* for future use */
    int   reservedi4;       /* for future use */
    int   reservedi3;       /* for future use */
    int   reservedi2;       /* for future use */
    int   reservedi1;       /* for future use */
};

/*
** defines for return codes of the access functions
**
** a return value > 0 indicates succes, but more data could have been passed
** if a resize for a dynamic parameter fails, data is passed back as far as memory allows
*/
#define IF4_RC_OK                0   /* success                                             */
#define IF4_RC_ILL_PNUM         -1   /* illegal parameter number                            */
#define IF4_RC_INT_ERROR        -2   /* internal error                                      */
#define IF4_RC_DATA_TRUNCATED   -3   /* data have been truncated                            */
#define IF4_RC_NOT_ARRAY        -4   /* parameter is not an array                           */
#define IF4_RC_WRT_PROTECTED    -5   /* parameter is write protected (constant or (AD=O)    */
#define IF4_RC_NO_MEMORY        -6   /* out of memory when dynamic parameter is resized     */
#define IF4_RC_BAD_VERSION      -7   /* invalid version of interface struct (or trad. CALL) */
#define IF4_RC_BAD_FORMAT       -8   /* invalid data format                                 */
#define IF4_RC_BAD_LENGTH       -9   /* invalid length or precision                         */
#define IF4_RC_BAD_DIM         -10   /* invalid dimension count                             */
#define IF4_RC_BAD_BOUNDS      -11   /* invalid x-array bound definition                    */
#define IF4_RC_NOT_RESIZABLE   -12   /* array cannot be resized in the way requested        */
#define IF4_RC_INCOMPL_UCHAR   -13   /* buffer includes an incomplete Unicode character     */
#define IF4_RC_BAD_INDEX_0    -100   /* index for dimension 0 out of range                  */
#define IF4_RC_BAD_INDEX_1    -101   /* index for dimension 1 out of range                  */
#define IF4_RC_BAD_INDEX_2    -102   /* index for dimension 2 out of range                  */

/*
** prototypes of the access functions
*/
int ncxr_get_parm_info ( int parmnum, void *parmhandle,
                         struct parameter_description *descr );

int ncxr_get_parm      ( int parmnum, void *parmhandle,
                         int buffer_length, void *buffer );

int ncxr_get_parm_array( int parmnum, void *parmhandle,
                         int buffer_length, void *buffer, int *indexes );

int ncxr_put_parm      ( int parmnum, void *parmhandle,
                         int buffer_length, void *buffer );

int ncxr_put_parm_array( int parmnum, void *parmhandle,
                         int buffer_length, void *buffer, int *indexes );

int ncxr_resize_parm_array( int parmnum, void *parmhandle, int *occ );

int ncxr_create_parm   ( int parmnum, void **pparmhandle );

int ncxr_delete_parm   ( void *parmhandle );

int ncxr_init_parm_s   ( int parmnum, void *parmhandle,
                         char format, int length, int precision, int flags );

int ncxr_init_parm_sa  ( int parmnum, void *parmhandle,
                         char format, int length, int precision, int dim, int *occ, int flags );

int ncxr_init_parm_d   ( int parmnum, void *parmhandle,
                         char format, int flags );

int ncxr_init_parm_da  ( int parmnum, void *parmhandle,
                         char format, int dim, int *occ, int flags );

int ncxr_if4_callnat( char *pname, int nparm, struct parameter_description *descp );

#ifdef __cplusplus
}
#endif

#endif /* DEF_NATUSER */

