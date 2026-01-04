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
 * @brief 具有 Markdown 支持和图片粘贴功能的自定义文本编辑器
 * 
 * 此编辑器扩展了 QPlainTextEdit 以提供：
 * - 从剪贴板粘贴图片的支持
 * - 图片自动保存到 notes/images/ 目录
 * - 为粘贴的图片插入 Markdown 链接
 */
/**
 * @brief 具有网格布局的自定义表情选择器对话框
 */
class EmojiPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmojiPickerDialog(QWidget *parent = nullptr);
    ~EmojiPickerDialog() override;

    /**
     * @brief 获取选中的表情
     * @return 选中的表情字符串，如果取消则为空
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
 * @brief 带有工具栏的 Markdown 编辑器容器部件
 */
class MarkdownEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = nullptr);
    ~MarkdownEditorWidget() override;

    /**
     * @brief 获取底层的文本编辑器
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
 * @brief 具有 Markdown 支持和图片粘贴功能的自定义文本编辑器
 */
class MarkdownEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor() override;

    /**
     * @brief 设置图片保存的目录
     * @param path 图片目录的绝对或相对路径
     */
    void setImageSaveDirectory(const QString &path);

    /**
     * @brief 获取当前的图片保存目录
     * @return 图片目录的路径
     */
    QString imageSaveDirectory() const;

protected:
    /**
     * @brief 检查 MIME 数据是否可以插入
     * @param source 要检查的 MIME 数据
     * @return 如果数据可以插入则返回 true
     */
    bool canInsertFromMimeData(const QMimeData *source) const override;

    /**
     * @brief 从 MIME 源插入数据（处理图片粘贴）
     * @param source 要插入的 MIME 数据
     */
    void insertFromMimeData(const QMimeData *source) override;

private:
    /**
     * @brief 将图片保存到磁盘并返回文件路径
     * @param image 要保存的图片
     * @return 保存图片的相对路径，失败时返回空字符串
     */
    QString saveImage(const QImage &image);

    /**
     * @brief 为图片生成唯一文件名
     * @return 格式为：img_时间戳_哈希.png 的文件名
     */
    QString generateImageFilename(const QImage &image) const;

    /**
     * @brief 确保图片保存目录存在
     * @return 如果目录存在或成功创建则返回 true
     */
    bool ensureImageDirectoryExists();

    QString m_imageSaveDir;  ///< 图片保存的目录
};

#endif // MARKDOWNEDITOR_H
