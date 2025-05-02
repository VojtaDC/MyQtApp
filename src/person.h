#pragma once

#include <QString>
#include <QJsonObject>
#include <filesystem>
#include <memory>

// Base class for all people in the hospital system
class Person 
{
public:
    Person();
    Person(const QString& id, const QString& name, const QString& contactInfo);
    virtual ~Person(); // Virtual destructor for proper cleanup in derived classes
    
    // Getters
    QString getId() const;
    QString getName() const;
    QString getContactInfo() const;
    
    // Setters
    void setId(const QString& id);
    void setName(const QString& name);
    void setContactInfo(const QString& contactInfo);
    
    // JSON conversion for storage - virtual for polymorphic behavior
    virtual QJsonObject toJson() const;
    
    // Static factory method for type determination will be implemented in derived classes

protected:
    QString m_id;           // Person ID
    QString m_name;         // Person name
    QString m_contactInfo;  // Contact information
};