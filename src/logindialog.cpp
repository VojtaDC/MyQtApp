#include "logindialog.h"
#include <QFont>
#include <QMessageBox>
#include <QGridLayout>

LoginDialog::LoginDialog(UserType userType, QWidget *parent)
    : QDialog(parent), userType(userType)
{
    setWindowTitle("COVID-19 X-Ray Classification - Login");
    setFixedSize(400, 250);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    setupUI();
}

LoginDialog::~LoginDialog()
{
    // Qt will automatically delete the UI elements
}

void LoginDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    
    // Title label with user type
    QString titleText = (userType == UserType::Doctor) ? "Doctor Login" : "Patient Login";
    titleLabel = new QLabel(titleText, this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Grid layout for form elements
    QGridLayout *formLayout = new QGridLayout();
    
    // Username
    usernameLabel = new QLabel("Username:", this);
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Enter your username");
    
    // Password
    passwordLabel = new QLabel("Password:", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Enter your password");
    
    // Add to form layout
    formLayout->addWidget(usernameLabel, 0, 0);
    formLayout->addWidget(usernameEdit, 0, 1);
    formLayout->addWidget(passwordLabel, 1, 0);
    formLayout->addWidget(passwordEdit, 1, 1);
    
    // Buttons in horizontal layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Login", this);
    loginButton->setEnabled(false); // Disabled until text is entered
    cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    
    // Add everything to main layout
    mainLayout->addSpacing(10);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // Set margins for better appearance
    mainLayout->setContentsMargins(30, 20, 30, 20);
    
    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &LoginDialog::onTextChanged);
    connect(passwordEdit, &QLineEdit::textChanged, this, &LoginDialog::onTextChanged);
}

void LoginDialog::onLoginClicked()
{
    username = usernameEdit->text();
    password = passwordEdit->text();
    
    bool isValid = false;
    
    // Simple authentication logic
    if (userType == UserType::Doctor) {
        isValid = (username == DOCTOR_USERNAME && password == DOCTOR_PASSWORD);
    } else {
        isValid = (username == PATIENT_USERNAME && password == PATIENT_PASSWORD);
    }
    
    if (isValid) {
        accept(); // Close with success
    } else {
        QMessageBox::warning(this, "Login Failed", 
            "Invalid username or password. Please try again.");
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::onCancelClicked()
{
    reject(); // Close dialog with cancel/reject
}

void LoginDialog::onTextChanged()
{
    // Enable login button only when both fields have text
    loginButton->setEnabled(!usernameEdit->text().isEmpty() && 
                           !passwordEdit->text().isEmpty());
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