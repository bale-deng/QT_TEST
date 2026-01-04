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
    
    // 表格
    QAction *tableAction = new QAction("⊞ Table");
    tableAction->setToolTip("Insert Table (Ctrl+T)");
    tableAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(tableAction, &QAction::triggered, this, &MarkdownEditorWidget::insertTable);
    m_toolbar->addAction(tableAction);
    
    m_toolbar->addSeparator();
    
    // 表情
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
        // 包裹选中的文本
        QString selectedText = cursor.selectedText();
        cursor.insertText(prefix + selectedText + suffix);
    } else {
        // 插入占位符
        QString text = placeholder.isEmpty() ? "text" : placeholder;
        cursor.insertText(prefix + text + suffix);
        
        // 选中占位符
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
    insertFormatting("**", "**", "加粗文本");
}

void MarkdownEditorWidget::insertItalic()
{
    insertFormatting("*", "*", "斜体文本");
}

void MarkdownEditorWidget::insertStrikethrough()
{
    insertFormatting("~~", "~~", "删除线文本");
}

void MarkdownEditorWidget::insertCode()
{
    insertFormatting("`", "`", "代码");
}

void MarkdownEditorWidget::insertCodeBlock()
{
    // 常用编程语言列表
    QStringList languages;
    languages << "python" << "cpp" << "c" << "java" << "javascript" 
              << "typescript" << "go" << "rust" << "php" << "ruby"
              << "swift" << "kotlin" << "csharp" << "sql" << "bash"
              << "html" << "css" << "json" << "xml" << "yaml"
              << "markdown" << "plaintext";
    
    // 显示语言选择对话框
    bool ok;
    QString language = QInputDialog::getItem(
        this,
        "选择语言",
        "为语法高亮选择编程语言：",
        languages,
        0,  // 默认为 python
        true,  // 可编辑（用户可以输入自定义语言）
        &ok
    );
    
    if (!ok) {
        // 用户取消
        return;
    }
    
    // 插入带有语言标识的代码块
    QTextCursor cursor = m_editor->textCursor();
    cursor.insertText("```" + language + "
");
    int pos = cursor.position();
    cursor.insertText("在此处输入代码
```");
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

void MarkdownEditorWidget::insertHorizontalRule()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText("
---

");
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
    insertFormatting("[", "](url)", "链接文本");
}

void MarkdownEditorWidget::insertImage()
{
    insertFormatting("![", "](image-url)", "替代文本");
}

void MarkdownEditorWidget::insertTable()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.insertText(
        "
| 表头 1 | 表头 2 | 表头 3 |
"
        "|----------|----------|----------|
"
        "| 单元格 1 | 单元格 2 | 单元格 3 |
"
        "| 单元格 4 | 单元格 5 | 单元格 6 |

"
    );
    m_editor->setTextCursor(cursor);
    m_editor->setFocus();
}

// ========== MarkdownEditor 实现 ==========

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_imageSaveDir("notes/images")
{
    // 设置制表符宽度为 2 个空格
    QFontMetrics metrics(font());
    setTabStopDistance(metrics.horizontalAdvance(' ') * 2);
    // 为编辑器设置默认样式
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
    
    setPlaceholderText("在此处开始输入您的 Markdown 笔记...

提示：您可以直接从剪贴板粘贴图片！");
}

MarkdownEditor::~MarkdownEditor()
{
    // Qt 自动处理清理工作
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
    // 如果包含图片或基类可以处理，则接受
    return source->hasImage() || QPlainTextEdit::canInsertFromMimeData(source);
}

void MarkdownEditor::insertFromMimeData(const QMimeData *source)
{
    // 检查 MIME 数据是否包含图片
    if (source->hasImage()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        
        if (!image.isNull()) {
            qDebug() << "图片已粘贴，大小：" << image.size();
            
            // 保存图片并获取相对路径
            QString imagePath = saveImage(image);
            
            if (!imagePath.isEmpty()) {
                // 在光标位置插入 Markdown 图片语法
                QString markdownLink = QString("![Image](%1)").arg(imagePath);
                QTextCursor cursor = textCursor();
                cursor.insertText(markdownLink);
                
                qDebug() << "已插入 Markdown 链接：" << markdownLink;
            } else {
                qWarning() << "无法保存图片";
                // 插入错误信息
                QTextCursor cursor = textCursor();
                cursor.insertText("[错误：无法保存图片]");
            }
            
            return;
        }
    }
    
    // 如果不是图片，使用默认行为
    QPlainTextEdit::insertFromMimeData(source);
}

QString MarkdownEditor::saveImage(const QImage &image)
{
    // 定义大小限制（最大文件大小 50MB，最大尺寸 16K x 16K）
    const qint64 MAX_FILE_SIZE = 50 * 1024 * 1024; // 50MB
    const int MAX_DIMENSION = 16384; // 16K 像素

    // 检查图片尺寸
    if (image.width() > MAX_DIMENSION || image.height() > MAX_DIMENSION) {
        qWarning() << "图片太大：" << image.size() << " - 最大尺寸为" << MAX_DIMENSION;
        return QString();
    }

    // 确保目录存在
    if (!ensureImageDirectoryExists()) {
        qWarning() << "无法创建图片目录：" << m_imageSaveDir;
        return QString();
    }

    // 使用哈希生成唯一文件名，不重新编码
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    // 直接从图片位计算哈希以避免二次编码
    QCryptographicHash hash(QCryptographicHash::Md5);
    QByteArrayView imageView(reinterpret_cast<const char*>(image.constBits()),
                             image.sizeInBytes());
    hash.addData(imageView);
    QString hashStr = hash.result().toHex().left(8);

    QString filename = QString("img_%1_%2.png").arg(timestamp).arg(hashStr);

    // 构建完整路径
    QDir appDir(QCoreApplication::applicationDirPath());
    QString fullPath = appDir.filePath(m_imageSaveDir + "/" + filename);

    qDebug() << "正在保存图片到：" << fullPath;

    // 使用 QSaveFile 进行原子写入，失败时回滚
    QSaveFile saveFile(fullPath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件进行写入：" << fullPath;
        return QString();
    }

    // 先将图片保存到缓冲区以检查大小
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    if (!image.save(&buffer, "PNG")) {
        qWarning() << "无法将图片编码为 PNG";
        saveFile.cancelWriting();
        return QString();
    }

    // 检查文件大小
    if (imageData.size() > MAX_FILE_SIZE) {
        qWarning() << "图片文件太大：" << imageData.size() << "字节 - 最大限制为" << MAX_FILE_SIZE;
        saveFile.cancelWriting();
        return QString();
    }

    // 写入文件
    if (saveFile.write(imageData) == -1) {
        qWarning() << "无法写入图片数据：" << saveFile.errorString();
        saveFile.cancelWriting();
        return QString();
    }

    // 提交文件（原子操作）
    if (!saveFile.commit()) {
        qWarning() << "无法提交文件：" << saveFile.errorString();
        return QString();
    }

    // 返回用于 Markdown 链接的相对路径
    QString relativePath = m_imageSaveDir + "/" + filename;
    qDebug() << "图片保存成功，相对路径：" << relativePath;
    return relativePath;
}

bool MarkdownEditor::ensureImageDirectoryExists()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    QString fullPath = appDir.filePath(m_imageSaveDir);
    
    QDir imageDir(fullPath);
    if (!imageDir.exists()) {
        qDebug() << "正在创建图片目录：" << fullPath;
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
