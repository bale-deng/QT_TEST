# Markdown Editor Toolbar Feature

## Overview
Added a comprehensive formatting toolbar to the Markdown editor with buttons and keyboard shortcuts for quick text formatting.

## New Component: MarkdownEditorWidget

### Architecture
Created a new wrapper widget `MarkdownEditorWidget` that contains:
- **QToolBar**: Formatting toolbar at the top
- **MarkdownEditor**: The actual text editor below

This replaces the standalone `MarkdownEditor` in the UI.

### Class Structure
```cpp
class MarkdownEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MarkdownEditorWidget(QWidget *parent = nullptr);
    QPlainTextEdit* editor() const;  // Access underlying editor
    
private slots:
    // Format insertion slots
    void insertHeading();
    void insertBold();
    void insertItalic();
    void insertStrikethrough();
    void insertCode();
    void insertCodeBlock();
    void insertHorizontalRule();
    void insertBulletList();
    void insertNumberedList();
    void insertLink();
    void insertImage();
    void insertTable();
    
private:
    void setupToolbar();
    void insertFormatting(const QString &prefix, const QString &suffix, const QString &placeholder);
    
    QVBoxLayout *m_layout;
    QToolBar *m_toolbar;
    MarkdownEditor *m_editor;
};
```

## Toolbar Buttons

### Text Formatting
| Button | Label | Shortcut | Function | Markdown Output |
|--------|-------|----------|----------|-----------------|
| H1 | H1 | Ctrl+1 | Insert heading | `# ` |
| B | B | Ctrl+B | Bold text | `**text**` |
| I | I | Ctrl+I | Italic text | `*text*` |
| S | S | Ctrl+Shift+S | Strikethrough | `~~text~~` |

### Code Formatting
| Button | Label | Shortcut | Function | Markdown Output |
|--------|-------|----------|----------|-----------------|
| </> | </> | Ctrl+` | Inline code | `` `code` `` |
| { } | { } | Ctrl+Shift+C | Code block | ` ```\ncode\n``` ` |

### Structure
| Button | Label | Shortcut | Function | Markdown Output |
|--------|-------|----------|----------|-----------------|
| — | — | Ctrl+H | Horizontal rule | `---` |
| • List | • List | Ctrl+Shift+8 | Bullet list | `- ` |
| 1. List | 1. List | Ctrl+Shift+7 | Numbered list | `1. ` |

### Media & Links
| Button | Label | Shortcut | Function | Markdown Output |
|--------|-------|----------|----------|-----------------|
| 🔗 Link | 🔗 Link | Ctrl+K | Insert link | `[text](url)` |
| 🖼️ Image | 🖼️ Image | Ctrl+Shift+I | Insert image | `![alt](url)` |
| ⊞ Table | ⊞ Table | Ctrl+T | Insert table | 3x2 table template |

## Behavior

### Smart Text Wrapping
When text is selected:
- Wraps selected text with formatting
- Example: Select "hello" → Click Bold → `**hello**`

When no text is selected:
- Inserts placeholder text with formatting
- Automatically selects the placeholder for easy replacement
- Example: Click Bold → `**bold text**` (with "bold text" selected)

### Special Behaviors

#### Heading (H1)
- Inserts `# ` at the start of the current line
- Cursor positioned after the space

#### Code Block
- Inserts triple backticks with newlines
- Cursor positioned inside the block
- Template:
  ```
  ```
  code here
  ```
  ```

#### Horizontal Rule
- Inserts `---` with blank lines before and after
- Creates visual separation

#### Lists
- Inserts list marker at start of line
- User can continue typing list items

#### Table
- Inserts a 3-column, 2-row table template
- Includes header row with separator
- Template:
  ```markdown
  | Header 1 | Header 2 | Header 3 |
  |----------|----------|----------|
  | Cell 1   | Cell 2   | Cell 3   |
  | Cell 4   | Cell 5   | Cell 6   |
  ```

## Toolbar Styling

### Dark Theme
```css
QToolBar {
    background-color: #2d2d2d;
    border: none;
    border-bottom: 2px solid #1a1a1a;
    padding: 5px;
}

QToolButton {
    background-color: #1a1a1a;
    color: #ffffff;
    border: 1px solid #3d3d3d;
    border-radius: 4px;
    padding: 6px 10px;
    font-size: 13px;
    font-weight: 600;
}

QToolButton:hover {
    background-color: #3d3d3d;
    border: 1px solid #00d4ff;
}

QToolButton:pressed {
    background-color: #00d4ff;
    color: #000000;
}
```

### Visual Separators
- Separators between button groups for better organization
- Groups: Text | Code | Structure | Media

## Keyboard Shortcuts

### Standard Shortcuts
- **Ctrl+B**: Bold (Qt standard)
- **Ctrl+I**: Italic (Qt standard)

### Custom Shortcuts
- **Ctrl+1**: Heading
- **Ctrl+Shift+S**: Strikethrough
- **Ctrl+`**: Inline code
- **Ctrl+Shift+C**: Code block
- **Ctrl+H**: Horizontal rule
- **Ctrl+Shift+8**: Bullet list
- **Ctrl+Shift+7**: Numbered list
- **Ctrl+K**: Link
- **Ctrl+Shift+I**: Image
- **Ctrl+T**: Table

### Shortcut Implementation
```cpp
QAction *boldAction = new QAction("B", this);
boldAction->setToolTip("Bold (Ctrl+B)");
boldAction->setShortcut(QKeySequence::Bold);  // Standard shortcut
connect(boldAction, &QAction::triggered, this, &MarkdownEditorWidget::insertBold);
```

## Integration with MainWindow

### Updated Components
```cpp
// In MainWindow.h
MarkdownEditorWidget *m_markdownEditorWidget;  // New wrapper
QPlainTextEdit *m_markdownEditor;              // Reference to editor

// In MainWindow.cpp constructor
m_markdownEditorWidget = new MarkdownEditorWidget(this);
m_markdownEditor = m_markdownEditorWidget->editor();  // Get reference

// In layout
notesLayout->addWidget(m_markdownEditorWidget);  // Add wrapper instead
```

### Signal Connections
- Text changed signal still works through the editor reference
- All existing functionality preserved

## Implementation Details

### insertFormatting() Helper
```cpp
void MarkdownEditorWidget::insertFormatting(
    const QString &prefix,      // e.g., "**"
    const QString &suffix,      // e.g., "**"
    const QString &placeholder  // e.g., "bold text"
)
{
    QTextCursor cursor = m_editor->textCursor();
    
    if (cursor.hasSelection()) {
        // Wrap selected text
        QString selectedText = cursor.selectedText();
        cursor.insertText(prefix + selectedText + suffix);
    } else {
        // Insert with placeholder
        QString text = placeholder.isEmpty() ? "text" : placeholder;
        cursor.insertText(prefix + text + suffix);
        
        // Select the placeholder for easy replacement
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 
                          suffix.length() + text.length());
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 
                          text.length());
        m_editor->setTextCursor(cursor);
    }
    
    m_editor->setFocus();
}
```

### Cursor Manipulation
- Uses `QTextCursor` for precise text insertion
- `movePosition()` for cursor movement
- `hasSelection()` to detect selected text
- `setTextCursor()` to update cursor position

## User Experience Improvements

### 1. Visual Feedback
- Hover effect: Lighter background + cyan border
- Press effect: Cyan background with black text
- Tooltips show function and keyboard shortcut

### 2. Keyboard-First Workflow
- All functions accessible via keyboard
- Standard shortcuts (Ctrl+B, Ctrl+I) for familiarity
- Logical shortcuts for other functions

### 3. Smart Placeholder Selection
- Inserted placeholders are automatically selected
- User can immediately type to replace
- No need to manually select text

### 4. Context-Aware Insertion
- Headings and lists insert at line start
- Code blocks and tables insert with proper spacing
- Horizontal rules add blank lines for separation

## Files Modified

### New Files
None - functionality added to existing files

### Modified Files
1. **include/MarkdownEditor.h**
   - Added `MarkdownEditorWidget` class declaration
   - Added toolbar-related includes

2. **src/MarkdownEditor.cpp**
   - Implemented `MarkdownEditorWidget` class
   - Added all formatting functions
   - Added toolbar setup

3. **include/MainWindow.h**
   - Changed `MarkdownEditor*` to `MarkdownEditorWidget*`
   - Added reference to underlying editor

4. **src/MainWindow.cpp**
   - Updated to use `MarkdownEditorWidget`
   - Get editor reference via `editor()` method

## Testing Checklist

### Button Functionality
- [x] H1 button inserts heading
- [x] Bold button wraps text
- [x] Italic button wraps text
- [x] Strikethrough button wraps text
- [x] Code button wraps text
- [x] Code block inserts template
- [x] Horizontal rule inserts separator
- [x] Bullet list inserts marker
- [x] Numbered list inserts marker
- [x] Link button inserts template
- [x] Image button inserts template
- [x] Table button inserts template

### Keyboard Shortcuts
- [x] Ctrl+B for bold
- [x] Ctrl+I for italic
- [x] Ctrl+1 for heading
- [x] Ctrl+Shift+S for strikethrough
- [x] Other shortcuts functional

### Smart Behavior
- [x] Selected text gets wrapped
- [x] Placeholders auto-selected
- [x] Cursor positioned correctly

### Visual
- [x] Toolbar displays correctly
- [x] Buttons styled properly
- [x] Hover effects work
- [x] Press effects work
- [x] Tooltips show

## Build Status
- ✅ Compiles successfully
- ✅ No warnings
- ✅ Application runs (PID: 13076)
- ✅ Memory: ~118 MB

## Future Enhancements
- Add more heading levels (H2, H3, etc.)
- Add quote block button
- Add checklist button
- Add emoji picker
- Add find/replace functionality
- Add word count display
- Add syntax highlighting in editor
- Add custom button icons
