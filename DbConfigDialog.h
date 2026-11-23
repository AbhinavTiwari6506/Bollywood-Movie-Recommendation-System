#ifndef DBCONFIGDIALOG_H
#define DBCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include "../database/DatabaseManager.h"

class DbConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit DbConfigDialog(QWidget *parent = nullptr);

signals:
    void databaseConfigured();

private slots:
    void onTestConnection();
    void onInitializeSchema();
    void onImportDataset();
    void onSaveAndConnect();

private:
    void setupUI();
    void loadCurrentValues();
    DbConfig getFormData() const;

    QLineEdit *m_hostEdit;
    QSpinBox *m_portSpin;
    QLineEdit *m_dbNameEdit;
    QLineEdit *m_userEdit;
    QLineEdit *m_passwordEdit;
    QComboBox *m_driverCombo;
    QLineEdit *m_datasetPathEdit;

    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QPushButton *m_importBtn;
};

#endif // DBCONFIGDIALOG_H
