//----------------------------------------------------------------------------
//
// Copyright (c) 2013-2015, Thierry Lelegard
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
#include "QtlLineEdit.h"
#include "QtlMessageBoxUtils.h"
#include "QtlListWidgetUtils.h"
#include "QtlLayoutUtils.h"
#include "QtlOpticalDrive.h"
#include "QtlStringUtils.h"
#include "QtlWinUtils.h"


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------

QtlMovieEditSettings::QtlMovieEditSettings(QtlMovieSettings* settings, QtlMovieHelp* help, QWidget* parent) :
    QtlDialog(parent),
    _settings(settings),
    _help(help),
    _outDirs(),
    _useDvdBurnerCombo(false)
{
    // Build the UI as defined in Qt Designer.
    _ui.setupUi(this);

    // Restore the window geometry from the saved settings.
    setGeometrySettings(_settings, true);

    // Set default files for executables (not editable).
    setDefaultExecutable("FFmpeg", _ui.defaultFFmpeg, _settings->ffmpegDefaultExecutable());
    setDefaultExecutable("FFprobe", _ui.defaultFFprobe, _settings->ffprobeDefaultExecutable());
    setDefaultExecutable("DVD Author", _ui.defaultDvdAuthor, _settings->dvdauthorDefaultExecutable());
    setDefaultExecutable("mkisofs", _ui.defaultMkisofs, _settings->mkisofsDefaultExecutable());
    setDefaultExecutable("growisofs", _ui.defaultGrowisofs, _settings->growisofsDefaultExecutable());
    setDefaultExecutable("CCExtractor", _ui.defaultCcextractor, _settings->ccextractorDefaultExecutable());

    // Create one line per output type for default output directory selection.
    // Add new widgets just before last row in group box layout.
    // The last row is reserved for "DVD Extraction", already set in Designer, not a real "output type".
    const QList<QtlMovieOutputFile::OutputType> allTypes(QtlMovieOutputFile::outputTypes());
    int insertRow = _ui.layoutDefaultOutputDir->rowCount() - 1;
    qtlLayoutInsertRows(_ui.layoutDefaultOutputDir, insertRow, allTypes.size());
    foreach (QtlMovieOutputFile::OutputType type, allTypes) {

        // Description of the output directory for this type.
        OutputDirectory& outDir(_outDirs[type]);

        // Left element is a label.
        outDir.label = new QLabel(QStringLiteral("%1 :").arg(QtlMovieOutputFile::outputTypeName(type)), _ui.groupBoxOutputDir);
        _ui.layoutDefaultOutputDir->addWidget(outDir.label, insertRow, 0, 1, 1);

        // Center element is a line edit for the directory.
        outDir.lineEdit = new QtlLineEdit(_ui.groupBoxOutputDir);
        _ui.layoutDefaultOutputDir->addWidget(outDir.lineEdit, insertRow, 1, 1, 1);

        // Right element is the corresponding "Browse ..." button.
        outDir.pushButton = new QPushButton(tr("Browse ..."), _ui.groupBoxOutputDir);
        _ui.layoutDefaultOutputDir->addWidget(outDir.pushButton, insertRow, 2, 1, 1);
        connect(outDir.pushButton, &QPushButton::clicked, this, &QtlMovieEditSettings::browseOutputDir);

        insertRow++;
    }

    // Extract list of DVD burner drive names from the list of all optical drives in the system.
    QStringList burnerDriveNames;
    foreach (const QtlOpticalDrive& drive, QtlOpticalDrive::getAllDrives()) {
        if (drive.canWriteDvd()) {
            burnerDriveNames << drive.name();
        }
    }
    burnerDriveNames.sort(Qt::CaseInsensitive);

    // If we could extract a list of DVD burners, replace the text edit box
    // with a combo box with only the list of possible drives. If we could not
    // get the list of DVD burners, this does not mean that there is none.
    // It means that the implementation of QtlOpticalDrive could not find
    // them. In that case, keep the free edit box.
    _useDvdBurnerCombo = !burnerDriveNames.isEmpty();

    if (_useDvdBurnerCombo) {
        // Hide the free edit version.
        _ui.groupBoxDvdBurningEdit->setVisible(false);
        // Preset the list of known DVD burners in the combo
        _ui.comboDvdBurner->addItems(burnerDriveNames);
    }
    else {
        // Hide the combo version.
        _ui.groupBoxDvdBurningCombo->setVisible(false);
    }

    // Setup the radio buttons to select the output type.
    foreach (QtlMovieOutputFile::OutputType type, QtlMovieOutputFile::outputTypes()) {
        QRadioButton* button(new QRadioButton(_ui.boxOutputType));
        button->setText(QtlMovieOutputFile::outputTypeName(type));
        _ui.boxOutputType->setButtonId(button, int(type));
    }

    // Setup the radio buttons to select iOS and Android sizes.
    setModelScreenSizes(_ui.boxIpadSize, QtlMovieDeviceProfile::iPadModels);
    setModelScreenSizes(_ui.boxIphoneSize, QtlMovieDeviceProfile::iPhoneModels);
    setModelScreenSizes(_ui.boxAndroidSize, QtlMovieDeviceProfile::androidModels);

    // Remove help button if no help provided.
    if (_help == 0) {
        QAbstractButton* b = _ui.buttonBox->button(QDialogButtonBox::Help);
        if (b != 0) {
            _ui.buttonBox->removeButton(b);
            delete b;
        }
    }

    // Load the initial values from the settings object.
    resetValues();
}


//-----------------------------------------------------------------------------
// Fill a button grid with all model screen sizes of a specific family.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::setModelScreenSizes(QtlButtonGrid* grid, const QVector<QtlMovieDeviceProfile>& models)
{
    for (int index = 0; index < models.size(); ++index) {
        QRadioButton* button(new QRadioButton(grid));
        button->setText(QStringLiteral("%1\n(%2x%3)").arg(models[index].name()).arg(models[index].width()).arg(models[index].height()));
        connect(button, &QRadioButton::toggled, this, &QtlMovieEditSettings::updateMaxBitRates);
        grid->setButtonId(button, index);
    }
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
    _ui.editCcextractor->setText(_settings->ccextractorExplicitExecutable());
    _ui.editInputDir->setText(_settings->initialInputDir());
    _ui.editDvdExtraction->setText(_settings->defaultDvdExtractionDir());
    _ui.checkBoxSameAsInput->setChecked(_settings->defaultOutputDirIsInput());
    (_settings->transcodeComplete() ? _ui.radioButtonComplete : _ui.radioButtonPartial)->setChecked(true);
    _ui.spinMaxTranscode->setValue(_settings->transcodeSeconds());
    _ui.spinDvdBitrate->setValue(_settings->dvdVideoBitRate() / 1000);   // input is kb/s
    _ui.spinIpadQuality->setValue(_settings->iPadVideoQuality());
    _ui.spinIphoneQuality->setValue(_settings->iPhoneVideoQuality());
    _ui.spinAndroidQuality->setValue(_settings->androidVideoQuality());
    _ui.checkBoxKeepIntermediateFiles->setChecked(_settings->keepIntermediateFiles());
    _ui.spinFFmpegProbeSeconds->setValue(_settings->ffmpegProbeSeconds());
    _ui.spinFFprobeExecutionTimeout->setValue(_settings->ffprobeExecutionTimeout());
    _ui.checkBoxSrtUseVideoSize->setChecked(_settings->srtUseVideoSizeHint());
    _ui.checkBoxCapitalizeCc->setChecked(_settings->capitalizeClosedCaptions());
    _ui.checkCreateChapters->setChecked(_settings->chapterMinutes() > 0);
    _ui.spinChapterMinutes->setValue(_settings->chapterMinutes() > 0 ? _settings->chapterMinutes() : 5);
    _ui.checkDvdRemuxAfterTranscode->setChecked(_settings->dvdRemuxAfterTranscode());
    _ui.radioPal->setChecked(_settings->createPalDvd());
    _ui.radioNtsc->setChecked(!_settings->createPalDvd());
    _ui.boxIpadSize->checkId(_settings->iPadScreenSize());
    _ui.boxIphoneSize->checkId(_settings->iPhoneScreenSize());
    _ui.boxAndroidSize->checkId(_settings->androidScreenSize());
    _ui.checkForceDvdTranscode->setChecked(_settings->forceDvdTranscode());
    _ui.checkNewVersionCheck->setChecked(_settings->newVersionCheck());
    _ui.spinAviQuality->setValue(_settings->aviVideoQuality());
    _ui.spinAviWidth->setValue(_settings->aviMaxVideoWidth());
    _ui.spinAviHeight->setValue(_settings->aviMaxVideoHeight());
    _ui.checkBoxNormalizeAudio->setChecked(_settings->audioNormalize());
    _ui.spinAudioMeanLevel->setValue(_settings->audioNormalizeMean());
    _ui.spinAudioPeakLevel->setValue(_settings->audioNormalizePeak());
    _ui.radioAudioCompress->setChecked(_settings->audioNormalizeMode() == QtlMovieSettings::Compress);
    _ui.radioAudioAlignPeak->setChecked(_settings->audioNormalizeMode() == QtlMovieSettings::AlignPeak);
    _ui.radioAudioClip->setChecked(_settings->audioNormalizeMode() == QtlMovieSettings::Clip);
    _ui.checkBoxAutoRotateVideo->setChecked(_settings->autoRotateVideo());
    _ui.checkBoxPlaySound->setChecked(_settings->playSoundOnCompletion());
    _ui.checkClearLog->setChecked(_settings->clearLogBeforeTranscode());
    _ui.checkSaveLog->setChecked(_settings->saveLogAfterTranscode());
    (_settings->useBatchMode() ? _ui.radioMultiFile : _ui.radioSingleFile)->setChecked(true);
    _ui.boxOutputType->checkId(int (_settings->defaultOutputType()));
    _ui.checkOriginalAudio->setChecked(_settings->selectOriginalAudio());
    _ui.checkTargetSubtitles->setChecked(_settings->selectTargetSubtitles());
    _ui.checkDvdExtractDirTree->setChecked(_settings->dvdExtractDirTree());
    _ui.checkDvdUseMaxSpeed->setChecked(_settings->dvdUseMaxSpeed());
    _ui.checkBoxCleanupSubtitles->setChecked(_settings->cleanupSubtitles());
    _ui.checkBoxUseHtmlInSrt->setChecked(_settings->useSrtHtmlTags());
    _ui.checkBoxDowngradeSsaToSrt->setChecked(_settings->downgradeSsaToSrt());

    // Update max bit rates according to video qualities.
    updateMaxBitRates();

    // DVD burner can be edited in two ways.
    if (_useDvdBurnerCombo) {

        // Check if the current setting is in the list of known burners.
        const QString current(_settings->dvdBurner());
        int index = _ui.comboDvdBurner->findText(current);

        if (index < 0) {
            // Current value from settings is not present. Could not be a valid
            // burner device but add it anyway at end of list.
            _ui.comboDvdBurner->addItem(current);
            index = _ui.comboDvdBurner->count() - 1;
        }

        // Set current setting as selected value.
        _ui.comboDvdBurner->setCurrentIndex(index);
    }
    else {
        // Use free edit for burner.
        _ui.editDvdBurner->setText(_settings->dvdBurner());
    }

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
// Invoked by the "Help" button.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::help()
{
    // Find help for the current tab.
    QWidget* current = _ui.tab->currentWidget();
    if (current != 0 && _help != 0) {
        // Use the object name of the tab as URL fragment (example: "#tabDirectories").
        _help->showHelp(this, QtlMovieHelp::User, current->objectName());
    }
}


//-----------------------------------------------------------------------------
// Apply the settings values from the dialog box to the QtlMovieSettings instance.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::applySettings()
{
    Q_ASSERT(_settings != 0);

    // On Windows, ensures that the DVD burner is only a device letter followed
    // by a colon, when manually entered.
#if defined(Q_OS_WIN)
    if (!_useDvdBurnerCombo) {
        QString dvd(_ui.editDvdBurner->text());
        if (dvd.length() >= 2 && dvd[1] == ':') {
            // Truncate after ':'
            dvd.truncate(2);
            _ui.editDvdBurner->setText(dvd.toUpper());
        }
        else if (!dvd.isEmpty()) {
            // Incorrect device name.
            qtlError(this, tr("Ignore incorrect DVD burner device %1, must be a drive name").arg(dvd));
            _ui.editDvdBurner->setText("");
        }
    }
#endif

    // Load the settings with the values from the UI.
    _settings->setMaxLogLines(_ui.spinMaxLogLines->value());
    _settings->setFFmpegExplicitExecutable(_ui.editFFmpeg->text());
    _settings->setFFprobeExplicitExecutable(_ui.editFFprobe->text());
    _settings->setDvdAuthorExplicitExecutable(_ui.editDvdAuthor->text());
    _settings->setMkisofsExplicitExecutable(_ui.editMkisofs->text());
    _settings->setGrowisofsExplicitExecutable(_ui.editGrowisofs->text());
    _settings->setCCextractorExplicitExecutable(_ui.editCcextractor->text());
    _settings->setInitialInputDir(_ui.editInputDir->text());
    _settings->setDefaultDvdExtractionDir(_ui.editDvdExtraction->text());
    _settings->setDefaultOutputDirIsInput(_ui.checkBoxSameAsInput->isChecked());
    _settings->setDvdBurner(_useDvdBurnerCombo ? _ui.comboDvdBurner->currentText() : _ui.editDvdBurner->text());
    _settings->setAudienceLanguages(qtlListItems(_ui.listLanguages));
    _settings->setTranscodeComplete(_ui.radioButtonComplete->isChecked());
    _settings->setTranscodeSeconds(_ui.spinMaxTranscode->value());
    _settings->setDvdVideoBitRate(_ui.spinDvdBitrate->value() * 1000);   // input is kb/s
    _settings->setIpadVideoQuality(_ui.spinIpadQuality->value());
    _settings->setIphoneVideoQuality(_ui.spinIphoneQuality->value());
    _settings->setAndroidVideoQuality(_ui.spinAndroidQuality->value());
    _settings->setKeepIntermediateFiles(_ui.checkBoxKeepIntermediateFiles->isChecked());
    _settings->setFFmpegProbeSeconds(_ui.spinFFmpegProbeSeconds->value());
    _settings->setFFprobeExecutionTimeout(_ui.spinFFprobeExecutionTimeout->value());
    _settings->setSrtUseVideoSizeHint(_ui.checkBoxSrtUseVideoSize->isChecked());
    _settings->setCapitalizeClosedCaptions(_ui.checkBoxCapitalizeCc->isChecked());
    _settings->setChapterMinutes(_ui.checkCreateChapters->isChecked() ? _ui.spinChapterMinutes->value() : 0);
    _settings->setDvdRemuxAfterTranscode(_ui.checkDvdRemuxAfterTranscode->isChecked());
    _settings->setCreatePalDvd(_ui.radioPal->isChecked());
    _settings->setIpadScreenSize(_ui.boxIpadSize->checkedId());
    _settings->setIphoneScreenSize(_ui.boxIphoneSize->checkedId());
    _settings->setAndroidScreenSize(_ui.boxAndroidSize->checkedId());
    _settings->setForceDvdTranscode(_ui.checkForceDvdTranscode->isChecked());
    _settings->setNewVersionCheck(_ui.checkNewVersionCheck->isChecked());
    _settings->setAviVideoQuality(_ui.spinAviQuality->value());
    _settings->setAviMaxVideoWidth(_ui.spinAviWidth->value());
    _settings->setAviMaxVideoHeight(_ui.spinAviHeight->value());
    _settings->setAudioNormalize(_ui.checkBoxNormalizeAudio->isChecked());
    _settings->setAudioNormalizeMean(_ui.spinAudioMeanLevel->value());
    _settings->setAudioNormalizePeak(_ui.spinAudioPeakLevel->value());
    if (_ui.radioAudioCompress->isChecked()) {
        _settings->setAudioNormalizeMode(QtlMovieSettings::Compress);
    }
    else if (_ui.radioAudioAlignPeak->isChecked()) {
        _settings->setAudioNormalizeMode(QtlMovieSettings::AlignPeak);
    }
    else if (_ui.radioAudioClip->isChecked()) {
        _settings->setAudioNormalizeMode(QtlMovieSettings::Clip);
    }
    _settings->setAutoRotateVideo(_ui.checkBoxAutoRotateVideo->isChecked());
    _settings->setPlaySoundOnCompletion(_ui.checkBoxPlaySound->isChecked());
    _settings->setClearLogBeforeTranscode(_ui.checkClearLog->isChecked());
    _settings->setSaveLogAfterTranscode(_ui.checkSaveLog->isChecked());
    _settings->setUseBatchMode(_ui.radioMultiFile->isChecked());
    _settings->setDefaultOutputType(QtlMovieOutputFile::OutputType(_ui.boxOutputType->checkedId()));
    _settings->setSelectOriginalAudio(_ui.checkOriginalAudio->isChecked());
    _settings->setSelectTargetSubtitles(_ui.checkTargetSubtitles->isChecked());
    _settings->setDvdExtractDirTree(_ui.checkDvdExtractDirTree->isChecked());
    _settings->setDvdUseMaxSpeed(_ui.checkDvdUseMaxSpeed->isChecked());
    _settings->setCleanupSubtitles(_ui.checkBoxCleanupSubtitles->isChecked());
    _settings->setUseSrtHtmlTags(_ui.checkBoxUseHtmlInSrt->isChecked());
    _settings->setDowngradeSsaToSrt(_ui.checkBoxDowngradeSsaToSrt->isChecked());

    // Load default output directories by output type.
    for (OutputDirectoryMap::ConstIterator it = _outDirs.begin(); it != _outDirs.end(); ++it) {
        _settings->setDefaultOutputDir(QtlMovieOutputFile::outputIdName(it.key()), it.value().lineEdit->text());
    }

    // Make sure the settings are saved.
    _settings->sync();
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
            qtlBrowseDirectory(this, it.value().lineEdit, tr("Default output directory for %1").arg(QtlMovieOutputFile::outputTypeName(it.key())));
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
    const QUrl computer(QTL_CLSID_URL(QTL_FOLDERID_COMPUTER));
    QString selected(QFileDialog::getExistingDirectoryUrl(this, title, computer).toLocalFile());
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
    foreach (QListWidgetItem* item, _ui.listLanguages->selectedItems()) {
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


//-----------------------------------------------------------------------------
// Invoked when the check box "normalize audio level" is toggled.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::setNormalizeAudioSelectable(bool normalize)
{
    // Enable / disable all widget within the "audio normalization" group box.
    foreach (QWidget* child, _ui.groupBoxNormalizeAudio->findChildren<QWidget*>()) {
        // Do not disable the "normalize audio", this is the one which triggers this event.
        if (child != _ui.checkBoxNormalizeAudio) {
            child->setEnabled(normalize);
        }
    }
}


//-----------------------------------------------------------------------------
// Invoked when iPhone/iPad/Android/AVI video quality is updated,
// update the corresponding max bitrate.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::updateMaxBitRates()
{
    // iPad max bit rate.
    const QtlMovieDeviceProfile iPad(QtlMovieDeviceProfile::iPadModels.value(_ui.boxIpadSize->checkedId()));
    updateMaxBitRate(_ui.labelIpadMaxBitRate, _ui.spinIpadQuality, iPad.width(), iPad.height(), iPad.frameRate());

    // iPhone max bit rate.
    const QtlMovieDeviceProfile iPhone(QtlMovieDeviceProfile::iPhoneModels.value(_ui.boxIphoneSize->checkedId()));
    updateMaxBitRate(_ui.labelIphoneMaxBitRate, _ui.spinIphoneQuality, iPhone.width(), iPhone.height(), iPhone.frameRate());

    // Android max bit rate.
    const QtlMovieDeviceProfile android(QtlMovieDeviceProfile::androidModels.value(_ui.boxAndroidSize->checkedId()));
    updateMaxBitRate(_ui.labelAndroidMaxBitRate, _ui.spinAndroidQuality, android.width(), android.height(), android.frameRate());

    // AVI max bit rate.
    updateMaxBitRate(_ui.labelAviMaxBitRate, _ui.spinAviQuality, _ui.spinAviWidth->value(), _ui.spinAviHeight->value(), QTL_AVI_FRAME_RATE);
}


//-----------------------------------------------------------------------------
// Update a label containing the maximum video bitrate for an output type.
//-----------------------------------------------------------------------------

void QtlMovieEditSettings::updateMaxBitRate(QLabel* labelMaxBitRate, const QSpinBox* spinBoxQuality, int width, int height, float frameRate)
{
    const int bitRate = QtlMovieDeviceProfile::videoBitRate(spinBoxQuality->value(), width, height, frameRate);
    const QString bitRateString(qtlStringSpace(QString::number(bitRate), 3, Qt::RightToLeft, " "));
    labelMaxBitRate->setText(tr("%1 bit / second for %2x%3 @%4").arg(bitRateString).arg(width).arg(height).arg(frameRate));
}
