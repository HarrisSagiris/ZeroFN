#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMessageBox>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QDir>

class ZeroFNLauncher : public QMainWindow {
    Q_OBJECT

public:
    ZeroFNLauncher(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("ZeroFN Launcher");
        setMinimumSize(800, 600);
        setStyleSheet("QMainWindow { background-color: #2d2d2d; }"
                     "QLabel { color: white; }"
                     "QPushButton { background-color: #0078d7; color: white; padding: 8px; border-radius: 4px; }"
                     "QPushButton:disabled { background-color: #666666; }"
                     "QLineEdit { padding: 6px; border-radius: 4px; }");

        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        setCentralWidget(centralWidget);

        // Title and logo
        QLabel *titleLabel = new QLabel("ZeroFN Launcher", this);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white; margin: 20px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);

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
        consoleOutput->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #ffffff; border-radius: 4px; padding: 10px; font-family: Consolas, monospace; }");
        layout->addWidget(consoleOutput);

        // Status indicators
        QHBoxLayout *statusLayout = new QHBoxLayout();
        nodeStatus = new QLabel("Node.js Server: Stopped", this);
        proxyStatus = new QLabel("Proxy Server: Stopped", this);
        fortniteStatus = new QLabel("Fortnite: Not Running", this);
        nodeStatus->setStyleSheet("color: #ff4444;");
        proxyStatus->setStyleSheet("color: #ff4444;");
        fortniteStatus->setStyleSheet("color: #ff4444;");
        statusLayout->addWidget(nodeStatus);
        statusLayout->addWidget(proxyStatus);
        statusLayout->addWidget(fortniteStatus);
        layout->addLayout(statusLayout);

        // Control buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        startButton = new QPushButton("Launch Game", this);
        stopButton = new QPushButton("Stop All Services", this);
        startButton->setFixedHeight(40);
        stopButton->setFixedHeight(40);
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

        // Connect process signals
        connect(&nodeProcess, &QProcess::readyReadStandardOutput, this, &ZeroFNLauncher::handleNodeOutput);
        connect(&nodeProcess, &QProcess::readyReadStandardError, this, &ZeroFNLauncher::handleNodeError);
        connect(&proxyProcess, &QProcess::readyReadStandardOutput, this, &ZeroFNLauncher::handleProxyOutput);
        connect(&proxyProcess, &QProcess::readyReadStandardError, this, &ZeroFNLauncher::handleProxyError);

        // Initial path validation
        validatePath();
        
        logMessage("ZeroFN Launcher initialized successfully");
    }

private slots:
    void browsePath() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Fortnite Installation Directory",
                                                      pathEdit->text(),
                                                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            pathEdit->setText(dir);
            QSettings settings("ZeroFN", "Launcher");
            settings.setValue("fortnitePath", dir);
        }
    }

    void startServer() {
        // Check if required files exist
        if (!QFile::exists("server.js") || !QFile::exists("redirect.py")) {
            showError("Required server files are missing!");
            return;
        }

        // Start Node.js server
        nodeProcess.start("node", QStringList() << "server.js");
        if (!nodeProcess.waitForStarted()) {
            showError("Failed to start Node.js server. Make sure Node.js is installed.");
            return;
        }
        nodeStatus->setText("Node.js Server: Running");
        nodeStatus->setStyleSheet("color: #44ff44;");
        logMessage("Node.js server started successfully");

        // Start mitmproxy
        proxyProcess.start("mitmdump", QStringList() << "-s" << "redirect.py");
        if (!proxyProcess.waitForStarted()) {
            showError("Failed to start mitmproxy. Make sure mitmproxy is installed.");
            stopServer();
            return;
        }
        proxyStatus->setText("Proxy Server: Running");
        proxyStatus->setStyleSheet("color: #44ff44;");
        logMessage("Mitmproxy started successfully");

        // Launch Fortnite
        QString fortnitePath = pathEdit->text() + "/FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe";
        if (!QFile::exists(fortnitePath)) {
            showError("Fortnite executable not found!");
            stopServer();
            return;
        }

        QProcess *fortniteProcess = new QProcess(this);
        if (!fortniteProcess->startDetached(fortnitePath, QStringList() << "-epicportal")) {
            showError("Failed to launch Fortnite");
            delete fortniteProcess;
            stopServer();
            return;
        }
        fortniteStatus->setText("Fortnite: Running");
        fortniteStatus->setStyleSheet("color: #44ff44;");
        logMessage("Fortnite launched successfully");

        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        pathEdit->setEnabled(false);
    }

    void stopServer() {
        // Stop Node.js server
        if (nodeProcess.state() != QProcess::NotRunning) {
            nodeProcess.terminate();
            nodeProcess.waitForFinished();
        }
        nodeStatus->setText("Node.js Server: Stopped");
        nodeStatus->setStyleSheet("color: #ff4444;");

        // Stop mitmproxy
        if (proxyProcess.state() != QProcess::NotRunning) {
            proxyProcess.terminate();
            proxyProcess.waitForFinished();
        }
        proxyStatus->setText("Proxy Server: Stopped");
        proxyStatus->setStyleSheet("color: #ff4444;");

        fortniteStatus->setText("Fortnite: Not Running");
        fortniteStatus->setStyleSheet("color: #ff4444;");

        logMessage("All services stopped");
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        pathEdit->setEnabled(true);
    }

    void validatePath() {
        QString path = pathEdit->text();
        bool valid = !path.isEmpty() && 
                    QFile::exists(path + "/FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe");
        startButton->setEnabled(valid);
        if (valid) {
            logMessage("Valid Fortnite installation found");
        }
    }

    void handleNodeOutput() {
        QString output = QString::fromUtf8(nodeProcess.readAllStandardOutput());
        logMessage("Node.js: " + output.trimmed());
    }

    void handleNodeError() {
        QString error = QString::fromUtf8(nodeProcess.readAllStandardError());
        logMessage("Node.js Error: " + error.trimmed());
    }

    void handleProxyOutput() {
        QString output = QString::fromUtf8(proxyProcess.readAllStandardOutput());
        logMessage("Proxy: " + output.trimmed());
    }

    void handleProxyError() {
        QString error = QString::fromUtf8(proxyProcess.readAllStandardError());
        logMessage("Proxy Error: " + error.trimmed());
    }

private:
    void showError(const QString &message) {
        QMessageBox::critical(this, "Error", message);
        logMessage("ERROR: " + message);
    }

    void logMessage(const QString &message) {
        QString timestamp = QTime::currentTime().toString("[hh:mm:ss]");
        consoleOutput->append(timestamp + " " + message);
        consoleOutput->verticalScrollBar()->setValue(consoleOutput->verticalScrollBar()->maximum());
    }

    QLineEdit *pathEdit;
    QTextEdit *consoleOutput;
    QPushButton *startButton;
    QPushButton *stopButton;
    QLabel *nodeStatus;
    QLabel *proxyStatus;
    QLabel *fortniteStatus;
    QProcess nodeProcess;
    QProcess proxyProcess;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZeroFNLauncher launcher;
    launcher.show();
    return app.exec();
}

#include "zerofnlauncher.moc"
