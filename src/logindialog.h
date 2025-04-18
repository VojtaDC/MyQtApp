#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include "usertypedialog.h"
#include "hospitaldatamanager.h"

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
    
    // Set the data manager
    void setDataManager(HospitalDataManager* dataManager);

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
    
    HospitalDataManager* m_dataManager;
};