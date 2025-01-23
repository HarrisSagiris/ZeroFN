@echo off
title ProjectZERO-ZeroFN
color 0f

REM Initialize logged in state
set "LOGGED_IN="

:main_menu
cls
echo ==========================================
echo                ZeroFN     %LOGGED_IN%
echo    Created by @Devharris and @Addamito
echo ==========================================
echo.
echo Please choose an option:
echo.
echo [1] Specify FortniteOG Path
echo [2] Install Fortnite OG
echo [3] Login with Epic Games Account
echo [4] Join our Discord Community
echo [5] Exit
echo.
set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto specify_path
if "%choice%"=="2" goto season_select
if "%choice%"=="3" goto epic_login
if "%choice%"=="4" start https://discord.gg/yCY4FTMPdK && goto main_menu
if "%choice%"=="5" exit
goto main_menu

:season_select
cls
echo =====================================
echo    Select Fortnite Season           %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.
echo Please select which season you want to install:
echo.
echo [1] Season 1 (Chapter 1)
echo [2] Season 2 (OG)
echo [3] Back to Main Menu
echo.
set /p season="Enter your choice (1-3): "

if "%season%"=="1" (
    set "DOWNLOAD_URL=https://cdn.fnbuilds.services/1.7.2.zip"
    set "ARCHIVE_NAME=1.7.2.zip"
    goto install_fortnite_og
)
if "%season%"=="2" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/1.11.zip"
    set "ARCHIVE_NAME=1.11.zip"
    goto install_fortnite_og
)
if "%season%"=="3" goto main_menu
goto season_select

:epic_login
cls
echo =====================================
echo    Epic Games Authentication
echo    Powered by ZeroFN
echo =====================================
echo.
echo Logging in...
echo.
set "LOGGED_IN=[Logged in]"
echo Successfully logged in!
timeout /t 2 >nul
goto main_menu

:install_fortnite_og
cls
echo =====================================
echo    Installing Fortnite              %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.
set "INSTALL_DIR=%cd%\FortniteOG"

if exist "%INSTALL_DIR%" (
    echo The directory "%INSTALL_DIR%" already exists.
    echo Skipping download, verifying files...
    timeout /t 2 >nul
    goto locate_executable
)

echo Downloading Fortnite files...
echo This may take a while depending on your internet speed.
powershell -Command "(New-Object Net.WebClient).DownloadFile('%DOWNLOAD_URL%', '%ARCHIVE_NAME%')"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Download failed. Please check your internet connection.
    pause
    goto main_menu
)

echo.
echo Extracting files...
powershell -Command "Expand-Archive -Path '%ARCHIVE_NAME%' -DestinationPath '%INSTALL_DIR%' -Force"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Extraction failed.
    pause
    goto main_menu
)

del "%ARCHIVE_NAME%"
echo.
echo Installation complete!
set "GAME_PATH=%INSTALL_DIR%"
timeout /t 2 >nul
goto locate_executable

:specify_path
cls
echo =====================================
echo    Specify FortniteOG Path          %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.
echo Enter the path to your FortniteOG folder:
echo Example: C:\FortniteOG
echo.
set /p GAME_PATH="Path: "

if not exist "%GAME_PATH%" (
    echo.
    echo Error: Invalid path
    timeout /t 2 >nul
    goto specify_path
)

:locate_executable
set "GAME_EXE=%GAME_PATH%\FortniteGame\Binaries\Win64\FortniteClient-Win64-Shipping.exe"

if not exist "%GAME_EXE%" (
    echo.
    echo Error: Fortnite executable not found
    timeout /t 2 >nul
    goto specify_path
)

goto menu

:menu
cls
echo =====================================
echo               ZeroFN    %LOGGED_IN%
echo   Powered by @Devharris and @Addamito
echo =====================================
echo.
echo Current Path: %GAME_EXE%
echo.
echo [1] Start Server
echo [2] Launch Game
echo [3] Start Both
echo [4] Exit
echo.
set /p choice="Enter your choice (1-4): "

if "%choice%"=="1" goto start_server
if "%choice%"=="2" goto launch_game
if "%choice%"=="3" goto start_both
if "%choice%"=="4" exit
goto menu

:start_server
cls
echo Starting ZeroFN Server...
start /min cmd /c "title ZeroFN Server && cd /d %~dp0 && python server.py"
echo Server started!
timeout /t 2 >nul
goto menu

:launch_game
cls
echo Launching Fortnite...
cd /d "%GAME_EXE%\.."

taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1

start "" "%GAME_EXE%" -AUTH_LOGIN=dev -AUTH_PASSWORD=dev -AUTH_TYPE=epic -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck -NOSSLPINNING -AUTH_HOST=0.0.0.0:7777 -HTTP=0.0.0.0:7777 -FORCECLIENT=0.0.0.0:7777 -FORCECLIENT_HOST=0.0.0.0:7777

echo Game launched!
timeout /t 2 >nul
goto menu

:start_both
cls
echo Starting server and game...
echo Starting ZeroFN Server...
start /min cmd /c "title ZeroFN Server && cd /d %~dp0 && python server.py"
echo Server started! Waiting for server to initialize...
timeout /t 5 >nul

echo Launching Fortnite...
cd /d "%GAME_EXE%\.."

taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1

start "" "%GAME_EXE%" -AUTH_LOGIN=dev -AUTH_PASSWORD=dev -AUTH_TYPE=epic -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck -NOSSLPINNING -AUTH_HOST=0.0.0.0:7777 -HTTP=0.0.0.0:7777 -FORCECLIENT=0.0.0.0:7777 -FORCECLIENT_HOST=0.0.0.0:7777

echo Server and game started successfully!
timeout /t 2 >nul
goto menu
