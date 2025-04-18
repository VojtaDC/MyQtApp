#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QString>

// Enumeration to represent user types
enum class UserType {
    Doctor,
    Patient,
    Unknown
};

class UserTypeDialog : public QDialog
{
    Q_OBJECT

public:
    UserTypeDialog(QWidget *parent = nullptr);
    ~UserTypeDialog();
    
    // Method to get the selected user type
    UserType getUserType() const;

private slots:
    void doctorSelected();
    void patientSelected();

private:
    void setupUI();
    
    QPushButton *doctorButton;
    QPushButton *patientButton;
    QLabel *titleLabel;
    QVBoxLayout *mainLayout;
    
    UserType selectedType;
};