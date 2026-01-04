# Phase 3 Implementation Details: File Operations and Zoom Functionality (Updated)

## Overview
This document describes the implementation of file operations and zoom functionality for the mdCoder markdown editor application, completed as part of Phase 3 development.

## File Operations Implementation

### Core Components

#### File Management State (MainWindow.h)
- `QString m_currentFilePath`: Stores the path of the currently opened file
- `bool m_isModified`: Tracks whether the document has unsaved changes

#### Key Methods (MainWindow.cpp)

1. **openFile()** (MainWindow.cpp:957-992)
   - Uses `QFileDialog::getOpenFileName()` to display file picker
   - Filters: Markdown Files (*.md *.markdown) and All Files (*)
   - Calls `maybeSave()` before opening to check for unsaved changes
   - Reads file using `QTextStream` with UTF-8 encoding (`QStringConverter::Utf8`)
   - Updates `m_currentFilePath`, resets `m_isModified`, and calls `updateWindowTitle()`

2. **saveFile()** (MainWindow.cpp:994-1017)
   - If no current file path exists, delegates to `saveFileAs()`
   - Writes content using `QTextStream` with UTF-8 encoding
   - Gets content from `m_markdownEditor->toPlainText()`
   - Resets `m_isModified` flag and updates window title on success

3. **saveFileAs()** (MainWindow.cpp:1019-1034)
   - Uses `QFileDialog::getSaveFileName()` to display save dialog
   - Default filename: "untitled.md" if no current file
   - Updates `m_currentFilePath` and calls `saveFile()`

4. **maybeSave()** (MainWindow.cpp:1036-1057)
   - Returns `true` if no unsaved changes
   - Shows warning dialog with Save/Discard/Cancel options
   - Returns `false` if user cancels, preventing the operation
   - Used before opening new files and closing the application

5. **updateWindowTitle()** (MainWindow.cpp:1059-1077)
   - Displays filename in title bar when file is open
   - Appends " *" to indicate unsaved changes
   - Format: "filename.md * - mdCoder" or "* - mdCoder" for new files

### Keyboard Shortcuts (MainWindow.cpp:236-244)
- **Ctrl+O**: Open file
- **Ctrl+S**: Save file
- **Ctrl+Shift+S**: Save As

### Integration Points
- `closeEvent()` updated to call `maybeSave()` before closing (MainWindow.cpp:89-110)
- Text change signal connected to set `m_isModified` flag (MainWindow.cpp:260-265)

## Zoom Functionality Implementation

### Core Components

#### Zoom State (MainWindow.h)
- `double m_currentZoom`: Current zoom level (1.0 = 100%)
- `double m_previewBaseZoom`: Tracks preview's base zoom factor for calculations
- Constants defined:
  - `ZOOM_STEP = 0.1` (10% increments)
  - `ZOOM_MIN = 0.5` (50% minimum)
  - `ZOOM_MAX = 3.0` (300% maximum)
  - `DEFAULT_ZOOM = 1.0` (100% default)

#### Key Methods (MainWindow.cpp)

1. **setZoom(double level)** (MainWindow.cpp:1101-1143)
   - Clamps zoom level to valid range using `qBound()`
   - Calculates zoom change ratio: `newZoom / m_currentZoom`
   - Saves zoom level to QSettings for persistence
   - **Editor zoom**: Scales font size directly: `baseFontSize * m_currentZoom`
   - **Preview zoom**: Uses QTextBrowser's `zoomIn()` and `zoomOut()` methods
     - Calculates number of zoom steps based on ratio
     - Each `zoomIn(1)` step ≈ 1.1x zoom
     - Each `zoomOut(1)` step ≈ 0.9x zoom (1/1.1)
     - Uses while loop with threshold (1.01/0.99) to avoid floating point issues
   - Refreshes preview to apply changes

2. **zoomIn()** (MainWindow.cpp:1145-1148)
   - Increases zoom by `ZOOM_STEP` (0.1)

3. **zoomOut()** (MainWindow.cpp:1150-1153)
   - Decreases zoom by `ZOOM_STEP` (0.1)

4. **zoomReset()** (MainWindow.cpp:1155-1158)
   - Resets zoom to `DEFAULT_ZOOM` (1.0)

5. **wheelEvent(QWheelEvent *event)** (MainWindow.cpp:1160-1179)
   - Overrides `QMainWindow::wheelEvent()`
   - Checks for Ctrl modifier key
   - Positive delta (scroll up) → zoom in
   - Negative delta (scroll down) → zoom out
   - Accepts event to prevent default behavior
   - Falls back to default behavior if Ctrl not pressed

### Keyboard Shortcuts (MainWindow.cpp:246-257)
- **Ctrl+=** and **Ctrl++**: Zoom in
- **Ctrl+-**: Zoom out
- **Ctrl+0**: Reset zoom to 100%

### Mouse Wheel Zoom (MainWindow.cpp:1160-1179)
- **Ctrl+Wheel Up**: Zoom in
- **Ctrl+Wheel Down**: Zoom out

### Zoom Persistence
- Zoom level saved to QSettings on every change (MainWindow.cpp:1113)
- Restored from QSettings in constructor (MainWindow.cpp:72-76)
- Storage key: "zoomLevel"

## Technical Notes

### UTF-8 Encoding
All file operations use UTF-8 encoding via `QStringConverter::Utf8` to ensure:
- Cross-platform compatibility
- Support for emojis and special characters
- Consistent behavior across different systems

### Zoom Implementation Details

#### Editor Zoom (Left Panel)
- Applied by directly scaling the font size
- Formula: `fontSize = baseFontSize * zoomLevel`
- Base font size: 14pt
- Direct and precise control

#### Preview Zoom (Right Panel) - UPDATED
- Uses QTextBrowser's built-in `zoomIn()` and `zoomOut()` methods
- **Why this approach:**
  - QTextBrowser doesn't have `setZoomFactor()` like QWebEngineView
  - CSS `zoom` property was accumulating and causing issues
  - Built-in methods provide smooth, native zoom behavior
- **Implementation:**
  - Calculates zoom change ratio between old and new zoom levels
  - Applies incremental zoom steps using while loop
  - Each step is approximately 1.1x (zoom in) or 0.9x (zoom out)
  - Uses threshold (1.01/0.99) to handle floating point precision
- **Advantages:**
  - Smooth zoom transitions
  - No CSS accumulation issues
  - Native Qt behavior
  - Works reliably across zoom range

### State Management
- Modified flag automatically set on text changes
- Window title updates automatically when file state changes
- Zoom level persists across application restarts
- Unsaved changes prompt appears on close or open operations

## Zoom Behavior

### How Zoom Works
1. User triggers zoom (keyboard shortcut or mouse wheel)
2. `setZoom()` calculates new zoom level and ratio
3. Editor font size updated directly
4. Preview zoom applied incrementally using `zoomIn()`/`zoomOut()`
5. Both panels zoom synchronously
6. Zoom level saved to settings

### Zoom Restoration on Startup
1. Application reads saved zoom level from QSettings
2. Calls `setZoom()` with saved value
3. Both editor and preview restore to previous zoom level

## Future Considerations

### Potential Improvements
1. **Auto-save**: Could implement periodic auto-save to temporary file
2. **Recent files**: Could add recent files menu
3. **Zoom UI indicator**: Could add zoom percentage display in status bar or toolbar
4. **Per-panel zoom**: Could allow independent zoom for editor and preview
5. **WebEngine migration**: If switching to QWebEngineView in future, can use `setZoomFactor()` for more precise zoom control

### Known Limitations
1. Preview zoom uses incremental steps, may have slight rounding differences
2. Zoom applies globally to entire preview, not per-element
3. No visual feedback for current zoom level (only keyboard shortcuts work)
4. Zoom restoration on startup applies incremental steps from 1.0, may accumulate small errors over time

## Files Modified
- `include/MainWindow.h`: Added file management and zoom members, method declarations, wheelEvent override
- `src/MainWindow.cpp`: Implemented all file operations, zoom functionality, and event handlers
- `CMakeLists.txt`: Added automatic Qt dependency deployment using windeployqt
- Constructor updated to initialize new members and restore zoom level
- setupUI() updated to add keyboard shortcuts and signal connections

## Deployment
- Added automatic Qt dependency deployment in CMakeLists.txt
- Uses `windeployqt` to copy required Qt DLLs and plugins
- Ensures application runs standalone without Qt installation
- Deploys: Qt6Core, Qt6Gui, Qt6Widgets, Qt6Network, Qt6Svg, platform plugins, image format plugins
