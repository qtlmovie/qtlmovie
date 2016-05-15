=======================================
QtlMovie Project Hosting on SourceForge
=======================================

This directory tree contains a mirror of the original files from the QtlMovie
project hosting on SourceForge. There are several distinct areas on SourceForge
for any project:

- The GIT repository (other source code revision systems are available).
- The web home page.
- The project page (file repository).

This directory tree is a mirror of the original files (ie. not otherwise copied
or generated) in the web home site and the project site. These files are stored
in the GIT repository. The various .gitignore files are here to exclude the
copied or generated files from the repository.

The various steps in the release process of a new version of QtlMovie are:

- Check that all translations are up-to-date for any new text in the application.
  On a Windows system, execute build\QtLinguist.ps1 to edit the translations.
- Check that any new feature is properly documented in help/qtlmovie-user.html
  and any translation in help/qtlmovie-user_*.html files.
- Execute either build\build-doc.ps1 (Windows) or build/build-doc.sh (Linux).
  It builds the Doxygen documentation for the source code. Check that no error
  is detected in the documentation.
- Commit and push all code to the git repository on SourceForge.
- Add a tag into the repository with the version name, push repository.
- On a Windows system, execute build\build-installer.ps1. It builds the Win32
  and Win64 installers and the source, wintools and mactools archives.
- On a Mac OS X system, execute build/build-mac.sh. It builds the DMG
  package for Mac OS X.
- On a Fedora Linux system, execute build/build-rpm.sh. It builds the RPM
  package for Fedora.
- On an Ubuntu Linux system, execute build/build-deb.sh. It builds the DEB
  package for Ubuntu.
- Copy the content of
    - QtlMovie\help               into QtlMovie\sourceforge\web\doc.
    - QtlMovie\build-doxygen\html into QtlMovie\sourceforge\web\doxy
- From QtlMovie\installers, copy
    - QtlMovie-*-src.zip              into QtlMovie\sourceforge\project\src
    - QtlMovie-*-wintools.zip         into QtlMovie\sourceforge\project\src
    - QtlMovie-*-mactools.zip         into QtlMovie\sourceforge\project\src
    - QtlMovie-Win32-*.exe            into QtlMovie\sourceforge\project\win32
    - QtlMovie-Win32-Standalone-*.zip into QtlMovie\sourceforge\project\win32
    - QtlMovie-Win64-*.exe            into QtlMovie\sourceforge\project\win64
    - QtlMovie-Win64-Standalone-*.zip into QtlMovie\sourceforge\project\win64
    - QtlMovie-*.dmg                  into QtlMovie\sourceforge\project\mac
    - qtlmovie-*-0.fc*.x86_64.rpm     into QtlMovie\sourceforge\project\fedora
    - qtlmovie_*_amd64.deb            into QtlMovie\sourceforge\project\ubuntu
- Synchronize the content of:
    - QtlMovie\sourceforge\project with http://sourceforge.net/projects/qtlmovie/
    - QtlMovie\sourceforge\web     with http://qtlmovie.sourceforge.net/
- Log in to SourceForge, go to the QtlMovie project.
    - Go to the Files/win64 directory, mark the latest QtlMovie-Win64-*.exe as
      default download for Windows.
    - Go to the Files/mac directory, mark the latest QtlMovie-*.dmg as
      default download for Mac.
   - Go to the Files/src directory, mark the latest QtlMovie-*-src.zip as
      default download for all platforms, except Windows and Mac. Note that
      the "default download" system of SourceForge does not allow specific
      files for Ubuntu and Fedora.
    - Go to the "General Discussion" forum and create a topic announcing the
      new release and a copy of the extract of CHANGELOG.txt for this release.
- Log in the VideoHelp.com forums, go to the thread discussing QtlMovie (see
  URL below), create a reply announcing the new release and a copy of the
  extract of CHANGELOG.txt for this release. Thread URL:
  http://forum.videohelp.com/threads/359441-Announcing-QtlMovie-a-specialized-graphical-front-end-to-FFmpeg
- There is also a thread for the Mac version in the VideoHelp.com forums at:
  http://forum.videohelp.com/threads/378564-Announcing-QtlMovie-for-Mac-OS-X-a-specialized-front-end-to-FFmpeg

QtlMovie Home Page
------------------
External link: http://qtlmovie.sourceforge.net/
Local subdirectory: web
FTP host: frs.sourceforge.net
Remote directory: /home/project-web/qtlmovie/htdocs

QtlMovie Project page
---------------------
External link: http://sourceforge.net/projects/qtlmovie/
Local subdirectory: project
FTP host: frs.sourceforge.net
Remote directory: /home/frs/project/qtlmovie
