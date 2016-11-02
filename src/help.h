/* help.h */

/* friendly messages */

/* AUTHOR:  Chris Reid <spikeysnack@gmail.com> */
/* LICENSE: Free for all purposes              */
/* COPYRIGHT: 2016- Chris Reid                 */


#ifndef _HELP_H_

#include <stdio.h>

/* external switch (replace.c */
extern bool verbose;

/* function prototypes */
void usage        ( FILE* fp );
void help         ( FILE* fp );
void version      ( FILE* fp );
void root_warning ( FILE* fp );

/*-------------------------------------------*/
/* usage  (function)                         */
/*                                           */
/* DESCRIPTION                               */
/*       A usage message to explain briefly  */
/*       how to use the program.             */
/*                                           */
/* INPUT                                     */
/*     none                                  */
/*                                           */
/* OUPUT                                     */
/*     none                                  */
/*                                           */

void usage( FILE* f )
{
  version(f);
  char* usage_str = 
  "usage: replace <string1>  <string2> <filename1> [ <filename2> ...]\n"
  "                  -a,        --all           replace all matches in each filename  \n"
  "                  -f         --force         do not ask Y/N before renaming files\n"
  "                  -h         --help          print this message  \n"
  "                  -n <int>   --num <int>     replace up to n matches per filename (default = 1) \n"
  "                  -t         --test          test mode -- do not actually change filenames \n"
  "                  -q         --quiet         rename files with no output \n"
  "                  -u         --undo          reverse the last run of replace \n" 
  "                  -v         --verbose       print out lots of things as they happen \n" 
  "                  -V         --version       print out version \n\n"
  "		     (hint:  use * for all files in dir) \n";

  fprintf( f , "%s\n" , usage_str );
}

/*-------------------------------------------*/
/* help (function)                           */
/*                                           */
/* DESCRIPTION                               */
/*       A brief message to demonstrate      */
/*       examples of calling the program.    */
/*       (prints usage first)                */
/*                                           */
/* INPUT                                     */
/*     none                                  */
/*                                           */
/* OUPUT                                     */
/*     none                                  */
/*                                           */

void help( FILE * f )
{
  printf("replace changes a matching string in all of a list of filenames.\n" ); 

  usage(f);

  char* example = 
    "Examples: \n"
    "replace  .txt .doc *       [ change suffix of all txt files]\n"
    "replace  2106  2016 *      [ fix typo in filenames ]\n"
    "replace  -t -v  LEROY_JENKINS   \"\" *.jpg      [ verbosely test results of cropping\n" 
    "                                                    an unwanted word from jpg files   ]\n";

  fprintf( f, "%s\n" , example);

}

/*-------------------------------------------*/
/* version (function)                        */
/*                                           */
/* DESCRIPTION                               */
/*           output version and              */ 
/*           date of last revision           */ 
/*                                           */
/* INPUT                                     */
/*     none                                  */
/*                                           */
/* OUPUT                                     */
/*     none                                  */
/*                                           */
/* NOTE                                      */
/*     relies on definitions in (replace.c)  */
/*                                           */

void version( FILE* f)
{
#ifndef REPLACE_VERSION
#error REPLACE_VERSION is not Defined yet
#endif
  fprintf( f , "%s version:\t %s  date:\t%s\n", "replace",  REPLACE_VERSION, REPLACE_DATE );
}

/*-------------------------------------------*/
/* root_warning (function)                   */
/*                                           */
/* DESCRIPTION                               */
/*           warn user if user is root       */
/*           or has super-user priveledge    */ 
/*                                           */
/* INPUT                                     */
/*     none                                  */
/*                                           */
/* OUPUT                                     */
/*     none                                  */
/*                                           */
/* NOTE                                      */
/*     ANSI DEFINED COLORS (default red)     */
/*                                           */

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void root_warning( FILE* fp)
{
  fprintf( fp, ANSI_COLOR_RED "[WARNING! -- super user mode.\n Renaming system files could destroy your system].\n");
  fprintf( fp, ANSI_COLOR_RESET );
}


#define _HELP_H_
#endif

/*END*/
