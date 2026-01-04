#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QProcess>
#include <QLineEdit>
#include <QTextEdit>
#include <QTabWidget>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QComboBox>
#include <QTimer>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QWheelEvent>
#include <QSplitter>
#include "MarkdownEditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessReadyRead();
    void onProcessTimeout();
    void onMarkdownTextChanged();
    void attemptPythonRestart();

    // 文件操作
    void openFile();
    void saveFile();
    void saveFileAs();

    // 缩放操作
    void zoomIn();
    void zoomOut();
    void zoomReset();

private:
    QString enhanceMarkdownPreview(const QString &markdown);
    QString processInlineMarkdown(const QString &text);
    void onDefaultCityChanged(int index);
    void loadWeatherForDefaultCity();
    void applyTheme(const QString &themeName);
    void toggleTheme();
    void updateWindowTitle();
    void setZoom(double level);
    bool maybeSave();

private:
    void setupUI();
    QString getPythonScriptPath() const;
    QString findPythonExecutable() const;
    QString formatWeatherHTML(const QString &output, const QString &city) const;
    QString getSkeletonHTML(const QString &city) const;
    void handleWeatherError(const QString &errorMsg);
    void restartPythonProcess();
    bool isPythonProcessHealthy() const;

    // 主布局组件
    QWidget *m_centralWidget;
    QHBoxLayout *m_mainLayout;
    
    // 侧边栏组件
    QWidget *m_sidebar;
    QVBoxLayout *m_sidebarLayout;
    QLabel *m_weatherCityLabel;
    QLabel *m_weatherTempLabel;
    QLabel *m_weatherConditionLabel;
    QLabel *m_weatherDetailsLabel;
    QComboBox *m_defaultCityCombo;
    QPushButton *m_refreshButton;
    QPushButton *m_themeButton;
    
    // 主内容区域
    QWidget *m_contentArea;
    QVBoxLayout *m_contentLayout;
    QTabWidget *m_tabWidget;

    // Markdown 编辑器组件
    MarkdownEditorWidget *m_markdownEditorWidget;
    QPlainTextEdit *m_markdownEditor;
    QTextBrowser *m_markdownPreview;

    // 缩放 UI
    QPushButton *m_zoomResetButton;
    

    
    // 后端
    QProcess *m_process;
    QTimer *m_processTimeout;
    QString m_currentCity;
    QString m_processOutput;
    QSettings *m_settings;
    
    // Python 进程管理
    int m_pythonRestartAttempts;
    int m_maxRestartAttempts;
    QTimer *m_restartTimer;
    QString m_pendingCity;

    // 文件管理
    QString m_currentFilePath;
    bool m_isModified;
    QStringConverter::Encoding m_currentEncoding;  // 追踪文件编码
    QByteArray m_fileBOM;  // 追踪 BOM 字节以便保留

    // 缩放管理
    double m_currentZoom;
    double m_previewBaseZoom;  // 追踪预览的基础缩放因子
    QTimer *m_previewUpdateTimer;  // 预览更新的防抖定时器
    QTimer *m_zoomSettingsSaveTimer;  // 缩放设置写入的防抖定时器
    static constexpr double ZOOM_STEP = 0.1;
    static constexpr double ZOOM_MIN = 0.5;
    static constexpr double ZOOM_MAX = 3.0;
    static constexpr double DEFAULT_ZOOM = 1.0;
};

#endif // MAINWINDOW_H
