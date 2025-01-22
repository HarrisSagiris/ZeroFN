@echo off
title ProjectZERO - Verify Installation
color 0a

echo ==========================================
echo     ProjectZERO - Verify Installation
echo ==========================================
echo.

REM Prompt user to input the installation directory
set /p GAME_PATH=Enter the path to your Fortnite installation folder (e.g., C:\Fortnite): 

REM Validate the entered path
if not exist "%GAME_PATH%" (
    echo [ERROR] Directory does not exist: %GAME_PATH%
    echo Please ensure the path is correct and try again.
    pause
    exit /b
)

REM Set filenames for required files and DLLs
set REQUIRED_DLLS=aqProf.dll VSPerf140.dll VtuneApi.dll VtuneApi32e.dll
set REQUIRED_FILES=FortniteClient-Win64-Shipping.exe FortniteServer-Win64-Shipping.exe

REM Check game binary directory
echo Checking essential files in: %GAME_PATH%\Binaries\Win64
echo.

REM Check for required files in the game binaries folder
for %%F in (%REQUIRED_FILES%) do (
    if not exist "%GAME_PATH%\Binaries\Win64\%%F" (
        echo [ERROR] Missing file: %%F
    ) else (
        echo [OK] Found: %%F
    )
)

REM Check for required DLLs
echo.
echo Checking for required DLLs...
for %%D in (%REQUIRED_DLLS%) do (
    if not exist "%GAME_PATH%\Binaries\Win64\%%D" (
        echo [WARNING] Missing DLL: %%D
        echo Creating dummy %%D...
        echo. > "%GAME_PATH%\Binaries\Win64\%%D"
    ) else (
        echo [OK] Found: %%D
    )
)

REM Verify network settings (optional)
echo.
echo Verifying network connectivity...
ping google.com -n 1 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] No internet connection detected!
    echo Please check your network settings.
) else (
    echo [OK] Internet connection detected.
)

REM Final confirmation
echo.
echo ==========================================
echo Verification complete. Review the logs above.
echo If there are no critical errors, you can proceed.
pause
