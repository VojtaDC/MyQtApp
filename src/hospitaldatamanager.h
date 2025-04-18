#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QDir>
#include "patient.h"
#include "doctor.h"

// Class to handle file system operations for hospital data
class HospitalDataManager
{
public:
    HospitalDataManager();
    ~HospitalDataManager();
    
    // Initialize the file system structure
    bool initialize();
    
    // Patient operations
    bool savePatient(const Patient& patient);
    bool deletePatient(const QString& patientId);
    Patient getPatient(const QString& patientId) const;
    QList<Patient> getAllPatients() const;
    QList<Patient> searchPatients(const QString& searchTerm) const;
    
    // Doctor operations
    bool saveDoctor(const Doctor& doctor);
    bool deleteDoctor(const QString& doctorId);
    Doctor getDoctor(const QString& doctorId) const;
    QList<Doctor> getAllDoctors() const;
    
    // Check if users exist in the system
    bool isDoctorRegistered(const QString& username, const QString& password) const;
    bool isPatientRegistered(const QString& username, const QString& password) const;
    
    // Get the data directory
    QString getDataDirectory() const;

private:
    QString m_dataDirectory;
    QString m_patientsDirectory;
    QString m_doctorsDirectory;
    
    // Helper methods
    bool createDirectoryIfNotExists(const QString& path);
    QString generateUniqueId() const;
    
    // Cache for frequently accessed data
    mutable QMap<QString, Patient> m_patientCache;
    mutable QMap<QString, Doctor> m_doctorCache;
    
    // Internal file management
    bool writeJsonToFile(const QString& filePath, const QJsonObject& jsonObj) const;
    QJsonObject readJsonFromFile(const QString& filePath) const;
};