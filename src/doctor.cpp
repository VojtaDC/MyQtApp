#include "doctor.h"

Doctor::Doctor()
    : Person(), m_specialization("")
{
}

Doctor::Doctor(const QString& id, const QString& name, const QString& specialization, 
              const QString& contactInfo)
    : Person(id, name, contactInfo), m_specialization(specialization)
{
}

Doctor::~Doctor()
{
    // Specific cleanup for Doctor if needed
}

QString Doctor::getSpecialization() const
{
    return m_specialization;
}

void Doctor::setSpecialization(const QString& specialization)
{
    m_specialization = specialization;
}

QJsonObject Doctor::toJson() const
{
    // Start with base class's JSON representation
    QJsonObject json = Person::toJson();
    
    // Add specialized fields
    json["specialization"] = m_specialization;
    
    return json;
}

Doctor Doctor::fromJson(const QJsonObject& json)
{
    Doctor doctor;
    doctor.setId(json["id"].toString());
    doctor.setName(json["name"].toString());
    doctor.setSpecialization(json["specialization"].toString());
    doctor.setContactInfo(json["contactInfo"].toString());
    
    return doctor;
}