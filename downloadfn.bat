@echo off
title ProjectZERO-ZeroFN
color 0f

:path_select
cls
echo =====================================
echo    Select Installation Path
echo    Powered by ZeroFN
echo =====================================
echo.
set "DEFAULT_PATH=%cd%"
echo Current directory: %DEFAULT_PATH%
echo.
echo [1] Install in current directory
echo [2] Specify custom installation path
echo [3] Exit
echo.
set /p path_choice="Enter your choice (1-3): "

if "%path_choice%"=="1" (
    set "BASE_PATH=%DEFAULT_PATH%"
    goto season_select
)
if "%path_choice%"=="2" (
    echo.
    echo Enter the full path where you want to install Fortnite:
    echo Example: C:\Games\Fortnite
    set /p BASE_PATH="Path: "
    if not exist "%BASE_PATH%" (
        mkdir "%BASE_PATH%"
    )
    goto season_select
)
if "%path_choice%"=="3" exit
goto path_select

:season_select
cls
echo =====================================
echo    Select Fortnite Season
echo    Powered by ZeroFN
echo =====================================
echo.
echo Please select which season you want to install:
echo.
echo [1] Season 1 (Chapter 1) --REVOKED
echo [2] Season 2 (OG) --INSTALL THIS !!
echo [3] Exit
echo.
set /p season="Enter your choice (1-3): "

if "%season%"=="1" (
    set "DOWNLOAD_URL=https://cdn.fnbuilds.services/1.7.2.zip"
    set "ARCHIVE_NAME=1.7.2.zip"
    goto install_fortnite
)
if "%season%"=="2" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/1.11.zip"
    set "ARCHIVE_NAME=1.11.zip"
    goto install_fortnite
)
if "%season%"=="3" exit
goto season_select

:install_fortnite
cls
echo =====================================
echo    Installing Fortnite
echo    Powered by ZeroFN
echo =====================================
echo.
set "INSTALL_DIR=%BASE_PATH%\FortniteOG"

if exist "%INSTALL_DIR%" (
    echo The directory "%INSTALL_DIR%" already exists.
    echo Please delete or rename the existing folder and try again.
    pause
    exit
)

echo Downloading Fortnite files...
echo This may take a while depending on your internet speed.
powershell -Command "(New-Object Net.WebClient).DownloadFile('%DOWNLOAD_URL%', '%ARCHIVE_NAME%')"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Download failed. Please check your internet connection.
    pause
    exit
)

echo.
echo Extracting files...
powershell -Command "Expand-Archive -Path '%ARCHIVE_NAME%' -DestinationPath '%INSTALL_DIR%' -Force"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Extraction failed.
    pause
    exit
)

del "%ARCHIVE_NAME%"
echo.
echo Installation complete! Files are located in: %INSTALL_DIR%
echo.
pause
exit
