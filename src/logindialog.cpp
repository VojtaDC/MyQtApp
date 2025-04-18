#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(UserType userType, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), userType(userType), m_dataManager(nullptr)
{
    ui->setupUi(this);
    
    // Set window properties
    setWindowTitle("COVID-19 X-Ray Classification - Login");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    // Set the title based on user type
    QString titleText = (userType == UserType::Doctor) ? "Doctor Login" : "Patient Login";
    ui->titleLabel->setText(titleText);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setDataManager(HospitalDataManager* dataManager)
{
    m_dataManager = dataManager;
}

void LoginDialog::on_loginButton_clicked()
{
    username = ui->usernameEdit->text();
    password = ui->passwordEdit->text();
    
    bool isValid = false;
    
    if (m_dataManager) {
        // Use data manager to verify credentials
        if (userType == UserType::Doctor) {
            isValid = m_dataManager->isDoctorRegistered(username, password);
        } else {
            isValid = m_dataManager->isPatientRegistered(username, password);
        }
    } else {
        // Fallback to default credentials for testing
        if (userType == UserType::Doctor) {
            isValid = (username == "doctor" && password == "password");
        } else {
            isValid = (username == "patient" && password == "password");
        }
    }
    
    if (isValid) {
        accept(); // Close with success
    } else {
        QMessageBox::warning(this, "Login Failed", 
            "Invalid username or password. Please try again.");
        ui->passwordEdit->clear();
        ui->passwordEdit->setFocus();
    }
}

void LoginDialog::on_cancelButton_clicked()
{
    reject(); // Close dialog with cancel/reject
}

void LoginDialog::on_usernameEdit_textChanged(const QString &text)
{
    Q_UNUSED(text);
    updateLoginButtonState();
}

void LoginDialog::on_passwordEdit_textChanged(const QString &text)
{
    Q_UNUSED(text);
    updateLoginButtonState();
}

void LoginDialog::updateLoginButtonState()
{
    // Enable login button only when both fields have text
    ui->loginButton->setEnabled(!ui->usernameEdit->text().isEmpty() && 
                              !ui->passwordEdit->text().isEmpty());
}

QString LoginDialog::getUsername() const
{
    return username;
}

QString LoginDialog::getPassword() const
{
    return password;
}

UserType LoginDialog::getUserType() const
{
    return userType;
}