#include "SettingsManager.h"
#include <QMutexLocker>

SettingsManager::SettingsManager()
{
    // Create settings file in application directory
    QString settingsPath = QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
    m_settings = new QSettings(settingsPath, QSettings::IniFormat);
}

SettingsManager::~SettingsManager()
{
    if (m_settings) {
        m_settings->sync();
        delete m_settings;
    }
}

SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

QVariant SettingsManager::getValue(const QString& key, const QVariant& defaultValue) const
{
    QMutexLocker locker(&m_mutex);
    return m_settings->value(key, defaultValue);
}

void SettingsManager::setValue(const QString& key, const QVariant& value)
{
    QMutexLocker locker(&m_mutex);
    m_settings->setValue(key, value);
}

bool SettingsManager::contains(const QString& key) const
{
    QMutexLocker locker(&m_mutex);
    return m_settings->contains(key);
}

void SettingsManager::remove(const QString& key)
{
    QMutexLocker locker(&m_mutex);
    m_settings->remove(key);
}

void SettingsManager::sync()
{
    QMutexLocker locker(&m_mutex);
    m_settings->sync();
}

// Convenience methods
QString SettingsManager::getTheme() const
{
    return getValue("theme", "dark").toString();
}

void SettingsManager::setTheme(const QString& theme)
{
    setValue("theme", theme);
}

QString SettingsManager::getDefaultCity() const
{
    return getValue("defaultCity", "Beijing").toString();
}

void SettingsManager::setDefaultCity(const QString& city)
{
    setValue("defaultCity", city);
}

QByteArray SettingsManager::getWindowGeometry() const
{
    return getValue("geometry").toByteArray();
}

void SettingsManager::setWindowGeometry(const QByteArray& geometry)
{
    setValue("geometry", geometry);
}

QByteArray SettingsManager::getWindowState() const
{
    return getValue("windowState").toByteArray();
}

void SettingsManager::setWindowState(const QByteArray& state)
{
    setValue("windowState", state);
}

int SettingsManager::getPythonRestartAttempts() const
{
    return getValue("pythonRestartAttempts", 3).toInt();
}

void SettingsManager::setPythonRestartAttempts(int attempts)
{
    setValue("pythonRestartAttempts", attempts);
}

int SettingsManager::getPythonRestartDelay() const
{
    return getValue("pythonRestartDelay", 2000).toInt();
}

void SettingsManager::setPythonRestartDelay(int delayMs)
{
    setValue("pythonRestartDelay", delayMs);
}
