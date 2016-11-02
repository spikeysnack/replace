/* bad_filenames.h */

/* AUTHOR:  Chris Reid <spikeysnack@gmail.com> */
/* LICENSE: Free for all purposes              */
/* COPYRIGHT: 2016- Chris Reid                 */


#ifndef BAD_FILENAMES_H
#define BAD_FILENAMES_H

/*--------------------------------------------------*/
/* invalid chars   (strings)                        */
/*                                                  */
/* DESCRIPTION                                      */
/*       A set of strings containing bad characters */
/*       and bad combinations of characters that    */
/*       make filenames problematic.                */
/*                                                  */
/* NOTE                                             */
/*     For simplicity the strings are stand-alone   */ 
/*     and global. This list can be added to by the */
/*     developer as needed.                         */
/*                                                  */


/* These should not be anywhere in the file name */
const char*   invalid_chars = "/><|:&";

/* the empty string is a zero length string */
const char*   emptystring = ""; 

/* the empty string is also a  zero length string */
const char*   nullstring = "\0"; 

/* Double spaces are 99.99% a typo */
const char*   dbl_space = "  ";


/* These are special symbols for the current and parent dir */
const char*   dot = ".";
const char*   dotdot = "..";


/* Files should not start with these characters */
const char*   other_bad = "-*!?;^+={}\"\'\\,"; 



/* add your bad chars or strings here */

#endif


/*END*/

