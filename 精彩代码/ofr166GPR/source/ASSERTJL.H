#if !defined(ASSERTJL)
/* assertjl.h
 * Custom version of assert macro.
 *    by Jeff Lucius, USGS, Golden, CO, November 29, 1995
 * This version should be able to be used with any C compiler.
 */
  #undef assert
  #ifndef NDEBUG
    #include <stdio.h>   /* fprintf,stderr */
    #include <stdlib.h>  /* abort */

    #define assert(_exp_)\
    {   if (_exp_) (void) 0;\
        else\
        {   fprintf(stderr,"Assertion failed: (%s), file %s, line %d\n",#_exp_,__FILE__,__LINE__);\
            abort();\
        }\
    }
  #else
    #define assert(ignore)
  #endif /* NDEBUG */
  #define ASSERTJL
#endif  /* #if !defined(ASSERTJL) */

