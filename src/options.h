/* options.h */

/* 
 * getopt options 
 * "man 3 getopt" 
*/

/* AUTHOR:  Chris Reid <spikeysnack@gmail.com> */
/* LICENSE: Free for all purposes              */
/* COPYRIGHT: 2016- Chris Reid                 */


#ifndef _OPTIONS_H_

#include <unistd.h>
#include <getopt.h>
#include "help.h"


/* external variables  (replace.c)*/
extern bool all_matches;
extern bool force_replace;
extern bool quiet_replace;
extern bool test_mode;
extern bool verbose;
extern bool undo;

/*-----------------------------------------------*/
/*  options  (function)                          */
/*                                               */
/*  DESCRIPTION                                  */
/*       parse command line options              */
/*       and set operational flags               */
/*                                               */
/*  INPUT                                        */
/*       argc    int:   argument count           */
/*       argv char**:   array of strings         */
/*                                               */
/*  OUTPUT                                       */
/*       none                                    */
/*                                               */
/*  NOTE:                                        */
/*       the following options are recognized:   */
/*                                               */
/*          (a)ll                                */
/*          (f)orce                              */
/*          (h)elp                               */
/*          (n)um : int                          */
/*          (q)uiet                              */
/*          (t)est                               */
/*          (v)erbose                            */
/*          (V)ersion                            */
/*                                               */

void options ( int argc, char** argv)
{
  int opt;
  opt = 0;

  /* note:  the options -n and --num expect an integer as an argument  */

  static struct option long_options[] = {
    {"all",      no_argument,       0,  'a' },
    {"force",    no_argument,       0,  'f' },
    {"help",     no_argument,       0,  'h' },
    {"num",      required_argument, 0,  'n' },
    {"quiet",    no_argument,       0,  'q' },
    {"test",     no_argument,       0,  't' },
    {"undo",     no_argument,       0,  'u' },
    {"verbose",  no_argument,       0,  'v' },
    {"version",  no_argument,       0,  'V' },
    {0,           0,                0,  0   },
  };

  /* see  <getopt.h>   */ 
  while ((opt = getopt_long(argc, argv, "afhn:qtuvV", long_options, NULL)) != -1) 
    {
      switch (opt) {
	
      case 'a':
	all_matches = true;
	break;
	
      case 'f':
	force_replace = true;
	break;

      case 'h':
	help(stdout);
	exit(EXIT_SUCCESS);
	break;

      case 'n':
	num_matches = atoi (optarg );
	break;
	
      case 'q':
	quiet_replace = true;
	break;

      case 't':
	test_mode = true;
	break;

      case 'u':
	undo = true;
	break;

      case 'v':
	verbose = true;
	break;

      case 'V':
	version(stdout);
	exit(EXIT_SUCCESS);
	break;
	
      default: /* '?' */
	usage(stderr);
	exit(EXIT_FAILURE);
	
      }// switch(opt)
      
    }//while(opt)
  
}/* options */


#define _OPTIONS_H_
#endif

/*END*/
