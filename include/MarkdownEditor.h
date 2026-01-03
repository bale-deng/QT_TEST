#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QPlainTextEdit>
#include <QMimeData>
#include <QImage>
#include <QString>
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QStringList>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>

/**
 * @brief Custom text editor with Markdown support and image paste functionality
 * 
 * This editor extends QPlainTextEdit to provide:
 * - Image paste support from clipboard
 * - Automatic image saving to notes/images/ directory
 * - Markdown link insertion for pasted images
 */
/**
 * @brief Custom emoji picker dialog with grid layout
 */
class EmojiPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmojiPickerDialog(QWidget *parent = nullptr);
    ~EmojiPickerDialog() override;

    /**
     * @brief Get the selected emoji
     * @return The selected emoji string, or empty if cancelled
     */
    QString selectedEmoji() const;

private slots:
    void onEmojiClicked();

private:
    void setupUI();
    QStringList getEmojiList() const;
    
    QString m_selectedEmoji;
    QGridLayout *m_gridLayout;
    QScrollArea *m_scrollArea;
};

/**
 * @brief Container widget for markdown editor with toolbar
 */
class MarkdownEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = nullptr);
    ~MarkdownEditorWidget() override;

    /**
     * @brief Get the underlying text editor
     */
    QPlainTextEdit* editor() const;

private slots:
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
    void insertEmoji();

private:
    void setupToolbar();
    void insertFormatting(const QString &prefix, const QString &suffix, const QString &placeholder = "");
    
    QVBoxLayout *m_layout;
    QToolBar *m_toolbar;
    class MarkdownEditor *m_editor;
};

/**
 * @brief Custom text editor with Markdown support and image paste functionality
 */
class MarkdownEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor() override;

    /**
     * @brief Set the directory where images will be saved
     * @param path Absolute or relative path to the images directory
     */
    void setImageSaveDirectory(const QString &path);

    /**
     * @brief Get the current image save directory
     * @return Path to the images directory
     */
    QString imageSaveDirectory() const;

protected:
    /**
     * @brief Check if the MIME data can be inserted
     * @param source The MIME data to check
     * @return true if the data can be inserted
     */
    bool canInsertFromMimeData(const QMimeData *source) const override;

    /**
     * @brief Insert data from MIME source (handles image paste)
     * @param source The MIME data to insert
     */
    void insertFromMimeData(const QMimeData *source) override;

private:
    /**
     * @brief Save an image to disk and return the file path
     * @param image The image to save
     * @return Relative path to the saved image, or empty string on failure
     */
    QString saveImage(const QImage &image);

    /**
     * @brief Generate a unique filename for an image
     * @return Filename in format: img_timestamp_hash.png
     */
    QString generateImageFilename(const QImage &image) const;

    /**
     * @brief Ensure the image save directory exists
     * @return true if directory exists or was created successfully
     */
    bool ensureImageDirectoryExists();

    QString m_imageSaveDir;  ///< Directory where images are saved
};

#endif // MARKDOWNEDITOR_H
