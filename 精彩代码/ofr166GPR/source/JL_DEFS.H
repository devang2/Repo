#ifndef JL_DEFS_H
#define JL_DEFS_H
/* jl_defs.h */
/* Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * November 1, 1995; July 12, 1999
 */
/****** Include header files ******/
#include <dos.h>               /* REGS,intdos,int86 */

/****** Manifest constants ******/
#define FUNC_CREATE      (0)   /* tells function to allocate storage */
#define FUNC_DESTROY     (1)   /* tells function to deallocate storage */
#define MAX_CMDLINELEN (164)   /* maximum line length in CMD files */
#define MAX_PATHLEN     (80)   /* max DOS path length (66) + filename.ext (12) + 2 */
#define ESC             (27)   /* <Esc> keyboard code */
#define ENTER           (13)   /* <Enter> keyboard code */
#define HELP            (59)   /* <F1> keyboard code */
#define TRUE             (1)   /* for Boolean-type expressions */
#define FALSE            (0)   /* for Boolean-type expressions */
#define INVALID_VALUE (1.0E19) /* flag to indicate an invalid value */

#ifndef DA_DEFS                /* these constants used by DrawAxis() */
  #define DA_DEFS
  #define MAX_TITLE_LEN 100    /* as below, defunct */
  #define DA_TITLE_LEN 100     /* max length of axis titles (including NULL) */
  #define DA_TICK_NUM 9        /* default number of ticks if not specified */
  #define DA_TITLE    0
  #define DA_RIGHT    1
  #define DA_TOP      2
  #define DA_LEFT   (-1)
  #define DA_BOTTOM (-2)
#endif

#ifndef INT_DOS
  #define INT_DOS       0x21  /* DOS interrupt number, 33 */
  #define INT_MULTIPLEX 0x2F  /* DOS multiplex interrupt number, 47 */
#endif

#ifndef INPUT_DATATYPES
  #define INPUT_DATATYPES
  #define CHAR     (1)
  #define UCHAR    (-1)
  #define SHORT    (2)
  #define USHORT   (-2)
  #define USHORT12 (-5)
  #define LONG     (3)
  #define ULONG    (-3)
  #define ULONG24  (-6)
  #define FLOAT    (4)
  #define DOUBLE   (8)
#endif

/****** Parameterized macros to test values ******/
#define _MINN(a,b) (((a) < (b)) ? (a) : (b))
#define _MAXX(a,b) (((a) > (b)) ? (a) : (b))
#define _ABS(a)    (((a) < 0) ? (-(a)) : (a))
#define _SIGN(a)   (((a) < 0) ? (-1) : (((a) > 0) ? (1) : (0)))
#define _BETWEEN(a,b,c) ( ((b) < (a)) ? (a) : ( ((b) > (c)) ? (c) : (b) ) )
#define _LIMIT(a,b,c)   ( ((b) < (a)) ? (a) : ( ((b) > (c)) ? (c) : (b) ) )

/****** Parameterized macros to allocate and free conventional memory ******/
#if defined(_INTELC32_)
  /* Example usage:  (11/1/95)
   *    char *buffer = NULL;
   *    size_t vbufsize = 64512;     ** (63K used only as an example) **
   *    FILE *I_file;                ** include stdio.h **
   *      ... code ...
   *    realmalloc(buffer,vbufsize);
   *      ... code that can use buffer[] ...
   *      ... here is an example using buffer[] to speed up disk I/O ...
   *    I_file = fopen(......);
   *    setvbuf(I_file,buffer,_IOFBF,vbufsize);
   *      ... code ...
   *    realfree(buffer);
   *    fclose(I_file);
   *      ... code that cannot use buffer[] ...
   */
  #ifndef realmalloc
    #define realmalloc(block,bytes)\
    {   union REGS _CPUREGS;\
        _CPUREGS.w.eax = 0x00004800;\
        _CPUREGS.w.ebx = (unsigned)(bytes);\
        intdos((const union REGS *)&_CPUREGS,&_CPUREGS);\
        (block) = (char *)_CPUREGS.w.eax;\
        if (_CPUREGS.w.cflag)  (block) = NULL;\
    }
    #define realfree(block)\
    {   if (block > 0)\
        {   union REGS _CPUREGS;\
            _CPUREGS.w.eax = 0x00004900;\
            _CPUREGS.w.edx = (unsigned)(block);\
            intdos((const union REGS *)&_CPUREGS,&_CPUREGS);\
            (block) = NULL;\
        }\
    }
  #endif  /* #ifndef realmalloc */
#endif  /* #if defined(_INTELC32_) */

/****** Parameterized macros to swap bytes in a 2- or 4-byte variable;
        such as character strings, integers, and floating points.
       (other expressions must not be used!)
 ******/
#ifndef SWAP_BYTES
  #define SWAP_2BYTES(val)\
  {   char *_CPTR,_CTEMP;\
      _CPTR  = (char *)&(val);\
      _CTEMP =_CPTR[0]; _CPTR[0] =_CPTR[1]; _CPTR[1] =_CTEMP;\
  }
  #define SWAP_4BYTES(val)\
  {   char *_CPTR  = (char *)&(val);\
      char _CTEMP =_CPTR[0]; _CPTR[0] =_CPTR[3]; _CPTR[3] =_CTEMP;\
           _CTEMP =_CPTR[1]; _CPTR[1] =_CPTR[2]; _CPTR[2] =_CTEMP;\
  }
  #define SWAP_BYTES
#endif

#endif /* #ifndef JL_DEFS_H */
