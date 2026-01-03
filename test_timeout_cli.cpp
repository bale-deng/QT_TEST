#include <QCoreApplication>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QDateTime>

class TimeoutTester : public QObject
{
    Q_OBJECT

public:
    TimeoutTester(QObject *parent = nullptr) : QObject(parent)
    {
        m_process = new QProcess(this);
        m_timeout = new QTimer(this);
        m_timeout->setSingleShot(true);
        m_timeout->setInterval(30000); // 30 seconds

        connect(m_timeout, &QTimer::timeout, this, &TimeoutTester::onTimeout);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &TimeoutTester::onFinished);
        connect(m_process, &QProcess::errorOccurred, this, &TimeoutTester::onError);
    }

    void runTest()
    {
        qDebug() << "=== TIMEOUT TEST STARTING ===";
        qDebug() << "Starting time:" << QDateTime::currentDateTime().toString("hh:mm:ss");
        qDebug() << "Expected timeout: 30 seconds";
        qDebug() << "";

        m_startTime = QDateTime::currentMSecsSinceEpoch();

        // Start the hanging Python script
        QString pythonExe = "python";
        QStringList args;
        args << "python/test_timeout.py" << "TestCity";

        qDebug() << "Starting process:" << pythonExe << args;
        m_process->start(pythonExe, args);
        m_timeout->start();
    }

private slots:
    void onTimeout()
    {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;
        qDebug() << "";
        qDebug() << "=== TIMEOUT TRIGGERED ===";
        qDebug() << "Time elapsed:" << elapsed << "ms (" << (elapsed/1000.0) << "seconds)";
        qDebug() << "Current time:" << QDateTime::currentDateTime().toString("hh:mm:ss");

        if (m_process->state() != QProcess::NotRunning) {
            qDebug() << "Terminating process...";
            m_process->terminate();

            QTimer::singleShot(2000, this, [this]() {
                if (m_process->state() != QProcess::NotRunning) {
                    qDebug() << "Process still running, killing...";
                    m_process->kill();
                }
            });
        }
    }

    void onFinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;
        qDebug() << "";
        qDebug() << "=== PROCESS FINISHED ===";
        qDebug() << "Exit code:" << exitCode;
        qDebug() << "Exit status:" << (exitStatus == QProcess::NormalExit ? "Normal" : "Crashed");
        qDebug() << "Total time:" << elapsed << "ms (" << (elapsed/1000.0) << "seconds)";
        qDebug() << "Current time:" << QDateTime::currentDateTime().toString("hh:mm:ss");

        QString output = m_process->readAllStandardOutput();
        QString errors = m_process->readAllStandardError();

        if (!errors.isEmpty()) {
            qDebug() << "";
            qDebug() << "STDERR output:";
            qDebug() << errors;
        }

        qDebug() << "";
        if (elapsed >= 29000 && elapsed <= 32000) {
            qDebug() << "✅ SUCCESS: Timeout triggered correctly (~30 seconds)";
        } else if (elapsed > 32000) {
            qDebug() << "❌ FAILURE: Process ran too long (timeout didn't work)";
        } else {
            qDebug() << "⚠️  WARNING: Process finished too quickly";
        }

        QCoreApplication::quit();
    }

    void onError(QProcess::ProcessError error)
    {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;
        qDebug() << "";
        qDebug() << "=== PROCESS ERROR ===";
        qDebug() << "Error:" << error;
        qDebug() << "Time elapsed:" << elapsed << "ms";

        QCoreApplication::quit();
    }

private:
    QProcess *m_process;
    QTimer *m_timeout;
    qint64 m_startTime;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    TimeoutTester tester;
    QTimer::singleShot(0, &tester, &TimeoutTester::runTest);

    return app.exec();
}

#include "test_timeout_cli.moc"
