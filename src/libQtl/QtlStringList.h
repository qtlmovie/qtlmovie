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
//! @file QtlStringList.h
//!
//! Declare the class QtlStringList.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLSTRINGLIST_H
#define QTLSTRINGLIST_H

#include <QStringList>

//!
//! A subclass of QStringList with specialized constructors.
//!
//! QStringList has a constructor from one QString but not from 2, 3, etc.
//! There is a version with a std::initializer_list<QString> but it requires
//! C++11 support. The subclass QtlStringList provides a few constructors
//! with a given number of QString.
//!
class QtlStringList : public QStringList
{
public:
    //!
    //! Constructor.
    //!
    QtlStringList();

    //!
    //! Constructor.
    //! @param [in] s1 First string to set in the list.
    //!
    QtlStringList(const QString& s1);

    //!
    //! Constructor.
    //! @param [in] s1 First string to set in the list.
    //! @param [in] s2 Next string to set in the list.
    //!
    QtlStringList(const QString& s1, const QString& s2);

    //!
    //! Constructor.
    //! @param [in] s1 First string to set in the list.
    //! @param [in] s2 Next string to set in the list.
    //! @param [in] s3 Next string to set in the list.
    //!
    QtlStringList(const QString& s1, const QString& s2, const QString& s3);

    //!
    //! Constructor.
    //! @param [in] s1 First string to set in the list.
    //! @param [in] s2 Next string to set in the list.
    //! @param [in] s3 Next string to set in the list.
    //! @param [in] s4 Next string to set in the list.
    //!
    QtlStringList(const QString& s1, const QString& s2, const QString& s3, const QString& s4);

    //!
    //! Constructor.
    //! @param [in] other A list to copy.
    //!
    QtlStringList(const QList<QString>& other);

    //!
    //! Get the maximum length of all strings.
    //! @return The maximum length of all strings.
    //!
    int maxLength() const;
};

#endif // QTLSTRINGLIST_H
