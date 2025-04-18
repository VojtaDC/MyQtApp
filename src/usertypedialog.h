#pragma once

#include <QDialog>
#include <QString>

// Define the UserType enum
enum class UserType {
    Doctor,
    Patient,
    Unknown
};

namespace Ui {
class UserTypeDialog;
}

class UserTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserTypeDialog(QWidget *parent = nullptr);
    ~UserTypeDialog();
    
    UserType getUserType() const;

private slots:
    void on_doctorButton_clicked();
    void on_patientButton_clicked();

private:
    Ui::UserTypeDialog *ui;
    UserType selectedType;
};