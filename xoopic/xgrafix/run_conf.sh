#!/usr/bin/bash
# this script will run configure with some standard
# edit this for the install dir:
prefix=/u/jblumenk/myoopic/xoopic2


# valid precision is either float or double:
precision=double

# base confopts:
# note that this file is duplicated in xgrafix and xoopic, so a couple of 
# values are overspecified - ignore warnings like 'urecognized options with-SCALAR'

confopts="--prefix=$prefix --with-SCALAR=$precision --enable-fulloptimize --with-XGRAFIX-lib=$prefix/lib --with-XGRAFIX-include=$prefix/include"

# MPI :
    # confopts="$confopts --enable-MPI"

# for Mac OS X:
    # extra="/sw" # "/opt/local" for macports
    # confopts="$confopts --with-tclconfig=$extra/lib --with-tkconfig=$extra/lib --with-dfftw-incdir=$extra/include --with-sfftw-incdir=$extra/include --with-hdf5-dir=$extra"

# I recommend getting dependencies for xoopic through macports (macports.org)
# I find thatxgrafix will build but will crash if you link against the system
# tcl/tk.  I'm not sure why, and this may be fixed at some point, but 
# currently my best solution has been using macports for tcl/tk and other deps.
# once you have macports, you can get the dependencies
# with the following:

# sudo port install imagemagick +no_x11 bison fftw automake libpng tcl tk
# add 'hdf5' to the end if you also want hdf5

# I also recommend getting gfortran / gcc 4.2 from AT&T:
# http://r.research.att.com/tools/
# 


if [ -d /usr/lib64 ]
    then
    #make sure 64-bit lib is checked for libraries
    export PATH=$PATH:/usr/lib64
fi

confopts="$confopts $@"

echo "configuring with options: $confopts"
./configure $confopts && \
echo "

Configure successful!
 if you have a multicore machine, now you can build with:
 'make -j 4' (for 4 threaded compiling)
 and install with:
 'make install' or 'sudo make install', depending on whether you have write
  permissions to $prefix"
