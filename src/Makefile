#!/bin/make

# AUTHOR:  Chris Reid <spikeysnack@gmail.com> #
# LICENSE: Free for all purposes              #
# COPYRIGHT: 2016- Chris Reid                 #


# modification allowed 
# but original attribution stays, add your name to any mods 
# no guarantees implied or inferred
# standard C 
# to build: "just type make"

#SHELL = /bin/sh
#prefix= $HOME  # set to home dir if you can't use sudo
#prefix = /usr/local
#bindir = $(prefix)/bin
#mandir = $(prefix)/share/man
#manext = 1
#docdir = $(prefix)/share/doc/replace

#INSTALL = $(shell which install)
#INSTALL_PROGRAM = $(INSTALL)

include env.mk

# CC = gcc

# OPTIMIZE_FLAGS = -mtune=native  -foptimize-strlen 

# BASE_CFLAGS = -fstrict-aliasing -fpic  -Wno-unused-function 

# DEBUG_CFLAGS = $(BASE_CFLAGS) -O0 -DDEBUG -ggdb3  -Wall -Werror  -fno-omit-frame-pointer 

# DEBUGALL = $(DEBUG_FLAGS) -DEBUG_MALLOC  -DDEBUG_ARGS -DDEBUG_OPTS -DDEBUG_REPLACE -DDEBUG_PATH 


all:	replace

#do precurser setup 
#env.mk:
#	bash setup.sh

replace:	replace.c
	$(CC) $(OPTIMIZE_FLAGS) $(BASE_CFLAGS) $(DEFINES) -o replace replace.c $(LIBS)


# debug symbols
debug:	replace.c
	$(CC) $(BASE_CFLAGS) $(DEBUG_CFLAGS) $(DEFINES) -o replace replace.c $(LIBS)


# more debugging and verboseness
debugall:	replace.c
	$(CC) $(BASE_CFLAGS) $(DEBUG_CFLAGS) $(DEBUGALL) $(DEFINES) -o replace replace.c  $(LIBS) 

.PHONY:	clean 

# remove intermediate files and binary
clean:
	rm -f *.o *~ env.mk replace 











