/* utils.h */

/* some utility functions  for basic operations */
/* on strings and files.                        */


/* AUTHOR:  Chris Reid <spikeysnack@gmail.com> */
/* LICENSE: Free for all purposes              */
/* COPYRIGHT: 2016- Chris Reid                 */


#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include "options.h"
#include "bad_filenames.h"


/* external variables  (replace.c)*/
extern size_t num_matches;
extern bool all_matches;
extern bool force_replace;
extern bool quiet_replace;
extern bool test_mode;
extern bool verbose;
extern bool answer;

extern char* replace_undo_home;  
extern char* replace_undo_dir;  
extern char* replace_undo_file;  
extern char* replace_undo_path;  


/* localized globals for fopen */
bool first_open = true;
char* working_dir = NULL;

/* BUFSIZE  (definition)      */
/* bufer size for string      */
#define BUFSIZE 256



/* Function prototype declarations */

bool file_exists(const char* filename);
bool valid_filename (const char* filename  );
bool rename_file ( const char* filename, const char* newname );

bool get_response(void);

char** get_entries( void);
void   save_entry( char* cmd );
char*  array_to_string( char** arr );
char** string_to_array( char* str, const char* delim);

size_t count_words(const char* str);

struct user_info
{
  char*             username;
  char*             userdir ;
  uid_t             user_id ;  
};

struct user_info*  get_user_info ( void );

char* get_undo_dir( struct user_info* ui );


/*-------------------------------------*/
/*  MACROS                             */
/*  some macros for strings            */


/* handle NULL strings (strlen won't) */
#define strlen(s) ( (s==NULL)?0:strlen(s) )


#define NEW_STRING( sz )  \
  (char*) calloc( (sz) +1 , sizeof(char) ); 

#define NEW_STRING_ARRAY( sz )  \
  (char**) calloc( (sz) +1 ,  sizeof(char*) );

#ifndef BOEHM_GC
      #define  FREE_STRING( S ) free( (S) ); (S) = NULL;
      #define  FREE_STRING_ARRAY( SA ) \
               do { int i = 0; while ( SA[i] != NULL) { free(SA[i]); i++; } } while(0); (SA) = NULL;

#else
      #define  FREE_STRING( S )  (S=NULL)
      #define  FREE_STRING_ARRAY( SA )  (SA=NULL)
#endif


/*-------------------------------------------*/
/* file_exists (function)                    */
/*                                           */ 
/* DESCRIPTION                               */
/*      check existence of a file            */
/*                                           */
/* INPUT                                     */
/*      filename  char*:  a filename         */
/*                                           */
/* OUTPUT                                    */
/*      bool: true if file exists            */
/*                                           */
/* NOTE                                      */
/*       returns false if user has           */
/*       no read access even if it exists    */
/*                                           */

bool file_exists(const char* filename)
{
  bool  ret       = false;
  char* errstring = NULL;
  int   err       = 0;
  
  
  if ( ! filename ) return ret;
  
  errno = 0;

  if ( access( filename , F_OK ) == 0 ) //check for existence
    {      
      ret = true;
    }
  else
    {
      err = errno;                                                                                                                                    
      errno = 0;  

      if ( err != ENOENT )  // file not found 
	{
	  fprintf(stderr, "[utils.h:141] ACCESS to [%s] failed.\n", filename );
	  errstring = strerror(err);                                                                                                                      
	  perror(errstring);   
	}                                                                                                                           
      ret = false;      
    }

  FREE_STRING(errstring);

  return ret;
}



/*-------------------------------------------*/
/* rename_file (function)                    */
/*                                           */ 
/* DESCRIPTION                               */
/*      rename a file                        */ 
/*                                           */
/* INPUT                                     */
/*      filename  char*:  a filename         */
/*      newname   char*:  a filename         */
/*                                           */
/* OUTPUT                                    */
/*      bool: true if file renamed           */
/*                                           */
/* NOTE                                      */
/*     wrapper for <stdio.h>libc rename      */
/*                                           */

bool rename_file ( const char* filename, const char* newname )
{
  int err;
  int ret = false;
  char* errstring;
  
  assert( newname != (char*) NULL );    
  errno = 0;

#ifdef DEBUG
  fprintf(stderr, "current dir:  %s \n", get_current_dir_name() );
  fprintf(stderr, "rename_file: filename: %s \t newname: %s\n" , filename, newname );
  fflush (stderr);
#endif
  
  if ( (file_exists( filename )) && (access(filename, W_OK) == 0) ) 
    {
      if (! force_replace )
	{
	  printf("rename %s to %s (y/n)? ", filename , newname );	      
	  answer = get_response();
	  if (! answer )
	    {
	      ret = answer;
	      goto DONE;
	    }
	} 

      if ( ! test_mode )
	{
	  if ( rename( filename, newname ) == 0 ) 
	    { 
	      if (! quiet_replace) fprintf(stdout , "%s ==> %s \n", filename, newname ); 
	      ret = true;
	    }
	  else
	    {
	      err = errno;
	      errno = 0;
	      errstring = strerror(err);
	      fprintf(stderr, "[utils.h:215] rename of %s to %s failed. \n", filename, newname );
	      perror(errstring);
	      ret = false;
	    }
	}
      else
	{
	  /* TEST MODE */
	  fprintf(stdout , "TEST  %s ==> %s  TEST \n", filename, newname );                                                                  
	  ret = true;                                                                
	}
    
    } /* file exists */
  else
    {
      err = errno;
      errno = 0;
      fprintf(stderr , "[utils.h:232] filename  [%s] exists or access denied \n" , filename  );
      errstring = strerror(err);
      perror(errstring);	  
      ret = false; 
    } //top if

 DONE:      
  FREE_STRING(errstring);
  return ret;
}/*rename file*/


/*-------------------------------------------*/
/* get_response (function)                   */
/*                                           */ 
/* DESCRIPTION                               */
/*      ask user before continuing           */ 
/*                                           */
/* INPUT                                     */
/*      none                                 */
/*                                           */
/* OUTPUT                                    */
/*      bool: true if answer is Y or y       */
/*                                           */

bool get_response()
{
  char line[BUFSIZE] = {0};
  char p;

  fflush (stdin);

  if (  scanf ("%s", line) == EOF )
    return false;

  if (line == (char*)NULL )
    return false;
  else     
    {
      p = line[0];
      if ( (p == 'Y') || ( p == 'y') )
	return true;
      else 
	return false;
    }

}/* get_response */


/*-------------------------------------------*/
/* valid_filename (function)                 */
/*                                           */ 
/* DESCRIPTION                               */
/*      checks a string for suitability      */ 
/*      as a filename                        */
/*                                           */           
/* INPUT                                     */
/*      filename  char*:  a filename         */
/*                                           */
/* OUTPUT                                    */
/*      bool: true if file name is OK        */
/*                                           */
/* NOTE                                      */
/*     see (bad_filenames.h)                 */
/*                                           */

bool valid_filename (const char* filename  )
{
  size_t i, len;
  const char first = filename[0];
  char c;

  if(verbose)
    if ( ! strncmp( filename , "", 1 ) )
      {
	fprintf( stderr, "empty filename would result .. skipping\n" );
	return false;
      }
  
  len = strlen(filename);

  if (! len ) return false;

  if ( first == dot[0] )
    {
      if ( len == 1 ) return false;

      if ( len == 2 )
	if ( (first+1) == dot[1]  )  
	  return false;
    }

  if ( strstr( dbl_space , filename ) ) 
    return false;
  
  len = strlen(invalid_chars);
  for (i=0 ; i < len ; i++)
    {
      c = invalid_chars[i];
      if ( strchr( filename, c ) )
	return false;
    }      

  len = strlen(other_bad);
  for (i=0 ; i < len; i++)
    {
      c = other_bad[i];
      if ( strchr( filename , c ) )
	return false;
    }      

  return true;
} // valid_filename 


/*-------------------------------------------*/
/* get_user_info (function)                  */
/*                                           */ 
/* DESCRIPTION                               */
/*      retrieves information about user     */
/*      such as username, userdir uid        */
/*                                           */           
/* INPUT                                     */
/*      none                                 */
/*                                           */
/* OUTPUT                                    */
/*     user info struct                      */
/*                                           */
/* NOTE                                      */
/*    only enough info to do find the        */
/*    home dir of the user is retreived      */
/*                                           */
/*    passwd struct is erased and destroyed  */  
/*    befor leaving the functon              */
/*                                           */
/*                                           */
/*    struct user_info                       */
/*    {                                      */
/*     char*             username;           */
/*     char*             userdir ;           */
/*     uid_t             user_id ;           */
/*    };                                     */
/*                                           */

struct user_info*  get_user_info ( void )
{

  struct passwd*    passwd;           /* man getpwuid */
  struct user_info*  userinfo;

  /* create the struct */
  userinfo = (struct user_info*) calloc( 1, sizeof(struct user_info) );

  //get real user if sudo 
  if ( geteuid() == 0 )
    {
      userinfo->username =  getenv ( "SUDO_USER"); // sudo env |grep SUDO 
      userinfo->user_id   =  (uid_t) atoi ( getenv ( "SUDO_UID") );
      //fprintf( stderr , "actual user %s  actual uid \%d\n", user_name,user_id ); 
    }  
  else
    {
      userinfo->user_id = geteuid();
    }

  passwd = getpwuid ( userinfo->user_id );   /* Get the uid of the running process;use it to get a record from /etc/passwd */

  userinfo->username =  strndup( passwd->pw_name, strlen( passwd->pw_name) ); 
  userinfo->userdir  =  strndup( passwd->pw_dir , strlen (passwd->pw_dir)  );

  endpwent(); /* close pw database */

  memset(passwd , 0 , sizeof(struct passwd) ); /* zero out sensitive data */

  return userinfo;

} // get_user_info


/* free inside struct */
#ifndef BOEHM_GC
        #define free_user_info(ui ) \
        do { free( ui->username); free( ui->userdir); free(ui);} while(0)
#else
        #define free_user_info(ui)  (ui = NULL)
#endif


/*-------------------------------------------*/
/* get_undo_dir (function)                   */
/*                                           */ 
/* DESCRIPTION                               */
/*      calculates a proper path             */
/*      for the  undo file                   */
/*                                           */           
/* INPUT                                     */
/*      ui  struct user_info*:  users info   */
/*                                           */
/* OUTPUT                                    */
/*       char*: full path to undo file       */
/*                                           */
/* NOTE                                      */
/*    default file:                          */
/*              $HOME/.replace/replace_undo) */
/*                                           */

char* get_undo_dir( struct user_info* ui )
{
  char* undo_dir;
  
  size_t sz = 0;

  sz = strlen( ui->userdir ) + strlen( REPLACE_UNDO_DIR_NAME ) ;

  undo_dir = NEW_STRING(sz);  
  
  snprintf( undo_dir, sz+2 , "%s/%s", ui->userdir,  REPLACE_UNDO_DIR_NAME  );

  return undo_dir;
} // get_undo_dir


/*-------------------------------------------*/
/* save_entry (function)                     */
/*                                           */ 
/* DESCRIPTION                               */
/*      saves successful renames of files    */
/*      to an undo file                      */
/*                                           */           
/* INPUT                                     */
/*      cmd  char*:  the string to save      */
/*                                           */
/* OUTPUT                                    */
/*       none                                */
/*                                           */
/* NOTE                                      */
/*    output to file:  OLD<tab>NEW<\n>       */
/*                                           */
/*    default file:                          */
/*              $HOME/.replace/replace_undo) */
/*                                           */
void save_entry( char* cmd )
{
  char* undo_dir   = NULL;
  char* undo_path  = NULL;
  FILE* undo_file  = NULL;
  int   errsrv     = 0;
  bool  error_exit = false;

  errno = 0; /* clear it */  

  undo_dir = replace_undo_dir;   /* ref (replace.c) */
  undo_path = replace_undo_path; /* ref (replace.c) */

#ifdef DEBUG_PATH
  fprintf(stderr, "[save_entry] undo_dir = [%s] \n" , undo_dir );
  fprintf(stderr, "[save_entry] undo_path= [%s] \n" , undo_path );
#endif

  //mkdir
  if  ( mkdir( undo_dir, 0766 ) != 0 ) 
    {
      errsrv = errno;
      if ( (errno != EEXIST)  ) //dir already there
	{
	  fprintf(stderr, "mkdir %s Failed.\t %s\n", replace_undo_dir, strerror(errsrv) );	  
	  error_exit = true;
	  goto bye;
	}
    }

  /* save cwd */
  working_dir = NEW_STRING(1024); 
  working_dir = getcwd( working_dir, 1024 );

  //chdir
  if ( chdir( undo_dir ) == -1 )
    {
      errsrv = errno;    
      fprintf(stderr, "chdir(cmdpath)  %s Failed.\t%s\n", undo_path,  strerror(errsrv) );
      error_exit = true;
      goto bye;
    }
  
  /* truncate file on first open */
  if (first_open) 
    {
      if ( (undo_file = fopen( undo_path, "w")) == NULL )
 	{
	  errsrv = errno;
	  fprintf(stderr, "[utils.h:526] fopen %s Failed.\t%s\n", undo_path,  strerror(errsrv) );
	  error_exit = true;
	  goto bye;
	}
      
      first_open = false;
      fclose(undo_file);
      undo_file= NULL;
    }


  //cmdfile
   if ( (undo_file = fopen(undo_path , "a")) == NULL )    // open it for append   
  {
    errsrv = errno;
    fprintf(stderr, "fopen %s Failed.\t %s\n", undo_path, strerror(errsrv) );
    error_exit = true;
    goto bye;
  }
	 
   /* working dir wth op  with  on line below */
   if ( fprintf( undo_file ,"%s\n", cmd ) < 0)
    {
      errsrv = errno;
      fprintf(stderr, "command write to  %s Failed. (%s)\n", replace_undo_file, strerror(errsrv) );
      error_exit = true;
      goto bye;
    }

#ifdef DEBUG_PATH
  fprintf(stderr, "[utils.h:513] working_dir = [%s] \n" , working_dir );
#endif
	    
  //restore working directory
  if ( chdir( working_dir ) == -1 )
  {
    errsrv = errno;    
    fprintf(stderr, "chdir(working_dir)  %s Failed.\t %s\n", working_dir,  strerror(errsrv) );
    error_exit = true;
    goto bye;
  }


  /* clean up and/or abort program */
 bye:
  
  fflush (undo_file);
  fclose (undo_file); // frees memory fopen allocated
  
  FREE_STRING(working_dir);

if (error_exit)
  exit(errsrv);
 else
   return;
} // save_entry(cmd)



/*-------------------------------------------*/
/* get_entries (function)                    */
/*                                           */ 
/* DESCRIPTION                               */
/*      retrieve name changes from           */
/*      the undo file                        */
/*                                           */           
/* INPUT                                     */
/*       none                                */
/*                                           */
/* OUTPUT                                    */
/*       char**: array of strings            */
/*                                           */
/* NOTE                                      */
/*    file entry format   OLD<tab>NEW<\n>    */
/*                                           */
/*    default file:                          */
/*              $HOME/.replace/replace_undo) */
/*                                           */

char** get_entries( )
{
  char** entries = NULL;
  size_t num_entries = 0;

  char*  line    = NULL;
  size_t line_sz = 80;
  size_t lines   = 0;
  char   ch      = '\0';

  char* working_dir = NULL;
  char* undo_dir    = NULL;
  char* undo_path   = NULL;
  FILE* undo_file   = NULL;
  bool  error_exit  = false;
  int errsrv = 0;
  errno = 0;

  undo_dir  = replace_undo_dir;  /*ref (replace.c) */
  undo_path = replace_undo_path; /*ref (replace.c) */
  
  /* save cwd */
  working_dir = NEW_STRING(1024);
  working_dir = getcwd( working_dir, 1024 );
  
  //chdir
  if ( chdir( undo_dir ) == -1 )
  {
    errsrv = errno;    
    fprintf(stderr, "[get_entries] chdir(replace_undo_dir) %s Failed.\t%s\n", undo_dir,  strerror(errsrv) );
    error_exit = true;
    goto bye;
  }

  // undo_file is a FILE*
  if ( (undo_file = fopen(undo_path , "rt")) != NULL )    // open it for read  
    {
      /* count lines */
      while(!feof(undo_file))
	{
	  ch = fgetc(undo_file);
	  if(ch == '\n') lines++; 
	}
      
      fseek(undo_file, 0L, SEEK_SET); // rewind to beginning of file
    }
  else
    {
      errsrv = errno;
      fprintf(stderr, "fopen %s Failed.\t %s\n", undo_path, strerror(errsrv) );
      error_exit = true;
      goto bye;
    }
  
  
  line    = NEW_STRING(line_sz);  
  entries = NEW_STRING_ARRAY(lines);
  entries[lines] = NULL;

  while (fgets(line, line_sz, undo_file) != NULL)
    {
      
#ifdef DEBUG_UNDO
      fprintf(stderr, "[get_entries] line =\t%s\n" , line );       
#endif
      num_entries++;
      entries[num_entries-1] = strndup( line , line_sz ); 
      memset( line, 0, line_sz);
    }
  
  fclose(undo_file);  /* close the file prior to exiting the routine */
  
#ifdef DEBUG_UNDO
  do 
    {
      int d;
      for (d = 0; d < num_entries ; d++)
	fprintf(stderr, "[get_entries] entries[%d] =\t%s\n" , d, entries[d] );       
    } while(0);
  
#endif
  
  
  //restore working directory
  if ( chdir( working_dir ) == -1 )
    {
      errsrv = errno;    
      fprintf(stderr, "[get_entries] chdir to (%s) Failed.\n", working_dir);
      fprintf(stderr, "\t with error:\t%s ..exiting. \n", strerror(errsrv) );
      error_exit = true;
      goto bye;
    }
  
  
 bye:
  
  FREE_STRING(working_dir);
  FREE_STRING(line);  
  
  if (error_exit)
    {
      FREE_STRING_ARRAY(entries);
      exit(errsrv);
    }
  else
    {
      return entries;
    }
} // get_entries



/*-------------------------------------------*/
/* array_to_string (function)                */
/*                                           */ 
/* DESCRIPTION                               */
/*      convert array of strings to          */
/*      one long string separated by         */
/*      spaces                               */           
/* INPUT                                     */
/*       char**:  array of strings           */
/*                                           */
/* OUTPUT                                    */
/*       char*:  a string                    */
/*                                           */

char* array_to_string( char** arr )
{
  size_t i = 0;
  size_t total = 0;
  char* s = NULL;

  if (! arr ) return (char*)NULL;

  // count bytes
  while(arr[i])
    {
      total += (strlen(arr[i]) +1 );
      i++;
    }

  // make a char* big enough
  s = NEW_STRING(total);

  i = 0;
  while (arr[i] )
    {
      strncat ( s , arr[i] , strlen(arr[i]) );
      strncat ( s , " " , 1 );
      i++;
    }
  
  s[total] = '\0'; // end the string

  return s;

} //array_to_string


/*-------------------------------------------*/
/* string_to_array (function)                */
/*                                           */ 
/* DESCRIPTION                               */
/*      convert one long string              */
/*      separated by a delimiter             */
/*      to an array of strings               */          
/*                                           */           
/* INPUT                                     */
/*      _str   char*:  a string              */
/*      delim  char*:  a string              */
/*                                           */
/* OUTPUT                                    */
/*       char**:  array of  strings          */
/*                                           */

char** string_to_array( char* _str , const char* delim)
{
  size_t arr_sz  = 0;
  size_t str_sz  = 0;
  size_t current = 0;
  char*  token   = NULL;
  char*  str     = NULL;
  char** arr     = NULL;


  if (! _str) return (char**)NULL;

  str = strndup( _str , strlen(_str) );

  token = strtok (str, delim);  /* token => "words" */

  arr_sz = 1;

  while ( (token = strtok (NULL, delim)) != NULL )    /* token => "separated" */
    {
      arr_sz++;      
    }	
  
  //  make pointers
  arr = NEW_STRING_ARRAY( arr_sz);


  //start over 
  FREE_STRING(str);
  current = 0;

  str = strndup( _str , strlen(_str) );
  token = strtok (str, delim);  /* token => "words" */
  str_sz = strlen(token);
  arr[current] = strndup( token, str_sz );
  current = 1;

  while ( (token = strtok (NULL, delim)) != NULL )    /* token => "separated" */
    {
      str_sz = strlen(token);
      arr[current] = strndup( token, str_sz );
      strtok(arr[current], "\n"); // strip newline
      arr[current][str_sz] = '\0'; // null terminate explicitly
      current++;
    }	

  arr[current] = NULL;
 
  FREE_STRING(token);
  FREE_STRING(str);

  return arr;
} // string_to_array



/*-------------------------------------------*/
/* count_words (function)                    */
/*                                           */ 
/* DESCRIPTION                               */
/*         count words in a string           */
/*                                           */           
/* INPUT                                     */
/*      str   const char*:  a string         */
/*                                           */
/* OUTPUT                                    */
/*       size_t:  numner words counted       */
/*                                           */
/* NOTE:                                     */
/*     nullchar, space, tab, newline, and    */ 
/*     carriage return all work asdelimiters */
/*                                           */

size_t count_words(const char* str)
{
  size_t counted = 0; // result

  const char* it = str;
  bool inword = false;
  
  do 
    switch(*it) 
      {
      case '\0': 
      case ' ' : 
      case '\t': 
      case '\n': 
      case '\r': 
	if (inword) 
	  { 
	    inword = false; 
	    counted++; 
	  }
	break;

      default: 
	inword = true;
	
      } while(*it++);

  return counted;
}



#endif // UTILS_h

/*END*/




