//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2017, Thierry Lelegard
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
//
// Define all MPEG-TS data for unitary tests.
//
//----------------------------------------------------------------------------

#ifndef QTSDATA_H
#define QTSDATA_H

#include <QtCore>

//
// Ensure that "const quint8*" can be used as QtTest data.
//
typedef const quint8* ConstBytePtr;
Q_DECLARE_METATYPE(ConstBytePtr)

//
// This macro is used inside data headers.
//
#if defined(QTSDATA_CPP)
// Define the test data.
#define QTSDATA(name,...) \
    extern const quint8 name[]; \
    extern const int name##_size; \
    const quint8 name[] = {__VA_ARGS__}; \
    const int name##_size = sizeof(name);
#else
// Declare the test data.
#define QTSDATA(name,...) \
    extern const quint8 name[]; \
    extern const int name##_size;
#endif

//
// All test data are in header files.
//
#include "QtsData/psi_bat_cplus_packets.h"
#include "QtsData/psi_bat_cplus_sections.h"
#include "QtsData/psi_bat_tvnum_packets.h"
#include "QtsData/psi_bat_tvnum_sections.h"
#include "QtsData/psi_cat_r3_packets.h"
#include "QtsData/psi_cat_r3_sections.h"
#include "QtsData/psi_cat_r6_packets.h"
#include "QtsData/psi_cat_r6_sections.h"
#include "QtsData/psi_nit_tntv23_packets.h"
#include "QtsData/psi_nit_tntv23_sections.h"
#include "QtsData/psi_pat_r4_packets.h"
#include "QtsData/psi_pat_r4_sections.h"
#include "QtsData/psi_pmt_planete_packets.h"
#include "QtsData/psi_pmt_planete_sections.h"
#include "QtsData/psi_sdt_r6_packets.h"
#include "QtsData/psi_sdt_r6_sections.h"
#include "QtsData/psi_tdt_tnt_packets.h"
#include "QtsData/psi_tdt_tnt_sections.h"
#include "QtsData/psi_tot_tnt_packets.h"
#include "QtsData/psi_tot_tnt_sections.h"

#endif // QTSDATA_H
