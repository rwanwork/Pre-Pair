##    Pre-Pair
##    Word-based Pre-processor for Re-Pair
##    Copyright (C) 2003, 2007 by Raymond Wan (rwan@kuicr.kyoto-u.ac.jp)
##
##    Version 1.0.1 -- 2007/04/02
##
##    This file is part of the Pre-Pair software.
##
##    Pre-Pair is free software; you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
##
##    Pre-Pair is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License along
##    with Pre-Pair; if not, write to the Free Software Foundation, Inc.,
##    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##

CC	= gcc
LIBS    = -lm
CFLAGS  = -Wno-long-long -pedantic -Wall -Wwrite-strings -Wcast-align \
          -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs   \
          -Wshadow -Winline -O3 $(EXTRA_CFLAGS)

COMMON_DIR	= ../common

############################################################################
##  Extra cflags for compile time.
EXTRA_CFLAGS = 
##  Perform time consuming counting of unique characters for 
##    statistics only:  -DCHARSTATS
##  Perform punctuation-aligned Re-Pair:  -DFLAG_WORDS

PROGRAMS	= stem prepair 

STEM_SRC	= main-stem.c ustring.c stem.c casefold.c wmalloc.c
STEM_OBJS	= ${STEM_SRC:.c=.o}

PREPAIR_SRC	= ustring.c stem.c casefold.c prepair.c word.c \
                  nonword.c fcode.c main-prepair.c wmalloc.c
PREPAIR_OBJS	=${PREPAIR_SRC:.c=.o}


############################################################################
# Rules
############################################################################

all:  $(PROGRAMS)

##  Not used.
link:
	rm -f common-def.h ustring.h ustring.c wmalloc.h wmalloc.c
	ln -s $(COMMON_DIR)/common-def.h common-def.h
	ln -s $(COMMON_DIR)/ustring.h ustring.h
	ln -s $(COMMON_DIR)/ustring.c ustring.c
	ln -s $(COMMON_DIR)/wmalloc.h wmalloc.h
	ln -s $(COMMON_DIR)/wmalloc.c wmalloc.c

stem:  $(STEM_OBJS)
	$(CC) $(CFLAGS) -o stem $(STEM_OBJS) $(LIBS)

prepair:  $(PREPAIR_OBJS)
	$(CC) $(CFLAGS) -o prepair $(PREPAIR_OBJS) $(LIBS)

############################################################################
# Rules
############################################################################

clean:
	rm -f *~ *.o 

clobber:
	make clean
	rm -f *.nw *.nwd *.sm *.wd *.ws *.cfm *.nws
	rm -f $(PROGRAMS)

