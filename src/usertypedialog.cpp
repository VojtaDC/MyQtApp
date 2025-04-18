#include "usertypedialog.h"
#include <QFont>

UserTypeDialog::UserTypeDialog(QWidget *parent)
    : QDialog(parent), selectedType(UserType::Unknown)
{
    setWindowTitle("COVID-19 X-Ray Classification");
    setFixedSize(400, 300);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    setupUI();
}

UserTypeDialog::~UserTypeDialog()
{
    // Qt will automatically delete the UI elements
}

void UserTypeDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    
    // Title label with larger font
    titleLabel = new QLabel("Select User Type", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Doctor button
    doctorButton = new QPushButton("Doctor", this);
    doctorButton->setMinimumHeight(60);
    QFont buttonFont = doctorButton->font();
    buttonFont.setPointSize(12);
    doctorButton->setFont(buttonFont);
    
    // Patient button
    patientButton = new QPushButton("Patient", this);
    patientButton->setMinimumHeight(60);
    patientButton->setFont(buttonFont);
    
    // Add widgets to layout with some spacing
    mainLayout->addSpacing(20);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(doctorButton);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(patientButton);
    mainLayout->addStretch();
    
    // Set margins for better appearance
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Connect buttons to slots
    connect(doctorButton, &QPushButton::clicked, this, &UserTypeDialog::doctorSelected);
    connect(patientButton, &QPushButton::clicked, this, &UserTypeDialog::patientSelected);
}

void UserTypeDialog::doctorSelected()
{
    selectedType = UserType::Doctor;
    accept();
}

void UserTypeDialog::patientSelected()
{
    selectedType = UserType::Patient;
    accept();
}

UserType UserTypeDialog::getUserType() const
{
    return selectedType;
}