//----------------------------------------------------------------------------
//
// Copyright (c) 2013, Thierry Lelegard
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
//! @file QtlMovieDvd.h
//!
//! Declare some DVD utilities.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEDVDIFOFILE_H
#define QTLMOVIEDVDIFOFILE_H

#include "QtlLogger.h"
#include "QtlByteBlock.h"
#include "QtlMediaStreamInfo.h"

//!
//! Namespace for DVD utilities.
//!
namespace QtlMovieDvd
{
    //!
    //! Build the list of DVD title set files for a given selected file.
    //! If @a fileName is not part of a DVD title set, return true (no error) but
    //! both @a ifoFileName and @a vobFileNames are empty.
    //! @param [in] fileName A selected input file. It is considered as part of a DVD title set
    //! if its extension is either ".IFO" or ".VOB".
    //! @param [out] ifoFileName Associated IFO file name. Empty string if no IFO is found.
    //! @param [out] vobFileNames List of existing VOB files. Empty array if not VOB is found.
    //! @param [in] log Where to log errors.
    //! @return True on success, false on error.
    //!
    bool buildFileNames(const QString& fileName, QString& ifoFileName, QStringList& vobFileNames, QtlLogger* log);

    //!
    //! Read an IFO file and load the description of its streams.
    //!
    //! This is a simplified analysis of a DVD IFO (information file) based on
    //! unofficial information from http://dvd.sourceforge.net/dvdinfo/ifo.html
    //!
    //! This function extracts only partial information from the IFO file,
    //! basically the video, audio and subpictures (ie subtitles) attributes.
    //! Only the attributes on the VTS (title set, ie movie) are extracted.
    //! The attributes of the VTSM (title set menu) and VMGM (management menus)
    //! are ignored. Thus, this method is only useful with a VTS_xx_0.IFO and
    //! not with the VIDEO_TS.IFO of the DVD.
    //!
    //! @param [in] fileName IFO file name.
    //! @param [out] streams Descriptions of streams.
    //! @param [out] palette Return the palette in YUV format. Each entry contains 4 bytes: (0, Y, Cr, Cb).
    //! @param [in] log Where to log errors.
    //! @return True on success, false on error.
    //!
    bool readIfoFile(const QString& fileName, QtlMediaStreamInfoPtrVector& streams, QtlByteBlock& palette, QtlLogger* log);

    //!
    //! Convert a YUV palette into RGB.
    //! @param [in,out] palette Palette to modify.
    //! On input, each entry contains 4 bytes: (0, Y, Cr, Cb).
    //! On output, each entry contains 4 bytes: (0, R, G, B).
    //! @param [in] log Where to log errors.
    //!
    void convertPaletteYuvToRgb(QtlByteBlock& palette, QtlLogger* log);

    //!
    //! Compare two QtlMediaStreamInfoPtr for DVD stream ordering.
    //! We reorder stream in "DVD order" for user convenience.
    //! @param [in] p1 First pointer.
    //! @param [in] p2 Second pointer.
    //! @return True is @a p1 is logically less than @a p2.
    //!
    bool lessThan(const QtlMediaStreamInfoPtr& p1, const QtlMediaStreamInfoPtr& p2);
}

#endif // QTLMOVIEDVDIFOFILE_H
