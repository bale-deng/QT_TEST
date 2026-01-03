# Phase 2 Implementation Details

## Overview
Phase 2 implements advanced features including JSON-based C++/Python integration and a custom Markdown editor with image paste support.

## Python Weather Service (python/weather_service.py)

### Key Changes
- **JSON-only output**: Modified `main()` function to output only JSON to stdout for clean C++ parsing
- **Real API integration**: Uses Open-Meteo API (free, no API key required)
- **Two-step process**:
  1. Geocoding API: Converts city name to coordinates
  2. Weather API: Fetches weather data using coordinates

### API Details
- **Geocoding**: `https://geocoding-api.open-meteo.com/v1/search`
- **Weather**: `https://api.open-meteo.com/v1/forecast`
- **Data returned**: temperature, humidity, weather_code, wind_speed, apparent_temperature, surface_pressure

### JSON Output Format
```json
{
  "city": "Beijing",
  "temperature": 5.2,
  "humidity": 45,
  "description": "Clear sky",
  "wind_speed": 12.5,
  "feels_like": 3.1,
  "pressure": 1013.2,
  "status": "success"
}
```

### Error Handling
- Returns JSON with `"status": "error"` and `"error_message"` field on failure
- Handles city not found, network errors, and API failures

## C++ JSON Parsing (src/MainWindow.cpp)

### Implementation Details
- **Added includes**: `<QJsonDocument>`, `<QJsonObject>`
- **Parsing logic in `formatWeatherHTML()`**:
  1. Parse JSON using `QJsonDocument::fromJson()`
  2. Check for parse errors
  3. Extract `status` field to detect errors
  4. Extract weather fields: city, temperature, humidity, description, wind_speed, feels_like, pressure
  5. Format into HTML with proper styling

### Key Code Pattern
```cpp
QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
QJsonObject obj = doc.object();
QString status = obj["status"].toString();
double temperature = obj["temperature"].toDouble();
```

### Async Processing
- Uses `QProcess` for non-blocking Python execution
- Signals: `finished()` and `errorOccurred()`
- Prevents UI freezing during API calls
- Shows skeleton screen while loading

## Markdown Editor (include/MarkdownEditor.h, src/MarkdownEditor.cpp)

### Architecture
- **Base class**: `QPlainTextEdit`
- **Key overrides**:
  - `canInsertFromMimeData()`: Checks if clipboard contains image
  - `insertFromMimeData()`: Handles image paste and saves to disk

### Image Handling Flow
1. **Detection**: Check `QMimeData::hasImage()`
2. **Extraction**: Get `QImage` from MIME data
3. **Filename generation**: `img_{timestamp}_{md5hash}.png`
4. **Save location**: `notes/images/` relative to executable
5. **Directory creation**: Auto-creates directory if missing
6. **Markdown insertion**: Inserts `![Image](notes/images/filename.png)` at cursor

### Key Methods
- `saveImage(const QImage &image)`: Saves image and returns relative path
- `generateImageFilename(const QImage &image)`: Creates unique filename using timestamp + MD5 hash
- `ensureImageDirectoryExists()`: Creates directory structure if needed

### Styling
- Dark theme matching main app
- Monospace font (Consolas, Monaco, Courier New)
- Cyan focus border (#00d4ff)
- Placeholder text with usage hint

## MainWindow Integration (src/MainWindow.cpp, include/MainWindow.h)

### UI Structure
- **Tab Widget**: Two tabs for Weather and Notes
- **Weather Tab**: Existing weather display
- **Notes Tab**: Split view with editor and preview
  - Left: `MarkdownEditor` for editing
  - Right: `QTextBrowser` for live preview

### New Components
- `QTabWidget *m_tabWidget`: Main tab container
- `MarkdownEditor *m_markdownEditor`: Custom editor
- `QTextBrowser *m_markdownPreview`: Live preview pane

### Signal Connections
- `m_markdownEditor->textChanged()` → `onMarkdownTextChanged()`
- Updates preview in real-time using `QTextBrowser::setMarkdown()`

### Layout
```
MainWindow
├── City Input (QLineEdit)
├── Get Weather Button (QPushButton)
└── Tab Widget (QTabWidget)
    ├── Weather Tab (QTextEdit)
    └── Notes Tab (QWidget)
        ├── Markdown Editor (MarkdownEditor) [Left]
        └── Markdown Preview (QTextBrowser) [Right]
```

## CMakeLists.txt Updates

### Added Files
- `src/MarkdownEditor.cpp` to SOURCES
- `include/MarkdownEditor.h` to HEADERS

### Build System
- Qt's MOC automatically processes MarkdownEditor (Q_OBJECT macro)
- No additional dependencies required (uses Qt6 Core/Widgets)

## Testing Checklist

### Weather Service
- [ ] Test with valid city names (Beijing, London, Tokyo)
- [ ] Test with invalid city names
- [ ] Test network error handling
- [ ] Verify JSON parsing in C++
- [ ] Check async behavior (UI doesn't freeze)

### Markdown Editor
- [ ] Paste image from clipboard
- [ ] Verify image saved to notes/images/
- [ ] Check Markdown link insertion
- [ ] Test live preview updates
- [ ] Verify directory auto-creation

## File Locations

### New Files
- `include/MarkdownEditor.h`: Header for custom editor
- `src/MarkdownEditor.cpp`: Implementation

### Modified Files
- `python/weather_service.py`: JSON-only output
- `src/MainWindow.cpp`: JSON parsing + tab widget integration
- `include/MainWindow.h`: New member variables and slots
- `CMakeLists.txt`: Added MarkdownEditor to build

## Runtime Directory Structure
```
build/bin/
├── WeatherApp.exe
├── python/
│   └── weather_service.py
└── notes/
    └── images/
        └── img_*.png (created at runtime)
```

## Dependencies
- Qt6 Core, Widgets (existing)
- Python3 with `requests` library (for weather API)
- No additional C++ libraries required
