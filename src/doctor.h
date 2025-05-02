#pragma once

#include "person.h"
#include <QString>
#include <QJsonObject>

// Class to represent a doctor in the hospital system, inherits from Person
class Doctor : public Person
{
public:
    Doctor();
    Doctor(const QString& id, const QString& name, const QString& specialization, 
           const QString& contactInfo);
    ~Doctor() override;
    
    // Specialized getters
    QString getSpecialization() const;
    
    // Specialized setters
    void setSpecialization(const QString& specialization);
    
    // Override JSON conversion for storage with specialized fields
    QJsonObject toJson() const override;
    static Doctor fromJson(const QJsonObject& json);

private:
    QString m_specialization;    // Specialization (e.g., Cardiology, Radiology)
};