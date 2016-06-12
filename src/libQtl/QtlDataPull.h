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
//! Base class to pull data from a base class into asynchronous devices such as QProcess or QTcpSocket.
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
        return !_devices.isEmpty();
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
    //! Start to transfer data into the several devices in parallel.
    //! The data are pulled only once but duplicated to all devices.
    //! @param [in] devices Data destinations. Must be open for write.
    //! @return True if successfully started.
    //!
    bool start(const QList<QIODevice*>& devices);

    //!
    //! Force a premature stop of the data transfer.
    //!
    void stop();

    //!
    //! Stop the transfer on the specified device.
    //! Transfer will continue to the other devices.
    //! If the @a device was the last one, then the transfer is aborted.
    //! @param [in] device The device to remove from the transfer.
    //!
    void stopDevice(QIODevice* device);

    //!
    //! This slot stops the transfer on the device which sent the signal.
    //! This is a convenience slot which can be connected to a signal from
    //! a destination device, any form of signal such as "finished",
    //! "completed", "aborted" or "disconnected" that the device may support.
    //!
    //! Note that the following device's signals are automatically connected
    //! to this slot:
    //! - QProcess::finished()
    //! - QAbstractSocket::disconnected()
    //!
    void stopCaller();

signals:
    //!
    //! Emitted when the transfer is started.
    //!
    void started();

    //!
    //! Emitted when the transfer is completed on one device.
    //! This signal can be used to close, flush, disconnect or any other
    //! appropriate operation on the device.
    //!
    //! Note that the following actions are automatically performed on
    //! various subclasses of QIODevice and the application does not need
    //! to take care about it.
    //! - QProcess::closeWriteChannel()
    //! - QAbstractSocket::disconnectFromHost()
    //!
    //! @param [in] device Device for which the transfer is completed.
    //! @param [in] success True if all data were successfully transmitted
    //! to this device, false on premature error.
    //!
    void deviceCompleted(QIODevice* device, bool success);

    //!
    //! Emitted when the transfer is completed on all devices.
    //! The signal deviceCompleted() is sent once for each device.
    //! Then the signal completed() is sent once.
    //! @param [in] success True if all data were successfully transmitted
    //! to all device, false on premature error on at least one device.
    //!
    void completed(bool success);

protected:
    //!
    //! Initialize the transfer.
    //! May be reimplemented by subclass.
    //! @return True on success, false on error. In case of error, the transfer is not started.
    //!
    virtual bool initializeTransfer()
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
    //! @param [in] closed If true, this is a clean termination, after the subclass called
    //! close(). When false, the transfer has been interrupted for an external reason.
    //!
    virtual void cleanupTransfer(bool closed)
    {
    }

    //!
    //! Write data to all devices.
    //! Must be called by subclass to transfer data.
    //! @param [in] data Address of data to transfer.
    //! @param [in] dataSize Size in bytes of data to transfer.
    //! @return True if all bytes are written on at least one device.
    //! False if all transfers failed. On error on one device, the transfer
    //! will be aborted automatically on this device and deviceCompleted()
    //! will be signalled.
    //!
    bool write(const void* data, int dataSize);

    //!
    //! Properly terminate the transfer.
    //! Must be called by subclass to signal the end of the transfer.
    //! The method cleanupTransfer() will be called later with @a closed set to @c true.
    //!
    void close();

private slots:
    //!
    //! Invoked when a device has written data.
    //! @param [in] bytes Number of bytes written.
    //!
    void bytesWritten(qint64 bytes);

    //!
    //! Invoked when a device object is destroyed.
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
        QMetaObject::invokeMethod(this, "processNewState", Qt::QueuedConnection);
    }

    //!
    //! Check if more data need to be pulled from the subclass.
    //! @return True is all active devices are underflow.
    //!
    bool needMoreData() const;

    //!
    //! Perform specific setup on a device, depending on its class.
    //! @param [in] dev Device to setup.
    //!
    void deviceSpecificSetup(QIODevice* dev);

    //!
    //! Perform specific cleanup on a device, depending on its class.
    //! @param [in] dev Device to setup.
    //! @param [in] closed True on normal termination, false if the transfer was aborted.
    //!
    void deviceSpecificCleanup(QIODevice* dev, bool closed);

    //!
    //! Describe the context of one device.
    //!
    class Context
    {
    public:
        QIODevice* device;    //!< Device descriptor.
        bool       running;   //!< The device is active.
        qint64     totalOut;  //!< Total written data as reported by the device.
        //!
        //! Constructor.
        //! @param [in] dev Optional device descriptor.
        //!
        Context(QIODevice* dev = 0);
    };

    QtlNullLogger  _nullLog;         //!< Default logger.
    QtlLogger*     _log;             //!< Message logger.
    bool           _autoDelete;      //!< Automatic object deletion on transfer completion.
    int            _minBufferSize;   //!< Lower limit of buffer size.
    QTime          _startTime;       //!< Time of start operation.
    bool           _closed;          //!< True when close() is requested by subclass.
    qint64         _totalIn;         //!< Total data transfered by write().
    QList<Context> _devices;         //!< Active output devices.
    int            _processingState; //!< A counter to protect processNewState().
};

#endif // QTLDATAPULL_H
