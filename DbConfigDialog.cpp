#include "DbConfigDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

DbConfigDialog::DbConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadCurrentValues();
}

void DbConfigDialog::setupUI() {
    setWindowTitle("Database & MySQL Configuration");
    setFixedSize(540, 560);
    setStyleSheet(
        "QDialog {"
        "  background-color: #14161D;"
        "  color: #FFFFFF;"
        "}"
        "QGroupBox {"
        "  color: #E2E8F0;"
        "  font-weight: bold;"
        "  border: 1px solid #2B2F3A;"
        "  border-radius: 8px;"
        "  margin-top: 12px;"
        "  padding-top: 16px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 4px;"
        "}"
        "QLineEdit, QSpinBox, QComboBox {"
        "  background-color: #1F222A;"
        "  color: #FFFFFF;"
        "  border: 1px solid #323644;"
        "  border-radius: 6px;"
        "  padding: 6px 10px;"
        "  font-size: 13px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus, QComboBox:focus {"
        "  border: 1px solid #E50914;"
        "}"
        "QPushButton {"
        "  background-color: #262933;"
        "  color: #FFFFFF;"
        "  border: 1px solid #383D4C;"
        "  border-radius: 6px;"
        "  padding: 8px 16px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #323644;"
        "}"
        "QPushButton#PrimaryBtn {"
        "  background-color: #E50914;"
        "  border: none;"
        "  font-weight: bold;"
        "}"
        "QPushButton#PrimaryBtn:hover {"
        "  background-color: #F40612;"
        "}"
        "QProgressBar {"
        "  border: 1px solid #323644;"
        "  border-radius: 4px;"
        "  text-align: center;"
        "  background-color: #1F222A;"
        "  color: #FFFFFF;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #E50914;"
        "  border-radius: 3px;"
        "}");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    QLabel *headingLabel = new QLabel("MySQL Database Settings", this);
    QFont hFont;
    hFont.setPointSize(14);
    hFont.setBold(true);
    headingLabel->setFont(hFont);
    mainLayout->addWidget(headingLabel);

    QLabel *subheading = new QLabel("Configure your MySQL 8 connection or switch to local SQLite.", this);
    subheading->setStyleSheet("color: #94A3B8; font-size: 12px;");
    mainLayout->addWidget(subheading);

    // Form Group
    QGroupBox *connGroup = new QGroupBox("Connection Parameters", this);
    QGridLayout *gridLayout = new QGridLayout(connGroup);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setHorizontalSpacing(10);

    // Driver
    gridLayout->addWidget(new QLabel("Database Driver:"), 0, 0);
    m_driverCombo = new QComboBox(connGroup);
    m_driverCombo->addItem("MySQL 8 (QMYSQL)", "QMYSQL");
    m_driverCombo->addItem("SQLite Local Database (QSQLITE)", "QSQLITE");
    gridLayout->addWidget(m_driverCombo, 0, 1);

    // Host & Port
    gridLayout->addWidget(new QLabel("Host / Server:"), 1, 0);
    m_hostEdit = new QLineEdit(connGroup);
    gridLayout->addWidget(m_hostEdit, 1, 1);

    gridLayout->addWidget(new QLabel("Port:"), 2, 0);
    m_portSpin = new QSpinBox(connGroup);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(3306);
    gridLayout->addWidget(m_portSpin, 2, 1);

    // Database Name
    gridLayout->addWidget(new QLabel("Database Name:"), 3, 0);
    m_dbNameEdit = new QLineEdit(connGroup);
    gridLayout->addWidget(m_dbNameEdit, 3, 1);

    // Username & Password
    gridLayout->addWidget(new QLabel("Username:"), 4, 0);
    m_userEdit = new QLineEdit(connGroup);
    gridLayout->addWidget(m_userEdit, 4, 1);

    gridLayout->addWidget(new QLabel("Password:"), 5, 0);
    m_passwordEdit = new QLineEdit(connGroup);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    gridLayout->addWidget(m_passwordEdit, 5, 1);

    // Dataset CSV path
    gridLayout->addWidget(new QLabel("Dataset CSV:"), 6, 0);
    QHBoxLayout *fileLayout = new QHBoxLayout();
    m_datasetPathEdit = new QLineEdit(connGroup);
    QPushButton *browseBtn = new QPushButton("Browse...", connGroup);
    browseBtn->setFixedHeight(32);
    connect(browseBtn, &QPushButton::clicked, this, [this]() {
        QString file = QFileDialog::getOpenFileName(this, "Select Dataset CSV", m_datasetPathEdit->text(), "CSV Files (*.csv)");
        if (!file.isEmpty()) {
            m_datasetPathEdit->setText(file);
        }
    });
    fileLayout->addWidget(m_datasetPathEdit, 1);
    fileLayout->addWidget(browseBtn);
    gridLayout->addLayout(fileLayout, 6, 1);

    mainLayout->addWidget(connGroup);

    // Connection testing row
    QHBoxLayout *testLayout = new QHBoxLayout();
    QPushButton *testBtn = new QPushButton("🔌 Test Connection", this);
    connect(testBtn, &QPushButton::clicked, this, &DbConfigDialog::onTestConnection);
    testLayout->addWidget(testBtn);

    QPushButton *schemaBtn = new QPushButton("🛠️ Create Schema", this);
    connect(schemaBtn, &QPushButton::clicked, this, &DbConfigDialog::onInitializeSchema);
    testLayout->addWidget(schemaBtn);

    m_importBtn = new QPushButton("📥 Import Dataset", this);
    connect(m_importBtn, &QPushButton::clicked, this, &DbConfigDialog::onImportDataset);
    testLayout->addWidget(m_importBtn);

    mainLayout->addLayout(testLayout);

    // Progress Bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->hide();
    mainLayout->addWidget(m_progressBar);

    // Status message
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #94A3B8; font-size: 12px;");
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel);

    mainLayout->addStretch();

    // Dialog buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    QPushButton *saveBtn = new QPushButton("Save & Connect", this);
    saveBtn->setObjectName("PrimaryBtn");
    connect(saveBtn, &QPushButton::clicked, this, &DbConfigDialog::onSaveAndConnect);
    btnLayout->addWidget(saveBtn);

    mainLayout->addLayout(btnLayout);
}

void DbConfigDialog::loadCurrentValues() {
    DbConfig cfg = DatabaseManager::instance().currentConfig();
    m_hostEdit->setText(cfg.host);
    m_portSpin->setValue(cfg.port);
    m_dbNameEdit->setText(cfg.databaseName);
    m_userEdit->setText(cfg.username);
    m_passwordEdit->setText(cfg.password);
    m_datasetPathEdit->setText(cfg.datasetPath);

    int idx = m_driverCombo->findData(cfg.driver);
    if (idx >= 0) m_driverCombo->setCurrentIndex(idx);

    if (DatabaseManager::instance().isConnected()) {
        int count = DatabaseManager::instance().getMovieCount();
        m_statusLabel->setText(QString("🟢 Status: Connected to %1 (%2 movies loaded)")
                               .arg(DatabaseManager::instance().activeDriverName())
                               .arg(count));
        m_statusLabel->setStyleSheet("color: #46D369; font-weight: bold; font-size: 12px;");
    } else {
        m_statusLabel->setText(QString("🔴 Status: Not connected. %1").arg(DatabaseManager::instance().lastError()));
        m_statusLabel->setStyleSheet("color: #FF6B6B; font-size: 12px;");
    }
}

DbConfig DbConfigDialog::getFormData() const {
    DbConfig cfg;
    cfg.host = m_hostEdit->text().trimmed();
    cfg.port = m_portSpin->value();
    cfg.databaseName = m_dbNameEdit->text().trimmed();
    cfg.username = m_userEdit->text().trimmed();
    cfg.password = m_passwordEdit->text();
    cfg.driver = m_driverCombo->currentData().toString();
    cfg.datasetPath = m_datasetPathEdit->text().trimmed();
    return cfg;
}

void DbConfigDialog::onTestConnection() {
    DbConfig cfg = getFormData();
    QString errMsg;
    m_statusLabel->setText("Testing connection...");
    m_statusLabel->setStyleSheet("color: #F5C518;");
    QApplication::processEvents();

    bool ok = DatabaseManager::instance().connectToDatabase(cfg, &errMsg);
    if (ok) {
        int count = DatabaseManager::instance().getMovieCount();
        m_statusLabel->setText(QString("✅ Connection successful! Found %1 movies in table.").arg(count));
        m_statusLabel->setStyleSheet("color: #46D369; font-weight: bold;");
    } else {
        m_statusLabel->setText(QString("❌ Connection failed:\n%1").arg(errMsg));
        m_statusLabel->setStyleSheet("color: #FF6B6B;");
    }
}

void DbConfigDialog::onInitializeSchema() {
    DbConfig cfg = getFormData();
    QString errMsg;
    if (!DatabaseManager::instance().isConnected()) {
        if (!DatabaseManager::instance().connectToDatabase(cfg, &errMsg)) {
            QMessageBox::warning(this, "Connection Error", QString("Cannot initialize schema without connecting first:\n%1").arg(errMsg));
            return;
        }
    }

    if (DatabaseManager::instance().initializeSchema(&errMsg)) {
        QMessageBox::information(this, "Success", "Database schema and 'movies' table created/verified successfully!");
        m_statusLabel->setText("✅ Schema initialized successfully!");
        m_statusLabel->setStyleSheet("color: #46D369;");
    } else {
        QMessageBox::critical(this, "Schema Error", QString("Failed to create schema:\n%1").arg(errMsg));
    }
}

void DbConfigDialog::onImportDataset() {
    DbConfig cfg = getFormData();
    QString errMsg;
    if (!DatabaseManager::instance().isConnected()) {
        if (!DatabaseManager::instance().connectToDatabase(cfg, &errMsg)) {
            QMessageBox::warning(this, "Connection Error", QString("Please connect to database before importing:\n%1").arg(errMsg));
            return;
        }
    }

    QString csvPath = m_datasetPathEdit->text().trimmed();
    if (!QFile::exists(csvPath)) {
        QMessageBox::warning(this, "File Not Found", QString("Dataset file does not exist at:\n%1").arg(csvPath));
        return;
    }

    m_progressBar->show();
    m_progressBar->setValue(0);
    m_importBtn->setEnabled(false);

    connect(&DatabaseManager::instance(), &DatabaseManager::importProgress, this, [this](int pct, const QString &text) {
        m_progressBar->setValue(pct);
        m_statusLabel->setText(text);
        QApplication::processEvents();
    });

    bool ok = DatabaseManager::instance().importDatasetCSV(csvPath, nullptr, &errMsg);
    m_importBtn->setEnabled(true);

    if (ok) {
        int count = DatabaseManager::instance().getMovieCount();
        QMessageBox::information(this, "Import Complete", QString("Successfully imported %1 movies into database!").arg(count));
        m_statusLabel->setText(QString("✅ Dataset loaded: %1 movies stored in database.").arg(count));
        m_statusLabel->setStyleSheet("color: #46D369; font-weight: bold;");
        emit databaseConfigured();
    } else {
        QMessageBox::critical(this, "Import Error", QString("Import failed:\n%1").arg(errMsg));
        m_statusLabel->setText("❌ Import failed.");
        m_statusLabel->setStyleSheet("color: #FF6B6B;");
    }
}

void DbConfigDialog::onSaveAndConnect() {
    DbConfig cfg = getFormData();
    DatabaseManager::instance().saveConfig(cfg);
    
    QString errMsg;
    bool ok = DatabaseManager::instance().connectToDatabase(cfg, &errMsg);
    if (ok) {
        emit databaseConfigured();
        accept();
    } else {
        int ret = QMessageBox::warning(
            this, "Connection Failed",
            QString("Could not connect with these settings:\n%1\n\nSave settings anyway?").arg(errMsg),
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            accept();
        }
    }
}
