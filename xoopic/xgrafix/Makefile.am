## ##########################################################################
##
## File:	Makefile.am
##
## Purpose:	Run automake at the top level to make Makefile.in in all
##		subdirectories.
##
## Copyright 2000 Tech-X Coporation.  All rights reserved.
##
## ##########################################################################

SUBDIRS = config src ctest @FTEST_DIR@ xgmovie

EXTRA_DIST = aclocal.m4

depend:
	for i in $(SUBDIRS) ; do \
	  (cd $$i; \
           echo "Making depend in $$i"; \
           make depend) \
	done

crufix: linuxfix

linuxfix:
	for i in $(SUBDIRS) ; do \
	( cd $$i; \
	  echo "fixing the Makefile in $$i"; \
	  mv Makefile Makefile.old ;\
	  cat Makefile.old | sed 's/ cru / /' > Makefile);\
	  mv Makefile.in Makefile.in.old ;\
	  cat Makefile.in.old | sed 's/ cru / /' > Makefile.in) \
	done

automake:
	./config/automake.sh
