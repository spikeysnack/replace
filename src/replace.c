/* replace.c */

/* removes a leading substring from a list of files */

/* AUTHOR:  Chris Reid <spikeysnack@gmail.com> */
/* LICENSE: Free for all purposes              */
/* COPYRIGHT: 2015- Chris Reid                 */

/* The main workhorse functions                */  
/* are in replace.h and utils.h.               */


#define _GNU_SOURCE

#define REPLACE_VERSION          "1.5"
#define REPLACE_DATE             "7 Nov 2015"

#define REPLACE_UNDO_DIR_NAME         ".replace"
#define REPLACE_UNDO_FILE_NAME        "replace_undo"



/* C library functions and constants */
#include "gc_malloc.h"
#include <stdlib.h>  /* free, calloc, exit              */

#include <stdio.h>   /* printf , fprintf,               */
#include <limits.h>  /* constants                       */
#include <locale.h>  /* localization                    */
#include <string.h>  /* strlen, strcmp, strstr, memmove */  
#include <ctype.h>   /* isspace , etc                   */
#include <unistd.h>  /* sizes                           */

#include <sys/types.h>  /* C types                      */ 


/* some C suites do not define bool */
#ifndef bool
typedef enum {false, true} bool;
#endif


/* external variable option index  <unistd.h>  */
extern int optind; 


/* options and their defaults */ 
size_t num_matches  = 1;
bool all_matches    = false;
bool force_replace  = false;
bool quiet_replace  = false;
bool test_mode      = false;
bool verbose        = false;
bool undo           = false;
bool undo_available = false;
bool answer         = false;


/* a place to organize arguments  */
struct  arg_struct
{
  int optind;
  char* match;
  char* replace;
  char** files;
  size_t num_files;
};


/* default undo file is $HOME/.replace/replace_undo */

char* replace_undo_home;
char* replace_undo_dir;
char* replace_undo_file;
char* replace_undo_path;


/* the replace code is in replace.h */
#include "replace.h"


/* function declarations */
void options ( int argc, char** argv);
size_t do_replace( int nonoptargc, char* match, char* replace, char** nonoptargs );
size_t do_undo(void);

bool set_and_check_args( int argc, char** argv,  struct arg_struct* as );
bool set_undo_file(void);
int main ( int argc, char** argv, char** env);



/* main driver */

int main ( int argc, char** argv, char** env)
{

  struct arg_struct* the_args = NULL;
  //  size_t arg_sz        = 0;
  size_t files_renamed = 0;
  int    return_code   = 0; 

#ifdef BOEHM_GC
  GC_INIT();
#endif


  if (argc  < 2 )  /* gotta have at least 2 arguments */
    {
      usage();
      return_code = EXIT_FAILURE;
      goto bye;
    }

  setlocale(LC_CTYPE, ""); /* we know the characters if we set baseline locale */

  if (geteuid() == 0) root_warning(); /* root is dangerous */

  /* if we can't set an undo file we are broken */

  /*clear out garbage */
  replace_undo_home = NULL;
  replace_undo_dir  = NULL;
  replace_undo_file = NULL;
  replace_undo_path = NULL;

  if (! set_undo_file() ) 
    {
      fprintf( stderr, "ERROR setting undo file\n" );
      return_code = EXIT_FAILURE;
      goto bye;
    }


  /* not in final compilation */  
#ifdef DEBUG_ARGS
  do{
    fprintf(stderr, "argc:%d \n", argc);
    int a;
    for (a = 0; a < argc ; a++)
      fprintf(stderr, "argv[%d]:\t %s \n", a, argv[a]);
    
    fprintf(stderr, "\n");
    fflush(stderr);
  }while(0);
#endif


  options( argc, argv );   /* get the command options */


  if (undo)   /* check if this is an undo op -- if so get filenames from the undo file */
    {
      files_renamed = do_undo();
      return_code = EXIT_SUCCESS;
      goto bye;    
    }


  /* make mem space for arg struct*/

  the_args = (struct arg_struct*) calloc(1, sizeof(struct arg_struct) );

  if (! the_args)
    {
      fprintf(stderr, "[the_args] calloc failed\n" );
      return_code = EXIT_FAILURE;                                                                                                                                  goto bye;
    }



  if ( ! set_and_check_args(argc, argv, the_args) )   /* check the arguments and if good do the renaming */
    {
      if (verbose) fprintf(stderr , "check_args failed\n");
      return_code = EXIT_FAILURE;
    }
  else
    {
      files_renamed = do_replace( the_args->num_files, the_args->match, the_args->replace, the_args->files ); /* GO! */
    }

  

  /* we have reached the end */
 bye:

  if (verbose ) 
    {  
      if ( test_mode)
	fprintf( stdout , "test mode -- no files renamed. -- would have " );
      fprintf( stdout , "renamed %zu files.\n", files_renamed );
    }

  replace_undo_home = NULL; 
  replace_undo_dir  = NULL;
  replace_undo_file = NULL;
  replace_undo_path = NULL;
  the_args = NULL;


#ifdef BOEHM_GC
  GC_gcollect();
#endif

  return return_code; /* YAY! bye */

} /* main */



/*----------------------------------------------------*/
/* set_and_check_args                                 */
/* DESCRIPTION                                        */
/*       check the arguments and populate the struct  */
/*                                                    */
/* INPUT                                              */
/*     argc    int: the argument count                */
/*     argv char**:  pointer to arrray o of strings   */ 
/*                   the 2 matching strings and       */
/*                   the filenames to operator on     */
/*                                                    */
/*     as arg_struct*: pointer to struct arg_struct   */
/*                    containing separated argv args  */
/* OUTPUT                                             */
/*    bool:   status of args and as struct set        */
/*                                                    */

bool set_and_check_args( int argc, char** argv,  struct arg_struct* as )
{
  size_t m_sz         = 0;
  size_t r_sz         = 0;
  int    missing_args = 0;
  bool   set          = false;
  bool   checked      = false;
  


#ifdef DEBUG_OPTS
  fprintf(stderr, "optind:\t %d\n", optind); 
  fprintf(stderr, "argv[0]:\t %s\n", argv[0] ); 
  fprintf(stderr, "argv[1]:\t %s\n", argv[1] ); 
#endif
  

  as->optind = optind;   /* optind is the number of options set by getopt */

  if ( optind > 0 )      /* calculate what goes where among the arguments */
    {
      as->match = argv[optind];
      as->replace = argv[optind+1];
      as->files = &argv[optind+2] ;
      as->num_files = argc - ( optind + 2 );
    }
  else                   /* no options given */
    { 
      as->match =       argv[1];
      as->replace =     argv[2];
      as->files = &argv[3];
      as->num_files = argc - 2;
    }
 
  set = true; 

  if  ( (!as->num_files) || (! as->match ) || (! as->replace ) ) 
    set = false;
  else 
    {
                         /* check if there is duplication of match an replace */
      m_sz = strlen(as->match);
      r_sz = strlen(as->replace);
      
      if ( m_sz == r_sz )
	if ( strncmp( as->match , as->replace , m_sz ) == 0 )
	  {
	    fprintf(stderr , "ERROR  match and replacement strings are the same. \nNothing to do. .. exiting \n");
	    set = false;
	  }
      
    } // else
  

#ifdef DEBUG_OPTS
  fprintf(stderr, "[check_args] as->files[0]:\t %s\nas->num_files = %zu\n match = %s\n replace= %s \n", 
	  as->files[0], as->num_files, as->match, as->replace );      
  fflush(stderr);
#endif

  missing_args += (as->match)? 0:1 ;
  missing_args += (as->replace)? 0:4;
  missing_args += (as->files[0])? 0:8;

  if (! quiet_replace )                      
    {
      switch(missing_args)
	{
	case 0:  // this is what we want
	  break;

	case 1:
	  fprintf(stderr , "need 3 more arguments --  <matchstring> <replacestring>  <filename(s)> \n");
	  break;
	  
	case 4:
	  fprintf(stderr , "need 2 more arguments -- <replacestring>  <filename(s)> \n");
	  break;
	  
	case 8:
	  fprintf(stderr , "need 1 more argument --  <filename(s)< \n");
	  break;
	  
	default:
	  fprintf(stderr , "missing_args is  %d. This is weird. you should not see this.\n", missing_args);
	  exit(EXIT_FAILURE);
	  break;
	}
	
    } // if(! quiet_replace)
  
  if (!missing_args) checked = true;
  
  return (set && checked);

}// check_args


/*----------------------------------------------------*/
/* set_undo_file                                      */
/* DESCRIPTION                                        */
/*       get user info and calculate paths            */
/*       of user's undo file                          */
/* INPUT                                              */
/*     none                                           */
/*                                                    */
/* OUTPUT                                             */
/*    bool:  completed ok                             */
/*                                                    */
/* NOTE                                               */
/*     sets global variables:                         */
/*     replace_undo_home, replace_undo_dir            */ 
/*     replace_undo_file, replace_undo_path           */
/*                                                    */

bool set_undo_file(void)
{
  struct user_info* uinfo;
  size_t sz   = 0;

  uinfo = get_user_info();
  
  replace_undo_home = uinfo->userdir;

  replace_undo_dir = get_undo_dir(uinfo);
  
  replace_undo_file = REPLACE_UNDO_FILE_NAME;

  sz = strlen(replace_undo_home) + strlen(replace_undo_dir) + strlen(replace_undo_file);

  replace_undo_path = NEW_STRING(sz);

  snprintf( replace_undo_path, sz+2 , "%s/%s",  replace_undo_dir, replace_undo_file );

#ifdef DEBUG_PATH
  fprintf(stderr, "[set_undo_file] replace_undo_home  [%s] \n", replace_undo_home );
  fprintf(stderr, "[set_undo_file] replace_undo_dir   [%s] \n", replace_undo_dir  );
  fprintf(stderr, "[set_undo_file] replace_undo_file  [%s] \n", replace_undo_file );
  fprintf(stderr, "[set_undo_file] replace_undo_path  [%s] \n", replace_undo_path );
#endif

#ifndef BOEHM_GC
  free_user_info(uinfo);
#else
  free(uinfo);
#endif
  
  if ( ! replace_undo_path ) 
    return false;

  return true;
}




/*END*/

