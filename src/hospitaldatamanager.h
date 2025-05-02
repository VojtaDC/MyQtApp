#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QDir>
#include <filesystem>
#include <span>
#include <vector>
#include <memory>
#include <regex>
#include <mutex>
#include <condition_variable>
#include "patient.h"
#include "doctor.h"

// Enum class for user roles
enum class UserRole {
    Doctor,
    Patient,
    Administrator,
    Unknown
};

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
    bool validatePatientId(const QString& patientId) const;
    
    // Get the data directory
    std::filesystem::path getDataDirectory() const;
    
    // Metadata processing - uses regex
    bool loadMetadata(const std::string& filePath);
    
    // Directory operations
    bool saveDirectory(const std::filesystem::path& sourcePath, const std::filesystem::path& destPath);

private:
    std::filesystem::path m_dataDirectory;
    std::filesystem::path m_patientsDirectory;
    std::filesystem::path m_doctorsDirectory;
    std::filesystem::path m_tempDirectory;
    
    // Helper methods
    bool createDirectoryIfNotExists(const std::filesystem::path& path);
    QString generateUniqueId() const;
    
    // Recursive directory saving - tail recursive implementation
    bool saveDirectoryRecursive(const std::filesystem::path& sourcePath, 
                               const std::filesystem::path& destPath,
                               std::vector<std::filesystem::path>& pendingPaths);
    
    // Cache for frequently accessed data with thread safety
    mutable std::mutex m_cacheMutex;
    mutable std::unordered_map<QString, Patient> m_patientCache;
    mutable std::unordered_map<QString, Doctor> m_doctorCache;
    
    // Regex for patient ID validation
    std::regex m_patientIdRegex;
    
    // Internal file management
    bool writeJsonToFile(const std::filesystem::path& filePath, const QJsonObject& jsonObj) const;
    QJsonObject readJsonFromFile(const std::filesystem::path& filePath) const;
    
    // Template function for statistics calculation over any range
    template<class Range>
    auto calculateStatistics(const Range& range) const -> std::pair<double, double>;
};