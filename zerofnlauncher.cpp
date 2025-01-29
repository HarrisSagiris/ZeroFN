#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QFileDialog>
#include <QLabel>

class ZeroFNLauncher : public QMainWindow {
    Q_OBJECT

public:
    ZeroFNLauncher(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("ZeroFN Launcher");
        setMinimumSize(600, 400);

        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        setCentralWidget(centralWidget);

        // Path selection
        QHBoxLayout *pathLayout = new QHBoxLayout();
        QLabel *pathLabel = new QLabel("Fortnite Path:", this);
        pathEdit = new QLineEdit(this);
        QPushButton *browseButton = new QPushButton("Browse", this);
        pathLayout->addWidget(pathLabel);
        pathLayout->addWidget(pathEdit);
        pathLayout->addWidget(browseButton);
        layout->addLayout(pathLayout);

        // Console output
        consoleOutput = new QTextEdit(this);
        consoleOutput->setReadOnly(true);
        consoleOutput->setStyleSheet("background-color: #1e1e1e; color: #ffffff;");
        layout->addWidget(consoleOutput);

        // Control buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        startButton = new QPushButton("Start Private Server", this);
        stopButton = new QPushButton("Stop Server", this);
        stopButton->setEnabled(false);
        buttonLayout->addWidget(startButton);
        buttonLayout->addWidget(stopButton);
        layout->addLayout(buttonLayout);

        // Load saved path
        QSettings settings("ZeroFN", "Launcher");
        QString savedPath = settings.value("fortnitePath").toString();
        pathEdit->setText(savedPath);

        // Connect signals
        connect(browseButton, &QPushButton::clicked, this, &ZeroFNLauncher::browsePath);
        connect(startButton, &QPushButton::clicked, this, &ZeroFNLauncher::startServer);
        connect(stopButton, &QPushButton::clicked, this, &ZeroFNLauncher::stopServer);
        connect(pathEdit, &QLineEdit::textChanged, this, &ZeroFNLauncher::validatePath);

        // Initial path validation
        validatePath();
    }

private slots:
    void browsePath() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Fortnite Installation Directory");
        if (!dir.isEmpty()) {
            pathEdit->setText(dir);
            QSettings settings("ZeroFN", "Launcher");
            settings.setValue("fortnitePath", dir);
        }
    }

    void startServer() {
        // Start Node.js server
        nodeProcess = new QProcess(this);
        nodeProcess->start("node", QStringList() << "server.js");
        if (!nodeProcess->waitForStarted()) {
            showError("Failed to start Node.js server");
            return;
        }
        logMessage("Node.js server started successfully");

        // Start mitmproxy
        proxyProcess = new QProcess(this);
        proxyProcess->start("mitmdump", QStringList() << "-s" << "redirect.py");
        if (!proxyProcess->waitForStarted()) {
            showError("Failed to start mitmproxy");
            stopServer();
            return;
        }
        logMessage("Mitmproxy started successfully");

        // Launch Fortnite
        QString fortnitePath = pathEdit->text() + "/FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe";
        if (!QProcess::startDetached(fortnitePath, QStringList() << "-epicportal")) {
            showError("Failed to launch Fortnite");
            stopServer();
            return;
        }
        logMessage("Fortnite launched successfully");

        startButton->setEnabled(false);
        stopButton->setEnabled(true);
    }

    void stopServer() {
        if (nodeProcess) {
            nodeProcess->terminate();
            nodeProcess->waitForFinished();
            delete nodeProcess;
            nodeProcess = nullptr;
        }

        if (proxyProcess) {
            proxyProcess->terminate();
            proxyProcess->waitForFinished();
            delete proxyProcess;
            proxyProcess = nullptr;
        }

        logMessage("All services stopped");
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
    }

    void validatePath() {
        QString path = pathEdit->text();
        bool valid = !path.isEmpty() && QFile::exists(path + "/FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe");
        startButton->setEnabled(valid);
    }

private:
    void showError(const QString &message) {
        QMessageBox::critical(this, "Error", message);
        logMessage("ERROR: " + message);
    }

    void logMessage(const QString &message) {
        consoleOutput->append(QTime::currentTime().toString("[hh:mm:ss] ") + message);
    }

    QLineEdit *pathEdit;
    QTextEdit *consoleOutput;
    QPushButton *startButton;
    QPushButton *stopButton;
    QProcess *nodeProcess = nullptr;
    QProcess *proxyProcess = nullptr;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZeroFNLauncher launcher;
    launcher.show();
    return app.exec();
}

#include "zerofnlauncher.moc"
