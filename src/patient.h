#pragma once

#include <QString>
#include <QDate>
#include <QList>
#include <QJsonObject>

// Class to represent a patient in the hospital system
class Patient 
{
public:
    Patient();
    Patient(const QString& id, const QString& name, const QString& dateOfBirth, 
            const QString& gender, const QString& contactInfo);
    ~Patient();
    
    // Getters
    QString getId() const;
    QString getName() const;
    QString getDateOfBirth() const;
    QString getGender() const;
    QString getContactInfo() const;
    QList<QString> getMedicalHistory() const;
    
    // Setters
    void setId(const QString& id);
    void setName(const QString& name);
    void setDateOfBirth(const QString& dateOfBirth);
    void setGender(const QString& gender);
    void setContactInfo(const QString& contactInfo);
    void addMedicalRecord(const QString& record);
    
    // JSON conversion for storage
    QJsonObject toJson() const;
    static Patient fromJson(const QJsonObject& json);

private:
    QString m_id;           // Patient ID
    QString m_name;         // Patient name
    QString m_dateOfBirth;  // Date of birth
    QString m_gender;       // Gender
    QString m_contactInfo;  // Contact information
    QList<QString> m_medicalHistory;  // Medical history records
};