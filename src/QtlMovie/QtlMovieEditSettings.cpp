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
// Define the class QtlMovieEditSettings.
//
//----------------------------------------------------------------------------

#include "QtlMovieEditSettings.h"
#include "QtlUtils.h"
#include "QtlLineEdit.h"
#include <QFileDialog>
#include <QDir>


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieEditSettings::QtlMovieEditSettings(QtlMovieSettings* settings, QWidget* parent) :
    QDialog(parent),
    _settings(settings),
    _outDirs()
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Set default files for executables (not editable).
    setDefaultExecutable("FFmpeg", _ui.defaultFFmpeg, _settings->ffmpegDefaultExecutable());
    setDefaultExecutable("FFprobe", _ui.defaultFFprobe, _settings->ffprobeDefaultExecutable());
    setDefaultExecutable("DVD Author", _ui.defaultDvdAuthor, _settings->dvdauthorDefaultExecutable());
    setDefaultExecutable("mkisofs", _ui.defaultMkisofs, _settings->mkisofsDefaultExecutable());
    setDefaultExecutable("growisofs", _ui.defaultGrowisofs, _settings->growisofsDefaultExecutable());
    setDefaultExecutable("telxcc", _ui.defaultTelxcc, _settings->telxccDefaultExecutable());
    setDefaultExecutable("DVD Decrypter", _ui.defaultDvdDecrypter, _settings->dvddecrypterDefaultExecutable());

    // Create one line per output type for default output directory selection.
    const QList<QtlMovieOutputFile::OutputType> outputTypes(QtlMovieOutputFile::outputTypes());
    foreach (QtlMovieOutputFile::OutputType type, outputTypes) {

        // Description of the output directory for this type.
        OutputDirectory& outDir(_outDirs[type]);

        // Add new widgets after last row in group box layout.
        const int row = _ui.layoutDefaultOutputDir->rowCount() + 1;

        // Left element is a label.
        outDir.label = new QLabel(QStringLiteral("%1 :").arg(QtlMovieOutputFile::outputTypeName(type)), _ui.groupBoxOutputDir);
        _ui.layoutDefaultOutputDir->addWidget(outDir.label, row, 0, 1, 1);

        // Center element is a line edit for the directory.
        outDir.lineEdit = new QtlLineEdit(_ui.groupBoxOutputDir);
        _ui.layoutDefaultOutputDir->addWidget(outDir.lineEdit, row, 1, 1, 1);

        // Right element is the corresponding "Browse ..." button.
        outDir.pushButton = new QPushButton(tr("Browse ..."), _ui.groupBoxOutputDir);
        _ui.layoutDefaultOutputDir->addWidget(outDir.pushButton, row, 2, 1, 1);
        connect(outDir.pushButton, SIGNAL(clicked()), this, SLOT(browseOutputDir()));
    }

    // Load the initial values from the settings object.
    resetValues();
}


//-----------------------------------------------------------------------------
// Set the default executable path of a media tool.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::setDefaultExecutable(const QString& name, QLabel* label, const QString& fileName)
{
    if (fileName.isEmpty()) {
        label->setText(tr("No default %1 found").arg(name));
    }
    else {
        label->setText(tr("Default: %1").arg(fileName));
    }
}


//-----------------------------------------------------------------------------
// Description of an output directory settings.
//-----------------------------------------------------------------------------

QtlMovieEditSettings::OutputDirectory::OutputDirectory() :
    label(0),
    lineEdit(0),
    pushButton(0)
{
}


//-----------------------------------------------------------------------------
// Reset the dialog box values from the settings object.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::resetValues(QAbstractButton* button)
{
    Q_ASSERT(_settings != 0);

    // This method can be invoked manually (with button == 0) or by a signal
    // from the button box. In that case, the button must be the reset button.
    // We do nothing when invoked from a button other than the reset one.
    if (button != 0 && _ui.buttonBox->buttonRole(button) != QDialogButtonBox::ResetRole) {
        // Some other button has been pressed, ignore it.
        return;
    }

    // Load the settings values into the UI.
    _ui.spinMaxLogLines->setValue(_settings->maxLogLines());
    _ui.editFFmpeg->setText(_settings->ffmpegExplicitExecutable());
    _ui.editFFprobe->setText(_settings->ffprobeExplicitExecutable());
    _ui.editDvdAuthor->setText(_settings->dvdauthorExplicitExecutable());
    _ui.editMkisofs->setText(_settings->mkisofsExplicitExecutable());
    _ui.editGrowisofs->setText(_settings->growisofsExplicitExecutable());
    _ui.editTelxcc->setText(_settings->telxccExplicitExecutable());
    _ui.editDvdDecrypter->setText(_settings->dvddecrypterExplicitExecutable());
    _ui.editInputDir->setText(_settings->initialInputDir());
    _ui.editDvdBurner->setText(_settings->dvdBurner());
    _ui.checkBoxSameAsInput->setChecked(_settings->defaultOutputDirIsInput());
    (_settings->transcodeComplete() ? _ui.radioButtonComplete : _ui.radioButtonPartial)->setChecked(true);
    _ui.spinMaxTranscode->setValue(_settings->transcodeSeconds());
    _ui.spinDvdBitrate->setValue(_settings->dvdVideoBitRate() / 1000); // input is kb/s
    _ui.checkBoxKeepIntermediateFiles->setChecked(_settings->keepIntermediateFiles());
    _ui.spinFFmpegProbeSeconds->setValue(_settings->ffmpegProbeSeconds());
    _ui.checkBoxSrtUseVideoSize->setChecked(_settings->srtUseVideoSizeHint());
    _ui.checkCreateChapters->setChecked(_settings->chapterMinutes() > 0);
    _ui.spinChapterMinutes->setValue(_settings->chapterMinutes() > 0 ? _settings->chapterMinutes() : 5);
    _ui.checkDvdRemuxAfterTranscode->setChecked(_settings->dvdRemuxAfterTranscode());
    _ui.radioPal->setChecked(_settings->createPalDvd());
    _ui.radioNtsc->setChecked(!_settings->createPalDvd());

    // Load default output directories by output type.
    for (OutputDirectoryMap::ConstIterator it = _outDirs.begin(); it != _outDirs.end(); ++it) {
        it.value().lineEdit->setText(_settings->defaultOutputDir(QtlMovieOutputFile::outputIdName(it.key()), true));
    }

    // Load audience languages. Make all items editable.
    _ui.listLanguages->addItems(_settings->audienceLanguages());
    for (int i =_ui.listLanguages->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = _ui.listLanguages->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled);
    }
    enableDeleteAudienceLanguage();

    // Make default output directories selectable or not.
    setDefaultOutputDirSelectable(_settings->defaultOutputDirIsInput());
}


//-----------------------------------------------------------------------------
// Apply the settings values from the dialog box to the QtlMovieSettings instance.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::applySettings()
{
    Q_ASSERT(_settings != 0);

    // Load the settings with the values from the UI.
    _settings->setMaxLogLines(_ui.spinMaxLogLines->value());
    _settings->setFFmpegExplicitExecutable(_ui.editFFmpeg->text());
    _settings->setFFprobeExplicitExecutable(_ui.editFFprobe->text());
    _settings->setDvdAuthorExplicitExecutable(_ui.editDvdAuthor->text());
    _settings->setMkisofsExplicitExecutable(_ui.editMkisofs->text());
    _settings->setGrowisofsExplicitExecutable(_ui.editGrowisofs->text());
    _settings->setTelxccExplicitExecutable(_ui.editTelxcc->text());
    _settings->setDvdDecrypterExplicitExecutable(_ui.editDvdDecrypter->text());
    _settings->setInitialInputDir(_ui.editInputDir->text());
    _settings->setDefaultOutputDirIsInput(_ui.checkBoxSameAsInput->isChecked());
    _settings->setDvdBurner(_ui.editDvdBurner->text());
    _settings->setAudienceLanguages(qtlListItems(_ui.listLanguages));
    _settings->setTranscodeComplete(_ui.radioButtonComplete->isChecked());
    _settings->setTranscodeSeconds(_ui.spinMaxTranscode->value());
    _settings->setDvdVideoBitRate(_ui.spinDvdBitrate->value() * 1000); // input is kb/s
    _settings->setKeepIntermediateFiles(_ui.checkBoxKeepIntermediateFiles->isChecked());
    _settings->setFFmpegProbeSeconds(_ui.spinFFmpegProbeSeconds->value());
    _settings->setSrtUseVideoSizeHint(_ui.checkBoxSrtUseVideoSize->isChecked());
    _settings->setChapterMinutes(_ui.checkCreateChapters->isChecked() ? _ui.spinChapterMinutes->value() : 0);
    _settings->setDvdRemuxAfterTranscode(_ui.checkDvdRemuxAfterTranscode->isChecked());
    _settings->setCreatePalDvd(_ui.radioPal->isChecked());

    // Load default output directories by output type.
    for (OutputDirectoryMap::ConstIterator it = _outDirs.begin(); it != _outDirs.end(); ++it) {
        _settings->setDefaultOutputDir(QtlMovieOutputFile::outputIdName(it.key()), it.value().lineEdit->text());
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Browse..." buttons for a file.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::browseFile(QLineEdit* edit, const QString& title)
{
    // Ask the user to select an input file.
    const QString current(edit->text());
    const QString initial(current.isEmpty()? QDir::homePath() : current);
    const QString selected(QFileDialog::getOpenFileName(this, title, initial));

    // Set the new file name.
    if (!selected.isEmpty()) {
        edit->setText(QtlFile::absoluteNativeFilePath(selected));
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Browse..." buttons for a directory.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::browseDirectory(QLineEdit* edit, const QString& title)
{
    // Ask the user to select a directory.
    const QString current(edit->text());
    const QString initial(current.isEmpty()? QDir::homePath() : current);
    const QString selected(QFileDialog::getExistingDirectory(this, title, initial));

    // Set the new file name.
    if (!selected.isEmpty()) {
        edit->setText(QtlFile::absoluteNativeFilePath(selected));
    }
}


//-----------------------------------------------------------------------------
// Invoked when the check box "default output directory is same as input"
// is toggled.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::setDefaultOutputDirSelectable(bool sameAsInput)
{
    // Enable/disable all widgets related to the selection of the default output directory.
    for (OutputDirectoryMap::ConstIterator it = _outDirs.begin(); it != _outDirs.end(); ++it) {
        it.value().label->setEnabled(!sameAsInput);
        it.value().lineEdit->setEnabled(!sameAsInput);
        it.value().pushButton->setEnabled(!sameAsInput);
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Browse..." buttons for the various default output directories.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::browseOutputDir()
{
    // Determine the push button which sent the signal.
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    // Determine the output type for this button.
    for (OutputDirectoryMap::ConstIterator it = _outDirs.begin(); it != _outDirs.end(); ++it) {
        if (it.value().pushButton == button) {
            // Found the output type. Browse directory.
            browseDirectory(it.value().lineEdit, tr("Default output directory for %1").arg(QtlMovieOutputFile::outputTypeName(it.key())));
            break;
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked by the "Browse..." button for the DVD burner device.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::browseDvdBurner()
{
    const QString title(tr("Select DVD Burning Device"));

#if defined(Q_OS_WIN)
    // On Windows, select a device letter.
    QString selected(QFileDialog::getExistingDirectory(this, title, QTL_WIN_MYCOMPUTER));
    if (!selected.isEmpty()) {
        const int colon = selected.indexOf(':');
        if (colon >= 0) {
            selected.remove(colon + 1, selected.size());
        }
        _ui.editDvdBurner->setText(selected);
    }
#else
    // On Unix systems, select a file, by default in /dev.
    const QString current(_ui.editDvdBurner->text());
    const QString initial(current.isEmpty()? "/dev" : current);
    const QString selected(QFileDialog::getOpenFileName(this, title, initial));
    if (!selected.isEmpty()) {
        _ui.editDvdBurner->setText(QtlFile::absoluteNativeFilePath(selected));
    }
#endif
}


//-----------------------------------------------------------------------------
// Invoked by the "New..." button in the target audience language editing.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::newAudienceLanguage()
{
    // Add an empty item at the end of the list.
    QListWidgetItem* item = new QListWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled);
    _ui.listLanguages->addItem(item);

    // Make sure the new item is visible.
    _ui.listLanguages->scrollToItem(item);

    // Start editing the new item.
    _ui.listLanguages->editItem(item);
}


//-----------------------------------------------------------------------------
// Invoked by the "Delete" button in the target audience language editing.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::deleteAudienceLanguage()
{
    // Delete all selected items.
    const QList<QListWidgetItem*> selected(_ui.listLanguages->selectedItems());
    foreach (QListWidgetItem* item, selected) {
        delete item;
    }
}


//-----------------------------------------------------------------------------
// Enable or disable the "Delete" button in the target audience language
// editing, based on the selection.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::enableDeleteAudienceLanguage()
{
    _ui.buttonDeleteLanguage->setEnabled(_ui.listLanguages->selectedItems().size() > 0);
}


//-----------------------------------------------------------------------------
// Invoked when the check box "create chapters" is toggled.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::createChapterToggled(bool createChapters)
{
    _ui.spinChapterMinutes->setEnabled(createChapters);
}
