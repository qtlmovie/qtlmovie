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
//! @file QtlIntValidator.h
//!
//! Declare the class QtlIntValidator.
//! Qtl, Qt utility library.
//!
//-----------------------------------------------------------------------------

#ifndef QTLINTVALIDATOR_H
#define QTLINTVALIDATOR_H

#include <QValidator>

//!
//! A subclass of QValidator which accepts all 32-bit decimal or hexadecimal integers.
//! Hexadecimal values are entered with a 0x prefix.
//!
class QtlIntValidator : public QValidator
{
#if !defined(DOXYGEN)
    Q_OBJECT
#endif

public:
    //!
    //! Constructor.
    //! @param [in] parent Optional parent object.
    //! @param [in] minimum Minimum acceptable value.
    //! @param [in] maximum Maximum acceptable value.
    //!
    explicit QtlIntValidator(QObject* parent = 0, quint32 minimum = 0, int maximum = 0xFFFFFFFFL) :
        QValidator(parent),
        _minimum(minimum),
        _maximum(maximum)
    {
    }

    //!
    //! Get the minimum acceptable value.
    //! @return The minimum acceptable value.
    //!
    quint32 minimum() const
    {
        return _minimum;
    }

    //!
    //! Set the minimum acceptable value.
    //! @param [in] minimum The minimum acceptable value.
    //!
    void setMinimum(quint32 minimum)
    {
        _minimum = minimum;
    }

    //!
    //! Get the maximum acceptable value.
    //! @return The maximum acceptable value.
    //!
    quint32 maximum() const
    {
        return _maximum;
    }

    //!
    //! Set the maximum acceptable value.
    //! @param [in] maximum The maximum acceptable value.
    //!
    void setMaximum(quint32 maximum)
    {
        _maximum = maximum;
    }

    //!
    //! Set the minimum and maximum acceptable values.
    //! @param [in] minimum The minimum acceptable value.
    //! @param [in] maximum The maximum acceptable value.
    //!
    void setRange(quint32 minimum, quint32 maximum)
    {
        _minimum = minimum;
        _maximum = maximum;
    }

    //!
    //! Reimplemented from QValidator::fixup().
    //! @param [in,out] input Input string.
    //!
    virtual void fixup(QString& input) const
    {
    }

    //!
    //! Reimplemented from QValidator::validate().
    //! @param [in,out] input Input string.
    //! @param [in,out] pos Cursor position.
    //! @return Either Invalid, Intermediate or Acceptable.
    //!
    virtual State validate(QString& input, int& pos) const;

private:
    static const QRegExp _hexaRegExp;  //!< A regular expression matching all hexadecimal strings.
    static const QRegExp _decRegExp;   //!< A regular expression matching all decimal strings.
    quint32              _minimum;     //!< Minimum acceptable value.
    quint32              _maximum;     //!< Maximum acceptable value.
};

#endif // QTLINTVALIDATOR_H
