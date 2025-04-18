#include "usertypedialog.h"
#include "ui_usertypedialog.h"

UserTypeDialog::UserTypeDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::UserTypeDialog), selectedType(UserType::Unknown)
{
    ui->setupUi(this);
    setWindowTitle("COVID-19 X-Ray Classification");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

UserTypeDialog::~UserTypeDialog()
{
    delete ui;
}

void UserTypeDialog::on_doctorButton_clicked()
{
    selectedType = UserType::Doctor;
    accept();
}

void UserTypeDialog::on_patientButton_clicked()
{
    selectedType = UserType::Patient;
    accept();
}

UserType UserTypeDialog::getUserType() const
{
    return selectedType;
}