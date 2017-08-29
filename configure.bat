:: This file is a helper for configuration (cmake) on Windows
@set build_type=Release
@if not "%1"=="" set build_type=%1
@echo Configuring for build type %build_type%

@set QWT_PATH=%cd%\..\..\qwt_6.1.2~osrf_qt5
@set QWT_LIBRARY_DIR=%QWT_PATH%\%build_type%\qwt-6.1.2-vc12-x64
@set QWT_INCLUDEDIR=%QWT_PATH%\include

@set QT5_PATH=C:\Qt5\5.7\\msvc2013_64
@set QT5_BIN_DIR=%QT5_PATH%\bin

@set PATH=%QT5_BIN_DIR%;%PATH%

cmake -Wno-dev -G "NMake Makefiles"^
    -DQWT_WIN_INCLUDE_DIR="%QWT_INCLUDEDIR%"^
    -DQWT_WIN_LIBRARY_DIR="%QWT_LIBRARY_DIR%"^
    -DCMAKE_INSTALL_PREFIX="install\%build_type%"^
    -DCMAKE_BUILD_TYPE="%build_type%"^
    ..

