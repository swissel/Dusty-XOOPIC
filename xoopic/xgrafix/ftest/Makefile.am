## ##########################################################################
##
## File:        Makefile.am
##
## Purpose:     Running "automake" at the top level creates Makefile.in
##              from this.
##
## ##########################################################################

EXTRA_DIST = test.f.in

bin_PROGRAMS = xftest
xftest_SOURCES = test.f extra.c
xftest_LDADD = -L../src -lXGF -lXGC @TCL_LIB@ -L@X11_LIBDIR@ -lX11 -L@XPM_LIBDIR@ @XPM_LIB@ @NET_LIBS@ @DYNLINK_LIB@ -lm
xftest_DEPENDENCIES = ../src/libXGF.a ../src/libXGC.a
