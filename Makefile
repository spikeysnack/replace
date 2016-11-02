#!/bin/make

# AUTHOR:  Chris Reid <spikeysnack@gmail.com> #
# LICENSE: Free for all purposes              #
# COPYRIGHT: 2016- Chris Reid                 #


# modification allowed 
# but original attribution stays, add your name to any mods 
# no guarantees implied or inferred
# standard C 
# to build: "just type make"

SHELL = /bin/sh

#prefix= $HOME  # set to home dir if you can't use sudo
prefix = /usr/local
bindir = $(prefix)/bin
mandir = $(prefix)/share/man
manext = 1
docdir = $(prefix)/share/doc/replace
srcdir = src
INSTALL = $(shell which install)
INSTALL_PROGRAM = $(INSTALL)

CC = gcc

OPTIMIZE_FLAGS = -mtune=native  -foptimize-strlen 

BASE_CFLAGS = -fstrict-aliasing -fpic  -Wno-unused-function 

DEBUG_CFLAGS = $(BASE_CFLAGS) -O0 -DDEBUG -ggdb3  -Wall -Werror  -fno-omit-frame-pointer 

DEBUGALL = $(DEBUG_FLAGS) -DEBUG_MALLOC  -DDEBUG_ARGS -DDEBUG_OPTS -DDEBUG_REPLACE -DDEBUG_PATH 

#mostly a check for boehm-gc
ifneq ( $(wildcard "env.mk") , )
include env.mk
else
SETUP=$(shell bash setup.bash)
include env.mk
endif

export 

.PHONY:	setup env.mk all replace debug debugall clean dist test install install-bin install-man install-doc uninstall 

all:	replace

#do precurser setup 
setup:	setup.bash
	@bash setup.bash


env.mk:	setup

replace:	env.mk 
	make -C src replace

debug:	
	make -C src debug

# more debugging and verboseness
debugall:
	make -C src debugall 

# remove intermediate files and binary
clean:
	make -C src clean
	@rm -f *~ 

distclean:	clean
	@rm -f *~ env.mk .setup

# make tarball for distribution
dist:	distclean
	cd .. ; tar czf replace-1.6.tgz --exclude devel replace/ ; mv replace-1.6.tgz replace/ 

test:	replace
	$(MAKE) -C tests test

install:	install-bin  install-man install-doc
	@sensible-browser "file:///usr/local/share/doc/replace/replace.html"

install-bin:	replace
	$(INSTALL_PROGRAM) --mode=0755 src/replace $(bindir)/replace

install-man:	replace doc/replace.1

	$(INSTALL_PROGRAM) --mode=0644 doc/replace.1 $(mandir)/man$(manext)/replace.1

install-doc:	replace
	$(INSTALL_PROGRAM) -d -g users --mode=0755 $(docdir)/
	cp -a doc/* $(docdir)/ 


uninstall:	
	rm  $(bindir)/replace 
	rm  $(mandir)/replace.1 
	rm  -rf $(docdir) 











