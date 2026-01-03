Project purpose: Qt6 C++ desktop WeatherApp that will call a Python weather service. Phase 1 is basic UI + mock Python script; Phase 2 will add C++/Python integration and real API calls.

Tech stack:
- C++17 with Qt6 Core/Widgets
- CMake 3.16+
- Embedded Python 3 (Interpreter + Development)
- Python script in python/weather_service.py

Codebase structure:
- CMakeLists.txt at repo root
- src/: C++ sources (main.cpp, MainWindow.cpp)
- include/: C++ headers (MainWindow.h)
- python/: Python scripts (weather_service.py)
- docs/: project docs
- resources/: assets (empty/unused in Phase 1)

Notes:
- Build copies python/ into build/bin/python/ for runtime use.