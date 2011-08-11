/* ./config.h.  Generated from config.h.in by configure.  */
/* ./config.h.in.  Generated from configure.in by autoheader.  */
/**
 * Tech-X C++ configuration defines
 *
 */


/* Define if your c++ compiler can't overload const type conversions. */
/* #undef HAVE_NO_CONST_TYPE_CONVERSION_OVERLOAD */

/* Define if your c++ compiler doesn't know mutable. */
/* #undef HAVE_NO_MUTABLE */

/* Define if your c++ compiler has namespaces. */
#define HAVE_NAMESPACES /**/

/* Define if your c++ compiler does not have RTTI */
/* #undef HAVE_NO_RTTI */

/* Define if your c++ compiler has complex in the namespace std */
#define HAVE_STD_COMPLEX 1

/* Define if your c++ compiler has streams in the namespace std */
#define HAVE_STD_STREAMS 1

/* Define if you use the <sstream> header file.  */
#define HAVE_SSTREAM 1

/* Define if you need the host type */
#define HOSTTYPE "x86_64-unknown-linux-gnu"

/* Define if your c++ compiler requires brackets in declarations 
of template friends. */
#define TEMPLATE_FRIENDS_NEED_BRACKETS 1

/* Define if your c++ compiler can handle nontyped templated operators*/
/* #undef HAVE_NONTYPE_TEMPLATE_OPERATORS */

/* Define if you have HDF5 */
/* #undef HAVE_HDF5 */

/* Define if you have installed an fft lib */
/* #undef HAVE_FFT */

/* Define if you have the dfftw.h header file.  */
/* #undef HAVE_DFFTW */

/* Define if you have the sfftw.h header file.  */
/* #undef HAVE_SFFTW */


/* "C++ Compiler to use" */
#define CXX "g++"

/* "C++ Optimization flags" */
#define CXX_OPTIFLAGS "-O3 -pipe -funroll-loops"

/* "shared library flag" */
#define CXX_SHAREDLIBFLAGS "-shared"

/* "C++ Compiler version" */
#define CXX_VERSION "4.1.2"

/* Define if have the double fftw libraries */
/* #undef HAVE_DFFTW */

/* Define to 1 if you have the <fstream> header file. */
#define HAVE_FSTREAM 1

/* Define if your C++ compiler allows static variables to be declared
   generally */
#define HAVE_GENERALLY_DECLARED_STATICS 1

/* whether the HDF5 library is present */
/* #undef HAVE_HDF5 */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <iostream> header file. */
#define HAVE_IOSTREAM 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if C++ compiler supports namespaces */
#define HAVE_NAMESPACES /**/

/* Define if your C++ compiler can handle nontyped templated operators */
/* #undef HAVE_NONTYPE_TEMPLATE_OPERATORS */

/* Define if C++ compiler cannot overload const type conversions */
/* #undef HAVE_NO_CONST_TYPE_CONVERSION_OVERLOAD */

/* Define if your C++ compiler doesn't know mutable. */
/* #undef HAVE_NO_MUTABLE */

/* Define if C++ compiler does not support RTTI */
/* #undef HAVE_NO_RTTI */

/* Define to 1 if you have the <sstream> header file. */
#define HAVE_SSTREAM 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if your C++ compiler has complex in the namespace std */
#define HAVE_STD_COMPLEX 1

/* Define if your C++ compiler has streams in the namespace std */
#define HAVE_STD_STREAMS 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <strstream> header file. */
#define HAVE_STRSTREAM 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* whether the new H5Sselect_hyperslab interface is in use */
/* #undef NEW_H5S_SELECT_HYPERSLAB_IFC */

/* Name of package */
#define PACKAGE "oopic"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if your C++ compiler requires brackets in declarations of template
   friends. */
#define TEMPLATE_FRIENDS_NEED_BRACKETS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* "top build dir" */
#define TOP_BUILDDIR "/u/jblumenk/myoopic/xoopic2/xoopic"

/* "top src dir" */
#define TOP_SRCDIR "/u/jblumenk/myoopic/xoopic2/xoopic"

/* Version number of package */
#define VERSION "2.6"

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
