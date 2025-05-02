#include "patient.h"
#include <QJsonArray>

Patient::Patient()
    : Person(), m_dateOfBirth(""), m_gender("")
{
}

Patient::Patient(const QString& id, const QString& name, const QString& dateOfBirth, 
                 const QString& gender, const QString& contactInfo)
    : Person(id, name, contactInfo), m_dateOfBirth(dateOfBirth), m_gender(gender)
{
}

Patient::~Patient()
{
    // Specific cleanup for Patient if needed
}

QString Patient::getDateOfBirth() const
{
    return m_dateOfBirth;
}

QString Patient::getGender() const
{
    return m_gender;
}

const QList<QString>& Patient::getMedicalHistory() const
{
    // Return a const reference to the medical history data
    return m_medicalHistory;
}

void Patient::setDateOfBirth(const QString& dateOfBirth)
{
    m_dateOfBirth = dateOfBirth;
}

void Patient::setGender(const QString& gender)
{
    m_gender = gender;
}

void Patient::addMedicalRecord(const QString& record)
{
    m_medicalHistory.append(record);
}

QJsonObject Patient::toJson() const
{
    // Start with base class's JSON representation
    QJsonObject json = Person::toJson();
    
    // Add specialized fields
    json["dateOfBirth"] = m_dateOfBirth;
    json["gender"] = m_gender;
    
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