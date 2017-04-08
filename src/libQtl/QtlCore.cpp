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
// Qtl, Qt utility library.
// Qtl core declarations.
//
//----------------------------------------------------------------------------

#include "QtlCore.h"


//----------------------------------------------------------------------------
// Return the runtime version of Qt as an integer 0xMMNNPP.
//----------------------------------------------------------------------------

int qtlQtVersion()
{
    // qVersion() returns  a string (for example, "4.1.2"). Split it into fields.
    const QStringList fields(QString::fromLatin1(qVersion()).split(QChar('.'), QString::SkipEmptyParts));
    const int count = fields.size();

    // Decode the first three fields. Invalid fields translate to zero.
    const int major = count < 1 ? 0 : (fields[0].toInt() & 0xFF);
    const int minor = count < 2 ? 0 : (fields[1].toInt() & 0xFF);
    const int patch = count < 3 ? 0 : (fields[2].toInt() & 0xFF);

    // Return the rebuilt value.
    return (major << 16) | (minor << 8) | patch;
}


//-----------------------------------------------------------------------------
// Get a suitable value for application name.
//-----------------------------------------------------------------------------

QString qtlApplicationName()
{
    const QString name = qApp->applicationDisplayName();
    return name.isEmpty() ? qApp->applicationName() : name;
}


//-----------------------------------------------------------------------------
// Get the first QWidget in the line of ancestors of an object.
//-----------------------------------------------------------------------------

QWidget* qtlWidgetAncestor(QObject* object)
{
    while (object != 0) {
        QWidget* widget = qobject_cast<QWidget*>(object);
        if (widget != 0) {
            return widget;
        }
        object = object->parent();
    }
    return 0;
}


//----------------------------------------------------------------------------
// Dump an object hierarchy.
//----------------------------------------------------------------------------

void qtlDumpObjectHierarchy(QTextStream& output, QObject* object, const QString& prefix, int maxDepth)
{
    if (object != 0) {
        // Dump root of tree.
        const QMetaObject* meta = object->metaObject();
        const char* className = meta == 0 ? "" : meta->className();
        output << object->objectName() << " (" << (className == 0 || *className == '\0' ? "no type" : className) << ")";

        // Dump children object if more depth is allowed.
        if (maxDepth != 0) {
            const QObjectList& children(object->children());
            const int last = children.size() - 1;
            for (int i = 0; i <= last; ++i) {
                output << endl << prefix << "+-- ";
                qtlDumpObjectHierarchy(output, children[i], prefix + (i == last ? "    " : "|   "), maxDepth - 1);
            }
        }
    }
}


//----------------------------------------------------------------------------
// Dump an object hierarchy in a string.
//----------------------------------------------------------------------------

QString qtlObjectHierarchy (QObject* object, const QString& prefix, int maxDepth)
{
    QString result;
    QTextStream output(&result);
    qtlDumpObjectHierarchy(output, object, prefix, maxDepth);
    output.flush();
    return result;
}
