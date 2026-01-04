# PHASE_3_DESIGN: Interaction and File System Integration

## 1. FileManager Class Design
The `FileManager` class will be a utility class (singleton or managed by `MainWindow`) to handle all file I/O operations with UTF-8 support.

### Interface (`FileManager.h`)
- **Properties**:
  - `QString m_currentFilePath`: Path of the currently opened file.
  - `bool m_isModified`: Tracks if the content has changed since last save.
- **Public Methods**:
  - `bool openFile(const QString &filePath, QString &content)`: Reads file content using `QFile` and `QTextStream` (UTF-8).
  - `bool saveFile(const QString &filePath, const QString &content)`: Writes content to disk.
  - `bool autoSave(const QString &content)`: Periodically saves to a temporary backup file.
  - `QString currentFilePath() const`: Getter for current path.
  - `void setCurrentFilePath(const QString &path)`: Setter for current path.
- **Signals**:
  - `fileOpened(QString path)`
  - `fileSaved(QString path)`
  - `errorOccurred(QString message)`

## 2. Zoom Logic Design
Applies to both the `MarkdownEditor` and the `MarkdownPreview`.

### Constants
- `ZOOM_STEP`: 0.1 (10%)
- `ZOOM_MIN`: 0.5 (50%)
- `ZOOM_MAX`: 3.0 (300%)
- `DEFAULT_ZOOM`: 1.0 (100%)

### Implementation
- `m_currentZoom`: double (initialized to 1.0).
- `void setZoom(double level)`: Calculates new font sizes and applies them to editor and preview.
- `void zoomIn()`: `setZoom(m_currentZoom + ZOOM_STEP)`
- `void zoomOut()`: `setZoom(m_currentZoom - ZOOM_STEP)`
- `void zoomReset()`: `setZoom(DEFAULT_ZOOM)`

## 3. UI Logic & Shortcut Mapping
Integration into `MainWindow`.

### Shortcuts
- `Ctrl + O`: Open File (triggers `QFileDialog::getOpenFileName`).
- `Ctrl + S`: Save File (if `m_currentFilePath` is empty, triggers `saveAs`).
- `Ctrl + Shift + S`: Save As.
- `Ctrl + =` / `Ctrl + +`: Zoom In.
- `Ctrl + -`: Zoom Out.
- `Ctrl + 0`: Reset Zoom.

### Signals & Slots Connection Plan
- `MainWindow` will own an instance of `FileManager`.
- `MarkdownEditor::textChanged` -> `MainWindow::onTextChanged` (marks `FileManager` as modified).
- `MainWindow::onSaveTriggered` -> `FileManager::saveFile`.
- `MainWindow::onZoomInTriggered` -> UI Update (Editor & Preview).

## 4. File Encoding
- Force `UTF-8` for all file operations to ensure cross-platform compatibility and support for emojis/special characters.
