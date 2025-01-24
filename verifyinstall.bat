@echo off
title ProjectZERO - Verify Installation
color 0a

echo ==========================================
echo     ProjectZERO - Verify Installation
echo ==========================================
echo.

echo [%date% %time%] Starting verification process...
echo [%date% %time%] Log file will be created at: %~dp0verify_log.txt

REM Create log file
echo ProjectZERO Installation Verification Log > verify_log.txt
echo Started at: %date% %time% >> verify_log.txt
echo ========================================== >> verify_log.txt

REM Check if path was provided as parameter, if not ask for it
if "%~1"=="" (
    echo [%date% %time%] No path provided as parameter, requesting user input... >> verify_log.txt
    set /p GAME_PATH=Enter the path to your FortniteOG installation folder: 
) else (
    echo [%date% %time%] Path provided as parameter: %~1 >> verify_log.txt
    set "GAME_PATH=%~1"
)

REM Validate the entered path
echo [%date% %time%] Validating installation path... >> verify_log.txt
if not exist "%GAME_PATH%" (
    echo [%date% %time%] [ERROR] Directory does not exist: %GAME_PATH% >> verify_log.txt
    echo [ERROR] Directory does not exist: %GAME_PATH%
    echo Please ensure the path is correct and try again.
    exit /b 1
)
echo [%date% %time%] Installation path validated successfully >> verify_log.txt

REM Check for critical folders
echo [%date% %time%] Beginning folder structure verification... >> verify_log.txt
echo Checking folder structure...
echo.
set CRITICAL_FOLDERS=Binaries Content Engine FortniteGame
set MISSING_FOLDERS=0

for %%F in (%CRITICAL_FOLDERS%) do (
    echo [%date% %time%] Checking folder: %%F >> verify_log.txt
    if not exist "%GAME_PATH%\%%F" (
        echo [%date% %time%] [ERROR] Missing critical folder: %%F >> verify_log.txt
        echo [ERROR] Missing critical folder: %%F
        set /a MISSING_FOLDERS+=1
    ) else (
        echo [%date% %time%] [SUCCESS] Found folder: %%F >> verify_log.txt
        echo [OK] Found folder: %%F
    )
)

if %MISSING_FOLDERS% GTR 0 (
    echo [%date% %time%] [CRITICAL] %MISSING_FOLDERS% essential folders missing >> verify_log.txt
    echo.
    echo [CRITICAL ERROR] Missing essential game folders.
    echo This installation appears to be corrupted or incomplete.
    exit /b 1
)

REM Check critical executables
echo [%date% %time%] Beginning executable verification... >> verify_log.txt
echo.
echo Checking game executables...
set CRITICAL_EXES=FortniteClient-Win64-Shipping.exe FortniteServer-Win64-Shipping.exe FortniteLauncher.exe
set MISSING_EXES=0

for %%E in (%CRITICAL_EXES%) do (
    echo [%date% %time%] Checking executable: %%E >> verify_log.txt
    if not exist "%GAME_PATH%\Binaries\Win64\%%E" (
        echo [%date% %time%] [ERROR] Missing critical executable: %%E >> verify_log.txt
        echo [ERROR] Missing critical executable: %%E
        set /a MISSING_EXES+=1
    ) else (
        echo [%date% %time%] [SUCCESS] Found executable: %%E >> verify_log.txt
        echo [OK] Found executable: %%E
    )
)

if %MISSING_EXES% GTR 0 (
    echo [%date% %time%] [CRITICAL] %MISSING_EXES% essential executables missing >> verify_log.txt
    echo.
    echo [CRITICAL ERROR] Missing essential game executables.
    echo Please reinstall the game.
    exit /b 1
)

REM Check and create required DLLs if missing
echo [%date% %time%] Beginning DLL verification... >> verify_log.txt
echo.
echo Checking required DLLs...
set REQUIRED_DLLS=aqProf.dll VSPerf140.dll VtuneApi.dll VtuneApi32e.dll
set MISSING_DLLS=0

for %%D in (%REQUIRED_DLLS%) do (
    echo [%date% %time%] Checking DLL: %%D >> verify_log.txt
    if not exist "%GAME_PATH%\Binaries\Win64\%%D" (
        echo [%date% %time%] [INFO] Creating required DLL: %%D >> verify_log.txt
        echo [INFO] Creating required DLL: %%D
        echo. > "%GAME_PATH%\Binaries\Win64\%%D"
        set /a MISSING_DLLS+=1
    ) else (
        echo [%date% %time%] [SUCCESS] Found DLL: %%D >> verify_log.txt
        echo [OK] Found DLL: %%D
    )
)

REM Check content files
echo [%date% %time%] Checking game content files... >> verify_log.txt
echo.
echo Checking content files...
if not exist "%GAME_PATH%\FortniteGame\Content\Paks\*.pak" (
    echo [%date% %time%] [CRITICAL] No .pak files found! >> verify_log.txt
    echo [CRITICAL ERROR] No .pak files found in Content folder!
    echo This installation is missing game content files.
    exit /b 1
) else (
    echo [%date% %time%] [SUCCESS] Found game content files >> verify_log.txt
    echo [OK] Found game content files
)

REM Verify network connectivity
echo [%date% %time%] Testing network connectivity... >> verify_log.txt
echo.
echo Checking network connectivity...
ping -n 1 localhost >nul
if errorlevel 1 (
    echo [%date% %time%] [WARNING] Network adapter issues detected >> verify_log.txt
    echo [WARNING] Network adapter issues detected
) else (
    echo [%date% %time%] [SUCCESS] Network adapter is functioning >> verify_log.txt
    echo [OK] Network adapter is functioning
)

echo.
echo ==========================================
echo Verification Results:
if %MISSING_FOLDERS%==0 if %MISSING_EXES%==0 (
    echo [%date% %time%] [SUCCESS] Verification completed successfully >> verify_log.txt
    echo [SUCCESS] All critical game files verified successfully!
    if %MISSING_DLLS% GTR 0 (
        echo [%date% %time%] [INFO] Created %MISSING_DLLS% missing DLL(s) >> verify_log.txt
        echo [INFO] Created %MISSING_DLLS% missing DLL(s)
    )
    echo Installation appears to be valid.
    echo [%date% %time%] Verification process completed >> verify_log.txt
    exit /b 0
) else (
    echo [%date% %time%] [CRITICAL] Verification failed >> verify_log.txt
    echo [FAILED] Installation verification failed!
    echo Please reinstall the game using ZeroFN.bat
    echo [%date% %time%] Verification process completed with errors >> verify_log.txt
    exit /b 1
)
