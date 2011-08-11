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

AR = @AR@
F77 = @F77@
lib_LIBRARIES = libXGC250.a  libXGC250mini.a  libXGF250.a
libXGC250_a_AR = $(AR)
libXGC250mini_a_AR = $(AR)
libXGF250_a_AR = $(AR)

libXGC250_a_SOURCES = xgpaint2d.c xgprint.c xgsetup.c xgcommands.c xginitvar.c xgpaint3d.c xgprintgif.c xgcrosshair.c xgmovie.c xgpaintvec.c xgreadrit.c xginitc.c xgpixmap.c xgrescale.c

libXGC250mini_a_SOURCES = xgmovie.c xgreadrit.c xgnox.c

libXGF250_a_SOURCES = xgfinterface.f xginitf.c
 
lib: $(lib_LIBRARIES)
	$(AR) ../lib/$(lib_LIBRARIES) $(libotools_a_OBJECTS)

xgfinterface.o: xgfinterface.f
	@if test -n "$(F77)"; then \
	  $(F77) -c -o xgfinterface.o $(srcdir)/xgfinterface.f; \
	else	\
	  touch xgfinterface.o ; \
	  echo "Warning: unusable fortran library!"; \
	fi

INCLUDES = -I@TCL_INCDIR@ -I@TK_INCDIR@ -DXGTCL=\"$(prefix)/lib\" -I@X11_INCDIR@

bin_PROGRAMS = ps6to1

ps6to1_SOURCES = ps6to1.c

include_HEADERS = xgscalar.h xgmini.h xgrafix.h xgio.h

EXTRA_INSTALL_LIBS = xgdialogs250.tcl xgrafix.ico xginit250.tcl xgsetup250.tcl

all-local:
	rm -f libXGC.a
	ln -s libXGC250.a libXGC.a
	rm -f libXGCmini.a
	ln -s libXGC250mini.a libXGCmini.a
	rm -f libXGF.a
	ln -s libXGF250.a libXGF.a

install-data-local:
	rm -f $(prefix)/lib/libXGC.a
	ln -s $(prefix)/lib/libXGC250.a $(prefix)/lib/libXGC.a
	rm -f $(prefix)/lib/libXGCmini.a
	ln -s $(prefix)/lib/libXGC250mini.a $(prefix)/lib/libXGCmini.a
	rm -f $(prefix)/lib/libXGF.a
	ln -s $(prefix)/lib/libXGF250.a $(prefix)/lib/libXGF.a
	@list='$(EXTRA_INSTALL_LIBS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

EXTRA_DIST = xgdialogs250.tcl xgrafix.ico xginit250.tcl xgsetup250.tcl xgscalar.h.in xgfinterface.f.in  xgdatamacros.h xgminint.h xgprintgif.h xgrafixint.h
