@echo off
setlocal EnableDelayedExpansion
title ProjectZERO-ZeroFN
color 0f

REM Initialize variables
set "LOGGED_IN="
set "CLIENT_ID=xyza7891TydzdNolyGQJYa9b6n6rLMJl"
set "CLIENT_SECRET=Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
set "CONFIG_FILE=%~dp0config.json"

REM Create config file if it doesn't exist
if not exist "%CONFIG_FILE%" (
    echo {"game_path": ""} > "%CONFIG_FILE%"
)

REM Load saved game path
for /f "tokens=* usebackq" %%a in (`powershell -Command "Get-Content '%CONFIG_FILE%' | ConvertFrom-Json | Select -ExpandProperty game_path"`) do set "SAVED_GAME_PATH=%%a"

REM Load auth token if exists
if exist "auth_token.json" (
    for /f "tokens=* usebackq delims=" %%a in (`powershell -Command "Get-Content auth_token.json | ConvertFrom-Json | Select -ExpandProperty access_token"`) do set "AUTH_TOKEN=%%a"
    for /f "tokens=* usebackq delims=" %%a in (`powershell -Command "Get-Content auth_token.json | ConvertFrom-Json | Select -ExpandProperty displayName"`) do set "USERNAME=%%a"
    if not "!USERNAME!"=="" set "LOGGED_IN=(Logged in as !USERNAME!)"
)

:main_menu
cls
echo ==========================================
echo             ZeroFN Launcher    %LOGGED_IN%
echo    Created by @Devharris and @Addamito
echo ==========================================
echo.

if "%LOGGED_IN%"=="" (
    echo Please choose an option:
    echo.
    echo [1] Login with Epic Games Account
    echo [2] Join our Discord Community
    echo [3] Exit
    echo.
    choice /c 123 /n /m "Enter your choice (1-3): "
    set choice=!errorlevel!

    if "!choice!"=="1" goto epic_login
    if "!choice!"=="2" start https://discord.gg/yCY4FTMPdK && goto main_menu
    if "!choice!"=="3" exit /b
) else (
    if not "!SAVED_GAME_PATH!"=="" (
        echo Current Installation: !SAVED_GAME_PATH!
        echo.
    )
    echo Please choose an option:
    echo.
    echo [1] Install/Update Fortnite OG
    echo [2] Launch Game (Client Only)
    echo [3] Launch Game and server (Hybrid Mode)
    echo [4] Manage Installation
    echo [5] Join Discord Community
    echo [6] Logout
    echo [7] Exit
    echo.
    choice /c 1234567 /n /m "Enter your choice (1-7): "
    set choice=!errorlevel!

    if "!choice!"=="1" goto season_select
    if "!choice!"=="2" goto launch_game
    if "!choice!"=="3" goto hybrid_launch
    if "!choice!"=="4" goto manage_installation
    if "!choice!"=="5" start https://discord.gg/yCY4FTMPdK && goto main_menu
    if "!choice!"=="6" (
        del /f /q auth_token.json 2>nul
        set "LOGGED_IN="
        set "AUTH_TOKEN="
        set "USERNAME="
        goto main_menu
    )
    if "!choice!"=="7" exit /b
)
goto main_menu

:hybrid_launch
if "!SAVED_GAME_PATH!"=="" (
    echo No installation found. Please install the game first.
    timeout /t 3 >nul
    goto main_menu
)

if "!AUTH_TOKEN!"=="" (
    echo You must be logged in to launch the game.
    timeout /t 3 >nul
    goto main_menu
)

cls
echo Starting ZeroFN Server...
taskkill /f /im python.exe >nul 2>&1
start "ZeroFN Server" /min cmd /c "python server.py"
echo Server started! Waiting for initialization...
timeout /t 5 >nul

cd /d "!SAVED_GAME_PATH!\FortniteGame\Binaries\Win64"

taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1

start "" "FortniteClient-Win64-Shipping.exe" -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=!AUTH_TOKEN! -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck -notexturestreaming -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777 -AUTH_SSL=0 -AUTH_VERIFY_SSL=0 -AUTH_EPIC=0 -AUTH_EPIC_ONLY=0 -FORCECLIENT=127.0.0.1:7777 -NOEPICWEB -NOEPICFRIENDS -NOEAC -NOBE -FORCECLIENT_HOST=127.0.0.1:7777 -DISABLEFORTNITELOGIN -DISABLEEPICLOGIN -DISABLEEPICGAMESLOGIN -DISABLEEPICGAMESPORTAL -DISABLEEPICGAMESVERIFY -epicport=7777

echo Game launched in hybrid mode!
timeout /t 2 >nul
goto main_menu

:launch_game
if "!SAVED_GAME_PATH!"=="" (
    echo No installation found. Please install the game first.
    timeout /t 3 >nul
    goto main_menu
)

if "!AUTH_TOKEN!"=="" (
    echo You must be logged in to launch the game.
    timeout /t 3 >nul
    goto main_menu
)

cls
echo Launching game in client-only mode...
cd /d "!SAVED_GAME_PATH!\FortniteGame\Binaries\Win64"

taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1

start "" "FortniteClient-Win64-Shipping.exe" -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=!AUTH_TOKEN! -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck -notexturestreaming

echo Game launched!
timeout /t 2 >nul
goto main_menu

:manage_installation
cls
echo =====================================
echo    Installation Manager    %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.
echo [1] Change Installation Path
echo [2] Verify Game Files
echo [3] Back to Main Menu
echo.
choice /c 123 /n /m "Enter your choice (1-3): "
set choice=!errorlevel!

if "!choice!"=="1" goto specify_path
if "!choice!"=="2" goto verify_files
if "!choice!"=="3" goto main_menu
goto manage_installation

:verify_files
cls
echo Verifying game files...
if not exist "!SAVED_GAME_PATH!\FortniteGame\Binaries\Win64\FortniteClient-Win64-Shipping.exe" (
    echo Game files are missing or corrupted. Please reinstall.
    timeout /t 3 >nul
    goto main_menu
)
echo Game files verified successfully!
timeout /t 2 >nul
goto main_menu

:season_select
cls
echo =====================================
echo    Select Fortnite Season    %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.
echo Available versions:
echo.
echo [1] Chapter 1 Season 1 (1.7.2)
echo [2] OG Season (1.11) - only that works in ZeroFN v1.1 
echo [3] Back to Main Menu
echo.
choice /c 123 /n /m "Enter your choice (1-3): "
set choice=!errorlevel!

if "!choice!"=="1" (
    set "DOWNLOAD_URL=https://cdn.fnbuilds.services/1.7.2.zip"
    set "ARCHIVE_NAME=1.7.2.zip"
    set "VERSION=Season 1"
    goto install_fortnite_og
)
if "!choice!"=="2" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/1.11.zip"
    set "ARCHIVE_NAME=1.11.zip"
    set "VERSION=OG Season"
    goto install_fortnite_og
)
if "!choice!"=="3" goto main_menu
goto season_select

:epic_login
cls
echo =====================================
echo    Epic Games Authentication
echo    Powered by ZeroFN
echo =====================================
echo.
echo Starting authentication server...

REM Check if Python is installed
python --version >nul 2>&1
if !errorlevel! neq 0 (
    echo Python is not installed! Please install Python 3.x to continue.
    echo Download from: https://www.python.org/downloads/
    pause
    goto main_menu
)

REM Check for required Python packages
python -c "import requests" >nul 2>&1
if !errorlevel! neq 0 (
    echo Installing required packages...
    pip install requests >nul 2>&1
)

REM Kill any existing auth server
taskkill /f /im python.exe >nul 2>&1

REM Start auth server
start "ZeroFN Auth Server" /min cmd /c "python auth.py"
timeout /t 3 >nul

echo Waiting for login completion...
echo Please complete the login in your browser.
echo.

:wait_login
if exist "auth_token.json" (
    for /f "tokens=* usebackq delims=" %%a in (`powershell -Command "Get-Content auth_token.json | ConvertFrom-Json | Select -ExpandProperty access_token"`) do set "AUTH_TOKEN=%%a"
    for /f "tokens=* usebackq delims=" %%a in (`powershell -Command "Get-Content auth_token.json | ConvertFrom-Json | Select -ExpandProperty displayName"`) do set "USERNAME=%%a"
    
    if "!USERNAME!"=="" (
        set "LOGGED_IN=(Guest)"
    ) else (
        set "LOGGED_IN=(Logged in as !USERNAME!)"
    )
    echo Login successful! Welcome !USERNAME!!
    timeout /t 2 >nul
    goto main_menu
)
timeout /t 1 >nul
goto wait_login

:install_fortnite_og
cls
echo =====================================
echo    Installing Fortnite %VERSION%    %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.

if "!SAVED_GAME_PATH!"=="" (
    set "INSTALL_DIR=%~dp0FortniteOG"
) else (
    set "INSTALL_DIR=!SAVED_GAME_PATH!"
)

echo Installing to: !INSTALL_DIR!
echo.
choice /c YN /n /m "Continue with installation? (Y/N) "
if !errorlevel! equ 2 goto main_menu

if exist "!INSTALL_DIR!" (
    echo Cleaning up existing installation...
    rmdir /s /q "!INSTALL_DIR!" >nul 2>&1
)

echo.
echo Downloading Fortnite files...
echo This may take a while depending on your internet speed.
powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest '%DOWNLOAD_URL%' -OutFile '%ARCHIVE_NAME%'"
if !errorlevel! neq 0 (
    echo Download failed. Please check your internet connection.
    timeout /t 3 >nul
    goto main_menu
)

echo.
echo Extracting files...
powershell -Command "Expand-Archive -Path '%ARCHIVE_NAME%' -DestinationPath '!INSTALL_DIR!' -Force"
if !errorlevel! neq 0 (
    echo Extraction failed.
    del "%ARCHIVE_NAME%" >nul 2>&1
    timeout /t 3 >nul
    goto main_menu
)

del "%ARCHIVE_NAME%" >nul 2>&1

REM Save installation path to config
echo {"game_path": "!INSTALL_DIR!"} > "%CONFIG_FILE%"
set "SAVED_GAME_PATH=!INSTALL_DIR!"

echo.
echo Installation complete!
timeout /t 2 >nul
goto main_menu

:specify_path
cls
echo =====================================
echo    Specify FortniteOG Path    %LOGGED_IN%
echo    Powered by ZeroFN
echo =====================================
echo.
echo Current path: !SAVED_GAME_PATH!
echo.
echo Enter the path to your FortniteOG folder:
echo Example: C:\FortniteOG
echo.
set /p "NEW_PATH=Path: "

if not exist "!NEW_PATH!\FortniteGame\Binaries\Win64\FortniteClient-Win64-Shipping.exe" (
    echo.
    echo Error: Invalid Fortnite installation path
    timeout /t 2 >nul
    goto specify_path
)

echo {"game_path": "!NEW_PATH!"} > "%CONFIG_FILE%"
set "SAVED_GAME_PATH=!NEW_PATH!"
echo.
echo Path updated successfully!
timeout /t 2 >nul
goto main_menu
