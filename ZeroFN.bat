@echo off
title ProjectZERO-ZeroFN
color 0f

REM Initialize logged in state
set "LOGGED_IN="
set "CLIENT_ID=xyza7891TydzdNolyGQJYa9b6n6rLMJl"
set "CLIENT_SECRET=Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"

:main_menu
cls
echo ==========================================
echo                ZeroFN     %LOGGED_IN%
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
    set /p choice="Enter your choice (1-3): "

    if "%choice%"=="1" goto epic_login
    if "%choice%"=="2" start https://discord.gg/yCY4FTMPdK && goto main_menu
    if "%choice%"=="3" exit
    goto main_menu
) else (
    echo Please choose an option:
    echo.
    echo [1] Specify FortniteOG Path
    echo [2] Install Fortnite OG
    echo [3] Join our Discord Community
    echo [4] Exit
    echo.
    set /p choice="Enter your choice (1-4): "

    if "%choice%"=="1" goto specify_path
    if "%choice%"=="2" goto season_select
    if "%choice%"=="3" start https://discord.gg/yCY4FTMPdK && goto main_menu
    if "%choice%"=="4" exit
    goto main_menu
)

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
echo Starting authentication server...
start "ZeroFN Auth Server" /wait cmd /c "cd /d %~dp0 && python auth.py"
timeout /t 3 >nul

echo Waiting for login completion...
echo Please complete the login in your browser.
echo The window will automatically close when done.
echo.
:wait_login
if exist "auth_token.json" (
    for /f "tokens=* usebackq delims=" %%a in (`powershell -Command "Get-Content auth_token.json | ConvertFrom-Json | Select -ExpandProperty displayName"`) do set "USERNAME=%%a"
    if "!USERNAME!"=="" (
        set "LOGGED_IN=(Guest)"
    ) else (
        set "LOGGED_IN=(Logged in successfull)"
    )
    for /f "tokens=* usebackq" %%a in ("auth_token.json") do set AUTH_TOKEN=%%a
    echo Login successful!
    timeout /t 2 >nul
    goto main_menu
)
timeout /t 1 >nul
goto wait_login

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
start "ZeroFN Server" /wait cmd /k "cd /d %~dp0 && python server.py && pause"
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

start "" "%GAME_EXE%" -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=%AUTH_TOKEN% -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck -notexturestreaming -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777 -AUTH_SSL=0 -AUTH_VERIFY_SSL=0 -AUTH_EPIC=0 -AUTH_EPIC_ONLY=0 -FORCECLIENT=127.0.0.1:7777 -NOEPICWEB -NOEPICFRIENDS -NOEAC -NOBE -FORCECLIENT_HOST=127.0.0.1:7777 -DISABLEFORTNITELOGIN -DISABLEEPICLOGIN -DISABLEEPICGAMESLOGIN -DISABLEEPICGAMESPORTAL -DISABLEEPICGAMESVERIFY -epicport=7777

echo Game launched!
timeout /t 2 >nul
goto menu

:start_both
cls
echo Starting server and game...
echo Starting ZeroFN Server...
start "ZeroFN Server" cmd /k "cd /d %~dp0 && python server.py"
echo Server started! Waiting for server to initialize...
timeout /t 5 >nul

echo Launching Fortnite...
cd /d "%GAME_EXE%\.."

taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1

start "" "%GAME_EXE%" -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=%AUTH_TOKEN% -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck -notexturestreaming -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777 -AUTH_SSL=0 -AUTH_VERIFY_SSL=0 -AUTH_EPIC=0 -AUTH_EPIC_ONLY=0 -FORCECLIENT=127.0.0.1:7777 -NOEPICWEB -NOEPICFRIENDS -NOEAC -NOBE -FORCECLIENT_HOST=127.0.0.1:7777 -DISABLEFORTNITELOGIN -DISABLEEPICLOGIN -DISABLEEPICGAMESLOGIN -DISABLEEPICGAMESPORTAL -DISABLEEPICGAMESVERIFY -epicport=7777

echo Server and game started successfully!
timeout /t 2 >nul
goto menu
