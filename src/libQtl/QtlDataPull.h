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
//! @file QtlDataPull.h
//!
//! Declare the class QtlDataPull.
//! Qtl, Qt utility library.
//!
//----------------------------------------------------------------------------

#ifndef QTLDATAPULL_H
#define QTLDATAPULL_H

#include "QtlCore.h"
#include "QtlNullLogger.h"

//!
//! Base class to pull data from a base class into an asynchronous device such as QProcess.
//!
//! This class is based on a "pull" model where the data are "pulled" from the
//! subclass when it is appropriate to write to the device's input. QProcess and
//! other asynchronous devices use an internal buffer mechanism. If the application
//! writes data into the device much faster that the process or network can read,
//! then the data accumulate in memory. I have not seen any limiting mechanism in
//! the Qt source code. So, for huge amounts of data (such as video files), the
//! application might saturate its virtual memory.
//!
//! This class provides a regulation mechanism which avoids that issue.
//! The subclass is "pulled" only when some space is available. On the
//! other hand, this class assumes that input data are always available
//! from the base class.
//!
class QtlDataPull : public QObject
{
    Q_OBJECT

public:
    //!
    //! Default minimum buffer size in bytes (128 kB).
    //!
    static const int DEFAULT_MIN_BUFFER_SIZE = 128 * 1024;

    //!
    //! Constructor.
    //! @param [in] minBufferSize The minimum buffer size is the lower limit of the
    //! buffered data. When the amount of data not yet written to the device is lower
    //! than this size, new data is pulled from the subclass.
    //! @param [in] log Where to log errors.
    //! @param [in] parent Optional parent object.
    //!
    explicit QtlDataPull(int minBufferSize = DEFAULT_MIN_BUFFER_SIZE, QtlLogger* log = 0, QObject* parent = 0);

    //!
    //! Destructor.
    //!
    virtual ~QtlDataPull();

    //!
    //! Set automatic object deletion on transfer completion.
    //! @param [in] on When true, the object deletes itself automatically at the end
    //! of the transfer or if the transfer failed to start.
    //! Use with care, the object must have been created on the heap.
    //!
    void setAutoDelete(bool on)
    {
        _autoDelete = on;
    }

    //!
    //! Check if the data transfer is started.
    //! @return True if the transfer is started.
    //!
    bool isStarted() const
    {
        return _state != Inactive;
    }

    //!
    //! Get the message logger.
    //! @return The message logger.
    //!
    QtlLogger* log() const
    {
        return _log;
    }

public slots:
    //!
    //! Start to transfer data into the specified device.
    //! @param [in] device Data destination. Must be open for write.
    //! @return True if successfully started.
    //!
    bool start(QIODevice* device);

    //!
    //! Force a premature stop of the data transfer.
    //! You should typically connect here termination signals for the device
    //! such as QTcpSocket::disconnected() or QProcess::finished().
    //!
    void stop();

signals:
    //!
    //! Emitted when the transfer is started.
    //!
    void started();

    //!
    //! Emitted when the transfer is completed.
    //! @param [in] success True if all data were successfully transmitted, false on premature error.
    //! @param [in] message Optional error message.
    //!
    void completed(bool success, const QString& message);

protected:
    //!
    //! Initialize the transfer.
    //! May be reimplemented by subclass.
    //! @param [in] device Data destination. Never null. The subclass may perform some
    //! initialization task on the device but should not write data into it.
    //! @return True on success, false on error. In case of error, cleanupTransfer()
    //! will be called later.
    //!
    virtual bool initializeTransfer(QIODevice* device)
    {
        return true;
    }

    //!
    //! Invoked when more data is needed.
    //! Must be reimplemented by subclass.
    //! An implementation will typically call write() to provide data.
    //! If no more data is available, an implementation must call close().
    //! @return True on success, false on error. In case of error, the transfer is aborted
    //! and cleanupTransfer() will be called later.
    //!
    virtual bool needTransfer() = 0;

    //!
    //! Cleanup the transfer.
    //! May be reimplemented by subclass.
    //! @param [in] device Data destination. It can be null. When not null, the subclass
    //! may perform some termination task on the device but should not write data into it.
    //! When null, the device has been destroyed and is no longer available; the subclass
    //! still have the opportunity to perform its own cleanup.
    //! @param [in] clean If true, this is a clean termination, after the subclass called
    //! cleanup(). When false, the transfer has been interrupted for an external reason.
    //!
    virtual void cleanupTransfer(QIODevice* device, bool clean)
    {
    }

    //!
    //! Write data to the device.
    //! Must be called by subclass to transfer data.
    //! @param [in] data Address of data to transfer.
    //! @param [in] dataSize Size in bytes of data to transfer.
    //! @return True on success, all bytes are written. On error, the transfer will be
    //! aborted automatically and cleanupTransfer() will be called later.
    //!
    bool write(const void* data, int dataSize);

    //!
    //! Properly terminate the transfer.
    //! Must be called by subclass to signal the end of the transfer.
    //! The method cleanupTransfer() will be called later with @a clean set to @c true.
    //!
    void close();

    //!
    //! Set the error message that will be sent after transfer completion.
    //! @param [in] message The message to set.
    //!
    void setError(const QString& message)
    {
        _error = message;
    }

private slots:
    //!
    //! Invoked when the device has written data.
    //! @param [in] bytes Number of bytes written.
    //!
    void bytesWritten(qint64 bytes);

    //!
    //! Invoked when the device object is destroyed.
    //!
    void deviceDestroyed(QObject* object);

    //!
    //! Invoked from the event loop to process a new state.
    //!
    void processNewState();

private:
    //!
    //! The appropriate processing of the new state will be performed later, after returning in the event loop.
    //!
    void processNewStateLater()
    {
        QTimer::singleShot(0, this, &QtlDataPull::processNewState);
    }

    //!
    //! Commit suicide if auto-delete is on.
    //!
    void autoDelete()
    {
        if (_autoDelete) {
            deleteLater();
        }
    }

    //!
    //! Check if the subclass needs to provide more data.
    //! @return True if we need more data.
    //!
    bool bufferUnderflow() const
    {
        return _state == Running && (_totalIn - _totalOut) < _minBufferSize;
    }

    //!
    //! Describe the state of the object.
    //!
    enum State {
        Inactive,  //!< Object is completely inactive.
        Running,   //!< Transfer is running normally.
        Aborting,  //!< Transfer is currently prematurely aborting.
        Closing    //!< Transfer was properly closed by subclass.
    };

    QtlNullLogger _nullLog;       //!< Default logger.
    QtlLogger*    _log;           //!< Message logger.
    bool          _autoDelete;    //!< Automatic object deletion on transfer completion.
    int           _minBufferSize; //!< Lower limit of buffer size.
    qint64        _totalIn;       //!< Total data transfered by write().
    qint64        _totalOut;      //!< Total written data as reported by the device.
    State         _state;         //!< Current object state.
    QIODevice*    _device;        //!< The output device.
    QString       _error;         //!< Last error message.
};

#endif // QTLDATAPULL_H
