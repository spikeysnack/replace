/* replace.h */

/* AUTHOR:  Chris Reid <spikeysnack@gmail.com> */
/* LICENSE: Free for all purposes              */
/* COPYRIGHT: 2016- Chris Reid                 */


#ifndef REPLACE_H
#define REPLACE_H

#include <stdlib.h>  /* free, calloc, exit              */
#include <stdio.h>   /* printf , fprintf,               */
#include <string.h>  /* strlen, strcmp, strstr, memmove */  
#include <ctype.h>   /* isspace , etc                   */
#include <limits.h>  /* sizes                           */



/* external variables  (replace.c) */
extern size_t num_matches;
extern bool all_matches;
extern bool force_replace;
extern bool quiet_replace;
extern bool test_mode;
extern bool undo;
extern bool verbose;
extern bool answer;


#include "options.h"       /* argument option parsing       */
#include "utils.h"         /* file and string ops           */
#include "bad_filenames.h" /* restrictions on file renaming */


/* function declarations */


size_t count_words(const char* str);
size_t do_replace(int numargs, char* match, char* replace, char** arglist );
size_t do_undo(void);
char* substring( const char* str, const char* match);
char* replacestring( const char* str, const char* match, const char* replacement );
char* replace_n_strings ( const char* str, const char* match, const char* replacement, size_t n );
char* replace_all_strings ( const char* str, const char* match, const char* replacement );


 /*----------------------------------------------------------*/
 /* do_replace  (function)                                   */
 /* DESCRIPTION                                              */  
 /*        replace letters in filenams matching a string     */
 /*        with a replacement string. renames files.         */
 /*                                                          */
 /* INPUT                                                    */
 /*         numargs      int: number of filenames in arglist */
 /*         match      char*: matching string                */
 /*         replace    char*: replacement string for match   */
 /*         arglist   char**: the array of filename strings  */
 /*                                                          */
 /* OUTPUT                                                   */
 /*        size_t:  number of files renamed                  */
 /*                                                          */

size_t do_replace(int numargs, char* match, char* replace, char** arglist )
{

  int i = 0;
  size_t s_sz     = 0;
  size_t m_sz     = 0;
  size_t r_sz     = 0;
  size_t new_sz   = 0;
  size_t entry_sz = 0;
  size_t files_renamed = 0;

  char *oldnm      = NULL;
  char *newnm      = NULL;
  char *undo_entry = NULL;
  

#ifdef DEBUG_ARGS  
  assert( numargs >= 0 );
  assert( match   != (char*) NULL );
  assert( arglist != (char**) NULL );
#endif

  /* check error conditions */
  if ( ! numargs ) return 0;  

  if (  (!match) || (!replace) ) return 0; 

  m_sz = strlen(match);
  r_sz = strlen(replace);

  if (  (!m_sz) || (!r_sz) ) return 0; 
  /* end error conditions */



  for (i = 0; i < numargs ; i++ )
    {
      s_sz = strlen( arglist[i] ); 

      /* -a, --all option  */
      if ( all_matches ) 
	newnm = replace_all_strings( arglist[i], match, replace );
      else
	{
	  if (num_matches > 0 )
	    newnm = replace_n_strings( arglist[i], match, replace, num_matches );
	  else
	    continue; 	    
	}

      
      /* file checks */
      if ( ! newnm ) continue;	/* no  matches  --  skip */
      
      if ( ! valid_filename(newnm) )
	{
	  fprintf(stderr, "ERROR This would create a bad filename: %s .. skipping\n", newnm );	  
	  continue;
	}

      if (  file_exists(newnm)  )
	{
	  fprintf(stderr, "ERROR There is already a file named \"%s\". ..  skipping \n", newnm );
	  continue;
	}

      /* new name for file is ok */

      new_sz = strlen(newnm);
      
      if ( rename_file(arglist[i], newnm) ) /* renames the file if possible (utils.h) */
	{
	  files_renamed +=1; /* increment rename count */

	  oldnm = strndup( arglist[i], s_sz );

	  entry_sz = s_sz + new_sz + 2 ;

	  if (entry_sz > 255 ) entry_sz = 255; // limit

	  undo_entry = NEW_STRING(256); 

	  snprintf( undo_entry , entry_sz, "%s\t%s\n" , oldnm , newnm  );

	  save_entry( undo_entry );          /* save to file (utils.h) */

	  /* clean up */
	  FREE_STRING(undo_entry);
	  FREE_STRING(oldnm);
	  FREE_STRING(newnm);

	}
      else
	{
	  if (answer)
	    fprintf(stderr, "ERROR can't rename %s to %s \n" , arglist[i] , newnm );
	  else
	    fprintf(stderr, "skip %s\n", arglist[i] );

	  FREE_STRING(newnm);

	  continue; 
	}      

    } /*for */

  return files_renamed;
}/* do_replace */




/*----------------------------------------------------------*/
/* do_undo                                                  */
/* DESCRIPTION                                              */ 
/*            get command from undo_file and execute it     */ 
/*            with match and replace reversed               */
/*                                                          */
/* INPUT                                                    */
/*         none                                             */
/* OUTPUT                                                   */
/*         size_t: number of files renamed                  */

size_t do_undo()
{
  char** undo_entries = NULL;
  char** undo_names   = NULL;
  size_t files_renamed   = 0;
    
  undo_entries = get_entries();
  
  undo_names = NEW_STRING_ARRAY( 2 ); 
  
  int ue = 0;      

  while( undo_entries[ue] != NULL )
    {
      undo_names = string_to_array( undo_entries[ue], "\t" ); 
      
      if ( ! rename_file ( undo_names[1], undo_names[0] ) )
	{
	  fprintf( stderr, "ERROR CAN'T RESTORE NAME  %s\n",  undo_names[0] );
	}
      else
	{
	  files_renamed += 1;
	}

      ue++;

      FREE_STRING(undo_names[0]);
      FREE_STRING(undo_names[1]);
      
    }// while


  FREE_STRING_ARRAY( undo_names );  

  FREE_STRING_ARRAY( undo_entries );  
  
  return files_renamed;

} // do_undo



 /*----------------------------------------------------------*/
 /* replacestring  (function)                                */
 /* DESCRIPTION                                              */  
 /*        replace letters in string matching a string       */
 /*        with a replacement string.                        */
 /*                                                          */
 /* INPUT                                                    */
 /*     str         char*: number of filenames in arglist    */
 /*     match       char*: matching string                   */
 /*     replacement char*: replacement string for match      */
 /*         arglist   char**: the array of filename strings  */
 /*                                                          */
 /* OUTPUT                                                   */
 /*     char*: the replaced string                           */
 /*                                                          */

char* replacestring( const char* str, const char* match, const char* replacement )
{

  int mlen   = 0;
  int slen   = 0;
  int sublen = 0;
  int replen = 0;
  int start  = 0; 
  int end    = 0;
  size_t new_sz = 0;

  int err;
  char* tmp  = {0};
  char* sub  = {0};  
  char* tail = {0};  
  
  errno = 0;

  slen = strlen(str);
  mlen = strlen(match);
  replen = strlen(replacement);

  new_sz = slen - mlen + replen ;
  
  tmp = NEW_STRING(new_sz) ;   // create chars
 
  if (!tmp)
    {
      err = errno;
      char errstring[256];
      snprintf(errstring, sizeof errstring, "%s%s%s", "substring: Calloc Failed:\t", strerror(err), "\n");
      perror (errstring );      
      errno = 0;
      exit(-1);
    }

  sub = strstr(str, match);  // find  first match or NULL

#ifdef DEBUG_REPLACE 
  fprintf(stderr , "[replacestring:285] [matched  %s in %s ]\n" , match , str );
#endif


 

  if ( sub )  // match
    {
      sublen  = strlen(sub);
      
      start = slen - sublen;
      
      tail = sub+mlen;
      end   = strlen(tail);

#ifdef DEBUG_REPLACE 
      fprintf(stderr, "\n[replacestring] start = [%d] , end = [%d]  tail = [%s] \n" , start, end, tail );
#endif      

      memmove(tmp, str, start  ); //first bytes before match

#ifdef DEBUG_REPLACE 
      fprintf(stderr, "\n[replacestring] mm1 tmp = %s\n" , tmp );
#endif

      memcpy(tmp+start, replacement, replen ); //

#ifdef DEBUG_REPLACE 
      fprintf(stderr, "\n[replacestring] mm2 tmp = %s\n" , tmp );
#endif
      memmove(tmp+start+replen, tail , end  ); //last bytes after match

#ifdef DEBUG_REPLACE 
      fprintf(stderr, "\n[replacestring] mm3 tmp = %s\n" , tmp );
#endif

      tmp[new_sz] = '\0'; // terminate

#ifdef DEBUG_REPLACE 
      fprintf(stderr, "\n[replacestring] mm4 tmp = %s\n" , tmp );
#endif
      
      FREE_STRING(tail);
      FREE_STRING(sub);

      return tmp;
    }   
  else  // no match
    {
      FREE_STRING(tail);
      FREE_STRING(sub);
      FREE_STRING(tmp);

      return (char*)NULL; // no match      
    }

}/*replacestring*/


 /*----------------------------------------------------------*/
 /* replace_n_strings  (function)                            */
 /* DESCRIPTION                                              */  
 /*        replace letters in string matching a string       */
 /*        with a replacement string. replaces up to n       */
 /*        occurances of match in str.                       */
 /*                                                          */
 /* INPUT                                                    */
 /*     str         char*: number of filenames in arglist    */
 /*     match       char*: matching string                   */
 /*     replacement char*: replacement string for match      */
 /*     n          size_t: how many times to replace         */
 /*                                                          */
 /* OUTPUT                                                   */
 /*     char*: the replaced string                           */
 /*                                                          */


char* replace_n_strings ( const char* str, const char* match, const char* replacement, size_t n )
{
  
  char* replaced = NULL;
  char* sub      = NULL;

  size_t str_sz  = 0;
  size_t m_sz    = 0;
  size_t max     = 0;
  size_t count   = 0;

  m_sz           = strlen(match);
  str_sz         = strlen(str);
  max            = str_sz / m_sz;
  max            = (max)? max : 1;
  count          = (n > max)? max : n;   /* count is always at least one  */


  if( str_sz ) 
    {
      sub = replacestring(str,  match, replacement);

      if (!sub ) return NULL;

#ifdef DEBUG_REPLACE
      fprintf(stderr, "[replace_n_strings] %zu  sub = %s \n ", count, sub );
#endif

      replaced = sub;
      count--;
    }
  else
    return NULL;
  

    while( count > 0 ) 
      {
	sub = replacestring(sub,  match, replacement);

#ifdef DEBUG_REPLACE
	fprintf(stderr, "[replace_n_strings] %zu sub = %s \n ", count, sub );
#endif
        if (! sub) break;
	replaced = sub;	

#ifdef DEBUG_REPLACE
	fprintf(stderr, "[replace_n_strings] %zu replaced = %s \n ", count, replaced );
#endif
	count--;
      } 
  
#ifdef DEBUG_REPLACE
    fprintf(stderr, "[replace_n_strings] %zu replaced = %s \n ", count, replaced );
#endif
        
    FREE_STRING(sub); 
        
    return replaced;
}


 /*----------------------------------------------------------*/
 /* replace_all_strings  (function)                          */
 /* DESCRIPTION                                              */  
 /*           wrapper for replace_n_strings                  */
 /*           with n  = UINT_MAX;                            */
 /*                                                          */
 /* INPUT                                                    */
 /*     str         char*: number of filenames in arglist    */
 /*     match       char*: matching string                   */
 /*     replacement char*: replacement string for match      */
  /*                                                         */
 /* OUTPUT                                                   */
 /*     char*: the replaced string with all occurrances      */
 /*            of match replaced                             */
 /*                                                          */

char* replace_all_strings ( const char* str, const char* match, const char* replacement )
{
  size_t max = strlen(str) / strlen(match);

  if (!max ) return (char*)NULL;
  
  return replace_n_strings ( str, match, replacement, max );
}



#endif // REPLACE_H

/*END*/



