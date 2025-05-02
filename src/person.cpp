#include "person.h"

Person::Person() 
    : m_id(""), m_name(""), m_contactInfo("")
{
}

Person::Person(const QString& id, const QString& name, const QString& contactInfo)
    : m_id(id), m_name(name), m_contactInfo(contactInfo)
{
}

Person::~Person()
{
    // Virtual destructor implementation
}

QString Person::getId() const
{
    return m_id;
}

QString Person::getName() const
{
    return m_name;
}

QString Person::getContactInfo() const
{
    return m_contactInfo;
}

void Person::setId(const QString& id)
{
    m_id = id;
}

void Person::setName(const QString& name)
{
    m_name = name;
}

void Person::setContactInfo(const QString& contactInfo)
{
    m_contactInfo = contactInfo;
}

QJsonObject Person::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["contactInfo"] = m_contactInfo;
    
    return json;
}