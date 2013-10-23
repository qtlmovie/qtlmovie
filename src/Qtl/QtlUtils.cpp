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
//
// Qtl, Qt utility library.
// Definition of utilities from QtlUtils.h
//
//----------------------------------------------------------------------------

#include "QtlUtils.h"


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
// Check if the application is running on a 64-bit version of Windows.
//-----------------------------------------------------------------------------

bool qtlRunningOnWin64()
{
#if defined(_WIN64) || defined(Q_OS_WIN64)
    // Compiled on Win64, can run only on Win64.
    return true;
#elif defined(_WIN32) || defined(Q_OS_WIN)
    // Running on another flavor of Windows, we must check.
    BOOL win64 = FALSE;
    return ::IsWow64Process(::GetCurrentProcess(), &win64) && win64;
#else
    // Not running on Windows at all.
    return false;
#endif
}


//-----------------------------------------------------------------------------
// Convert a string into an integer.
//-----------------------------------------------------------------------------

int qtlToInt(const QString& str, int def, int min, int max, int base)
{
    bool ok = false;
    int value = str.trimmed().toInt(&ok, base);
    return ok && value >= min && value <= max ? value : def;
}

qint64 qtlToInt64(const QString& str, qint64 def, qint64 min, qint64 max, int base)
{
    bool ok = false;
    qint64 value = str.trimmed().toLongLong(&ok, base);
    return ok && value >= min && value <= max ? value : def;
}


//-----------------------------------------------------------------------------
// Format a duration into a string.
//-----------------------------------------------------------------------------

QString qtlSecondsToString(int seconds)
{
    const int sec = seconds % 60;
    const int min = (seconds / 60) % 60;
    const int hrs = seconds / 3600;

    if (hrs > 0) {
        return QObject::tr("%1 h %2 mn %3 sec.").arg(hrs).arg(min).arg(sec);
    }
    else if (min > 0) {
        return QObject::tr("%1 mn %2 sec.").arg(min).arg(sec);
    }
    else {
        return QObject::tr("%1 sec.").arg(sec);
    }
}


//-----------------------------------------------------------------------------
// Build an HTML string containing a link to a URL.
//-----------------------------------------------------------------------------

QString qtlHtmlLink(const QString& link, const QString& text)
{
    return QStringLiteral("<a href=\"%1\">%2</a>").arg(link).arg((text.isEmpty() ? link : text).toHtmlEscaped());
}


//-----------------------------------------------------------------------------
// Get all items in a QListWidget as a list of strings.
//-----------------------------------------------------------------------------

QStringList qtlListItems(const QListWidget* list)
{
    QStringList items;
    const int count = list->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = list->item(i);
        if (item != 0) {
            items << item->text();
        }
    }
    return items;
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

QWidget*qtlWidgetAncestor(QObject* object)
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


//-----------------------------------------------------------------------------
// Ask a question with Yes/No buttons (default to No).
//-----------------------------------------------------------------------------

bool qtlConfirm(QObject* parent, const QString& question, const QString& title)
{
    return QMessageBox::Yes ==
        QMessageBox::warning(qtlWidgetAncestor(parent),
                             title,
                             question,
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No);
}


//-----------------------------------------------------------------------------
// Report an error message.
//-----------------------------------------------------------------------------

void qtlError(QObject* parent, const QString& message, const QString& title)
{
    QMessageBox::critical(qtlWidgetAncestor(parent), title, message);
}


//-----------------------------------------------------------------------------
// Display a simple "about" box similar to QMessageBox::about() but add a specific icon.
//-----------------------------------------------------------------------------

void qtlAbout(QObject* parent, const QString& title, const QString& iconPath, const QString& text)
{
    // Create a message box. We cannot use QMessageBox::about() since we want a specific icon.
    QMessageBox* box = new QMessageBox(qtlWidgetAncestor(parent));
    box->setAttribute(Qt::WA_DeleteOnClose); // automatic delete when closed.
    box->setStandardButtons(QMessageBox::Ok);
    box->setWindowTitle(title);
    box->setText(text);

    // Get and set icon.
    if (!iconPath.isEmpty()) {
        QPixmap pixmap(iconPath);
        if (!pixmap.isNull()) {
            box->setIconPixmap(pixmap);
        }
    }

    // Display the message box.
    box->exec();
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
