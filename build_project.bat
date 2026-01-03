@echo off
REM ============================================
REM WeatherApp Build and Deployment Script
REM ============================================
REM This script automates the build and packaging process
REM for the WeatherApp Qt6 + Python application

echo ============================================
echo WeatherApp Build and Deployment Script
echo ============================================
echo.

REM Set project root directory
set PROJECT_ROOT=%~dp0
cd /d "%PROJECT_ROOT%"

REM Configuration
set BUILD_DIR=build
set RELEASE_DIR=release
set CMAKE_BUILD_TYPE=Release

echo [1/6] Cleaning previous build...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
)
if exist "%RELEASE_DIR%" (
    rmdir /s /q "%RELEASE_DIR%"
)
mkdir "%BUILD_DIR%"
mkdir "%RELEASE_DIR%"

echo [2/6] Configuring CMake...
cd "%BUILD_DIR%"
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% -DCMAKE_PREFIX_PATH=E:/Qt/6.5.3/mingw_64 ..
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

echo [3/6] Building project...
cmake --build . --config %CMAKE_BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo [4/6] Copying executable and dependencies...
cd ..
copy "%BUILD_DIR%\bin\WeatherApp.exe" "%RELEASE_DIR%\" >nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to copy executable!
    pause
    exit /b 1
)

echo [5/6] Deploying Qt dependencies...
REM Use windeployqt to copy Qt DLLs
E:\Qt\6.5.3\mingw_64\bin\windeployqt.exe "%RELEASE_DIR%\WeatherApp.exe" --no-translations --no-system-d3d-compiler --no-opengl-sw
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: windeployqt failed or not found. You may need to manually copy Qt DLLs.
)

REM Copy correct MinGW runtime DLLs (must match compiler version)
echo Copying MinGW runtime DLLs...
copy /Y "E:\CXX_PATH\mingw64\bin\libgcc_s_seh-1.dll" "%RELEASE_DIR%\" >nul
copy /Y "E:\CXX_PATH\mingw64\bin\libstdc++-6.dll" "%RELEASE_DIR%\" >nul
copy /Y "E:\CXX_PATH\mingw64\bin\libwinpthread-1.dll" "%RELEASE_DIR%\" >nul

echo [6/7] Copying Python scripts and resources...
REM Copy Python scripts
xcopy /E /I /Y "python" "%RELEASE_DIR%\python\" >nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to copy Python scripts!
    pause
    exit /b 1
)

echo [7/7] Copying Python runtime (if available)...
REM Copy embedded Python runtime if it exists
if exist "python_runtime\python.exe" (
    echo Found embedded Python runtime, copying...
    xcopy /E /I /Y "python_runtime" "%RELEASE_DIR%\python_runtime\" >nul
    if %ERRORLEVEL% NEQ 0 (
        echo WARNING: Failed to copy Python runtime!
        echo The application will require system Python installation.
    ) else (
        echo Embedded Python runtime copied successfully.
        echo Application will be self-contained and not require system Python.
    )
) else (
    echo WARNING: No embedded Python runtime found at python_runtime\
    echo Run download_python_runtime.bat to create a self-contained package.
    echo The application will require system Python installation.
)

REM Create notes directory for markdown images
mkdir "%RELEASE_DIR%\notes" 2>nul
mkdir "%RELEASE_DIR%\notes\images" 2>nul

REM Copy README and documentation
if exist "docs\QUICK_START.md" copy "docs\QUICK_START.md" "%RELEASE_DIR%\" >nul
if exist "README.md" copy "README.md" "%RELEASE_DIR%\" >nul

REM Create requirements.txt if it doesn't exist
if not exist "python\requirements.txt" (
    echo requests>=2.31.0 > "%RELEASE_DIR%\python\requirements.txt"
)

REM Create deployment info file
echo WeatherApp - Qt6 + Python > "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
echo Build Date: %DATE% %TIME% >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
echo Build Type: %CMAKE_BUILD_TYPE% >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
echo. >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"

if exist "%RELEASE_DIR%\python_runtime\python.exe" (
    echo Python Runtime: Embedded (Self-contained) >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo. >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo Installation Instructions: >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo 1. Extract all files to a directory >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo 2. Run WeatherApp.exe >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo. >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo No Python installation required - runtime is included! >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
) else (
    echo Python Runtime: System (Requires Installation) >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo. >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo Installation Instructions: >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo 1. Ensure Python 3.x is installed and in PATH >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo 2. Install Python dependencies: pip install -r python\requirements.txt >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
    echo 3. Run WeatherApp.exe >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
)
echo. >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"
echo For more information, see QUICK_START.md >> "%RELEASE_DIR%\DEPLOYMENT_INFO.txt"

echo.
echo ============================================
echo Build completed successfully!
echo ============================================
echo.
echo Release package location: %RELEASE_DIR%
echo.
echo Next steps:
echo 1. Test the application: cd %RELEASE_DIR% ^&^& WeatherApp.exe
echo 2. Install Python dependencies: pip install -r %RELEASE_DIR%\python\requirements.txt
echo 3. Distribute the %RELEASE_DIR% folder to users
echo.
pause
