# UI Redesign - Sidebar Layout with Persistent Settings

## Overview
The UI has been completely redesigned with a modern sidebar layout that displays weather information persistently. The default city setting is saved and automatically loaded on startup.

## New UI Layout

### Main Structure
```
┌─────────────────────────────────────────────────────┐
│  Sidebar (320px)  │  Main Content Area             │
│                   │                                 │
│  🌤️ Weather       │  📝 Notes Tab                  │
│                   │  🔍 Weather Search Tab         │
│  Default City: ▼  │                                 │
│  [Beijing     ]   │                                 │
│                   │                                 │
│  🔄 Refresh       │                                 │
│  ─────────────    │                                 │
│  📍 Beijing       │                                 │
│  -2.2°C           │                                 │
│  Overcast         │                                 │
│                   │                                 │
│  🌡️ Feels: -6.2°C │                                 │
│  💧 Humidity: 42% │                                 │
│  💨 Wind: 3.7km/h │                                 │
│  🔽 Press: 1020mb │                                 │
└─────────────────────────────────────────────────────┘
```

### Sidebar Components (320px fixed width)

#### 1. Weather Section
- **Title**: "🌤️ Weather" (24px, bold, cyan)
- **Default City Selector**: QComboBox with 8 preset cities
  - Beijing, Tokyo, London, New York, Paris, Sydney, Shanghai, Hong Kong
  - Styled with dark theme
  - Saves selection to QSettings
- **Refresh Button**: "🔄 Refresh Weather"
  - Reloads weather for current default city
  - Shows loading state during fetch

#### 2. Weather Display
- **City Label**: Large city name with 📍 icon (20px, bold, cyan)
- **Temperature**: Very large display (48px, bold, cyan)
- **Condition**: Weather description (18px, light blue)
- **Details**: Rich text with icons
  - Feels like temperature (orange)
  - Humidity (cyan)
  - Wind speed (green)
  - Pressure (red)

### Main Content Area

#### Tab 1: 📝 Notes
- **Left Pane**: MarkdownEditor
  - Supports image paste
  - Saves images to notes/images/
  - Monospace font
- **Right Pane**: QTextBrowser
  - Live markdown preview
  - Updates in real-time

#### Tab 2: 🔍 Weather Search
- **City Input**: Text field for custom city search
- **Search Button**: "🔍 Search Weather"
- **Results Display**: Full weather details in formatted HTML

## Persistent Settings (QSettings)

### Storage
- **Organization**: "WeatherApp"
- **Application**: "QtWeatherApp"
- **Platform-specific locations**:
  - Windows: Registry or INI file
  - Linux: ~/.config/WeatherApp/QtWeatherApp.conf
  - macOS: ~/Library/Preferences/com.WeatherApp.QtWeatherApp.plist

### Saved Data
- **Key**: "defaultCity"
- **Value**: Selected city name (QString)
- **Default**: "Beijing"

### Implementation
```cpp
m_settings = new QSettings("WeatherApp", "QtWeatherApp", this);

// Load on startup
QString savedCity = m_settings->value("defaultCity", "Beijing").toString();

// Save on change
m_settings->setValue("defaultCity", city);
```

## Auto-Load on Startup

### Mechanism
- Uses `QTimer::singleShot(500ms)` to delay initial load
- Allows UI to fully render before making API call
- Prevents blocking during window creation

### Implementation
```cpp
// In constructor after setupUI()
QTimer::singleShot(500, this, &MainWindow::loadWeatherForDefaultCity);
```

### Loading Flow
1. Window opens with "Loading..." in sidebar
2. After 500ms, triggers `loadWeatherForDefaultCity()`
3. Reads default city from QSettings
4. Calls Python weather service asynchronously
5. Updates sidebar with real weather data
6. No user interaction required

## Signal/Slot Connections

### New Connections
```cpp
// Refresh button → Load weather
connect(m_refreshButton, &QPushButton::clicked, 
        this, &MainWindow::loadWeatherForDefaultCity);

// Default city changed → Save and reload
connect(m_defaultCityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        this, &MainWindow::onDefaultCityChanged);

// Search button → Custom search
connect(m_searchButton, &QPushButton::clicked, 
        this, &MainWindow::onButtonClicked);

// Markdown editor → Update preview
connect(m_markdownEditor, &QPlainTextEdit::textChanged, 
        this, &MainWindow::onMarkdownTextChanged);
```

## New Member Functions

### `void loadWeatherForDefaultCity()`
- Loads weather for the currently selected default city
- Shows loading state in sidebar
- Calls Python service asynchronously
- Updates sidebar on completion

### `void onDefaultCityChanged(int index)`
- Triggered when user changes default city in combo box
- Saves new selection to QSettings
- Automatically loads weather for new city

### `void handleWeatherError(const QString &errorMsg)`
- Centralized error handling
- Updates both sidebar and search display
- Shows user-friendly error messages

## Styling

### Color Scheme
- **Background**: #0a0a0a (very dark)
- **Sidebar**: #1a1a1a (dark gray)
- **Borders**: #2d2d2d (medium gray)
- **Primary Accent**: #00d4ff (cyan)
- **Text**: #ffffff (white)
- **Secondary Text**: #aaaaaa, #cccccc (light gray)
- **Error**: #ff4757 (red)

### Typography
- **Font Family**: 'SF Pro Display', 'Segoe UI', 'Microsoft YaHei', sans-serif
- **Monospace**: 'Consolas', 'Monaco', 'Courier New' (for editor)
- **Sizes**: 14px-48px depending on element

### Interactive States
- **Focus**: Cyan border (#00d4ff)
- **Hover**: Lighter background
- **Pressed**: Darker background
- **Disabled**: Gray (#666666)

## Window Properties
- **Title**: "Weather App - Qt + Python"
- **Size**: 1200x700 (increased from 650x550)
- **Layout**: Horizontal split (sidebar + content)

## User Experience Improvements

### 1. Persistent Default City
- User sets default city once
- Saved across sessions
- No need to re-enter on each launch

### 2. Auto-Load on Startup
- Weather loads automatically
- No button click required
- Immediate information display

### 3. Always-Visible Weather
- Sidebar shows weather at all times
- No need to switch tabs
- Quick glance information

### 4. Dual Weather Access
- **Sidebar**: Quick view of default city
- **Search Tab**: Detailed search for any city

### 5. Visual Feedback
- Loading states for all async operations
- Error messages with icons
- Color-coded information

## Technical Implementation Details

### QSettings Integration
- Automatic platform-specific storage
- No manual file handling required
- Thread-safe operations

### Async Weather Loading
- Non-blocking UI
- QProcess for Python execution
- Signal/slot for completion handling

### Layout Management
- QHBoxLayout for main split
- QVBoxLayout for sidebar and content
- Fixed width sidebar (320px)
- Flexible content area

## Testing Checklist

### Persistent Settings
- [x] Default city saves on change
- [x] Default city loads on startup
- [x] Settings persist across app restarts

### Auto-Load
- [x] Weather loads automatically on startup
- [x] No UI blocking during load
- [x] Loading state visible to user

### Sidebar Display
- [x] Weather updates in sidebar
- [x] Refresh button works
- [x] Error handling in sidebar

### Search Functionality
- [x] Custom city search works
- [x] Results display in search tab
- [x] Independent from sidebar

## Files Modified

### Header Files
- `include/MainWindow.h`: Added sidebar components, QSettings, new slots

### Source Files
- `src/MainWindow.cpp`: Complete UI redesign, new functions

### New Includes
- `<QSettings>`: Persistent storage
- `<QComboBox>`: City selector
- `<QTimer>`: Delayed auto-load
- `<QLabel>`: Sidebar labels

## Memory Usage
- Running: ~117 MB (increased from ~109 MB due to more UI components)
- Acceptable for desktop application

## Future Enhancements
- Add more cities to default list
- Allow custom city addition
- Weather history/favorites
- Notification on weather changes
- Weather icons/images
