/* gc_malloc.h */

/* 
   This file rewrites malloc calls to utilise
   the Boehm Conservative Garbage Collector.
   All calls to malloc, calloc, realloc, and free are rewired
   through the garbage collector transparently, so 
   all  dynamically created objects are marked and 
   periodically swept up if there is no active pointer
   to them left in the C runtime. "Conservative" here
   means the gc only runs periodically and only
   clears memory when it is appropriate. 
   USAGE: 
          Include this file in any .h file that has  dynamically 
          created memory operations, add "-lgc" to your gcc compile comamnd
          and it just works. you can see how the code has changed by 
          using the gcc -E <file.c> | less  command.
*/

#ifndef GC_MALLOC_H
#define GC_MALLOC_H


#ifdef BOEHM_GC

#ifdef DEBUG
#define GC_DEBUG
#endif


#ifndef GC_DONT_INCLUDE_STDLIB
/* We ensure stdlib.h and string.h are included before        */
/* redirecting malloc() and the accompanying functions.       */
# include <stdlib.h>
# include <string.h>
#endif

#include <pthread.h>  
#include <gc/gc.h>

// textual replacement works fine 
// because the function signatures match identically
#define malloc(n) GC_MALLOC(n)

#define calloc(m,sz) GC_MALLOC((m)*(sz))

#define realloc( o , sz ) GC_REALLOC(o,sz)

#define free( n ) GC_FREE(n)

#define CHECK_LEAKS() GC_gcollect()

#define strdup(s) GC_STRDUP(s)

#define strndup(s,sz) GC_STRNDUP(s, sz) 
  

//NOT WITH_GC
#else 
//these get erased in the preprocesssor  if GC not used 
#define GC_INIT()   
#define CHECK_LEAKS()   

#endif // BOEHM_GC

#endif // GC_MALLOC_H

/*
void example(void)
{
  char* ex = calloc ( 100, sizeof(char) );
  free(ex);
}
*/

/*END*/
