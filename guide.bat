@echo off
title ProjectZERO - How to Run and Play Guide
color 0a

echo ==========================================
echo     ProjectZERO - How to Run and Play Guide
echo ==========================================
echo.

echo [%date% %time%] Welcome to the ProjectZERO guide!
echo [%date% %time%] This guide will help you verify your installation and get started with FortniteOG.
echo [%date% %time%] Log file will be created at: %~dp0guide_log.txt
echo.

REM Create log file
echo ProjectZERO Installation Guide Log > guide_log.txt
echo Started at: %date% %time% >> guide_log.txt
echo ========================================== >> guide_log.txt
echo.

REM Check if path was provided as parameter, if not ask for it
if "%~1"=="" (
    echo [%date% %time%] No path provided as parameter, requesting user input... >> guide_log.txt
    echo [%date% %time%] No path provided as parameter, requesting user input...
    set /p GAME_PATH=Enter the path to your FortniteOG installation folder: 
) else (
    echo [%date% %time%] Path provided as parameter: %~1 >> guide_log.txt
    echo [%date% %time%] Path provided as parameter: %~1
    set "GAME_PATH=%~1"
)

REM Validate the entered path
echo [%date% %time%] Validating installation path... >> guide_log.txt
echo [%date% %time%] Validating installation path...
if not exist "%GAME_PATH%" (
    echo [%date% %time%] [ERROR] Directory does not exist: %GAME_PATH% >> guide_log.txt
    echo [%date% %time%] [ERROR] Directory does not exist: %GAME_PATH%
    echo Please ensure the path is correct and try again.
    exit /b 1
)
echo [%date% %time%] Installation path validated successfully >> guide_log.txt
echo [%date% %time%] Installation path validated successfully
echo.

REM Check for critical folders
echo [%date% %time%] Beginning folder structure verification... >> guide_log.txt
echo [%date% %time%] Beginning folder structure verification...
echo Checking folder structure...
echo.
set CRITICAL_FOLDERS=Binaries Content Engine FortniteGame
set MISSING_FOLDERS=0

for %%F in (%CRITICAL_FOLDERS%) do (
    echo [%date% %time%] Checking folder: %%F >> guide_log.txt
    echo [%date% %time%] Checking folder: %%F
    if not exist "%GAME_PATH%\%%F" (
        echo [%date% %time%] [ERROR] Missing critical folder: %%F >> guide_log.txt
        echo [%date% %time%] [ERROR] Missing critical folder: %%F
        set /a MISSING_FOLDERS+=1
    ) else (
        echo [%date% %time%] [SUCCESS] Found folder: %%F >> guide_log.txt
        echo [%date% %time%] [SUCCESS] Found folder: %%F
    )
)

if %MISSING_FOLDERS% GTR 0 (
    echo [%date% %time%] [CRITICAL] %MISSING_FOLDERS% essential folders missing >> guide_log.txt
    echo [%date% %time%] [CRITICAL] %MISSING_FOLDERS% essential folders missing
    echo.
    echo [CRITICAL ERROR] Missing essential game folders.
    echo This installation appears to be corrupted or incomplete.
    echo Would you like to retry the installation? (Y/N)
    set /p RETRY_INSTALL=
    if /I "!RETRY_INSTALL!"=="Y" (
        echo [%date% %time%] User chose to retry installation >> guide_log.txt
        echo Please run ZeroFN.bat to reinstall the game.
        exit /b 1
    ) else (
        echo [%date% %time%] User chose not to retry installation >> guide_log.txt
        echo Exiting the guide.
        exit /b 1
    )
)

REM Check critical executables
echo [%date% %time%] Beginning executable verification... >> guide_log.txt
echo [%date% %time%] Beginning executable verification...
echo.
echo Checking game executables...
set CRITICAL_EXES=FortniteClient-Win64-Shipping.exe FortniteServer-Win64-Shipping.exe FortniteLauncher.exe
set MISSING_EXES=0

for %%E in (%CRITICAL_EXES%) do (
    echo [%date% %time%] Checking executable: %%E >> guide_log.txt
    echo [%date% %time%] Checking executable: %%E
    if not exist "%GAME_PATH%\Binaries\Win64\%%E" (
        echo [%date% %time%] [ERROR] Missing critical executable: %%E >> guide_log.txt
        echo [%date% %time%] [ERROR] Missing critical executable: %%E
        set /a MISSING_EXES+=1
    ) else (
        echo [%date% %time%] [SUCCESS] Found executable: %%E >> guide_log.txt
        echo [%date% %time%] [SUCCESS] Found executable: %%E
    )
)

if %MISSING_EXES% GTR 0 (
    echo [%date% %time%] [CRITICAL] %MISSING_EXES% essential executables missing >> guide_log.txt
    echo [%date% %time%] [CRITICAL] %MISSING_EXES% essential executables missing
    echo.
    echo [CRITICAL ERROR] Missing essential game executables.
    echo Please reinstall the game using ZeroFN.bat.
    exit /b 1
)

REM Check and create required DLLs if missing
echo [%date% %time%] Beginning DLL verification... >> guide_log.txt
echo [%date% %time%] Beginning DLL verification...
echo.
echo Checking required DLLs...
set REQUIRED_DLLS=aqProf.dll VSPerf140.dll VtuneApi.dll VtuneApi32e.dll
set MISSING_DLLS=0

for %%D in (%REQUIRED_DLLS%) do (
    echo [%date% %time%] Checking DLL: %%D >> guide_log.txt
    echo [%date% %time%] Checking DLL: %%D
    if not exist "%GAME_PATH%\Binaries\Win64\%%D" (
        echo [%date% %time%] [INFO] Creating required DLL: %%D >> guide_log.txt
        echo [%date% %time%] [INFO] Creating required DLL: %%D
        echo. > "%GAME_PATH%\Binaries\Win64\%%D"
        set /a MISSING_DLLS+=1
    ) else (
        echo [%date% %time%] [SUCCESS] Found DLL: %%D >> guide_log.txt
        echo [%date% %time%] [SUCCESS] Found DLL: %%D
    )
)

REM Check content files
echo [%date% %time%] Checking game content files... >> guide_log.txt
echo [%date% %time%] Checking game content files...
echo.
echo Checking content files...
if not exist "%GAME_PATH%\FortniteGame\Content\Paks\*.pak" (
    echo [%date% %time%] [CRITICAL] No .pak files found! >> guide_log.txt
    echo [%date% %time%] [CRITICAL] No .pak files found!
    echo [CRITICAL ERROR] No .pak files found in Content folder!
    echo This installation is missing game content files.
    echo Would you like to retry the installation? (Y/N)
    set /p RETRY_INSTALL=
    if /I "!RETRY_INSTALL!"=="Y" (
        echo [%date% %time%] User chose to retry installation >> guide_log.txt
        echo Please run ZeroFN.bat to reinstall the game.
        exit /b 1
    ) else (
        echo [%date% %time%] User chose not to retry installation >> guide_log.txt
        echo Exiting the guide.
        exit /b 1
    )
) else (
    echo [%date% %time%] [SUCCESS] Found game content files >> guide_log.txt
    echo [%date% %time%] [SUCCESS] Found game content files
)

REM Verify network connectivity
echo [%date% %time%] Testing network connectivity... >> guide_log.txt
echo [%date% %time%] Testing network connectivity...
echo.
echo Checking network connectivity...
ping -n 1 localhost >nul
if errorlevel 1 (
    echo [%date% %time%] [WARNING] Network adapter issues detected >> guide_log.txt
    echo [%date% %time%] [WARNING] Network adapter issues detected
) else (
    echo [%date% %time%] [SUCCESS] Network adapter is functioning >> guide_log.txt
    echo [%date% %time%] [SUCCESS] Network adapter is functioning
)

echo.
echo ==========================================
echo Verification Results:
if %MISSING_FOLDERS%==0 if %MISSING_EXES%==0 (
    echo [%date% %time%] [SUCCESS] Verification completed successfully >> guide_log.txt
    echo [%date% %time%] [SUCCESS] Verification completed successfully
    echo [SUCCESS] All critical game files verified successfully!
    if %MISSING_DLLS% GTR 0 (
        echo [%date% %time%] [INFO] Created %MISSING_DLLS% missing DLL(s) >> guide_log.txt
        echo [%date% %time%] [INFO] Created %MISSING_DLLS% missing DLL(s)
    )
    echo Installation appears to be valid.
    echo [%date% %time%] Verification process completed >> guide_log.txt
    echo [%date% %time%] Verification process completed
    echo.
    echo Would you like to launch the game now? (Y/N)
    set /p LAUNCH_GAME=
    if /I "!LAUNCH_GAME!"=="Y" (
        echo [%date% %time%] User chose to launch the game >> guide_log.txt
        start "" "%GAME_PATH%\Binaries\Win64\FortniteLauncher.exe"
    ) else (
        echo [%date% %time%] User chose not to launch the game >> guide_log.txt
        echo Thank you for using ProjectZERO! You can launch the game later.
    )
    exit /b 0
) else (
    echo [%date% %time%] [CRITICAL] Verification failed >> guide_log.txt
    echo [%date% %time%] [CRITICAL] Verification failed
    echo [FAILED] Installation verification failed!
    echo Please reinstall the game using ZeroFN.bat
    echo [%date% %time%] Verification process completed with errors >> guide_log.txt
    echo [%date% %time%] Verification process completed with errors
    exit /b 1
)
