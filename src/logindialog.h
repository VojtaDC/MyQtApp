#pragma once

#include <QDialog>
#include "usertypedialog.h"
#include "hospitaldatamanager.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(UserType userType, QWidget *parent = nullptr);
    ~LoginDialog();
    
    // Methods to get login information
    QString getUsername() const;
    QString getPassword() const;
    UserType getUserType() const;
    
    // Set the data manager
    void setDataManager(HospitalDataManager* dataManager);

private slots:
    void on_loginButton_clicked();
    void on_cancelButton_clicked();
    void on_usernameEdit_textChanged(const QString &text);
    void on_passwordEdit_textChanged(const QString &text);
    void updateLoginButtonState();

private:
    Ui::LoginDialog *ui;
    QString username;
    QString password;
    UserType userType;
    HospitalDataManager* m_dataManager;
};