#pragma once

#include <QString>
#include <QJsonObject>

// Class to represent a doctor in the hospital system
class Doctor 
{
public:
    Doctor();
    Doctor(const QString& id, const QString& name, const QString& specialization, 
           const QString& contactInfo);
    ~Doctor();
    
    // Getters
    QString getId() const;
    QString getName() const;
    QString getSpecialization() const;
    QString getContactInfo() const;
    
    // Setters
    void setId(const QString& id);
    void setName(const QString& name);
    void setSpecialization(const QString& specialization);
    void setContactInfo(const QString& contactInfo);
    
    // JSON conversion for storage
    QJsonObject toJson() const;
    static Doctor fromJson(const QJsonObject& json);

private:
    QString m_id;                // Doctor ID
    QString m_name;              // Doctor name
    QString m_specialization;    // Specialization (e.g., Cardiology, Radiology)
    QString m_contactInfo;       // Contact information
};