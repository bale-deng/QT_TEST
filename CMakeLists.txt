cmake_minimum_required(VERSION 3.16)
project(WeatherApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Find Python3 (optional - we'll use embedded runtime if available)
find_package(Python3 COMPONENTS Interpreter)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MarkdownEditor.cpp
    src/SettingsManager.cpp
    resources.qrc
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/MarkdownEditor.h
    include/SettingsManager.h
)

# Create executable (WIN32 flag removes console window on Windows)
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
)

# Link libraries (no Python linking needed for QProcess approach)
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)

# No longer pass hardcoded Python path - runtime will search dynamically

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy Python scripts to the same directory as the executable
# This works correctly with multi-config generators (VS Debug/Release) and macOS bundles
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/python
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/python
    COMMENT "Copying Python scripts to executable directory"
)
