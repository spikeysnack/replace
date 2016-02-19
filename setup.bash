#!/bin/bash

FIND=$(which find)
LIB_DIRS="/usr/lib /usr/local/lib"
HEADER_DIRS="/usr/include /usr/local/include"

function finder()
{
    local search_term
    local found
   
    search_term="${1}"
    shift
    
    for D in "$@"
    do
	found=$(${FIND} $D -name "${search_term}" )

	if [[ $found ]] ; then
	    break
	fi	
    done
    echo "$found"
}

# creates a few defaults for the Makefile
if [ -e "src/env.mk" ] ; then
    exit 0
else
echo "creating src/env.mk"
fi


# if we can find the Boehm garbage collecter, use it
BOEHM_GC_HEADER=$(finder "gc.h"  "${HEADER_DIRS}" 2>/dev/null)
BOEHM_GC_LIB=$(finder "libgc.so" "${LIB_DIRS}" 2>/dev/null)


# clear file
echo "" > env.mk

echo "# env.mk" >> env.mk
echo "# this file is included in Makefile" >> env.mk

if [[ $BOEHM_GC_HEADER ]]; then

    echo "DEFINES += -D BOEHM_GC" >> env.mk
fi


if [[ $BOEHM_GC_LIB ]]; then

    echo "LIBS += -lgc" >> env.mk
fi

echo "SETUP=1" >> env.mk

mv env.mk src/

echo "Setup complete. running make."

