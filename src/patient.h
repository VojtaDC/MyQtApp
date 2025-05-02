#pragma once

#include "person.h"
#include <QString>
#include <QDate>
#include <QList>
#include <QJsonObject>

// Class to represent a patient in the hospital system, inherits from Person
class Patient : public Person
{
public:
    Patient();
    Patient(const QString& id, const QString& name, const QString& dateOfBirth, 
            const QString& gender, const QString& contactInfo);
    ~Patient() override;
    
    // Specialized getters
    QString getDateOfBirth() const;
    QString getGender() const;
    const QList<QString>& getMedicalHistory() const;
    
    // Specialized setters
    void setDateOfBirth(const QString& dateOfBirth);
    void setGender(const QString& gender);
    void addMedicalRecord(const QString& record);
    
    // Override JSON conversion for storage with specialized fields
    QJsonObject toJson() const override;
    static Patient fromJson(const QJsonObject& json);

private:
    QString m_dateOfBirth;  // Date of birth
    QString m_gender;       // Gender
    QList<QString> m_medicalHistory;  // Medical history records
};