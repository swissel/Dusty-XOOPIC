Summary:  Graphics library for PTSG simulation programs 
Name:  XGrafix
Version: 2.41
Release: 1
Copyright: Berkeley, distributable, UC Regents
Group: Applications/Engineering
Source: xgrafix-2.41.tar.gz
%description

  A graphics library and controller for physics simulation programs.
3-d surface plots, scatter plots, 2-d line plots.


%prep

%build
make

%install
install -m 755 -o 0 -g 0 src/libXGC241.so /usr/lib
install -m 755 -o 0 -g 0 src/libXGC241.so.0 /usr/lib

install -m 755 -o 0 -g 0 -d /usr/local
install -m 755 -o 0 -g 0 -d /usr/local/lib
install -m 755 -o 0 -g 0 -d /usr/local/bin
install -s -m 755 -o 0 -g 0 xgmovie/xgmovie /usr/local/bin
install -s -m 755 -o 0 -g 0 src/ps6to1 /usr/local/bin
install -m 755 -o 0 -g 0 -d /usr/local/include
install -m 755 -o 0 -g 0 -d /usr/local/lib/xgrafix
install -m 444 -o 0 -g 0 src/xgdialogs.tcl /usr/local/lib/xgrafix
install -m 444 -o 0 -g 0 src/xginit.tcl /usr/local/lib/xgrafix
install -m 444 -o 0 -g 0 src/xgsetup.tcl /usr/local/lib/xgrafix
install -m 444 -o 0 -g 0 src/xgrafix.ico /usr/local/lib/xgrafix
install -m 444 -o 0 -g 0 src/xgrafix.h /usr/local/include

%files 

/usr/lib/libXGC241.so
/usr/lib/libXGC241.so.0
/usr/local/bin/xgmovie
/usr/local/bin/ps6to1
/usr/local/lib/xgrafix/xgdialogs.tcl
/usr/local/lib/xgrafix/xginit.tcl
/usr/local/lib/xgrafix/xgsetup.tcl
/usr/local/lib/xgrafix/xgrafix.ico
/usr/local/include/xgrafix.h

%doc doc xgmovie/xgmovie.doc xgmovie/sample



