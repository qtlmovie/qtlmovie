Original libdvdcss code
=======================

This directory contains the original source code for libdvdcss. This code has
been included here to be integrated into the automatic build of QtlMovie, using
qmake and to avoid the external and independent configuration of libdvdcss.

See files AUTHORS and COPYING for original authors and license.


Original source code
--------------------
http://download.videolan.org/pub/libdvdcss/1.4.0/libdvdcss-1.4.0.tar.bz2


Original configuration on Windows with Msys2 and Qt 5.x.y
---------------------------------------------------------
QTDIR=/c/Qt/Qt5.x.y # use your own version
export PATH=$QTDIR/Tools/mingw492_32/bin:$PATH
./configure --enable-static=yes --enable-shared=no


Original configuration on Linux
-------------------------------
./configure --enable-static=yes --enable-shared=no
