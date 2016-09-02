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
//! @file QtlDataPullSynchronousWrapper.h
//!
//! Declare the class QtlDataPullSynchronousWrapper.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDATAPULLSYNCHRONOUSWRAPPER_H
#define QTLDATAPULLSYNCHRONOUSWRAPPER_H

#include "QtlDataPull.h"

//!
//! A class which synchronously waits for a QtlDataPull.
//!
class QtlDataPullSynchronousWrapper : public QObject
{
    Q_OBJECT

public:
    //!
    //! Synchronous contructor starting a QtlDataPull and waiting for its completion.
    //! @param [in] dataPull The QtlDataPull instance to start.
    //! @param [in] device Data destination. Must be open for write.
    //! @param [in] parent Optional parent object.
    //!
    QtlDataPullSynchronousWrapper(QtlDataPull* dataPull, QIODevice* device, QObject* parent = 0);

    //!
    //! Synchronous contructor starting a QtlDataPull and waiting for its completion.
    //! @param [in] dataPull The QtlDataPull instance to start.
    //! @param [in] devices Data destinations. Must be open for write.
    //! @param [in] parent Optional parent object.
    //!
    QtlDataPullSynchronousWrapper(QtlDataPull* dataPull, const QList<QIODevice*>& devices, QObject* parent = 0);

    //!
    //! Check if the QtlDataPull completed without error.
    //! @return True if the QtlDataPull completed without error.
    //!
    bool success() const
    {
        return _success;
    }

private slots:
    //!
    //! Invoked when the transfer is completed on all devices.
    //! @param [in] success True if all data were successfully transmitted.
    //!
    void completed(bool success);

private:
    QEventLoop _loop;     //!< Internal event loop for synchronous wait.
    bool       _success;  //!< True if the QtlDataPull completed without error.

    // Unaccessible operations.
    QtlDataPullSynchronousWrapper() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlDataPullSynchronousWrapper)
};

#endif // QTLDATAPULLSYNCHRONOUSWRAPPER_H
