Word-based pre-processor for Re-Pair
====================================


Introduction
------------

Pre-Pair is the name of the algorithm and the software which is a 
pre-processor for word-based Re-Pair.  Pre-Pair separates a document into 
a word sequence and a non-word sequence, as well as their corresponding 
lexicons.  Then, Re-Pair can be used to compress the word sequence for the 
purpose of phrase browsing.  Stemming and case-folding are reversible 
through the use of modifiers.  Modifiers are integers which indicate 
either which rules were applied for stemming or which letters were folded. 
 Stemming was done using the Porter Stemming algorithm.

The length of a word or a non-word can be at most 16 characters, due to a 
limitation imposed by front coding.  Front coding encodes a word in a 
sorted lexicon by using the matching prefix of the word prior to it.

This source code includes two programs.  prepair is the Pre-Pair software. 
 stem is a test program which can be used to stem a single word or a list 
of words via stdin.  In the first case, the stemmed word and the modifiers 
are shown.  In the second case, something is output only if there was an 
error in the stemming / unstemming process.


About The Source Code
---------------------

The source code is written in C and compiled using v4.1.2 of gcc for 
Debian v4.0 (etch).  It has been compiled and successfully run on both an 
Intel Pentium 4 CPU (32-bit) and an Intel Core 2 Duo CPU (64-bit).  
However, it takes no advantages of the 64-bit architecture.

This main purpose of this software was for research.  Therefore, 
additional checks and extraneous information has been added into the 
source code which, if removed, may have a small impact on the execution 
time.


Compiling
---------

The archive includes a Makefile.  Simply type "make" to build the source 
code.  There is no "configure" script for determining the system set-up, 
though this may happen with a future release.

To encode a file, run it as:  "prepair -i <base filename> -c -s <file".  
Note that the argument after -i is the base filename used to name the 
output files.  An input file is required via stdin.  The -c and -s options 
are used to case-fold and stem words, respectively.  Decoding is done 
using the command "prepair -d -i <base filename>".

Run "prepair" without any arguments to see the list of options.


Citing
------

Pre-Pair is described in chapter 4 of:
  - R. Wan. "Browsing and Searching Compressed Documents". PhD thesis,
    University of Melbourne, Australia, December 2003.

Of course, providing the web site where you got this software (see below) 
is also possible.


Contact
-------

This software was implemented by me (Raymond Wan) for my PhD thesis at the University of Melbourne (under the supervision of [Prof. Alistair Moffat](http://people.eng.unimelb.edu.au/ammoffat/).  My contact details:

     E-mail:  rwan.work@gmail.com 

My homepage is [here](http://www.rwanwork.info/).

The latest version of Re-Pair can be downloaded from [GitHub](https://github.com/rwanwork/Pre-Pair).

If you have any information about bugs, suggestions for the documentation or just have some general comments, feel free to contact me via e-mail or GitHub.


Version
-------

Changes to this software are recorded in the file CHANGES up until April 2007.  Since moving the source code to GitHub on January 31, 2021, any changes are recorded in the repository's history.


Copyright and License
---------------------

Copyright (C) 2003-2021 by Raymond Wan (rwan.work@gmail.com)

Pre-Pair is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.  Please see the accompanying file, LICENSE for further details.


About This Repository
---------------------

This GitHub repository was created from the original tarball on my homepage a few years ago.  Initially, it was identical to the version as used in my PhD thesis.  Hopefully, it will be easier for me to maintain in GitHub.


    Raymond Wan
    January 31, 2021



