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

    // File operations
    void openFile();
    void saveFile();
    void saveFileAs();

    // Zoom operations
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

    // Main layout components
    QWidget *m_centralWidget;
    QHBoxLayout *m_mainLayout;
    
    // Sidebar components
    QWidget *m_sidebar;
    QVBoxLayout *m_sidebarLayout;
    QLabel *m_weatherCityLabel;
    QLabel *m_weatherTempLabel;
    QLabel *m_weatherConditionLabel;
    QLabel *m_weatherDetailsLabel;
    QComboBox *m_defaultCityCombo;
    QPushButton *m_refreshButton;
    QPushButton *m_themeButton;
    
    // Main content area
    QWidget *m_contentArea;
    QVBoxLayout *m_contentLayout;
    QTabWidget *m_tabWidget;

    // Markdown editor components
    MarkdownEditorWidget *m_markdownEditorWidget;
    QPlainTextEdit *m_markdownEditor;
    QTextBrowser *m_markdownPreview;

    // Zoom UI
    QPushButton *m_zoomResetButton;
    

    
    // Backend
    QProcess *m_process;
    QTimer *m_processTimeout;
    QString m_currentCity;
    QString m_processOutput;
    QSettings *m_settings;
    
    // Python process management
    int m_pythonRestartAttempts;
    int m_maxRestartAttempts;
    QTimer *m_restartTimer;
    QString m_pendingCity;

    // File management
    QString m_currentFilePath;
    bool m_isModified;
    QStringConverter::Encoding m_currentEncoding;  // Track file encoding
    QByteArray m_fileBOM;  // Track BOM bytes for preservation

    // Zoom management
    double m_currentZoom;
    double m_previewBaseZoom;  // Track preview's base zoom factor
    QTimer *m_previewUpdateTimer;  // Debounce timer for preview updates
    QTimer *m_zoomSettingsSaveTimer;  // Debounce timer for zoom settings writes
    static constexpr double ZOOM_STEP = 0.1;
    static constexpr double ZOOM_MIN = 0.5;
    static constexpr double ZOOM_MAX = 3.0;
    static constexpr double DEFAULT_ZOOM = 1.0;
};

#endif // MAINWINDOW_H
