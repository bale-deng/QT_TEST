#include "MainWindow.h"
#include <QDebug>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTabWidget>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QTimer>
#include <QRegularExpression>
#include "MarkdownEditor.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QWheelEvent>
#include <QStringConverter>
#include <QSaveFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_sidebar(nullptr)
    , m_sidebarLayout(nullptr)
    , m_weatherCityLabel(nullptr)
    , m_weatherTempLabel(nullptr)
    , m_weatherConditionLabel(nullptr)
    , m_weatherDetailsLabel(nullptr)
    , m_defaultCityCombo(nullptr)
    , m_refreshButton(nullptr)
    , m_contentArea(nullptr)
    , m_contentLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_markdownEditorWidget(nullptr)
    , m_markdownEditor(nullptr)
    , m_markdownPreview(nullptr)
    , m_zoomResetButton(nullptr)
    , m_process(new QProcess(this))
    , m_processTimeout(new QTimer(this))
    , m_pythonRestartAttempts(0)
    , m_maxRestartAttempts(3)
    , m_restartTimer(new QTimer(this))
    , m_currentFilePath("")
    , m_isModified(false)
    , m_currentEncoding(QStringConverter::Utf8)
    , m_currentZoom(DEFAULT_ZOOM)
    , m_previewBaseZoom(1.0)
    , m_previewUpdateTimer(new QTimer(this))
    , m_zoomSettingsSaveTimer(new QTimer(this))
{
    // Initialize settings with config.ini in the application directory
    QString settingsPath = QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);

    // Configure process timeout (30 seconds)
    m_processTimeout->setSingleShot(true);
    m_processTimeout->setInterval(30000);
    connect(m_processTimeout, &QTimer::timeout, this, &MainWindow::onProcessTimeout);
    
    // Configure restart timer
    m_restartTimer->setSingleShot(true);
    connect(m_restartTimer, &QTimer::timeout, this, &MainWindow::attemptPythonRestart);
    
    // Configure preview update timer for debouncing (200ms delay)
    m_previewUpdateTimer->setSingleShot(true);
    m_previewUpdateTimer->setInterval(200);
    connect(m_previewUpdateTimer, &QTimer::timeout, this, [this]() {
        QString markdownText = m_markdownEditor->toPlainText();
        QString enhancedHtml = enhanceMarkdownPreview(markdownText);
        m_markdownPreview->setHtml(enhancedHtml);
    });
    
    // Configure zoom settings save timer for debouncing (500ms delay)
    m_zoomSettingsSaveTimer->setSingleShot(true);
    m_zoomSettingsSaveTimer->setInterval(500);
    connect(m_zoomSettingsSaveTimer, &QTimer::timeout, this, [this]() {
        m_settings->setValue("zoomLevel", m_currentZoom);
    });

    setupUI();
    setWindowTitle("mdCoder");
    setWindowIcon(QIcon(":/icon/md_coder.ico"));

    // Restore window geometry and state
    if (m_settings->contains("geometry")) {
        restoreGeometry(m_settings->value("geometry").toByteArray());
    } else {
        resize(1200, 700);
    }
    if (m_settings->contains("windowState")) {
        restoreState(m_settings->value("windowState").toByteArray());
    }

    // Restore zoom level
    if (m_settings->contains("zoomLevel")) {
        double savedZoom = m_settings->value("zoomLevel").toDouble();
        setZoom(savedZoom);
    }

    // Load weather for default city on startup
    QTimer::singleShot(500, this, &MainWindow::loadWeatherForDefaultCity);
}

MainWindow::~MainWindow()
{
    // Ensure process is terminated
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(3000)) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
    }
    // Qt handles cleanup automatically through parent-child relationship
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Check for unsaved changes
    if (!maybeSave()) {
        event->ignore();
        return;
    }

    // Save window geometry and state
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());

    // Ensure process is terminated before closing
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(3000)) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::setupUI()
{
    // Create central widget
    m_centralWidget = new QWidget(this);
    m_centralWidget->setObjectName("centralWidget");
    setCentralWidget(m_centralWidget);

    // Create main horizontal layout
    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // ========== SIDEBAR ==========
    m_sidebar = new QWidget(this);
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(240);
    
    m_sidebarLayout = new QVBoxLayout(m_sidebar);
    m_sidebarLayout->setContentsMargins(15, 15, 15, 15);
    m_sidebarLayout->setSpacing(10);
    
    // Sidebar title
    QLabel *sidebarTitle = new QLabel("🌤️ Weather", this);
    sidebarTitle->setObjectName("sidebarTitle");
    m_sidebarLayout->addWidget(sidebarTitle);
    
    // Default city selector
    QLabel *defaultCityLabel = new QLabel("Default City:", this);
    defaultCityLabel->setProperty("class", "secondaryLabel");
    m_sidebarLayout->addWidget(defaultCityLabel);
    
    m_defaultCityCombo = new QComboBox(this);
    m_defaultCityCombo->addItems({
        "Beijing", "Shanghai", "Guangzhou", "Shenzhen", 
        "Zhuhai", "Foshan", "Maoming",
        "Chengdu", "Hangzhou", "Wuhan", "Xi'an",
        "Chongqing", "Tianjin", "Nanjing", "Suzhou",
        "Hong Kong", "Taipei", "Tokyo", "Seoul",
        "Singapore", "London", "New York", "Paris"
    });
    
    // Load saved default city
    QString savedCity = m_settings->value("defaultCity", "Beijing").toString();
    int index = m_defaultCityCombo->findText(savedCity);
    if (index >= 0) {
        m_defaultCityCombo->setCurrentIndex(index);
    }
    
    m_sidebarLayout->addWidget(m_defaultCityCombo);
    
    // Refresh button
    m_refreshButton = new QPushButton("🔄 Refresh Weather", this);
    m_sidebarLayout->addWidget(m_refreshButton);

    // Theme toggle button
    QString currentTheme = m_settings->value("theme", "dark").toString();
    m_themeButton = new QPushButton(currentTheme == "dark" ? "☀️ Light Mode" : "🌙 Dark Mode", this);
    m_sidebarLayout->addWidget(m_themeButton);
    
    // Separator line
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setObjectName("sidebarSeparator");
    m_sidebarLayout->addWidget(separator);
    
    // Weather display area
    m_weatherCityLabel = new QLabel("Loading...", this);
    m_weatherCityLabel->setObjectName("weatherCityLabel");
    m_weatherCityLabel->setWordWrap(true);
    m_sidebarLayout->addWidget(m_weatherCityLabel);
    
    m_weatherTempLabel = new QLabel("--°C", this);
    m_weatherTempLabel->setObjectName("weatherTempLabel");
    m_sidebarLayout->addWidget(m_weatherTempLabel);
    
    m_weatherConditionLabel = new QLabel("--", this);
    m_weatherConditionLabel->setObjectName("weatherConditionLabel");
    m_weatherConditionLabel->setWordWrap(true);
    m_sidebarLayout->addWidget(m_weatherConditionLabel);
    
    m_weatherDetailsLabel = new QLabel("", this);
    m_weatherDetailsLabel->setObjectName("weatherDetailsLabel");
    m_weatherDetailsLabel->setWordWrap(true);
    m_weatherDetailsLabel->setTextFormat(Qt::RichText);
    m_sidebarLayout->addWidget(m_weatherDetailsLabel);
    
    // Add stretch to push everything to the top
    m_sidebarLayout->addStretch();
    
    m_mainLayout->addWidget(m_sidebar);
    
    // ========== CONTENT AREA ==========
    m_contentArea = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentArea);
    m_contentLayout->setContentsMargins(20, 20, 20, 20);
    m_contentLayout->setSpacing(15);

    // Create toolbar for file and zoom operations
    QWidget *toolbarWidget = new QWidget(this);
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbarWidget);
    toolbarLayout->setContentsMargins(0, 0, 0, 10);
    toolbarLayout->setSpacing(8);

    // File operation buttons - simplified icons
    QPushButton *newButton = new QPushButton("💬", this);
    newButton->setToolTip("New file (Ctrl+N)");
    newButton->setFixedSize(35, 35);
    connect(newButton, &QPushButton::clicked, this, [this]() {
        if (maybeSave()) {
            m_markdownEditor->clear();
            m_currentFilePath.clear();
            m_isModified = false;
            updateWindowTitle();
        }
    });
    toolbarLayout->addWidget(newButton);

    QPushButton *openButton = new QPushButton("📂", this);
    openButton->setToolTip("Open file (Ctrl+O)");
    openButton->setFixedSize(35, 35);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    toolbarLayout->addWidget(openButton);

    QPushButton *saveButton = new QPushButton("✔️", this);
    saveButton->setToolTip("Save file (Ctrl+S)");
    saveButton->setFixedSize(35, 35);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveFile);
    toolbarLayout->addWidget(saveButton);

    QPushButton *saveAsButton = new QPushButton("💾", this);
    saveAsButton->setToolTip("Save as (Ctrl+Shift+S)");
    saveAsButton->setFixedSize(35, 35);
    connect(saveAsButton, &QPushButton::clicked, this, &MainWindow::saveFileAs);
    toolbarLayout->addWidget(saveAsButton);

    toolbarLayout->addSpacing(15);

    // Zoom control buttons - compact design
    QPushButton *zoomOutButton = new QPushButton("−", this);
    zoomOutButton->setToolTip("Zoom out (Ctrl+-)");
    zoomOutButton->setFixedSize(30, 30);
    zoomOutButton->setStyleSheet("font-size: 18px; font-weight: bold;");
    connect(zoomOutButton, &QPushButton::clicked, this, &MainWindow::zoomOut);
    toolbarLayout->addWidget(zoomOutButton);

    m_zoomResetButton = new QPushButton("100%", this);
    m_zoomResetButton->setToolTip("Reset zoom (Ctrl+0)");
    m_zoomResetButton->setFixedSize(55, 30);
    m_zoomResetButton->setStyleSheet("font-size: 12px; font-weight: bold;");
    connect(m_zoomResetButton, &QPushButton::clicked, this, &MainWindow::zoomReset);
    toolbarLayout->addWidget(m_zoomResetButton);

    QPushButton *zoomInButton = new QPushButton("+", this);
    zoomInButton->setToolTip("Zoom in (Ctrl++)");
    zoomInButton->setFixedSize(30, 30);
    zoomInButton->setStyleSheet("font-size: 18px; font-weight: bold;");
    connect(zoomInButton, &QPushButton::clicked, this, &MainWindow::zoomIn);
    toolbarLayout->addWidget(zoomInButton);

    toolbarLayout->addStretch();

    m_contentLayout->addWidget(toolbarWidget);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    
    // ========== NOTES TAB ==========
    m_markdownEditorWidget = new MarkdownEditorWidget(this);
    m_markdownEditor = m_markdownEditorWidget->editor();
    m_markdownPreview = new QTextBrowser(this);
    m_markdownPreview->setObjectName("markdownPreview");
    m_markdownPreview->setOpenExternalLinks(true);

    // Use QSplitter to allow resizing between editor and preview
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_markdownEditorWidget);
    splitter->addWidget(m_markdownPreview);

    // Set initial sizes (50% each)
    splitter->setSizes(QList<int>() << 500 << 500);

    // Allow both widgets to be resized
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    // Style the splitter handle
    splitter->setHandleWidth(3);
    splitter->setStyleSheet(
        "QSplitter::handle {"
        "    background-color: #3d3d3d;"
        "}"
        "QSplitter::handle:hover {"
        "    background-color: #00d4ff;"
        "}"
    );

    QWidget *notesWidget = new QWidget(this);
    QVBoxLayout *notesLayout = new QVBoxLayout(notesWidget);
    notesLayout->setContentsMargins(0, 0, 0, 0);
    notesLayout->setSpacing(0);
    notesLayout->addWidget(splitter);

    // Add tabs
    m_tabWidget->addTab(notesWidget, "📝 Notes");
    
    m_contentLayout->addWidget(m_tabWidget);
    m_mainLayout->addWidget(m_contentArea);
    
    // Apply saved theme
    applyTheme(currentTheme);

    // Connect signals
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::loadWeatherForDefaultCity);
    connect(m_themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    connect(m_defaultCityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDefaultCityChanged);
    // Connect markdown editor to debounced preview update
    connect(m_markdownEditor, &QPlainTextEdit::textChanged, this, [this]() {
        m_previewUpdateTimer->start();  // Restart timer on each change (debouncing)
    });

    // Setup keyboard shortcuts for file operations
    QShortcut *newShortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(newShortcut, &QShortcut::activated, this, [this]() {
        if (maybeSave()) {
            m_markdownEditor->clear();
            m_currentFilePath.clear();
            m_currentEncoding = QStringConverter::Utf8;  // Reset to UTF-8 for new files
            m_fileBOM.clear();  // Clear BOM for new files
            m_isModified = false;
            updateWindowTitle();
        }
    });

    QShortcut *openShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(openShortcut, &QShortcut::activated, this, &MainWindow::openFile);

    QShortcut *saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::saveFile);

    QShortcut *saveAsShortcut = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
    connect(saveAsShortcut, &QShortcut::activated, this, &MainWindow::saveFileAs);

    // Setup keyboard shortcuts for zoom operations
    QShortcut *zoomInShortcut1 = new QShortcut(QKeySequence("Ctrl+="), this);
    connect(zoomInShortcut1, &QShortcut::activated, this, &MainWindow::zoomIn);

    QShortcut *zoomInShortcut2 = new QShortcut(QKeySequence("Ctrl++"), this);
    connect(zoomInShortcut2, &QShortcut::activated, this, &MainWindow::zoomIn);

    QShortcut *zoomOutShortcut = new QShortcut(QKeySequence("Ctrl+-"), this);
    connect(zoomOutShortcut, &QShortcut::activated, this, &MainWindow::zoomOut);

    QShortcut *zoomResetShortcut = new QShortcut(QKeySequence("Ctrl+0"), this);
    connect(zoomResetShortcut, &QShortcut::activated, this, &MainWindow::zoomReset);

    // Track text changes for modified flag
    connect(m_markdownEditor, &QPlainTextEdit::textChanged, this, [this]() {
        if (!m_isModified) {
            m_isModified = true;
            updateWindowTitle();
        }
    });

    // Update window title initially
    updateWindowTitle();
}

void MainWindow::applyTheme(const QString &themeName)
{
    // Whitelist validation: only allow known themes
    static const QStringList allowedThemes = {"dark", "light"};
    QString validatedTheme = themeName;

    if (!allowedThemes.contains(themeName)) {
        qWarning() << "Invalid theme name:" << themeName << "- falling back to 'dark'";
        validatedTheme = "dark";
    }

    QString qssPath = QString(":/resources/styles/%1.qss").arg(validatedTheme);
    QFile file(qssPath);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
        
        m_settings->setValue("theme", validatedTheme);
        if (m_themeButton) {
            m_themeButton->setText(validatedTheme == "dark" ? "☀️ Light Mode" : "🌙 Dark Mode");
        }
        
        // Refresh weather display to update HTML colors if needed
        // (Though the HTML currently has some hardcoded colors, 
        // we might want to refactor that later)
        if (!m_currentCity.isEmpty()) {
            // Update the preview if it exists
            onMarkdownTextChanged();
        }
    } else {
        qWarning() << "Could not load theme:" << qssPath;
    }
}

void MainWindow::toggleTheme()
{
    QString currentTheme = m_settings->value("theme", "dark").toString();
    applyTheme(currentTheme == "dark" ? "light" : "dark");
}

QString MainWindow::getPythonScriptPath() const
{
    // Get application directory using Qt's cross-platform API
    QString appDir = QCoreApplication::applicationDirPath();
    
    // Build path to Python script using QDir for cross-platform separator handling
    QDir pythonDir(appDir);
    QString scriptPath = pythonDir.filePath("python/weather_service.py");
    
    // Clean path to normalize separators and remove redundant elements
    scriptPath = QDir::cleanPath(scriptPath);
    
    // Verify the script exists
    QFileInfo scriptInfo(scriptPath);
    if (!scriptInfo.exists() || !scriptInfo.isFile()) {
        qWarning() << "Python script not found at:" << scriptPath;
        return QString();
    }
    
    return scriptPath;
}

QString MainWindow::findPythonExecutable() const
{
    QString appDir = QCoreApplication::applicationDirPath();
    
    // Priority 1: Check for embedded Python runtime in python_runtime/
#ifdef Q_OS_WIN
    QString embeddedPython = QDir(appDir).filePath("python_runtime/python.exe");
#else
    QString embeddedPython = QDir(appDir).filePath("python_runtime/bin/python3");
#endif
    
    QFileInfo embeddedInfo(embeddedPython);
    if (embeddedInfo.exists() && embeddedInfo.isExecutable()) {
        qDebug() << "Using embedded Python runtime:" << embeddedPython;
        return embeddedPython;
    }
    
    // Priority 2: Check for Python in PATH (system installation)
    // Try common Python executable names
    QStringList pythonCandidates;
#ifdef Q_OS_WIN
    pythonCandidates << "python.exe" << "python3.exe";
#else
    pythonCandidates << "python3" << "python";
#endif
    
    for (const QString &candidate : pythonCandidates) {
        QString pythonPath = QStandardPaths::findExecutable(candidate);
        if (!pythonPath.isEmpty()) {
            qDebug() << "Using system Python:" << pythonPath;
            return pythonPath;
        }
    }
    
    // Priority 3: Fallback to default name (will fail if not in PATH)
    qWarning() << "No Python executable found. Falling back to default 'python'.";
#ifdef Q_OS_WIN
    return "python";
#else
    return "python3";
#endif
}

QString MainWindow::formatWeatherHTML(const QString &output, const QString &city) const
{
    qDebug() << "Parsing JSON weather data for:" << city;
    
    QString theme = m_settings->value("theme", "dark").toString();
    bool isDark = (theme == "dark");
    
    QString accentColor = isDark ? "#00d4ff" : "#0071e3";
    QString secondaryTextColor = isDark ? "#aaaaaa" : "#86868b";
    QString textColor = isDark ? "#cccccc" : "#1d1d1f";
    QString conditionColor = isDark ? "#5dade2" : "#0071e3";
    
    // Parse JSON output from Python
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Failed to parse JSON output";
        return QString(
            "<div style='font-family: \"SF Pro Display\", \"Segoe UI\", \"Microsoft YaHei\", sans-serif;'>"
            "<div style='font-size: 24px; font-weight: bold; color: #ff4757; margin-bottom: 20px;'>"
            "❌ Parse Error"
            "</div>"
            "<pre style='font-size: 14px; color: %1; white-space: pre-wrap;'>%2</pre>"
            "</div>"
        ).arg(secondaryTextColor).arg(output.toHtmlEscaped());
    }
    
    QJsonObject obj = doc.object();
    
    // Check for error status
    QString status = obj["status"].toString();
    if (status == "error") {
        QString errorMsg = obj["error_message"].toString();
        return QString(
            "<div style='font-family: \"SF Pro Display\", \"Segoe UI\", \"Microsoft YaHei\", sans-serif;'>"
            "<div style='font-size: 28px; font-weight: bold; color: #ff4757; margin-bottom: 15px;'>"
            "❌ Error"
            "</div>"
            "<div style='font-size: 16px; color: %1;'>"
            "%2"
            "</div>"
            "</div>"
        ).arg(secondaryTextColor).arg(errorMsg.toHtmlEscaped());
    }
    
    // Extract weather data from JSON
    QString cityName = obj["city"].toString();
    double temperature = obj["temperature"].toDouble();
    double feelsLike = obj["feels_like"].toDouble();
    int humidity = obj["humidity"].toInt();
    QString description = obj["description"].toString();
    double windSpeed = obj["wind_speed"].toDouble();
    double pressure = obj["pressure"].toDouble();
    
    // Build HTML with styled formatting
    QString html = QString(
        "<div style='font-family: \"SF Pro Display\", \"Segoe UI\", \"Microsoft YaHei\", sans-serif;'>"
        
        // City name
        "<div style='font-size: 36px; font-weight: bold; color: %1; margin-bottom: 25px;'>"
        "📍 %2"
        "</div>"
        
        // Temperature
        "<div style='font-size: 64px; font-weight: bold; color: %1; margin-bottom: 15px;'>"
        "%3°C"
        "</div>"
        
        // Conditions
        "<div style='font-size: 28px; font-weight: 600; color: %4; margin-bottom: 25px;'>"
        "%5"
        "</div>"
        
        // Feels like
        "<div style='font-size: 20px; color: %6; margin-bottom: 20px;'>"
        "🌡️ Feels Like: <span style='font-weight: bold; color: #ffa502;'>%7°C</span>"
        "</div>"
        
        // Details section
        "<div style='font-size: 18px; color: %8; line-height: 2.2;'>"
        "<div>💧 Humidity: <span style='font-weight: 600; color: #48dbfb;'>%9%</span></div>"
        "<div>💨 Wind Speed: <span style='font-weight: 600; color: #1dd1a1;'>%10 km/h</span></div>"
        "<div>🔽 Pressure: <span style='font-weight: 600; color: #ee5a6f;'>%11 mb</span></div>"
        "</div>"
        
        "</div>"
    ).arg(accentColor)
     .arg(cityName)
     .arg(temperature, 0, 'f', 1)
     .arg(conditionColor)
     .arg(description)
     .arg(secondaryTextColor)
     .arg(feelsLike, 0, 'f', 1)
     .arg(textColor)
     .arg(humidity)
     .arg(windSpeed, 0, 'f', 1)
     .arg(pressure, 0, 'f', 1);
    
    return html;
}

QString MainWindow::getSkeletonHTML(const QString &city) const
{
    QString theme = m_settings->value("theme", "dark").toString();
    bool isDark = (theme == "dark");
    
    QString accentColor = isDark ? "#00d4ff" : "#0071e3";
    QString secondaryTextColor = isDark ? "#888888" : "#86868b";
    QString placeholderColor = isDark ? "#666666" : "#aaaaaa";

    // Skeleton screen with loading animation
    QString html = QString(
        "<div style='font-family: \"SF Pro Display\", \"Segoe UI\", \"Microsoft YaHei\", sans-serif;'>"
        
        // City name
        "<div style='font-size: 36px; font-weight: bold; color: %1; margin-bottom: 25px;'>"
        "📍 %2"
        "</div>"
        
        // Temperature placeholder
        "<div style='font-size: 56px; font-weight: bold; color: %3; margin-bottom: 15px;'>"
        "⏳ Loading..."
        "</div>"
        
        // Conditions placeholder
        "<div style='font-size: 26px; font-weight: 600; color: %4; margin-bottom: 25px;'>"
        "Fetching weather data..."
        "</div>"
        
        // Details placeholders
        "<div style='font-size: 18px; color: %3; line-height: 2.2;'>"
        "<div>🌡️ Feels Like: <span style='color: %4;'>--°C</span></div>"
        "<div>💧 Humidity: <span style='color: %4;'>--%</span></div>"
        "<div>💨 Wind Speed: <span style='color: %4;'>-- km/h</span></div>"
        "<div>🔽 Pressure: <span style='color: %4;'>-- mb</span></div>"
        "</div>"
        
        "</div>"
    ).arg(accentColor).arg(city).arg(placeholderColor).arg(secondaryTextColor);
    
    return html;
}



void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Process finished with exit code:" << exitCode;

    // Stop timeout timer
    m_processTimeout->stop();

    // Read any remaining output
    m_processOutput += QString::fromUtf8(m_process->readAllStandardOutput());
    QString errors = QString::fromUtf8(m_process->readAllStandardError());

    // Parse JSON from buffered output
    QJsonDocument doc = QJsonDocument::fromJson(m_processOutput.toUtf8());

    if (exitStatus == QProcess::NormalExit && exitCode == 0 && !m_processOutput.isEmpty() && !doc.isNull()) {
        QJsonObject obj = doc.object();
        QString status = obj["status"].toString();
        
        if (status == "success") {
            // Extract weather data
            QString cityName = obj["city"].toString();
            double temperature = obj["temperature"].toDouble();
            double feelsLike = obj["feels_like"].toDouble();
            int humidity = obj["humidity"].toInt();
            QString description = obj["description"].toString();
            double windSpeed = obj["wind_speed"].toDouble();
            double pressure = obj["pressure"].toDouble();
            
            // Update sidebar if this is the default city
            QString defaultCity = m_defaultCityCombo->currentText();
            if (cityName == defaultCity || m_currentCity == defaultCity) {
                m_weatherCityLabel->setText("📍 " + cityName);
                m_weatherTempLabel->setText(QString::number(temperature, 'f', 1) + "°C");
                m_weatherConditionLabel->setText(description);
                
                QString theme = m_settings->value("theme", "dark").toString();
                bool isDark = (theme == "dark");
                QString secondaryTextColor = isDark ? "#aaaaaa" : "#86868b";

                QString details = QString(
                    "<div style='line-height: 1.8;'>"
                    "<div>🌡️ Feels Like: <span style='color: #ffa502;'>%1°C</span></div>"
                    "<div>💧 Humidity: <span style='color: #48dbfb;'>%2%</span></div>"
                    "<div>💨 Wind: <span style='color: #1dd1a1;'>%3 km/h</span></div>"
                    "<div>🔽 Pressure: <span style='color: #ee5a6f;'>%4 mb</span></div>"
                    "</div>"
                ).arg(feelsLike, 0, 'f', 1)
                 .arg(humidity)
                 .arg(windSpeed, 0, 'f', 1)
                 .arg(pressure, 0, 'f', 1);
                
                m_weatherDetailsLabel->setText(details);
            }
            

            
            if (m_refreshButton) {
                m_refreshButton->setEnabled(true);
                m_refreshButton->setText("🔄 Refresh Weather");
            }
        } else {
            // Error status from Python
            QString errorMsg = obj["error_message"].toString();
            handleWeatherError(errorMsg);
        }
    } else if (!errors.isEmpty()) {
        qWarning() << "Python stderr:" << errors;

        // Check for missing dependencies
        if (errors.contains("ModuleNotFoundError") || errors.contains("No module named")) {
            handleWeatherError("Missing Python dependencies. Please install required packages:\n"
                             "pip install -r python/requirements.txt");
        } else {
            handleWeatherError(errors);
        }
    } else {
        handleWeatherError("Unable to retrieve weather data. Please try again.");
    }
}

void MainWindow::handleWeatherError(const QString &errorMsg)
{
    // Update sidebar
    m_weatherCityLabel->setText("❌ Error");
    m_weatherTempLabel->setText("--°C");
    m_weatherConditionLabel->setText("Failed to load");
    m_weatherDetailsLabel->setText("<span style='color: #ff4757;'>" + errorMsg + "</span>");
    

    
    if (m_refreshButton) {
        m_refreshButton->setEnabled(true);
        m_refreshButton->setText("🔄 Refresh Weather");
    }
}

void MainWindow::onProcessError(QProcess::ProcessError error)
{
    qWarning() << "Process error:" << error;

    // Stop timeout timer
    m_processTimeout->stop();

    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start Python. Please check your Python installation.";
            break;
        case QProcess::Timedout:
            errorMsg = "Request timed out. Please check your internet connection.";
            break;
        default:
            errorMsg = "An error occurred while fetching weather data.";
            break;
    }

    handleWeatherError(errorMsg);
}

void MainWindow::onProcessReadyRead()
{
    // Stream output as it becomes available to prevent buffer blocking
    m_processOutput += QString::fromUtf8(m_process->readAllStandardOutput());
}

void MainWindow::onProcessTimeout()
{
    qWarning() << "Process timeout - terminating Python process";

    // Terminate the process
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        QTimer::singleShot(2000, this, [this]() {
            if (m_process->state() != QProcess::NotRunning) {
                m_process->kill();
            }
        });
    }

    handleWeatherError("Request timed out after 30 seconds. Please check your internet connection.");
}

void MainWindow::onMarkdownTextChanged()
{
    // Trigger debounced preview update
    if (m_previewUpdateTimer) {
        m_previewUpdateTimer->start();
    }
}

QString MainWindow::enhanceMarkdownPreview(const QString &markdown)
{
    QString theme = m_settings->value("theme", "dark").toString();
    bool isDark = (theme == "dark");
    
    QString bgColor = isDark ? "#1a1a1a" : "#ffffff";
    QString textColor = isDark ? "#ffffff" : "#1d1d1f";
    QString accentColor = isDark ? "#00d4ff" : "#0071e3";
    QString secondaryTextColor = isDark ? "#aaaaaa" : "#86868b";
    QString borderColor = isDark ? "#2d2d2d" : "#d2d2d7";
    QString codeBgColor = isDark ? "#0a0a0a" : "#f5f5f7";
    QString inlineCodeBg = isDark ? "#2d2d2d" : "#e8e8ed";
    QString tableEvenRow = isDark ? "#0f0f0f" : "#f9f9fb";

    QString html = QString("<style>"
        "body { font-family: 'SF Pro Display', 'Segoe UI', 'Microsoft YaHei', sans-serif; color: %2; background-color: %1; padding: 20px; line-height: 1.6; }"
        "h1, h2, h3, h4, h5, h6 { color: %3; margin-top: 24px; margin-bottom: 16px; font-weight: 600; }"
        "h1 { font-size: 2em; border-bottom: 2px solid %5; padding-bottom: 8px; }"
        "h2 { font-size: 1.5em; border-bottom: 1px solid %5; padding-bottom: 6px; }"
        "h3 { font-size: 1.25em; }"
        "p { margin-bottom: 16px; }"
        "code { background-color: %7; color: %3; padding: 2px 6px; border-radius: 4px; font-family: 'Consolas', 'Monaco', monospace; font-size: 0.9em; }"
        "pre { background-color: %6; border: 1px solid %5; border-radius: 8px; padding: 16px; overflow-x: auto; margin: 16px 0; tab-size: 2; -moz-tab-size: 2; }"
        "pre code { background-color: transparent; padding: 0; color: %2; display: block; }"
        ".code-block-header { background-color: %5; color: %3; padding: 6px 12px; border-radius: 8px 8px 0 0; font-size: 0.85em; font-weight: 600; font-family: 'Consolas', 'Monaco', monospace; margin-bottom: -1px; }"
        ".code-block-container { margin: 16px 0; }"
        "blockquote { border-left: 4px solid %3; padding-left: 16px; margin-left: 0; color: %4; font-style: italic; }"
        "a { color: %3; text-decoration: none; }"
        "a:hover { text-decoration: underline; }"
        "ul, ol { padding-left: 24px; margin-bottom: 16px; }"
        "li { margin-bottom: 8px; }"
        "table { border-collapse: collapse; width: 100%; margin: 16px 0; }"
        "th, td { border: 1px solid %5; padding: 8px 12px; text-align: left; }"
        "th { background-color: %5; color: %3; font-weight: 600; }"
        "tr:nth-child(even) { background-color: %8; }"
        "hr { border: none; border-top: 2px solid %5; margin: 24px 0; }"
        "img { max-width: 100%; height: auto; border-radius: 8px; margin: 16px 0; }"
        "strong { color: %3; font-weight: 600; }"
        "em { color: %3; font-style: italic; opacity: 0.9; }"
        "del { color: %4; text-decoration: line-through; }"
        "</style><body>")
        .arg(bgColor, textColor, accentColor, secondaryTextColor, borderColor)
        .arg(codeBgColor, inlineCodeBg, tableEvenRow);
    
    // Process markdown line by line
    QStringList lines = markdown.split('\n');
    bool inCodeBlock = false;
    QString codeLanguage;
    QString codeContent;
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        
        // Check for code block start/end
        if (line.startsWith("```")) {
            if (!inCodeBlock) {
                // Start of code block
                inCodeBlock = true;
                codeLanguage = line.mid(3).trimmed();
                if (codeLanguage.isEmpty()) {
                    codeLanguage = "plaintext";
                }
                codeContent.clear();
                
                // Add language header
                html += QString("<div class='code-block-container'>"
                               "<div class='code-block-header'>%1</div>"
                               "<pre><code>").arg(codeLanguage.toHtmlEscaped());
            } else {
                // End of code block
                html += codeContent.toHtmlEscaped();
                html += "</code></pre></div>";
                inCodeBlock = false;
                codeLanguage.clear();
                codeContent.clear();
            }
        } else if (inCodeBlock) {
            // Inside code block
            codeContent += line + "\n";
        } else {
            // Regular markdown line - use simple conversion
            QString processedLine = line;
            
            // Headers
            if (processedLine.startsWith("# ")) {
                html += "<h1>" + processedLine.mid(2).toHtmlEscaped() + "</h1>";
            } else if (processedLine.startsWith("## ")) {
                html += "<h2>" + processedLine.mid(3).toHtmlEscaped() + "</h2>";
            } else if (processedLine.startsWith("### ")) {
                html += "<h3>" + processedLine.mid(4).toHtmlEscaped() + "</h3>";
            }
            // Horizontal rule
            else if (processedLine.trimmed() == "---" || processedLine.trimmed() == "***") {
                html += "<hr>";
            }
            // Unordered list
            else if (processedLine.trimmed().startsWith("- ") || processedLine.trimmed().startsWith("* ")) {
                QString content = processedLine.trimmed().mid(2);
                html += "<ul><li>" + processInlineMarkdown(content) + "</li></ul>";
            }
            // Ordered list
            else if (processedLine.trimmed().contains(QRegularExpression("^\\d+\\. "))) {
                QString content = processedLine.trimmed();
                content = content.mid(content.indexOf(". ") + 2);
                html += "<ol><li>" + processInlineMarkdown(content) + "</li></ol>";
            }
            // Blockquote
            else if (processedLine.startsWith("> ")) {
                html += "<blockquote>" + processInlineMarkdown(processedLine.mid(2)) + "</blockquote>";
            }
            // Empty line
            else if (processedLine.trimmed().isEmpty()) {
                html += "<br>";
            }
            // Regular paragraph
            else {
                html += "<p>" + processInlineMarkdown(processedLine) + "</p>";
            }
        }
    }
    
    html += "</body>";
    return html;
}

QString MainWindow::processInlineMarkdown(const QString &text)
{
    QString result = text.toHtmlEscaped();
    
    // Bold: **text** or __text__
    result.replace(QRegularExpression("\\*\\*(.+?)\\*\\*"), "<strong>\\1</strong>");
    result.replace(QRegularExpression("__(.+?)__"), "<strong>\\1</strong>");
    
    // Italic: *text* or _text_
    result.replace(QRegularExpression("\\*(.+?)\\*"), "<em>\\1</em>");
    result.replace(QRegularExpression("_(.+?)_"), "<em>\\1</em>");
    
    // Strikethrough: ~~text~~
    result.replace(QRegularExpression("~~(.+?)~~"), "<del>\\1</del>");
    
    // Inline code: `code`
    result.replace(QRegularExpression("`(.+?)`"), "<code>\\1</code>");
    
    // Links: [text](url)
    result.replace(QRegularExpression("\\[(.+?)\\]\\((.+?)\\)"), "<a href='\\2'>\\1</a>");
    
    // Images: ![alt](url)
    result.replace(QRegularExpression("!\\[(.+?)\\]\\((.+?)\\)"), "<img src='\\2' alt='\\1'>");
    
    return result;
}

void MainWindow::onDefaultCityChanged(int index)
{
    if (index < 0) return;
    
    QString city = m_defaultCityCombo->currentText();
    qDebug() << "Default city changed to:" << city;
    
    // Save to settings
    m_settings->setValue("defaultCity", city);
    
    // Load weather for new default city
    loadWeatherForDefaultCity();
}

void MainWindow::loadWeatherForDefaultCity()
{
    QString city = m_defaultCityCombo->currentText();
    qDebug() << "Loading weather for default city:" << city;
    
    m_currentCity = city;
    
    QString scriptPath = getPythonScriptPath();
    if (scriptPath.isEmpty()) {
        handleWeatherError("Python script not found");
        return;
    }
    
    // Show loading state in sidebar
    m_weatherCityLabel->setText("📍 " + city);
    m_weatherTempLabel->setText("⏳");
    m_weatherConditionLabel->setText("Loading...");
    m_weatherDetailsLabel->setText("<span style='color: #888888;'>Fetching weather data...</span>");
    
    // Disable refresh button
    if (m_refreshButton) {
        m_refreshButton->setEnabled(false);
        m_refreshButton->setText("🔄 Loading...");
    }
    
    // Kill any existing process and stop timeout timer
    m_processTimeout->stop();
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        // Don't block UI - let the process terminate asynchronously
        QTimer::singleShot(1000, this, [this]() {
            if (m_process->state() != QProcess::NotRunning) {
                m_process->kill();
            }
        });
        // Wait briefly for termination without blocking
        QCoreApplication::processEvents();
        return; // Restart the request after cleanup
    }

    // Clear previous output buffer
    m_processOutput.clear();

    // Disconnect old connections to avoid duplicates
    disconnect(m_process, nullptr, this, nullptr);

    // Connect signals for async processing
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &MainWindow::onProcessError);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onProcessReadyRead);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &MainWindow::onProcessReadyRead);
    
    // Find Python executable dynamically (embedded runtime or system Python)
    QString pythonExecutable = findPythonExecutable();
    
    QStringList arguments;
    arguments << scriptPath << city;
    
    qDebug() << "Executing:" << pythonExecutable << arguments;

    // Start the process asynchronously
    m_process->start(pythonExecutable, arguments);

    // Start timeout timer
    m_processTimeout->start();
}

void MainWindow::restartPythonProcess()
{
    qDebug() << "Attempting to restart Python process...";
    
    if (m_pythonRestartAttempts >= m_maxRestartAttempts) {
        qWarning() << "Max restart attempts reached. Giving up.";
        handleWeatherError(QString("Python process failed after %1 restart attempts. Please check your Python installation.").arg(m_maxRestartAttempts));
        m_pythonRestartAttempts = 0;
        return;
    }
    
    m_pythonRestartAttempts++;
    m_pendingCity = m_currentCity;
    
    // Show restart attempt in UI
    m_weatherDetailsLabel->setText(QString("<span style='color: #ffa502;'>⚠️ Restarting Python process (attempt %1/%2)...</span>")
                                   .arg(m_pythonRestartAttempts)
                                   .arg(m_maxRestartAttempts));
    
    // Wait before restarting (exponential backoff)
    int delayMs = m_settings->value("pythonRestartDelay", 2000).toInt() * m_pythonRestartAttempts;
    m_restartTimer->setInterval(delayMs);
    m_restartTimer->start();
}

void MainWindow::attemptPythonRestart()
{
    qDebug() << "Executing Python restart attempt" << m_pythonRestartAttempts;
    
    // Ensure previous process is terminated
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    
    // Try to reload weather for the pending city
    if (!m_pendingCity.isEmpty()) {
        m_currentCity = m_pendingCity;
        loadWeatherForDefaultCity();
    }
}

bool MainWindow::isPythonProcessHealthy() const
{
    return m_process && m_process->state() == QProcess::Running;
}

// ========== File Operations ==========

void MainWindow::openFile()
{
    if (!maybeSave()) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Markdown File"),
        QString(),
        tr("Markdown Files (*.md *.markdown);;All Files (*)")
    );

    if (fileName.isEmpty()) {
        return;
    }

    // File safety checks
    QFileInfo fileInfo(fileName);
    
    // Check file size (10MB hard limit, warn for 1MB-10MB)
    qint64 fileSize = fileInfo.size();
    const qint64 WARN_SIZE = 1 * 1024 * 1024;  // 1MB
    const qint64 MAX_SIZE = 20 * 1024 * 1024;  // 20MB
    
    if (fileSize > MAX_SIZE) {
        QMessageBox::warning(this, tr("File Too Large"),
                           tr("Cannot open file %1:\nFile size (%2 MB) exceeds maximum limit of 20 MB.")
                           .arg(QDir::toNativeSeparators(fileName))
                           .arg(fileSize / (1024.0 * 1024.0), 0, 'f', 2));
        return;
    }
    
    if (fileSize > WARN_SIZE) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Large File Warning"),
            tr("File %1 is %2 MB.\nOpening large files may affect performance.\nContinue?")
            .arg(QDir::toNativeSeparators(fileName))
            .arg(fileSize / (1024.0 * 1024.0), 0, 'f', 2),
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply != QMessageBox::Yes) {
            return;
        }
    }
    
    // Check file type (warn for non-markdown files)
    QString suffix = fileInfo.suffix().toLower();
    if (suffix != "md" && suffix != "markdown") {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Non-Markdown File"),
            tr("File %1 does not appear to be a Markdown file.\nOpen anyway?")
            .arg(QDir::toNativeSeparators(fileName)),
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot read file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName))
                           .arg(file.errorString()));
        return;
    }

    // Read raw bytes to detect encoding
    QByteArray rawData = file.readAll();
    file.close();
    
    // Detect BOM and encoding
    m_currentEncoding = QStringConverter::Utf8;  // Default
    m_fileBOM.clear();  // Clear previous BOM
    int bomSize = 0;
    
    if (rawData.size() >= 3 && 
        (unsigned char)rawData[0] == 0xEF && 
        (unsigned char)rawData[1] == 0xBB && 
        (unsigned char)rawData[2] == 0xBF) {
        // UTF-8 BOM
        m_currentEncoding = QStringConverter::Utf8;
        m_fileBOM = rawData.left(3);
        bomSize = 3;
    } else if (rawData.size() >= 2) {
        if ((unsigned char)rawData[0] == 0xFF && (unsigned char)rawData[1] == 0xFE) {
            // UTF-16 LE BOM
            m_currentEncoding = QStringConverter::Utf16LE;
            m_fileBOM = rawData.left(2);
            bomSize = 2;
        } else if ((unsigned char)rawData[0] == 0xFE && (unsigned char)rawData[1] == 0xFF) {
            // UTF-16 BE BOM
            m_currentEncoding = QStringConverter::Utf16BE;
            m_fileBOM = rawData.left(2);
            bomSize = 2;
        }
    }
    
    // Decode content
    QStringDecoder decoder(m_currentEncoding);
    QString content = decoder(rawData.mid(bomSize));
    
    // Check for decoding errors
    if (decoder.hasError()) {
        QMessageBox::warning(this, tr("Encoding Error"),
                           tr("Failed to decode file %1 with detected encoding.\nTrying UTF-8...")
                           .arg(QDir::toNativeSeparators(fileName)));
        
        // Fallback to UTF-8
        m_currentEncoding = QStringConverter::Utf8;
        QStringDecoder utf8Decoder(QStringConverter::Utf8);
        content = utf8Decoder(rawData);
        
        if (utf8Decoder.hasError()) {
            QMessageBox::critical(this, tr("Error"),
                               tr("Cannot decode file %1.\nFile may be corrupted or use an unsupported encoding.")
                               .arg(QDir::toNativeSeparators(fileName)));
            return;
        }
    }

    m_markdownEditor->setPlainText(content);
    m_currentFilePath = fileName;
    m_isModified = false;
    updateWindowTitle();
}

void MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    // Use QSaveFile for atomic writes
    QSaveFile file(m_currentFilePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(m_currentFilePath))
                           .arg(file.errorString()));
        return;
    }

    // Encode content using original encoding
    QString content = m_markdownEditor->toPlainText();
    QStringEncoder encoder(m_currentEncoding);
    QByteArray encodedData = encoder(content);
    
    if (encoder.hasError()) {
        QMessageBox::warning(this, tr("Encoding Error"),
                           tr("Failed to encode file with original encoding.\nFalling back to UTF-8."));
        m_currentEncoding = QStringConverter::Utf8;
        QStringEncoder utf8Encoder(QStringConverter::Utf8);
        encodedData = utf8Encoder(content);
    }
    
    // Write BOM if present
    if (!m_fileBOM.isEmpty()) {
        if (file.write(m_fileBOM) == -1) {
            QMessageBox::warning(this, tr("Error"),
                               tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(m_currentFilePath))
                               .arg(file.errorString()));
            file.cancelWriting();
            return;
        }
    }
    
    // Write data
    if (file.write(encodedData) == -1) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(m_currentFilePath))
                           .arg(file.errorString()));
        file.cancelWriting();
        return;
    }
    
    // Commit the file (atomic operation)
    if (!file.commit()) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot save file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(m_currentFilePath))
                           .arg(file.errorString()));
        return;
    }

    m_isModified = false;
    updateWindowTitle();
}

void MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Markdown File"),
        m_currentFilePath.isEmpty() ? "untitled.md" : m_currentFilePath,
        tr("Markdown Files (*.md *.markdown);;All Files (*)")
    );

    if (fileName.isEmpty()) {
        return;
    }

    // Check if file exists and confirm overwrite
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists() && fileInfo.filePath() != m_currentFilePath) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Overwrite File"),
            tr("File %1 already exists.\nOverwrite?")
            .arg(QDir::toNativeSeparators(fileName)),
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    // Save old path in case save fails
    QString oldPath = m_currentFilePath;
    bool oldModified = m_isModified;
    
    // Temporarily set new path for saveFile
    m_currentFilePath = fileName;
    
    // Use QSaveFile for atomic writes
    QSaveFile file(m_currentFilePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(m_currentFilePath))
                           .arg(file.errorString()));
        // Restore old path on failure
        m_currentFilePath = oldPath;
        m_isModified = oldModified;
        return;
    }

    // Encode content using original encoding
    QString content = m_markdownEditor->toPlainText();
    QStringEncoder encoder(m_currentEncoding);
    QByteArray encodedData = encoder(content);
    
    if (encoder.hasError()) {
        QMessageBox::warning(this, tr("Encoding Error"),
                           tr("Failed to encode file with original encoding.\nFalling back to UTF-8."));
        m_currentEncoding = QStringConverter::Utf8;
        QStringEncoder utf8Encoder(QStringConverter::Utf8);
        encodedData = utf8Encoder(content);
    }
    
    // Write BOM if present
    if (!m_fileBOM.isEmpty()) {
        if (file.write(m_fileBOM) == -1) {
            QMessageBox::warning(this, tr("Error"),
                               tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(m_currentFilePath))
                               .arg(file.errorString()));
            file.cancelWriting();
            // Restore old path on failure
            m_currentFilePath = oldPath;
            m_isModified = oldModified;
            return;
        }
    }
    
    // Write data
    if (file.write(encodedData) == -1) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(m_currentFilePath))
                           .arg(file.errorString()));
        file.cancelWriting();
        // Restore old path on failure
        m_currentFilePath = oldPath;
        m_isModified = oldModified;
        return;
    }
    
    // Commit the file (atomic operation)
    if (!file.commit()) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot save file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(m_currentFilePath))
                           .arg(file.errorString()));
        // Restore old path on failure
        m_currentFilePath = oldPath;
        m_isModified = oldModified;
        return;
    }

    // Only update state after successful save
    m_isModified = false;
    updateWindowTitle();
}

bool MainWindow::maybeSave()
{
    if (!m_isModified) {
        return true;
    }

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        tr("Unsaved Changes"),
        tr("The document has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Save) {
        saveFile();
        return !m_isModified;  // Return true only if save succeeded
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }

    return true;
}

void MainWindow::updateWindowTitle()
{
    QString title = "mdCoder";

    if (!m_currentFilePath.isEmpty()) {
        QFileInfo fileInfo(m_currentFilePath);
        title = fileInfo.fileName();

        if (m_isModified) {
            title += " *";
        }

        title += " - mdCoder";
    } else if (m_isModified) {
        title = "* - mdCoder";
    }

    setWindowTitle(title);
}

// ========== Zoom Operations ==========

void MainWindow::setZoom(double level)
{
    // Clamp zoom level to valid range
    double newZoom = qBound(ZOOM_MIN, level, ZOOM_MAX);
    
    // Early return if zoom hasn't changed (avoid redundant operations)
    if (qAbs(newZoom - m_currentZoom) < 0.001) {
        return;
    }

    // Calculate the zoom change ratio
    double zoomChange = newZoom / m_currentZoom;

    // Update current zoom
    m_currentZoom = newZoom;

    // Save zoom level to settings (debounced)
    m_zoomSettingsSaveTimer->start();

    // Update zoom button text to show current percentage
    if (m_zoomResetButton) {
        int percentage = qRound(m_currentZoom * 100);
        m_zoomResetButton->setText(QString("%1%").arg(percentage));
    }

    // Apply zoom to editor by scaling font
    QFont editorFont = m_markdownEditor->font();
    int baseFontSize = 14;  // Base font size from stylesheet
    editorFont.setPointSizeF(baseFontSize * m_currentZoom);
    m_markdownEditor->setFont(editorFont);

    // Apply zoom to preview using QTextBrowser's built-in zoomIn/zoomOut
    // QTextBrowser's zoom is multiplicative, so we apply the ratio
    if (zoomChange > 1.0) {
        // Zoom in - calculate how many steps
        double factor = zoomChange;
        while (factor > 1.01) {  // Small threshold to avoid floating point issues
            m_markdownPreview->zoomIn(1);
            factor /= 1.1;  // Each zoomIn step is approximately 1.1x
        }
    } else if (zoomChange < 1.0) {
        // Zoom out - calculate how many steps
        double factor = zoomChange;
        while (factor < 0.99) {  // Small threshold to avoid floating point issues
            m_markdownPreview->zoomOut(1);
            factor *= 1.1;  // Each zoomOut step is approximately 0.9x (1/1.1)
        }
    }

    // Note: Preview zoom is already applied via zoomIn/zoomOut calls above
    // No need to regenerate HTML during zoom operations

    qDebug() << "Zoom level set to:" << m_currentZoom << "(" << qRound(m_currentZoom * 100) << "%)";
}

void MainWindow::zoomIn()
{
    setZoom(m_currentZoom + ZOOM_STEP);
}

void MainWindow::zoomOut()
{
    setZoom(m_currentZoom - ZOOM_STEP);
}

void MainWindow::zoomReset()
{
    setZoom(DEFAULT_ZOOM);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    // Check if Ctrl key is pressed
    if (event->modifiers() & Qt::ControlModifier) {
        // Get the scroll delta
        int delta = event->angleDelta().y();

        if (delta > 0) {
            zoomIn();
        } else if (delta < 0) {
            zoomOut();
        }

        event->accept();
        return;
    }

    // If Ctrl is not pressed, use default behavior
    QMainWindow::wheelEvent(event);
}
