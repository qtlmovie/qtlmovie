//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//! @file QtlDvdProgramChain.h
//!
//! Declare the class QtlDvdProgramChain.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDVDPROGRAMCHAIN_H
#define QTLDVDPROGRAMCHAIN_H

#include "QtlNullLogger.h"
#include "QtlByteBlock.h"
#include "QtlSmartPointer.h"
#include "QtlDvd.h"

class QtlDvdProgramChain;

//!
//! Smart pointer to QtlDvdProgramChain, non thread-safe.
//!
typedef QtlSmartPointer<QtlDvdProgramChain,QtlNullMutexLocker> QtlDvdProgramChainPtr;
Q_DECLARE_METATYPE(QtlDvdProgramChainPtr)

//!
//! A class which describes a Program Chain (PGC) in a DVD Video Title Set (VTS).
//!
class QtlDvdProgramChain
{
public:
    //!
    //! Constructor.
    //! @param [in] ifo Content of the IFO file.
    //! @param [in] index Starting index of the PGC data in @a ifo.
    //! @param [in] titleNumber Title number in VTS. First title is #1.
    //! @param [in] log Where to log errors.
    //!
    QtlDvdProgramChain(const QtlByteBlock& ifo = QtlByteBlock(), int index = -1, int titleNumber = 0, QtlLogger* log = 0);

    //!
    //! Check if the Program Chain data was correctly analyzed.
    //! @return True when this object is valid, false when it is incorrect.
    //!
    bool isValid() const
    {
        return _valid;
    }

    //!
    //! Get the title number of this PCG inside the VTS.
    //! @return The title number of this PCG inside the VTS.
    //! The first title number in the VTS is 1.
    //!
    int titleNumber() const
    {
        return _titleNumber;
    }

    //!
    //! Get the title number of the next PCG to play after this one inside the VTS.
    //! @return The title number of next PCG to play or zero if there is none.
    //!
    int nextTitleNumber() const
    {
        return _nextPgc;
    }

    //!
    //! Get the title number of the previous PCG to play before this one inside the VTS.
    //! @return The title number of previous PCG to play or zero if there is none.
    //!
    int previousTitleNumber() const
    {
        return _previousPgc;
    }

    //!
    //! Get the title number of the parent PCG of this one inside the VTS.
    //! @return The title number of parent PCG or zero if there is none.
    //!
    int parentTitleNumber() const
    {
        return _parentPgc;
    }

    //!
    //! Get the title set playback duration in seconds.
    //! @return The title set playback duration in seconds.
    //!
    int durationInSeconds() const
    {
        return _duration;
    }

    //!
    //! Get the list of sectors inside the VTS for this PGC.
    //! @return A list of ranges of sectors. The sectors are numbered inside
    //! the VTS, meaning that sector #0 is the first sector of VTS_nn_1.VOB.
    //! The list is ordered and minimal (there are no adjacent ranges, they
    //! are merged into one larger ranges).
    //!
    QtlRangeList sectors() const
    {
        return _sectors;
    }

    //!
    //! Get the color palette of the title set in YUV format.
    //! Typically used to render subtitles.
    //! @return The palette in YUV format. Each entry contains 4 bytes: (0, Y, Cr, Cb).
    //!
    QtlByteBlock yuvPalette() const
    {
        return _palette;
    }

    //!
    //! Get the color palette of the title set in RGB format.
    //! Typically used to render subtitles.
    //! @return The palette in RGB format. Each entry contains 4 bytes: (0, R, G, B).
    //!
    QtlByteBlock rgbPalette() const;

    //!
    //! Convert a YUV palette into RGB.
    //! @param [in,out] palette Palette to modify.
    //! On input, each entry contains 4 bytes: (0, Y, Cr, Cb).
    //! On output, each entry contains 4 bytes: (0, R, G, B).
    //! @param [in] log Where to log errors.
    //!
    static void convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log);

    //!
    //! Convert a YUV or RBG palette into a string.
    //! @param [in] palette Palette convert.
    //! @return A string in the form "nnnnnn,nnnnnn,...".
    //!
    static QString paletteToString(const QtlByteBlock& palette);

private:
    //!
    //! Decode a 32-bit value as a playback duration.
    //! @param [in] value 32-bit value from the IFO file.
    //! @return Decoded duration in seconds.
    //!
    static int toPlaybackDuration(quint32 value);

    QtlNullLogger _nullLog;          //!< Dummy null logger if none specified by caller.
    QtlLogger*    _log;              //!< Where to log errors.
    bool          _valid;            //!< Object is valid.
    int           _titleNumber;      //!< Title number in VTS.
    int           _duration;         //!< Playback duration in seconds.
    int           _programCount;     //!< Number of programs in the chain.
    int           _cellCount;        //!< Number of cells.
    int           _nextPgc;          //!< Next PGC to play.
    int           _previousPgc;      //!< Previous PGC to play.
    int           _parentPgc;        //!< Parent PGC.
    QtlByteBlock  _palette;          //!< VTS color palette in YUV format.
    QtlRangeList  _sectors;          //!< List of sectors inside the VTS for this PGC.
};

//!
//! List of smart pointers to QtlDvdProgramChain (non thread-safe).
//!
class QtlDvdProgramChainPtrList : public QList<QtlDvdProgramChainPtr>
{
public:
    //!
    //! Redefine the superclass type.
    //!
    typedef QList<QtlDvdProgramChainPtr> SuperClass;

    //!
    //! Default constructor.
    //!
    QtlDvdProgramChainPtrList() : SuperClass() {}

    //!
    //! Copy constructor.
    //! @param [in] other Other instance to copy.
    //!
    QtlDvdProgramChainPtrList(const SuperClass &other) : SuperClass(other) {}


    //!
    //! Get the sum of all title set playback durations in seconds.
    //! @return The sum of all title set playback durations in seconds.
    //!
    int totalDurationInSeconds() const;
};

#endif // QTLDVDPROGRAMCHAIN_H
