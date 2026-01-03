#include "MarkdownEditor.h"
#include <QDir>
#include <QDateTime>
#include <QCryptographicHash>
#include <QBuffer>
#include <QTextCursor>
#include <QDebug>
#include <QCoreApplication>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QFontMetrics>
#include <QSaveFile>

// ========== EmojiPickerDialog Implementation ==========

EmojiPickerDialog::EmojiPickerDialog(QWidget *parent)
    : QDialog(parent)
    , m_selectedEmoji("")
    , m_gridLayout(nullptr)
    , m_scrollArea(nullptr)
{
    setupUI();
}

EmojiPickerDialog::~EmojiPickerDialog()
{
    // Qt handles cleanup
}

QString EmojiPickerDialog::selectedEmoji() const
{
    return m_selectedEmoji;
}

void EmojiPickerDialog::onEmojiClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        m_selectedEmoji = button->text();
        accept();
    }
}

QStringList EmojiPickerDialog::getEmojiList() const
{
    QStringList emojis;
    
    // Smileys & Emotion
    emojis << "😀" << "😃" << "😄" << "😁" << "😆" << "😅" << "🤣" << "😂"
           << "🙂" << "🙃" << "😉" << "😊" << "😇" << "🥰" << "😍" << "🤩"
           << "😘" << "😗" << "😚" << "😙" << "😋" << "😛" << "😜" << "🤪"
           << "😝" << "🤑" << "🤗" << "🤭" << "🤫" << "🤔" << "🤐" << "🤨"
           << "😐" << "😑" << "😶" << "😏" << "😒" << "🙄" << "😬" << "🤥"
           << "😌" << "😔" << "😪" << "🤤" << "😴" << "😷" << "🤒" << "🤕"
           << "🤢" << "🤮" << "🤧" << "🥵" << "🥶" << "😶‍🌫️" << "🥴" << "😵"
           << "🤯" << "🤠" << "🥳" << "😎" << "🤓" << "🧐" << "😕" << "😟"
           << "🙁" << "☹️" << "😮" << "😯" << "😲" << "😳" << "🥺" << "😦"
           << "😧" << "😨" << "😰" << "😥" << "😢" << "😭" << "😱" << "😖"
           << "😣" << "😞" << "😓" << "😩" << "😫" << "🥱" << "😤" << "😡"
           << "😠" << "🤬" << "😈" << "👿" << "💀" << "☠️" << "💩" << "🤡";
    
    // Gestures & Body Parts
    emojis << "👋" << "🤚" << "🖐️" << "✋" << "🖖" << "👌" << "🤏" << "✌️"
           << "🤞" << "🤟" << "🤘" << "🤙" << "👈" << "👉" << "👆" << "🖕"
           << "👇" << "☝️" << "👍" << "👎" << "✊" << "👊" << "🤛" << "🤜"
           << "👏" << "🙌" << "👐" << "🤲" << "🤝" << "🙏" << "✍️" << "💅"
           << "🤳" << "💪" << "🦾" << "🦿" << "🦵" << "🦶" << "👂" << "🦻"
           << "👃" << "🧠" << "🦷" << "🦴" << "👀" << "👁️" << "👅" << "👄";
    
    // Animals & Nature
    emojis << "🐶" << "🐱" << "🐭" << "🐹" << "🐰" << "🦊" << "🐻" << "🐼"
           << "🐨" << "🐯" << "🦁" << "🐮" << "🐷" << "🐽" << "🐸" << "🐵"
           << "🙈" << "🙉" << "🙊" << "🐒" << "🐔" << "🐧" << "🐦" << "🐤"
           << "🐣" << "🐥" << "🦆" << "🦅" << "🦉" << "🦇" << "🐺" << "🐗"
           << "🐴" << "🦄" << "🐝" << "🐛" << "🦋" << "🐌" << "🐞" << "🐜"
           << "🦟" << "🦗" << "🕷️" << "🕸️" << "🦂" << "🐢" << "🐍" << "🦎"
           << "🦖" << "🦕" << "🐙" << "🦑" << "🦐" << "🦞" << "🦀" << "🐡"
           << "🐠" << "🐟" << "🐬" << "🐳" << "🐋" << "🦈" << "🐊" << "🐅"
           << "🐆" << "🦓" << "🦍" << "🦧" << "🐘" << "🦛" << "🦏" << "🐪"
           << "🐫" << "🦒" << "🦘" << "🐃" << "🐂" << "🐄" << "🐎" << "🐖"
           << "🐏" << "🐑" << "🦙" << "🐐" << "🦌" << "🐕" << "🐩" << "🦮"
           << "🐕‍🦺" << "🐈" << "🐈‍⬛" << "🐓" << "🦃" << "🦚" << "🦜" << "🦢";
    
    // Food & Drink
    emojis << "🍏" << "🍎" << "🍐" << "🍊" << "🍋" << "🍌" << "🍉" << "🍇"
           << "🍓" << "🍈" << "🍒" << "🍑" << "🥭" << "🍍" << "🥥" << "🥝"
           << "🍅" << "🍆" << "🥑" << "🥦" << "🥬" << "🥒" << "🌶️" << "🌽"
           << "🥕" << "🧄" << "🧅" << "🥔" << "🍠" << "🥐" << "🥯" << "🍞"
           << "🥖" << "🥨" << "🧀" << "🥚" << "🍳" << "🧈" << "🥞" << "🧇"
           << "🥓" << "🥩" << "🍗" << "🍖" << "🦴" << "🌭" << "🍔" << "🍟"
           << "🍕" << "🥪" << "🥙" << "🧆" << "🌮" << "🌯" << "🥗" << "🥘"
           << "🍝" << "🍜" << "🍲" << "🍛" << "🍣" << "🍱" << "🥟" << "🦪"
           << "🍤" << "🍙" << "🍚" << "🍘" << "🍥" << "🥠" << "🥮" << "🍢"
           << "🍡" << "🍧" << "🍨" << "🍦" << "🥧" << "🧁" << "🍰" << "🎂"
           << "🍮" << "🍭" << "🍬" << "🍫" << "🍿" << "🍩" << "🍪" << "🌰"
           << "🥜" << "🍯" << "🥛" << "🍼" << "☕" << "🍵" << "🧃" << "🥤"
           << "🍶" << "🍺" << "🍻" << "🥂" << "🍷" << "🥃" << "🍸" << "🍹"
           << "🧉" << "🍾" << "🧊" << "🥄" << "🍴" << "🍽️" << "🥣" << "🥡";
    
    // Sports & Activities
    emojis << "⚽" << "🏀" << "🏈" << "⚾" << "🥎" << "🎾" << "🏐" << "🏉"
           << "🥏" << "🎱" << "🪀" << "🏓" << "🏸" << "🏒" << "🏑" << "🥍"
           << "🏏" << "🥅" << "⛳" << "🪁" << "🏹" << "🎣" << "🤿" << "🥊"
           << "🥋" << "🎽" << "🛹" << "🛷" << "⛸️" << "🥌" << "🎿" << "⛷️"
           << "🏂" << "🪂" << "🏋️" << "🤼" << "🤸" << "🤺" << "⛹️" << "🤾"
           << "🏌️" << "🏇" << "🧘" << "🏊" << "🤽" << "🚣" << "🧗" << "🚴"
           << "🚵" << "🤹" << "🎪" << "🎭" << "🎨" << "🎬" << "🎤" << "🎧"
           << "🎼" << "🎹" << "🥁" << "🎷" << "🎺" << "🎸" << "🪕" << "🎻"
           << "🎲" << "♟️" << "🎯" << "🎳" << "🎮" << "🎰" << "🧩";
    
    // Travel & Places
    emojis << "🚗" << "🚕" << "🚙" << "🚌" << "🚎" << "🏎️" << "🚓" << "🚑"
           << "🚒" << "🚐" << "🚚" << "🚛" << "🚜" << "🦯" << "🦽" << "🦼"
           << "🛴" << "🚲" << "🛵" << "🏍️" << "🛺" << "🚨" << "🚔" << "🚍"
           << "🚘" << "🚖" << "🚡" << "🚠" << "🚟" << "🚃" << "🚋" << "🚞"
           << "🚝" << "🚄" << "🚅" << "🚈" << "🚂" << "🚆" << "🚇" << "🚊"
           << "🚉" << "✈️" << "🛫" << "🛬" << "🛩️" << "💺" << "🛰️" << "🚀"
           << "🛸" << "🚁" << "🛶" << "⛵" << "🚤" << "🛥️" << "🛳️" << "⛴️"
           << "🚢" << "⚓" << "⛽" << "🚧" << "🚦" << "🚥" << "🚏" << "🗺️"
           << "🗿" << "🗽" << "🗼" << "🏰" << "🏯" << "🏟️" << "🎡" << "🎢"
           << "🎠" << "⛲" << "⛱️" << "🏖️" << "🏝️" << "🏜️" << "🌋" << "⛰️"
           << "🏔️" << "🗻" << "🏕️" << "⛺" << "🏠" << "🏡" << "🏘️" << "🏚️"
           << "🏗️" << "🏭" << "🏢" << "🏬" << "🏣" << "🏤" << "🏥" << "🏦";
    
    // Objects
    emojis << "⌚" << "📱" << "📲" << "💻" << "⌨️" << "🖥️" << "🖨️" << "🖱️"
           << "🖲️" << "🕹️" << "🗜️" << "💽" << "💾" << "💿" << "📀" << "📼"
           << "📷" << "📸" << "📹" << "🎥" << "📽️" << "🎞️" << "📞" << "☎️"
           << "📟" << "📠" << "📺" << "📻" << "🎙️" << "🎚️" << "🎛️" << "🧭"
           << "⏱️" << "⏲️" << "⏰" << "🕰️" << "⌛" << "⏳" << "📡" << "🔋"
           << "🔌" << "💡" << "🔦" << "🕯️" << "🪔" << "🧯" << "🛢️" << "💸"
           << "💵" << "💴" << "💶" << "💷" << "💰" << "💳" << "💎" << "⚖️"
           << "🧰" << "🔧" << "🔨" << "⚒️" << "🛠️" << "⛏️" << "🔩" << "⚙️"
           << "🧱" << "⛓️" << "🧲" << "🔫" << "💣" << "🧨" << "🪓" << "🔪"
           << "🗡️" << "⚔️" << "🛡️" << "🚬" << "⚰️" << "⚱️" << "🏺" << "🔮"
           << "📿" << "🧿" << "💈" << "⚗️" << "🔭" << "🔬" << "🕳️" << "🩹"
           << "🩺" << "💊" << "💉" << "🩸" << "🧬" << "🦠" << "🧫" << "🧪";
    
    // Symbols & Hearts
    emojis << "❤️" << "🧡" << "💛" << "💚" << "💙" << "💜" << "🖤" << "🤍"
           << "🤎" << "💔" << "❣️" << "💕" << "💞" << "💓" << "💗" << "💖"
           << "💘" << "💝" << "💟" << "☮️" << "✝️" << "☪️" << "🕉️" << "☸️"
           << "✡️" << "🔯" << "🕎" << "☯️" << "☦️" << "🛐" << "⛎" << "♈"
           << "♉" << "♊" << "♋" << "♌" << "♍" << "♎" << "♏" << "♐"
           << "♑" << "♒" << "♓" << "🆔" << "⚛️" << "🉑" << "☢️" << "☣️"
           << "📴" << "📳" << "🈶" << "🈚" << "🈸" << "🈺" << "🈷️" << "✴️"
           << "🆚" << "💮" << "🉐" << "㊙️" << "㊗️" << "🈴" << "🈵" << "🈹"
           << "🈲" << "🅰️" << "🅱️" << "🆎" << "🆑" << "🅾️" << "🆘" << "❌"
           << "⭕" << "🛑" << "⛔" << "📛" << "🚫" << "💯" << "💢" << "♨️"
           << "🚷" << "🚯" << "🚳" << "🚱" << "🔞" << "📵" << "🚭" << "❗"
           << "❕" << "❓" << "❔" << "‼️" << "⁉️" << "🔅" << "🔆" << "〽️"
           << "⚠️" << "🚸" << "🔱" << "⚜️" << "🔰" << "♻️" << "✅" << "🈯"
           << "💹" << "❇️" << "✳️" << "❎" << "🌐" << "💠" << "Ⓜ️" << "🌀";
    
    // Flags (selection)
    emojis << "🏁" << "🚩" << "🎌" << "🏴" << "🏳️" << "🏳️‍🌈" << "🏳️‍⚧️" << "🏴‍☠️"
           << "🇨🇳" << "🇺🇸" << "🇬🇧" << "🇯🇵" << "🇰🇷" << "🇫🇷" << "🇩🇪" << "🇮🇹"
           << "🇪🇸" << "🇷🇺" << "🇨🇦" << "🇦🇺" << "🇧🇷" << "🇮🇳" << "🇲🇽" << "🇸🇬";
    
    return emojis;
}

void EmojiPickerDialog::setupUI()
{
    setWindowTitle("Select Emoji 😀");
    setMinimumSize(500, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Add category label
    QLabel *titleLabel = new QLabel("Click an emoji to insert:", this);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Create scroll area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Create container widget for grid
    QWidget *container = new QWidget(this);
    m_gridLayout = new QGridLayout(container);
    m_gridLayout->setSpacing(5);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    
    // Get emoji list and create buttons
    QStringList emojis = getEmojiList();
    int columns = 10;  // 10 emojis per row
    
    for (int i = 0; i < emojis.size(); ++i) {
        QPushButton *emojiButton = new QPushButton(emojis[i], container);
        emojiButton->setFixedSize(45, 45);
        emojiButton->setStyleSheet(
            "QPushButton {"
            "    font-size: 24px;"
            "    background-color: #2d2d2d;"
            "    border: 1px solid #3d3d3d;"
            "    border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #3d3d3d;"
            "    border: 2px solid #00d4ff;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #00d4ff;"
            "}"
        );
        
        connect(emojiButton, &QPushButton::clicked, this, &EmojiPickerDialog::onEmojiClicked);
        
        int row = i / columns;
        int col = i % columns;
        m_gridLayout->addWidget(emojiButton, row, col);
    }
    
    m_scrollArea->setWidget(container);
    mainLayout->addWidget(m_scrollArea);
    
    // Add cancel button
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #1a1a1a;"
        "    color: #ffffff;"
        "    border: 1px solid #3d3d3d;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3d3d3d;"
        "    border: 1px solid #00d4ff;"
        "}"
    );
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    mainLayout->addWidget(cancelButton);
}

// ========== MarkdownEditorWidget Implementation ==========

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_toolbar(nullptr)
    , m_editor(nullptr)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    
    setupToolbar();
    
    m_editor = new MarkdownEditor(this);
    m_layout->addWidget(m_editor);
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    // Qt handles cleanup
}

QPlainTextEdit* MarkdownEditorWidget::editor() const
{
    return m_editor;
}

void MarkdownEditorWidget::setupToolbar()
{
    m_toolbar = new QToolBar(this);
    m_toolbar->setMovable(false);
    m_toolbar->setStyleSheet(
        "QToolBar {"
        "    background-color: #2d2d2d;"
        "    border: none;"
        "    border-bottom: 2px solid #1a1a1a;"
        "    padding: 5px;"
        "    spacing: 3px;"
        "}"
        "QToolButton {"
        "    background-color: #1a1a1a;"
        "    color: #ffffff;"
        "    border: 1px solid #3d3d3d;"
        "    border-radius: 4px;"
        "    padding: 6px 10px;"
        "    font-size: 13px;"
        "    font-weight: 600;"
        "    margin: 2px;"
        "}"
        "QToolButton:hover {"
        "    background-color: #3d3d3d;"
        "    border: 1px solid #00d4ff;"
        "}"
        "QToolButton:pressed {"
        "    background-color: #00d4ff;"
        "    color: #000000;"
        "}"
    );
    
    // Heading
    QAction *headingAction = new QAction("H1", this);
    headingAction->setToolTip("Heading (Ctrl+1)");
    headingAction->setShortcut(QKeySequence("Ctrl+1"));
    connect(headingAction, &QAction::triggered, this, &MarkdownEditorWidget::insertHeading);
    m_toolbar->addAction(headingAction);
    
    // Bold
    QAction *boldAction = new QAction("B", this);
    boldAction->setToolTip("Bold (Ctrl+B)");
    boldAction->setShortcut(QKeySequence::Bold);
    connect(boldAction, &QAction::triggered, this, &MarkdownEditorWidget::insertBold);
    m_toolbar->addAction(boldAction);
    
    // Italic
    QAction *italicAction = new QAction("I", this);
    italicAction->setToolTip("Italic (Ctrl+I)");
    italicAction->setShortcut(QKeySequence::Italic);
    connect(italicAction, &QAction::triggered, this, &MarkdownEditorWidget::insertItalic);
    m_toolbar->addAction(italicAction);
    
    // Strikethrough
    QAction *strikeAction = new QAction("S", this);
    strikeAction->setToolTip("Strikethrough (Ctrl+Shift+S)");
    strikeAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(strikeAction, &QAction::triggered, this, &MarkdownEditorWidget::insertStrikethrough);
    m_toolbar->addAction(strikeAction);
    
    m_toolbar->addSeparator();
    
    // Code
    QAction *codeAction = new QAction("</>");
    codeAction->setToolTip("Inline Code (Ctrl+`)");
    codeAction->setShortcut(QKeySequence("Ctrl+`"));
    connect(codeAction, &QAction::triggered, this, &MarkdownEditorWidget::insertCode);
    m_toolbar->addAction(codeAction);
    
    // Code Block
    QAction *codeBlockAction = new QAction("{ }");
    codeBlockAction->setToolTip("Code Block (Ctrl+Shift+C)");
    codeBlockAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(codeBlockAction, &QAction::triggered, this, &MarkdownEditorWidget::insertCodeBlock);
    m_toolbar->addAction(codeBlockAction);
    
    m_toolbar->addSeparator();
    
    // Horizontal Rule
    QAction *hrAction = new QAction("—");
    hrAction->setToolTip("Horizontal Rule (Ctrl+H)");
    hrAction->setShortcut(QKeySequence("Ctrl+H"));
    connect(hrAction, &QAction::triggered, this, &MarkdownEditorWidget::insertHorizontalRule);
    m_toolbar->addAction(hrAction);
    
    // Bullet List
    QAction *bulletAction = new QAction("• List");
    bulletAction->setToolTip("Bullet List (Ctrl+Shift+8)");
    bulletAction->setShortcut(QKeySequence("Ctrl+Shift+8"));
    connect(bulletAction, &QAction::triggered, this, &MarkdownEditorWidget::insertBulletList);
    m_toolbar->addAction(bulletAction);
    
    // Numbered List
    QAction *numberedAction = new QAction("1. List");
    numberedAction->setToolTip("Numbered List (Ctrl+Shift+7)");
    numberedAction->setShortcut(QKeySequence("Ctrl+Shift+7"));
    connect(numberedAction, &QAction::triggered, this, &MarkdownEditorWidget::insertNumberedList);
    m_toolbar->addAction(numberedAction);
    
    m_toolbar->addSeparator();
    
    // Link
    QAction *linkAction = new QAction("🔗 Link");
    linkAction->setToolTip("Insert Link (Ctrl+K)");
    linkAction->setShortcut(QKeySequence("Ctrl+K"));
    connect(linkAction, &QAction::triggered, this, &MarkdownEditorWidget::insertLink);
    m_toolbar->addAction(linkAction);
    
    // Image
    QAction *imageAction = new QAction("🖼️ Image");
    imageAction->setToolTip("Insert Image (Ctrl+Shift+I)");
    imageAction->setShortcut(QKeySequence("Ctrl+Shift+I"));
    connect(imageAction, &QAction::triggered, this, &MarkdownEditorWidget::insertImage);
    m_toolbar->addAction(imageAction);
    
    // Table
    QAction *tableAction = new QAction("⊞ Table");
    tableAction->setToolTip("Insert Table (Ctrl+T)");
    tableAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(tableAction, &QAction::triggered, this, &MarkdownEditorWidget::insertTable);
    m_toolbar->addAction(tableAction);
    
    m_toolbar->addSeparator();
    
    // Emoji
    QAction *emojiAction = new QAction("😀 Emoji", this);
    emojiAction->setToolTip("Insert Emoji (Ctrl+E)");
    emojiAction->setShortcut(QKeySequence("Ctrl+E"));
    connect(emojiAction, &QAction::triggered, this, &MarkdownEditorWidget::insertEmoji);
    m_toolbar->addAction(emojiAction);
    
    m_layout->addWidget(m_toolbar);
}

void MarkdownEditorWidget::insertFormatting(const QString &prefix, const QString &suffix, const QString &placeholder)
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
        
        // Select the placeholder
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, suffix.length() + text.length());
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, text.length());
        m_editor->setTextCursor(cursor);
    }
    
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertHeading()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText("# ");
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertBold()
{
    insertFormatting("**", "**", "bold text");
}

void MarkdownEditorWidget::insertItalic()
{
    insertFormatting("*", "*", "italic text");
}

void MarkdownEditorWidget::insertStrikethrough()
{
    insertFormatting("~~", "~~", "strikethrough text");
}

void MarkdownEditorWidget::insertCode()
{
    insertFormatting("`", "`", "code");
}

void MarkdownEditorWidget::insertCodeBlock()
{
    // List of popular programming languages
    QStringList languages;
    languages << "python" << "cpp" << "c" << "java" << "javascript" 
              << "typescript" << "go" << "rust" << "php" << "ruby"
              << "swift" << "kotlin" << "csharp" << "sql" << "bash"
              << "html" << "css" << "json" << "xml" << "yaml"
              << "markdown" << "plaintext";
    
    // Show language selection dialog
    bool ok;
    QString language = QInputDialog::getItem(
        this,
        "Select Language",
        "Choose programming language for syntax highlighting:",
        languages,
        0,  // Default to python
        true,  // Editable (user can type custom language)
        &ok
    );
    
    if (!ok) {
        // User cancelled
        return;
    }
    
    // Insert code block with language identifier
    QTextCursor cursor = m_editor->textCursor();
    cursor.insertText("```" + language + "\n");
    int pos = cursor.position();
    cursor.insertText("code here\n```");
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertHorizontalRule()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText("\n---\n\n");
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertBulletList()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText("- ");
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertNumberedList()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText("1. ");
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertLink()
{
    insertFormatting("[", "](url)", "link text");
}

void MarkdownEditorWidget::insertImage()
{
    insertFormatting("![", "](image-url)", "alt text");
}

void MarkdownEditorWidget::insertTable()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.insertText(
        "\n| Header 1 | Header 2 | Header 3 |\n"
        "|----------|----------|----------|\n"
        "| Cell 1   | Cell 2   | Cell 3   |\n"
        "| Cell 4   | Cell 5   | Cell 6   |\n\n"
    );
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

// ========== MarkdownEditor Implementation ==========

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_imageSaveDir("notes/images")
{
    // Set tab width to 2 spaces
    QFontMetrics metrics(font());
    setTabStopDistance(metrics.horizontalAdvance(' ') * 2);
    // Set default styling for the editor
    setStyleSheet(
        "QPlainTextEdit {"
        "    font-family: 'Consolas', 'Monaco', 'Courier New', monospace;"
        "    font-size: 14px;"
        "    border: 2px solid #2d2d2d;"
        "    border-radius: 12px;"
        "    padding: 15px;"
        "    background-color: #1a1a1a;"
        "    color: #ffffff;"
        "    line-height: 1.6;"
        "}"
        "QPlainTextEdit:focus {"
        "    border: 2px solid #00d4ff;"
        "}"
    );
    
    setPlaceholderText("Start typing your markdown notes here...\n\nTip: You can paste images directly from clipboard!");
}

MarkdownEditor::~MarkdownEditor()
{
    // Qt handles cleanup automatically
}

void MarkdownEditor::setImageSaveDirectory(const QString &path)
{
    m_imageSaveDir = path;
}

QString MarkdownEditor::imageSaveDirectory() const
{
    return m_imageSaveDir;
}

bool MarkdownEditor::canInsertFromMimeData(const QMimeData *source) const
{
    // Accept if it has an image or if the base class can handle it
    return source->hasImage() || QPlainTextEdit::canInsertFromMimeData(source);
}

void MarkdownEditor::insertFromMimeData(const QMimeData *source)
{
    // Check if the MIME data contains an image
    if (source->hasImage()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        
        if (!image.isNull()) {
            qDebug() << "Image pasted, size:" << image.size();
            
            // Save the image and get the relative path
            QString imagePath = saveImage(image);
            
            if (!imagePath.isEmpty()) {
                // Insert Markdown image syntax at cursor position
                QString markdownLink = QString("![Image](%1)").arg(imagePath);
                QTextCursor cursor = textCursor();
                cursor.insertText(markdownLink);
                
                qDebug() << "Inserted Markdown link:" << markdownLink;
            } else {
                qWarning() << "Failed to save image";
                // Insert error message
                QTextCursor cursor = textCursor();
                cursor.insertText("[Error: Failed to save image]");
            }
            
            return;
        }
    }
    
    // If not an image, use default behavior
    QPlainTextEdit::insertFromMimeData(source);
}

QString MarkdownEditor::saveImage(const QImage &image)
{
    // Define size limits (50MB max file size, 16K x 16K max dimensions)
    const qint64 MAX_FILE_SIZE = 50 * 1024 * 1024; // 50MB
    const int MAX_DIMENSION = 16384; // 16K pixels

    // Check image dimensions
    if (image.width() > MAX_DIMENSION || image.height() > MAX_DIMENSION) {
        qWarning() << "Image too large:" << image.size() << "- max dimension is" << MAX_DIMENSION;
        return QString();
    }

    // Ensure the directory exists
    if (!ensureImageDirectoryExists()) {
        qWarning() << "Failed to create image directory:" << m_imageSaveDir;
        return QString();
    }

    // Generate unique filename using hash without re-encoding
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    // Calculate hash directly from image bits to avoid double encoding
    QCryptographicHash hash(QCryptographicHash::Md5);
    QByteArrayView imageView(reinterpret_cast<const char*>(image.constBits()),
                             image.sizeInBytes());
    hash.addData(imageView);
    QString hashStr = hash.result().toHex().left(8);

    QString filename = QString("img_%1_%2.png").arg(timestamp).arg(hashStr);

    // Build full path
    QDir appDir(QCoreApplication::applicationDirPath());
    QString fullPath = appDir.filePath(m_imageSaveDir + "/" + filename);

    qDebug() << "Saving image to:" << fullPath;

    // Use QSaveFile for atomic write with rollback on failure
    QSaveFile saveFile(fullPath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << fullPath;
        return QString();
    }

    // Save image to buffer first to check size
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    if (!image.save(&buffer, "PNG")) {
        qWarning() << "Failed to encode image to PNG";
        saveFile.cancelWriting();
        return QString();
    }

    // Check file size
    if (imageData.size() > MAX_FILE_SIZE) {
        qWarning() << "Image file too large:" << imageData.size() << "bytes - max is" << MAX_FILE_SIZE;
        saveFile.cancelWriting();
        return QString();
    }

    // Write to file
    if (saveFile.write(imageData) == -1) {
        qWarning() << "Failed to write image data:" << saveFile.errorString();
        saveFile.cancelWriting();
        return QString();
    }

    // Commit the file (atomic operation)
    if (!saveFile.commit()) {
        qWarning() << "Failed to commit file:" << saveFile.errorString();
        return QString();
    }

    // Return relative path for Markdown link
    QString relativePath = m_imageSaveDir + "/" + filename;
    qDebug() << "Image saved successfully, relative path:" << relativePath;
    return relativePath;
}

bool MarkdownEditor::ensureImageDirectoryExists()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    QString fullPath = appDir.filePath(m_imageSaveDir);
    
    QDir imageDir(fullPath);
    if (!imageDir.exists()) {
        qDebug() << "Creating image directory:" << fullPath;
        return appDir.mkpath(m_imageSaveDir);
    }
    
    return true;
}

void MarkdownEditorWidget::insertEmoji()
{
    EmojiPickerDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString selectedEmoji = dialog.selectedEmoji();
        if (!selectedEmoji.isEmpty()) {
            QTextCursor cursor = m_editor->textCursor();
            cursor.insertText(selectedEmoji);
            m_editor->setFocus();
        }
    }
}
