## ##########################################################################
##
## File:        Makefile.am
##
## Purpose:     Running "automake" at the top level creates Makefile.in
##              from this.
##
## ##########################################################################

EXTRA_DIST = test.inp

INCLUDES = -I@TCL_INCDIR@ -I@TK_INCDIR@ -I../src

bin_PROGRAMS = xtest
xtest_SOURCES = test.c
xtest_LDADD = -L../src -lXGC @TCL_LIB@ -L@X11_LIBDIR@ -lX11 -L@XPM_LIBDIR@ @XPM_LIB@ @NET_LIBS@ @DYNLINK_LIB@ -lm
xtest_DEPENDENCIES = ../src/libXGC.a
