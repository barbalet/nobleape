#!/bin/bash
#	test_object_string.sh
#
#	=============================================================
#
#   Copyright 1996-2019 Tom Barbalet. All rights reserved.
#
#   Permission is hereby granted, free of charge, to any person
#   obtaining a copy of this software and associated documentation
#   files (the "Software"), to deal in the Software without
#   restriction, including without limitation the rights to use,
#   copy, modify, merge, publish, distribute, sublicense, and/or
#   sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following
#   conditions:
#
#   The above copyright notice and this permission notice shall be
#	included in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#   OTHER DEALINGS IN THE SOFTWARE.
#
#   This software and Noble Ape are a continuing work of Tom Barbalet,
#   begun on 13 June 1996. No apes or cats were harmed in the writing
#   of this software.

SOURCEDIR=..

if [ $# -ge 1 -a "$1" == "--debug" ]
then
    CFLAGS=-g
else
    CFLAGS=-O2 
fi

gcc ${CFLAGS} -c $SOURCEDIR/noble/graph.c -o graph.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/vect.c -o vect.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/memory.c -o memory.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/io.c -o io.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/file.c -o file.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/console.c -o console.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/math.c -o math.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/parse.c -o parse.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/interpret.c -o interpret.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/land.c -o land.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/object.c -o object.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/audio.c -o audio.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/territory.c -o territory.o
gcc ${CFLAGS} -c $SOURCEDIR/noble/tile.c -o tile.o

gcc ${CFLAGS} -c test_object_string.c -o test_object_string.o
gcc ${CFLAGS} -I/usr/include -o test_object_string *.o -lz -lm -lpthread

./test_object_string

rm test_object_string

rm *.o
