#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include "usertypedialog.h"

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(UserType userType, QWidget *parent = nullptr);
    ~LoginDialog();
    
    // Methods to get login information
    QString getUsername() const;
    QString getPassword() const;
    UserType getUserType() const;

private slots:
    void onLoginClicked();
    void onCancelClicked();
    void onTextChanged();

private:
    void setupUI();
    
    QLabel *titleLabel;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QVBoxLayout *mainLayout;
    
    QString username;
    QString password;
    UserType userType;
    
    // Sample credentials (in a real app, these would be stored securely)
    const QString DOCTOR_USERNAME = "doctor";
    const QString DOCTOR_PASSWORD = "doctor123";
    const QString PATIENT_USERNAME = "patient";
    const QString PATIENT_PASSWORD = "patient123";
};