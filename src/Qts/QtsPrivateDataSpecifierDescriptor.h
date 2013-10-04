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
//! @file QtsPrivateDataSpecifierDescriptor.h
//!
//! Declare the class QtsPrivateDataSpecifierDescriptor.
//! Qts, the Qt MPEG Transport Stream library.
//!
//----------------------------------------------------------------------------

#ifndef QTSPRIVATEDATASPECIFIERDESCRIPTOR_H
#define QTSPRIVATEDATASPECIFIERDESCRIPTOR_H

#include "QtsAbstractDescriptor.h"

//!
//! The DVD-defined private_data_specifier_descriptor.
//!
class QtsPrivateDataSpecifierDescriptor : public QtsAbstractDescriptor
{
public:
    //!
    //! Default constructor.
    //! @param [in] p The private data specifier to set in the descriptor.
    //!
    QtsPrivateDataSpecifierDescriptor(QtsPrivDataSpec p = 0);

    //!
    //! Constructor from a binary descriptor.
    //!
    QtsPrivateDataSpecifierDescriptor(const QtsDescriptor& desc);

    //!
    //! This abstract method serializes a descriptor.
    //! Implementation required by QtsAbstractDescriptor.
    //! @param [out] descriptor Replaced with a binary representation of this object.
    //! @return True on success, false on error (invalid descriptor, data too large, etc.)
    //!
    virtual bool serialize(QtsDescriptor& descriptor) const;

    //!
    //! This abstract method deserializes a binary descriptor.
    //! Implementation required by QtsAbstractDescriptor.
    //! @param [in] descriptor Binary descriptor to deserialize.
    //! @return True on success, false on error (invalid data, etc.)
    //!
    virtual bool deserialize(const QtsDescriptor& descriptor);

    //!
    //! Private data specifier in the descriptor (public field).
    //!
    QtsPrivDataSpec pds;
};

#endif // QTSPRIVATEDATASPECIFIERDESCRIPTOR_H
