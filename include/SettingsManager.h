#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QMutex>

/**
 * @brief 管理应用程序设置的单例类
 * 
 * 此类提供对应用程序配置的集中访问，
 * 使用 INI 格式的 QSettings。它确保线程安全访问，
 * 并为常见的设置操作提供便捷方法。
 */
class SettingsManager
{
public:
    // 删除拷贝构造函数和赋值运算符
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    /**
     * @brief 获取单例实例
     * @return SettingsManager 实例的引用
     */
    static SettingsManager& instance();

    /**
     * @brief 获取设置值
     * @param key 设置键
     * @param defaultValue 如果键不存在的默认值
     * @return 设置值
     */
    QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief 设置设置值
     * @param key 设置键
     * @param value 设置值
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief 检查设置是否存在
     * @param key 设置键
     * @return 如果设置存在返回 true，否则返回 false
     */
    bool contains(const QString& key) const;

    /**
     * @brief 移除设置
     * @param key 设置键
     */
    void remove(const QString& key);

    /**
     * @brief 将设置同步到磁盘
     */
    void sync();

    // 常见设置的便捷方法
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
