#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QMutex>

/**
 * @brief Singleton class for managing application settings
 * 
 * This class provides centralized access to application configuration
 * using QSettings with INI format. It ensures thread-safe access and
 * provides convenient methods for common settings operations.
 */
class SettingsManager
{
public:
    // Delete copy constructor and assignment operator
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    /**
     * @brief Get the singleton instance
     * @return Reference to the SettingsManager instance
     */
    static SettingsManager& instance();

    /**
     * @brief Get a setting value
     * @param key Setting key
     * @param defaultValue Default value if key doesn't exist
     * @return Setting value
     */
    QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Set a setting value
     * @param key Setting key
     * @param value Setting value
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief Check if a setting exists
     * @param key Setting key
     * @return true if setting exists, false otherwise
     */
    bool contains(const QString& key) const;

    /**
     * @brief Remove a setting
     * @param key Setting key
     */
    void remove(const QString& key);

    /**
     * @brief Sync settings to disk
     */
    void sync();

    // Convenience methods for common settings
    QString getTheme() const;
    void setTheme(const QString& theme);

    QString getDefaultCity() const;
    void setDefaultCity(const QString& city);

    QByteArray getWindowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QByteArray getWindowState() const;
    void setWindowState(const QByteArray& state);

    int getPythonRestartAttempts() const;
    void setPythonRestartAttempts(int attempts);

    int getPythonRestartDelay() const;
    void setPythonRestartDelay(int delayMs);

private:
    SettingsManager();
    ~SettingsManager();

    QSettings* m_settings;
    mutable QMutex m_mutex;
};

#endif // SETTINGSMANAGER_H
