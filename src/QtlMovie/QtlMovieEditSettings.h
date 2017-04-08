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
//!
//! @file QtlMovieEditSettings.h
//!
//! Declare the class QtlMovieEditSettings, an instance of the UI for the
//! "Edit Settings" action.
//!
//----------------------------------------------------------------------------

#ifndef QTLMOVIEEDITSETTINGS_H
#define QTLMOVIEEDITSETTINGS_H

#include "ui_QtlMovieEditSettings.h"
#include "QtlMovieSettings.h"
#include "QtlMovieOutputFile.h"
#include "QtlMovieDeviceProfile.h"
#include "QtlMovieHelp.h"
#include "QtlFileDialogUtils.h"

//!
//! A subclass of QtlDialog which implements the UI for the "Edit Settings" action.
//! The design of the UI is done using Qt Designer.
//!
class QtlMovieEditSettings : public QtlDialog
{
    Q_OBJECT

public:
    //!
    //! Constructor.
    //! @param [in] settings The settings from which to load the initial values.
    //! @param [in] help Object to display help.
    //! @param [in] parent Optional parent widget.
    //!
    explicit QtlMovieEditSettings(QtlMovieSettings* settings, QtlMovieHelp* help, QWidget* parent = 0);

    //!
    //! Apply the settings values from the dialog box to the QtlTestSettings instance.
    //!
    void applySettings();

private slots:
    //!
    //! Reset the dialog box values from the settings object.
    //! @param button The clicked button. Do nothing if not null and not the Reset button.
    //!
    void resetValues(QAbstractButton* button = 0);

    //!
    //! Invoked by the "Help" button.
    //!
    void help();

    //!
    //! Invoked by the "Browse..." button for the FFmpeg executable.
    //!
    void browseFFmpeg()
    {
        qtlBrowseFile(this, _ui.editFFmpeg, tr("Locate FFmpeg executable"));
    }

    //!
    //! Invoked by the "Browse..." button for the FFprobe executable.
    //!
    void browseFFprobe()
    {
        qtlBrowseFile(this, _ui.editFFprobe, tr("Locate FFprobe executable"));
    }

    //!
    //! Invoked by the "Browse..." button for the DvdAuthor executable.
    //!
    void browseDvdAuthor()
    {
        qtlBrowseFile(this, _ui.editDvdAuthor, tr("Locate DvdAuthor executable"));
    }

    //!
    //! Invoked by the "Browse..." button for the Mkisofs executable.
    //!
    void browseMkisofs()
    {
        qtlBrowseFile(this, _ui.editMkisofs, tr("Locate Mkisofs executable"));
    }

    //!
    //! Invoked by the "Browse..." button for the Growisofs executable.
    //!
    void browseGrowisofs()
    {
        qtlBrowseFile(this, _ui.editGrowisofs, tr("Locate Growisofs executable"));
    }

    //!
    //! Invoked by the "Browse..." button for the CCExtractor executable.
    //!
    void browseCcextractor()
    {
        qtlBrowseFile(this, _ui.editCcextractor, tr("Locate CCExtractor executable"));
    }

    //!
    //! Invoked by the "Browse..." button for the default input directory.
    //!
    void browseInputDir()
    {
        qtlBrowseDirectory(this, _ui.editInputDir, tr("Default input directory"));
    }

    //!
    //! Invoked by the "Browse..." button for the default output directory for DVD extraction.
    //!
    void browseDvdExtractionDir()
    {
        qtlBrowseDirectory(this, _ui.editDvdExtraction, tr("Default DVD extraction directory"));
    }

    //!
    //! Invoked when the check box "default output directory is same as input" is toggled.
    //! @param sameAsInput State of the check box.
    //!
    void setDefaultOutputDirSelectable(bool sameAsInput);

    //!
    //! Invoked by the "Browse..." buttons for the various default output directories.
    //!
    void browseOutputDir();

    //!
    //! Invoked by the "Browse..." button for the DVD burner device.
    //!
    void browseDvdBurner();

    //!
    //! Invoked by the "New..." button in the target audience language editing.
    //!
    void newAudienceLanguage();

    //!
    //! Invoked by the "Delete" button in the target audience language editing.
    //!
    void deleteAudienceLanguage();

    //!
    //! Enable or disable the "Delete" button in the target audience language editing, based on the selection.
    //!
    void enableDeleteAudienceLanguage();

    //!
    //! Invoked when the check box "create chapters" is toggled.
    //! @param createChapters State of the check box.
    //!
    void createChapterToggled(bool createChapters);

    //!
    //! Invoked when the check box "normalize audio level" is toggled.
    //! @param normalize State of the check box.
    //!
    void setNormalizeAudioSelectable(bool normalize);

    //!
    //! Invoked when iPhone/iPad/Android/AVI video quality is updated, update the corresponding max bitrate.
    //!
    void updateMaxBitRates();

    //!
    //! Invoked when the DVD burning speed is changed in the spin box.
    //! @param [in] speed The new speed in Nx.
    //!
    void dvdBurningSpeedChanged(int speed);

private:
    //!
    //! Description of an output directory settings.
    //!
    class OutputDirectory
    {
    public:
        OutputDirectory();        //!< Default constructor.
        QLabel*      label;       //!< Output type name.
        QtlLineEdit* lineEdit;    //!< Output directory path.
        QPushButton* pushButton;  //!< Browse button for output directory.
    };

    //!
    //! A map of OutputDirectory indexed by output type.
    //!
    typedef QMap<QtlMovieOutputFile::OutputType,OutputDirectory> OutputDirectoryMap;

    // Private fields.
    Ui::QtlMovieEditSettings _ui;                //!< UI from Qt Designer.
    QtlMovieSettings*        _settings;          //!< Associated QtlMovieSettings object.
    QtlMovieHelp*            _help;              //!< Object to display help.
    OutputDirectoryMap       _outDirs;           //!< Output directories settings.
    bool                     _useDvdBurnerCombo; //!< Alternate combo box for DVD burner selection.

    //!
    //! Fill a button grid with all model screen sizes of a specific family of devices.
    //! @param [in] grid The button grid to fill.
    //! @param [in] models List of devices.
    //!
    void setModelScreenSizes(QtlButtonGrid* grid, const QVector<QtlMovieDeviceProfile>& models);

    //!
    //! Set the default executable path of a media tool.
    //! @param [in] name Product name.
    //! @param [in,out] label The label widget to update.
    //! @param [in] fileName The default executable path.
    //!
    void setDefaultExecutable(const QString& name, QLabel* label, const QString& fileName);

    //!
    //! Update a label containing the maximum video bitrate for an output type.
    //! @param [in,out] labelMaxBitRate Label to update.
    //! @param [in] spinBoxQuality Sin box containing the video quality.
    //! @param [in] width Reference video width.
    //! @param [in] height Reference video height.
    //! @param [in] frameRate Reference video frame rate.
    //!
    void updateMaxBitRate(QLabel* labelMaxBitRate, const QSpinBox* spinBoxQuality, int width, int height, float frameRate);

    // Unaccessible operations.
    QtlMovieEditSettings() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(QtlMovieEditSettings)
};

#endif // QTLMOVIEEDITSETTINGS_H
