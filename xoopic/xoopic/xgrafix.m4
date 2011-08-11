dnl ######################################################################
dnl
dnl find XGRAFIX and setup flags
dnl
dnl ######################################################################
if test "$NOX" = "1"; then
  XGRAFIX_LIB_NAME="XGC250mini"
else
  XGRAFIX_LIB_NAME="XGC250"
fi
XGRAFIX_LIB=lib${XGRAFIX_LIB_NAME}.a

dnl
dnl Find XGRAFIX_LIBDIR
dnl
AC_ARG_WITH(XGRAFIX-lib,[  --with-XGRAFIX-lib=<XGRAFIX_LIBDIR>	location of XGRAFIX lib],
	XGRAFIX_LIBDIR="$withval")

dnl try to find the location of the library.
if test -z "$XGRAFIX_LIBDIR"; then
   XGRAFIX_LIBPATH=$HOME/$host/lib:$HOME/xgrafix/lib:/usr/local/xgrafix/lib:/usr/X11R6/lib:/usr/openwin/lib:/usr/lib:/usr/local/lib:/lib:/usr/X11/lib:/usr/local/lib/xgrafix:$PATH
   AC_PATH_PROGS(XGRAFIX_LIBDIR_PATH,$XGRAFIX_LIB,"",$XGRAFIX_LIBPATH)

   if test -z "$XGRAFIX_LIBDIR_PATH"; then
      echo
      echo ERROR
      echo "XGRAFIX not found.  Install XGRAFIX in /usr/local/lib/xgrafix"
      echo "or specify the location of XGRAFIX with the"
      echo "--with-XGRAFIX-lib=<XGRAFIX_LIBDIR> parameter"
      AC_MSG_ERROR(Cannot find the XGRAFIX library: $XGRAFIX_LIB)

   else
      XGRAFIX_LIBDIR="`dirname $XGRAFIX_LIBDIR_PATH`"
      XGRAFIX_LIB="-l$XGRAFIX_LIB_NAME"
   fi

else
dnl verify the library from the user inputs
   echo "Checking for XGRAFIX library..."
   echo "$XGRAFIX_LIBDIR"/libXGC250.a
   if test -r "$XGRAFIX_LIBDIR"/libXGC250.a; then
     echo found:  "$XGRAFIX_LIBDIR"/libXGC250.a
     XGRAFIX_LIBDIR="$XGRAFIX_LIBDIR"
     XGRAFIX_LIB="-l$XGRAFIX_LIB_NAME"
   else
     echo
     echo ERROR
     echo "XGRAFIX not found.  Install XGRAFIX in /usr/local/lib/xgrafix"
     echo "or specify the location"
     echo "of XGRAFIX with the --with-XGRAFIX-lib=<XGRAFIX_LIBDIR> parameter."
  fi

fi

dnl Find XGRAFIX_INCLUDE
AC_ARG_WITH(XGRAFIX-include,[  --with-XGRAFIX-include=<XGRAFIX_INCLUDEDIR>	location of xgrafix.h],
	XGRAFIX_INCLUDE="$withval")

dnl try to find the location of the include file.
if test -z "$XGRAFIX_INCLUDE"; then
   XGRAFIX_INCPATH=$HOME/$host/include:$HOME/xgrafix/include:/usr/local/xgrafix/include:/usr/include:/usr/openwin/include:/usr/local/include:/include:/usr/local/include/xgrafix:/usr/X11/include:$PATH
   AC_PATH_PROGS(XGRAFIX_INCLUDE_PATH,xgrafix.h,"",$XGRAFIX_INCPATH)
 
   if test -z "$XGRAFIX_INCLUDE_PATH"; then
      echo
      echo ERROR
      echo "xgrafix.h not found.  Install xgrafix.h in /usr/local/include, or specify the location"
      echo "of xgrafix.h with the --with-XGRAFIX-include=<XGRAFIX_INCLUDEDIR> parameter."

      AC_MSG_ERROR(Cannot find the XGRAFIX include file: xgrafix.h)
 
   else
      XGRAFIX_INCLUDE_FILE=$XGRAFIX_INCLUDE_PATH
      XGRAFIX_INCLUDE="-I`dirname $XGRAFIX_INCLUDE_PATH`"
      XGRAFIX_XGMINI_FILE="`dirname $XGRAFIX_INCLUDE_PATH`/xgmini.h"
   fi

else
   echo "Checking for xgrafix.h..."
   if test -r "$XGRAFIX_INCLUDE"/xgrafix.h; then
      echo found:  "$XGRAFIX_INCLUDE"/xgrafix.h
      XGRAFIX_INCLUDE_FILE=$XGRAFIX_INCLUDE/xgrafix.h
      XGRAFIX_XGMINI_FILE=$XGRAFIX_INCLUDE/xgmini.h
      XGRAFIX_INCLUDE=-I"$XGRAFIX_INCLUDE"
   else
      echo 
      echo ERROR
      echo "xgrafix.h not found.  Install xgrafix.h in /usr/local/include, or specify the location"
      echo "of xgrafix.h with the --with-XGRAFIX-include=<XGRAFIX_INCLUDEDIR> parameter."
      AC_MSG_ERROR(Cannot find the XGRAFIX include file: xgrafix.h)
   fi

fi
XGRAFIX_INCLUDE_DIR=`dirname $XGRAFIX_INCLUDE_FILE`

dnl Try to find SCALAR in xgscalar.h first
AC_PATH_PROGS(XGRAFIX_SCALAR_FILE,xgscalar.h,"",$XGRAFIX_INCLUDE_DIR)
XGSCALAR="no"
XGTYPE="typedef"
if test ! -z "$XGRAFIX_SCALAR_FILE"; then
   XGRAFIX_INCLUDE_FILE=$XGRAFIX_SCALAR_FILE
   XGSCALAR="yes"
   XGTYPE="define"
fi


dnl Find SCALAR in XGRAFIX_INCLUDE_FILE
AC_MSG_CHECKING(for SCALAR in $XGRAFIX_INCLUDE_FILE)
SCALAR_FLOAT=`grep SCALAR $XGRAFIX_INCLUDE_FILE | grep $XGTYPE |grep -v "SCALAR_CHAR" | grep -v "\/\*" | grep -v "\/\/" |grep float`

if test ! -z "$SCALAR_FLOAT"; then
  AC_MSG_RESULT($SCALAR_FLOAT)
dnl This is not needed since the code defaults to float
# DEFINE_SCALAR=-DSCALAR_FLOAT
  DEFINE_SCALAR=
  SCALAR="float"
fi

SCALAR_DOUBLE=`grep SCALAR $XGRAFIX_INCLUDE_FILE | grep $XGTYPE |grep -v "SCALAR_CHAR" | grep -v "\/\*" | grep -v "\/\/" |grep double`
if test ! -z "$SCALAR_DOUBLE"; then
  AC_MSG_RESULT($SCALAR_DOUBLE)
  DEFINE_SCALAR=-DSCALAR_DOUBLE
  SCALAR="double"
fi

# removed by DLB, because XGrafix/XOOPIC cannot be built in "long double" precision
#
# SCALAR_LONG=`grep SCALAR $XGRAFIX_INCLUDE_FILE | grep typedef | grep -v "\/\*" | grep -v "\/\/" |grep long`
# if test ! -z "$SCALAR_LONG"; then
#   AC_MSG_RESULT($SCALAR_LONG)
#   DEFINE_SCALAR=-DSCALAR_LONG
#   SCALAR="long"
# fi

dnl Only test this if the new upgrades are not included
if test "$XGSCALR" = "no"; then

dnl Find SCALAR in XGRAFIX_XGMINI_FILE
  AC_MSG_CHECKING(for SCALAR in $XGRAFIX_XGMINI_FILE)
  SCALAR_FLOAT=`grep SCALAR $XGRAFIX_XGMINI_FILE | grep typedef | grep -v "\/\*" | grep -v "\/\/" |grep float`

  if test ! -z "$SCALAR_FLOAT"; then
    AC_MSG_RESULT($SCALAR_FLOAT)
    if test ! "$SCALAR" = "float"; then
     echo "WARNING"
     echo "xgrafix.h and xgmini.h have different typedef's for SCALAR!"
    fi
  fi

  SCALAR_DOUBLE=`grep SCALAR $XGRAFIX_XGMINI_FILE | grep typedef | grep -v "\/\*" | grep -v "\/\/" |grep double`

  if test ! -z "$SCALAR_DOUBLE"; then
    AC_MSG_RESULT($SCALAR_DOUBLE)
    if test ! "$SCALAR" = "double"; then
     echo "WARNING"
     echo "xgrafix.h and xgmini.h have different typedef's for SCALAR!"
    fi
  fi

  SCALAR_LONG=`grep SCALAR $XGRAFIX_XGMINI_FILE | grep typedef | grep -v "\/\*" | grep -v "\/\/" |grep long`
  if test ! -z "$SCALAR_LONG"; then
    AC_MSG_RESULT($SCALAR_LONG)
    if test ! "$SCALAR" = "long"; then
       echo "WARNING"
       echo "xgrafix.h and xgmini.h have different typedef's for SCALAR!"
    fi
  fi

dnl Check SetUpNewVar configuration
  AC_MSG_CHECKING(SetUpNewVar in $XGRAFIX_INCLUDE_FILE)
  SNVAR=`grep SetUpNewVar $XGRAFIX_INCLUDE_FILE | grep SCALAR `

  if test -z "$SNVAR"; then
    AC_MSG_RESULT(inconsistent)
    echo
    echo "WARNING! "
    echo "SetupNewVar in $XGRAFIX_INCLUDE_FILE needs to be defined as: "
    echo "extern void SetUpNewVar(SCALAR*, const char *, const char *);"
    echo "rather than: "
    echo "`grep SetUpNewVar $XGRAFIX_INCLUDE_FILE`"
    echo
  else
    AC_MSG_RESULT(ok)
  fi
 fi
AC_SUBST(DEFINE_SCALAR)
AC_SUBST(XGRAFIX_INCLUDE)
AC_SUBST(XGRAFIX_LIBDIR)
AC_SUBST(XGRAFIX_LIB)

