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
//! @file QtsDvdProgramChain.h
//!
//! Declare the class QtsDvdProgramChain.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSDVDPROGRAMCHAIN_H
#define QTSDVDPROGRAMCHAIN_H

#include "QtlNullLogger.h"
#include "QtlByteBlock.h"
#include "QtsDvdProgramChapter.h"
#include "QtsDvdProgramCell.h"
#include "QtsDvdOriginalCell.h"
#include "QtsDvd.h"

class QtsDvdProgramChain;

//!
//! Smart pointer to QtsDvdProgramChain, non thread-safe.
//!
typedef QtlSmartPointer<QtsDvdProgramChain,QtlNullMutexLocker> QtsDvdProgramChainPtr;
Q_DECLARE_METATYPE(QtsDvdProgramChainPtr)

//!
//! List of smart pointers to QtsDvdProgramChain (non thread-safe).
//!
typedef QList<QtsDvdProgramChainPtr> QtsDvdProgramChainList;

//!
//! A class which describes a Program Chain (PGC) in a DVD Video Title Set (VTS).
//! Note that a Program Chain is also named a "Title", a VTS being a set of PGC's.
//!
class QtsDvdProgramChain
{
public:
    //!
    //! Constructor.
    //! @param [in] ifo Content of the IFO file.
    //! @param [in] index Starting index of the PGC data in @a ifo.
    //! @param [in] titleNumber Title number in VTS. First title is #1.
    //! @param [in] originalCells List of cells in original input files.
    //! @param [in] log Where to log errors.
    //!
    QtsDvdProgramChain(const QtlByteBlock& ifo = QtlByteBlock(),
                       int index = -1,
                       int titleNumber = 0,
                       const QtsDvdOriginalCellList& originalCells = QtsDvdOriginalCellList(),
                       QtlLogger* log = 0);

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
    //! Get the number of specific angles.
    //! @return The number of specific angles. If there is no specific angle,
    //! only common content as in most cases, return 0.
    //!
    int angleCount() const {
        return _angleCount;
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
    //! Get the number of chapters in this program chain.
    //! @return The number of chapters in this program chain.
    //!
    int chapterCount() const
    {
        return _chapters.size();
    }

    //!
    //! Get the list of chapters in this program chain.
    //! Note that a chapter is also named a "program" or a "part of a title" (PTT),
    //! a program chain (PGC) being a chain of chapters.
    //! @return The list of chapters in this program chain.
    //!
    QtsDvdProgramChapterList chapters() const
    {
        return _chapters;
    }

    //!
    //! Get the number of cells in this program chain.
    //! @return The number of cells in this program chain.
    //!
    int cellCount() const
    {
        return _cells.size();
    }

    //!
    //! Get the list of cells in this program chain.
    //! @return The list of cells in this program chain.
    //!
    QtsDvdProgramCellList cells() const
    {
        return _cells;
    }

    //!
    //! Get the total number of sectors in all cells.
    //! Note that the actual number of sectors can be a bit smaller (see QtsDvdProgramCell::sectors()).
    //! @return The total number of sectors in all cells.
    //!
    int totalSectorCount() const;

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
    int           _nextPgc;          //!< Next PGC to play.
    int           _previousPgc;      //!< Previous PGC to play.
    int           _parentPgc;        //!< Parent PGC.
    int           _angleCount;       //!< Number of angles.
    QtlByteBlock  _palette;          //!< VTS color palette in YUV format.
    QtsDvdProgramCellList    _cells;     //!< List of cells in this program chain.
    QtsDvdProgramChapterList _chapters;  //!< List of chapters in this program chain.
};

#endif // QTSDVDPROGRAMCHAIN_H
