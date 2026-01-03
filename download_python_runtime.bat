@echo off
REM Download and setup Python embeddable runtime for Windows
REM This script downloads Python 3.11 embeddable and installs required packages

setlocal enabledelayedexpansion

echo ============================================
echo Python Embeddable Runtime Setup
echo ============================================
echo.

set PYTHON_VERSION=3.11.9
set PYTHON_URL=https://www.python.org/ftp/python/%PYTHON_VERSION%/python-%PYTHON_VERSION%-embed-amd64.zip
set RUNTIME_DIR=python_runtime
set PYTHON_ZIP=%RUNTIME_DIR%\python-embed.zip

REM Create runtime directory
if not exist "%RUNTIME_DIR%" mkdir "%RUNTIME_DIR%"

REM Check if already downloaded
if exist "%RUNTIME_DIR%\python.exe" (
    echo Python runtime already exists at %RUNTIME_DIR%
    echo To re-download, delete the %RUNTIME_DIR% directory first.
    goto :install_packages
)

echo [1/4] Downloading Python %PYTHON_VERSION% embeddable...
powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%PYTHON_URL%' -OutFile '%PYTHON_ZIP%'}"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to download Python runtime!
    pause
    exit /b 1
)

echo [2/4] Extracting Python runtime...
powershell -Command "Expand-Archive -Path '%PYTHON_ZIP%' -DestinationPath '%RUNTIME_DIR%' -Force"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to extract Python runtime!
    pause
    exit /b 1
)

REM Clean up zip file
del "%PYTHON_ZIP%"

echo [3/4] Configuring Python runtime...
REM Enable site-packages by uncommenting import site in python311._pth
set PTH_FILE=%RUNTIME_DIR%\python311._pth
if exist "%PTH_FILE%" (
    powershell -Command "(Get-Content '%PTH_FILE%') -replace '#import site', 'import site' | Set-Content '%PTH_FILE%'"
)

:install_packages
echo [4/4] Installing pip and required packages...

REM Download get-pip.py
if not exist "%RUNTIME_DIR%\get-pip.py" (
    echo Downloading get-pip.py...
    powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://bootstrap.pypa.io/get-pip.py' -OutFile '%RUNTIME_DIR%\get-pip.py'}"
)

REM Install pip
if not exist "%RUNTIME_DIR%\Scripts\pip.exe" (
    echo Installing pip...
    "%RUNTIME_DIR%\python.exe" "%RUNTIME_DIR%\get-pip.py"
)

REM Install required packages
if exist "python\requirements.txt" (
    echo Installing Python dependencies from requirements.txt...
    "%RUNTIME_DIR%\python.exe" -m pip install -r python\requirements.txt
) else (
    echo Installing default dependencies...
    "%RUNTIME_DIR%\python.exe" -m pip install requests>=2.31.0
)

echo.
echo ============================================
echo Python runtime setup complete!
echo ============================================
echo.
echo Python executable: %RUNTIME_DIR%\python.exe
echo.
echo You can now build the project with build_project.bat
echo.
pause
