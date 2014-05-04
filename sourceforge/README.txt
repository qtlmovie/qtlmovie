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
- On a Windows system, execute build\build-installer.ps1. It builds the Win32
  and Win64 installers and the source and wintools archives.
- Execute either build\build-doc.ps1 (Windows) or build/build-doc.sh (Linux).
  It builds the Doxygen documentation for the source code.
- On a Fedora Linux system, execute build/build-rpm.sh. It builds the RPM
  package for Fedora.
- Copy the content of QtlMovie\help into QtlMovie\sourceforge\web\doc.
- Copy the content of QtlMovie\build-doxygen\html into QtlMovie\sourceforge\web\doxy.
- Copy QtlMovie\build-QtlMovie-Desktop-Release\qtlmovie-*-0.fc*.x86_64.rpm into
  QtlMovie\sourceforge\project\fedora.
- From QtlMovie\build-Win32-Release-Static, copy
    - QtlMovie-*-src.zip      into QtlMovie\sourceforge\project\src
    - QtlMovie-*-wintools.zip into QtlMovie\sourceforge\project\src
    - QtlMovie-Win32-*.exe    into QtlMovie\sourceforge\project\win32
    - QtlMovie-Win64-*.exe    into QtlMovie\sourceforge\project\win64
- Synchronize the content of:
    - QtlMovie\sourceforge\project with http://sourceforge.net/projects/qtlmovie/
    - QtlMovie\sourceforge\web     with http://qtlmovie.sourceforge.net/


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
