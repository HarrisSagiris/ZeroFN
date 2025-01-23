@echo off
title ProjectZERO-ZeroFN
color 0f

REM Initial menu to choose between specifying a path or installing Fortnite OG
:main_menu
cls
echo ==========================================
echo    ProjectZERO 
echo    Created by root404 and the ZeroFN team
echo ==========================================
echo.
echo Please choose an option:
echo.
echo [1] Specify FortniteOG Path
echo [2] Install Fortnite OG (Downloads Fortnite OG to a folder named 'FortniteOG' in the current directory)
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
echo    Select Fortnite Season
echo    Powered by ZeroFN
echo =====================================
echo.
echo Please select which season you want to install:
echo.
echo [1] Season 1 
echo [2] Season 2 (OG)
echo [3] Season 3
echo [4] Season 4
echo [5] Season 5
echo [6] Season 6
echo [7] Season 7
echo [8] Back to Main Menu
echo.
set /p season="Enter your choice (1-8): "

if "%season%"=="1" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/1.7.2.zip"
    set "ARCHIVE_NAME=1.7.2.zip"
    goto install_fortnite_og
)
if "%season%"=="2" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/1.11.zip"
    set "ARCHIVE_NAME=1.11.zip"
    goto install_fortnite_og
)
if "%season%"=="3" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/3.0.zip"
    set "ARCHIVE_NAME=3.0.zip"
    goto install_fortnite_og
)
if "%season%"=="4" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/4.0.zip"
    set "ARCHIVE_NAME=4.0.zip"
    goto install_fortnite_og
)
if "%season%"=="5" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/5.00.rar"
    set "ARCHIVE_NAME=5.00.rar"
    goto install_fortnite_og
)
if "%season%"=="6" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/6.00.rar"
    set "ARCHIVE_NAME=6.00.rar"
    goto install_fortnite_og
)
if "%season%"=="7" (
    set "DOWNLOAD_URL=https://public.simplyblk.xyz/7.00.rar"
    set "ARCHIVE_NAME=7.00.rar"
    goto install_fortnite_og
)
if "%season%"=="8" goto main_menu
goto season_select

:epic_login
cls
echo =====================================
echo    Epic Games Authentication
echo    Powered by ZeroFN
echo =====================================
echo.
echo Opening Epic Games login in your browser...
start "" "https://www.epicgames.com/id/api/redirect?clientId=xyza7891TydzdNolyGQJYa9b6n6rLMJl&responseType=code&redirectUrl=eos.xyza7891TydzdNolyGQJYa9b6n6rLMJl://epic/auth"
echo Please login to your Epic Games account in the browser.
echo After logging in, you'll be redirected back to the launcher.
echo.
pause
goto main_menu

:install_fortnite_og
cls
echo =====================================
echo    Installing Fortnite
echo    Powered by ZeroFN
echo =====================================
echo.
set "INSTALL_DIR=%cd%\FortniteOG"

if exist "%INSTALL_DIR%" (
    echo The directory "%INSTALL_DIR%" already exists.
    echo Skipping download, verifying files...
    pause
    goto locate_executable
)

echo This procedure depends on your internet speeds..it can take time..
echo Downloading Fortnite files from: %DOWNLOAD_URL%
curl -o "%ARCHIVE_NAME%" "%DOWNLOAD_URL%"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Failed to download files. Please check your internet connection and try again.
    pause
    exit /b 1
)

echo Extracting files to "%INSTALL_DIR%"...
mkdir "%INSTALL_DIR%"
if "%ARCHIVE_NAME:~-3%"=="zip" (
    tar -xf "%ARCHIVE_NAME%" -C "%INSTALL_DIR%"
) else (
    echo Please extract the RAR file manually to: %INSTALL_DIR%
    echo Press any key after extraction is complete...
    pause >nul
)
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Failed to extract files. Ensure 'tar' is available or manually extract "%ARCHIVE_NAME%".
    pause
    exit /b 1
)

echo.
echo Fortnite successfully installed in "%INSTALL_DIR%".
del "%ARCHIVE_NAME%"
pause
set "GAME_PATH=%INSTALL_DIR%"
goto locate_executable

:specify_path
cls
echo =====================================
echo    Specify FortniteOG Path
echo    Powered by ZeroFN
echo =====================================
echo.
echo Please enter the path to your FortniteOG folder:
echo Example: C:\Users\username\Desktop\ProjectZERO\FortniteOG
echo.
set /p GAME_PATH="Path: "

REM Validate the specified path
if not exist "%GAME_PATH%" (
    echo.
    echo Error: The specified path does not exist: %GAME_PATH%
    echo Please verify the path and try again.
    pause
    goto specify_path
)

:locate_executable
REM Set the exact path for FortniteOG executable
set "GAME_EXE=%GAME_PATH%\FortniteGame\Binaries\Win64\FortniteClient-Win64-Shipping.exe"

REM Validate the executable exists
if not exist "%GAME_EXE%" (
    echo.
    echo Error: Could not locate FortniteClient-Win64-Shipping.exe at:
    echo %GAME_EXE%
    echo.
    echo Please ensure your Fortnite installation is complete and contains all required files.
    pause
    goto specify_path
)

echo Found Fortnite executable at: %GAME_EXE%
goto menu

:menu
cls
echo =====================================
echo    ProjectZERO
echo    Powered by ZeroFN
echo =====================================
echo.
echo Current Fortnite executable: %GAME_EXE%
echo.
echo [1] Start Server Only
echo [2] Launch Game Only
echo [3] Start Both (Game + Server)
echo [4] Exit 
echo.
set /p choice="Enter your choice (1-4): "

if "%choice%"=="1" goto start_server_only
if "%choice%"=="2" goto launch_game
if "%choice%"=="3" goto start_hybrid
if "%choice%"=="4" exit
goto menu

:start_server_only
cls
echo =====================================
echo    Starting ProjectZERO server only
echo    Powered by ZeroFN
echo =====================================
echo.
echo [INFO] Initializing ZeroFN server on 127.0.0.1:7777
echo [INFO] Using game files from: %GAME_EXE%
echo.
start cmd /k "title ZeroFN Server && echo [INFO] Starting server process... && python server.py"
echo [SUCCESS] Server window opened with live logs
pause
goto menu

:launch_game
cls
echo =====================================
echo    Launching Game Client
echo    Powered by ZeroFN
echo =====================================
echo.
cd /d "%GAME_EXE%\.."

echo [INFO] Starting ZeroFN server first...
start cmd /k "title ZeroFN Server && echo [INFO] Starting server process... && python server.py"

echo [INFO] Waiting 45 seconds for server to fully initialize...
timeout /t 45 >nul

echo [INFO] Cleaning up existing processes...
taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1
taskkill /f /im FortniteLauncher.exe >nul 2>&1
taskkill /f /im EpicGamesLauncher.exe >nul 2>&1

echo [INFO] Cleaning temporary files and logs...
del /f /q "%localappdata%\FortniteGame\Saved\Logs\*.*" >nul 2>&1
del /f /q "%localappdata%\CrashReportClient\*.*" >nul 2>&1
del /f /q "%localappdata%\FortniteGame\Saved\Config\CrashReportClient\*.*" >nul 2>&1

echo [INFO] Setting compatibility flags...
reg add "HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%GAME_EXE%" /t REG_SZ /d "~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE DISABLETHEMES" /f >nul 2>&1

echo [INFO] Launching game with hybrid configuration...
start "" "%GAME_EXE%" -HTTP=http://127.0.0.1:7777 ^
-NOSPLASH -USEALLAVAILABLECORES -dx11 ^
-AUTH_TYPE=epic ^
-AUTH_LOGIN=unused ^
-AUTH_PASSWORD=unused ^
-epicapp=Fortnite ^
-epicenv=Prod ^
-epiclocale=en-us ^
-epicportal ^
-noeac -nobe -fromfl=be -fltoken= ^
-nolog ^
-NOSSLPINNING ^
-preferredregion=NAE ^
-skippatchcheck ^
-notexturestreaming ^
-AUTH_HOST=127.0.0.1:7777 ^
-AUTH_SSL=0 ^
-AUTH_VERIFY_SSL=0 ^
-FORCECLIENT=127.0.0.1:7777 ^
-NOBE ^
-FORCECLIENT_HOST=127.0.0.1:7777

echo [SUCCESS] Fortnite client launched in hybrid mode!
echo [INFO] Using Epic authentication with custom server
echo.
timeout /t 3
goto menu

:start_hybrid
cls
echo =====================================
echo    Starting ProjectZERO Hybrid Mode (Game + Server)
echo    Powered by ZeroFN
echo =====================================
echo.
echo [INFO] Initializing ZeroFN server on 127.0.0.1:7777
echo [INFO] Using game files from: %GAME_EXE%
echo.

echo [INFO] Starting ZeroFN server process...
start cmd /k "title ZeroFN Server && echo [INFO] Starting server process... && python server.py"
echo [SUCCESS] Server initialized

echo [INFO] Waiting 45 seconds for server to fully initialize...DONT CLOSE THE SERVER WINDOW
timeout /t 45 >nul

cd /d "%GAME_EXE%\.."

echo [INFO] Cleaning up existing processes...
taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1
taskkill /f /im FortniteLauncher.exe >nul 2>&1
taskkill /f /im EpicGamesLauncher.exe >nul 2>&1

echo [INFO] Cleaning temporary files and logs...
del /f /q "%localappdata%\FortniteGame\Saved\Logs\*.*" >nul 2>&1
del /f /q "%localappdata%\CrashReportClient\*.*" >nul 2>&1
del /f /q "%localappdata%\FortniteGame\Saved\Config\CrashReportClient\*.*" >nul 2>&1

echo [INFO] Setting compatibility flags...
reg add "HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%GAME_EXE%" /t REG_SZ /d "~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE DISABLETHEMES" /f >nul 2>&1

echo [INFO] Launching game with hybrid configuration...
start "" "%GAME_EXE%" -HTTP=http://127.0.0.1:7777 ^
-NOSPLASH -USEALLAVAILABLECORES -dx11 ^
-AUTH_TYPE=epic ^
-AUTH_LOGIN=unused ^
-AUTH_PASSWORD=unused ^
-epicapp=Fortnite ^
-epicenv=Prod ^
-epiclocale=en-us ^
-epicportal ^
-noeac -nobe -fromfl=be -fltoken= ^
-nolog ^
-NOSSLPINNING ^
-preferredregion=NAE ^
-skippatchcheck ^
-notexturestreaming ^
-AUTH_HOST=127.0.0.1:7777 ^
-AUTH_SSL=0 ^
-AUTH_VERIFY_SSL=0 ^
-FORCECLIENT=127.0.0.1:7777 ^
-NOBE ^
-FORCECLIENT_HOST=127.0.0.1:7777

echo [SUCCESS] Game client launched in hybrid mode!
echo [INFO] Server is running in separate window with live logs
echo [INFO] Using Epic authentication with custom server
echo.
echo Press any key to return to menu...
pause >nul
goto menu
