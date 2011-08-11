## ##########################################################################
##
## File:        Makefile.am
##
## Purpose:     Running "automake" at the top level creates Makefile.in
##              from this.
##
## ##########################################################################

# Needed for later use
OBJEXT          = @OBJEXT@

INCLUDES = -I@TCL_INCDIR@ -I../src -I@X11_INCDIR@

bin_PROGRAMS = xgmovie xgmreduce
xgmovie_SOURCES = xgmovie.cpp
xgmovie_LDADD = -L../src -lXGC  @TCL_LIB@ -L@X11_LIBDIR@ -lX11 -L@XPM_LIBDIR@ @XPM_LIB@ @NET_LIBS@ @DYNLINK_LIB@ -lm
xgmovie_DEPENDENCIES = ../src/libXGC.a
xgmreduce_SOURCES = xgmreduce.cpp
xgmreduce_LDADD = -L../src -lXGC  @TCL_LIB@ -L@X11_LIBDIR@ -lX11 -L@XPM_LIBDIR@ @XPM_LIB@ @NET_LIBS@ @DYNLINK_LIB@ -lm
xgmreduce_DEPENDENCIES = ../src/libXGC.a
EXTRA_DIST = list.h xgmovie.h
