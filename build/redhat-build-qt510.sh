#!/bin/bash
# Rebuild Qt and Qt Creator from sources on Linux.
#
# Prior required downloads:
# http://download.qt-project.org/archive/qt/5.1/5.1.0/single/qt-everywhere-opensource-src-5.1.0.tar.gz
# http://download.qt-project.org/official_releases/qtcreator/2.7/2.7.2/qt-creator-2.7.2-src.tar.gz

QT_SRC=qt-everywhere-opensource-src-5.1.0
QT_CREATOR_SRC=qt-creator-2.7.2-src
QT_INSTALL_ROOT=/usr/local/qt-5.1.0
QT_CREATOR_INSTALL_ROOT=/usr/local/qt-creator-2.7.2

# Create Qt installation directories:
sudo install -d -o $USER -m 755 $QT_INSTALL_ROOT $QT_CREATOR_INSTALL_ROOT

# This script:
SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)
SCRIPT_NAME=$(basename ${BASH_SOURCE[0]} .sh)

# Exit script on error, redirect output and error in a log file
set -e
exec </dev/null &>$SCRIPT_DIR/$SCRIPT_NAME.log

# Extract Qt sources.
cd $SCRIPT_DIR
tar xf $QT_SRC.tar.gz
cd $QT_SRC

# Build and install Qt.
./configure -prefix $QT_INSTALL_ROOT -opensource -confirm-license -nomake examples -nomake tests
make -j6
make install
make docs
make install_docs

# Extract Qt Creator sources.
cd $SCRIPT_DIR
tar xf $QT_CREATOR_SRC.tar.gz
cd $QT_CREATOR_SRC

# Build and install Qt Creator.
export PATH=$QT_INSTALL_ROOT/bin:$PATH
export LD_LIBRARY_PATH=$QT_INSTALL_ROOT/lib:$LD_LIBRARY_PATH
qmake qtcreator.pro
make -j6
make install INSTALL_ROOT=$QT_CREATOR_INSTALL_ROOT
make docs
make install_docs INSTALL_ROOT=$QT_CREATOR_INSTALL_ROOT
make deployqt INSTALL_ROOT=$QT_CREATOR_INSTALL_ROOT
