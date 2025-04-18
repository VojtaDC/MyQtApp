#include "doctor.h"

Doctor::Doctor()
    : m_id(""), m_name(""), m_specialization(""), m_contactInfo("")
{
}

Doctor::Doctor(const QString& id, const QString& name, const QString& specialization, 
              const QString& contactInfo)
    : m_id(id), m_name(name), m_specialization(specialization), m_contactInfo(contactInfo)
{
}

Doctor::~Doctor()
{
}

QString Doctor::getId() const
{
    return m_id;
}

QString Doctor::getName() const
{
    return m_name;
}

QString Doctor::getSpecialization() const
{
    return m_specialization;
}

QString Doctor::getContactInfo() const
{
    return m_contactInfo;
}

void Doctor::setId(const QString& id)
{
    m_id = id;
}

void Doctor::setName(const QString& name)
{
    m_name = name;
}

void Doctor::setSpecialization(const QString& specialization)
{
    m_specialization = specialization;
}

void Doctor::setContactInfo(const QString& contactInfo)
{
    m_contactInfo = contactInfo;
}

QJsonObject Doctor::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["specialization"] = m_specialization;
    json["contactInfo"] = m_contactInfo;
    
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