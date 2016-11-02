![crop](doc/replace.png?raw=true  "replace")

##replace

###DESCRIPTION
    Rename a list of filenames based upon a matching string 
    and a replacement string.


####author      : Chris Reid

####category    : batch renaming

####copyright   : Copyright 2016

####country     : United States of America

####credits     : [Python Software Foundation, Free Software Foundation ]

####email       : spikeysnack@gmail.com

####file        : replace

####license     : Free for all non-commercial purposes. 
#####              Modifications allowed but original attribution must be included. 
#####              See (http://creativecommons.org/licenses/by-nc/4.0/)

####maintainer  : chris Reid

####modified    : 01 Nov 2016

####version     : 1.4

####status      : Release Candidate


###QUICK INSTALL:
      make
      sudo make install

	The default installation is: 
    		replace          /usr/local/bin
    		replace.1        /usr/local/share/man/man1/replace.1
    

###EXPLANATION 
     
This utility does a chore that can be done with the traditional unix tools, 
but then you have to know *sed* regular expressions, and that is a chore 
within itself sometimes.

replace simply searches a list of filenames given
to it, matches a string in their names, replaces it 
with a replacement string, and renames the files accordingly.

####features
	
	* a test mode to make sure the replacement is warranted as typed.

	* protections and warnings agains super-use (use by root or sudo).

	* basic precautions 

            * like not creating illegal filenames

	    * not overwriting existing files

	    * avoiding problematic file names such as 
		leading dashes 
		leading slashes 
		punctuation marks 
		double spaces

	    * It will also not create the files "." and ".." , 
		as these are aliase for the current 
		and parent directory to the current working directory 
	      	in unix-like operating systems. 
	
	* Directory names, directories being files themselves, are valid input to replace. 
  

###INTERACTIVE

Interactive mode is the safest if not the most efficient way to use replace. 

	It asks the user for a yes or no before each file is renamed.

	Force mode (-f) goes ahead without user interaction. 
	It is quite convenient and fast ,but should not be used thoughtlessly or
	serious messing-up of directories full of files may result. 

###UNDO
One level of undo is implemented.

	when run an undo file is created that can reverse the last run of the program.
	The file is placed in the users home directory in the subdirectory [.replace].
	The default undo file name is replace_undo. ($HOME/.replace/replace_undo).

	It is a plain text file with file name pairs, one pair per line, of
	the original file name and the new one. Upon an undo operation, each
	line is taken up and the file is renamed from its new name back to 
	its original name in a kind of short-cut operation without calculation.
 
	Quick undo is not to be relied on,but it could save some work 
	if and when an oops momemt happens.

	Test mode is the recommended way to see what would happen.Please.


###USAGE
The most common use for replace would be for changing the name of
a group of files in a directory, for some reason. 


###Example

	change all .doc files to .txt because they are actually plain text files.

	        $ replace  .doc .txt *
		   
		      (The shell expands the asterisk to all files in the dir, and those 
		      not matching are ignored. if there was already a .txt file in the directory
		      that would be overwriten, the program halts before that happens.)

		      (Please see the man page for replace(1) for some more examples)

###NOTE
It is a straightforward text substitution that operates on lists of filenames,
with a few restrictions built in to stop bad things from happening, somewhat
simpler than the  regular expression syntax of sed and the rename utility, 
and less dangerous than the tr utility.


