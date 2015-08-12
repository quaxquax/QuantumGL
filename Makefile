all:
	echo Sorry, just saying make isn't enough.
	echo make parser      # for just building the parser
	echo make -f Makefile.x11     # to build the X11 version
	echo make -f Makefile.mingw   # to cross-compile to windows
	echo ... or (on Mac OS X) double-click QuantumGL.pbproj :-)

parser:	quantum.tab.cp lex.yy.cp quantum.tab.h
		
quantum.tab.h quantum.tab.cp: quantum.y
		bison -d quantum.y
		mv quantum.tab.c quantum.tab.cp

lex.yy.cp: quantum.l
		flex quantum.l
		mv lex.yy.c lex.yy.cp
