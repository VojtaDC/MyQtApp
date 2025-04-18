#include "patient.h"
#include <QJsonArray>

Patient::Patient()
    : m_id(""), m_name(""), m_dateOfBirth(""), m_gender(""), m_contactInfo("")
{
}

Patient::Patient(const QString& id, const QString& name, const QString& dateOfBirth, 
                 const QString& gender, const QString& contactInfo)
    : m_id(id), m_name(name), m_dateOfBirth(dateOfBirth), m_gender(gender), m_contactInfo(contactInfo)
{
}

Patient::~Patient()
{
}

QString Patient::getId() const
{
    return m_id;
}

QString Patient::getName() const
{
    return m_name;
}

QString Patient::getDateOfBirth() const
{
    return m_dateOfBirth;
}

QString Patient::getGender() const
{
    return m_gender;
}

QString Patient::getContactInfo() const
{
    return m_contactInfo;
}

QList<QString> Patient::getMedicalHistory() const
{
    return m_medicalHistory;
}

void Patient::setId(const QString& id)
{
    m_id = id;
}

void Patient::setName(const QString& name)
{
    m_name = name;
}

void Patient::setDateOfBirth(const QString& dateOfBirth)
{
    m_dateOfBirth = dateOfBirth;
}

void Patient::setGender(const QString& gender)
{
    m_gender = gender;
}

void Patient::setContactInfo(const QString& contactInfo)
{
    m_contactInfo = contactInfo;
}

void Patient::addMedicalRecord(const QString& record)
{
    m_medicalHistory.append(record);
}

QJsonObject Patient::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["dateOfBirth"] = m_dateOfBirth;
    json["gender"] = m_gender;
    json["contactInfo"] = m_contactInfo;
    
    QJsonArray historyArray;
    for (const QString& record : m_medicalHistory) {
        historyArray.append(record);
    }
    json["medicalHistory"] = historyArray;
    
    return json;
}

Patient Patient::fromJson(const QJsonObject& json)
{
    Patient patient;
    patient.setId(json["id"].toString());
    patient.setName(json["name"].toString());
    patient.setDateOfBirth(json["dateOfBirth"].toString());
    patient.setGender(json["gender"].toString());
    patient.setContactInfo(json["contactInfo"].toString());
    
    QJsonArray historyArray = json["medicalHistory"].toArray();
    for (int i = 0; i < historyArray.size(); ++i) {
        patient.addMedicalRecord(historyArray[i].toString());
    }
    
    return patient;
}