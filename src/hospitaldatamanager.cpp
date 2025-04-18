#include "hospitaldatamanager.h"
#include <QJsonDocument>
#include <QFile>
#include <QUuid>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>

HospitalDataManager::HospitalDataManager()
{
    // Set up directories for data storage
    m_dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/HospitalData";
    m_patientsDirectory = m_dataDirectory + "/patients";
    m_doctorsDirectory = m_dataDirectory + "/doctors";
}

HospitalDataManager::~HospitalDataManager()
{
}

bool HospitalDataManager::initialize()
{
    // Create the main data directory if it doesn't exist
    if (!createDirectoryIfNotExists(m_dataDirectory)) {
        return false;
    }
    
    // Create the patients directory if it doesn't exist
    if (!createDirectoryIfNotExists(m_patientsDirectory)) {
        return false;
    }
    
    // Create the doctors directory if it doesn't exist
    if (!createDirectoryIfNotExists(m_doctorsDirectory)) {
        return false;
    }
    
    return true;
}

bool HospitalDataManager::savePatient(const Patient& patient)
{
    QString patientId = patient.getId();
    
    // If ID is empty, generate a new one
    if (patientId.isEmpty()) {
        patientId = generateUniqueId();
        const_cast<Patient&>(patient).setId(patientId);
    }
    
    // Convert patient to JSON
    QJsonObject patientJson = patient.toJson();
    
    // Write to file
    QString filePath = m_patientsDirectory + "/" + patientId + ".json";
    if (!writeJsonToFile(filePath, patientJson)) {
        return false;
    }
    
    // Update cache
    m_patientCache[patientId] = patient;
    
    return true;
}

bool HospitalDataManager::deletePatient(const QString& patientId)
{
    QString filePath = m_patientsDirectory + "/" + patientId + ".json";
    QFile file(filePath);
    if (file.exists()) {
        bool success = file.remove();
        if (success) {
            m_patientCache.remove(patientId);
        }
        return success;
    }
    
    return false;
}

Patient HospitalDataManager::getPatient(const QString& patientId) const
{
    // Check if patient is in cache
    if (m_patientCache.contains(patientId)) {
        return m_patientCache[patientId];
    }
    
    // Read from file
    QString filePath = m_patientsDirectory + "/" + patientId + ".json";
    QJsonObject patientJson = readJsonFromFile(filePath);
    
    if (!patientJson.isEmpty()) {
        Patient patient = Patient::fromJson(patientJson);
        m_patientCache[patientId] = patient;
        return patient;
    }
    
    // Return empty patient if not found
    return Patient();
}

QList<Patient> HospitalDataManager::getAllPatients() const
{
    QList<Patient> patients;
    
    QDir dir(m_patientsDirectory);
    QStringList filters;
    filters << "*.json";
    
    QStringList patientFiles = dir.entryList(filters, QDir::Files);
    for (const QString& fileName : patientFiles) {
        QString patientId = fileName;
        patientId.chop(5); // Remove ".json"
        
        Patient patient = getPatient(patientId);
        if (!patient.getId().isEmpty()) {
            patients.append(patient);
        }
    }
    
    return patients;
}

QList<Patient> HospitalDataManager::searchPatients(const QString& searchTerm) const
{
    QList<Patient> allPatients = getAllPatients();
    QList<Patient> matchingPatients;
    
    QString lowerSearchTerm = searchTerm.toLower();
    
    for (const Patient& patient : allPatients) {
        if (patient.getName().toLower().contains(lowerSearchTerm) ||
            patient.getId().toLower().contains(lowerSearchTerm)) {
            matchingPatients.append(patient);
        }
    }
    
    return matchingPatients;
}

bool HospitalDataManager::saveDoctor(const Doctor& doctor)
{
    QString doctorId = doctor.getId();
    
    // If ID is empty, generate a new one
    if (doctorId.isEmpty()) {
        doctorId = generateUniqueId();
        const_cast<Doctor&>(doctor).setId(doctorId);
    }
    
    // Convert doctor to JSON
    QJsonObject doctorJson = doctor.toJson();
    
    // Write to file
    QString filePath = m_doctorsDirectory + "/" + doctorId + ".json";
    if (!writeJsonToFile(filePath, doctorJson)) {
        return false;
    }
    
    // Update cache
    m_doctorCache[doctorId] = doctor;
    
    return true;
}

bool HospitalDataManager::deleteDoctor(const QString& doctorId)
{
    QString filePath = m_doctorsDirectory + "/" + doctorId + ".json";
    QFile file(filePath);
    if (file.exists()) {
        bool success = file.remove();
        if (success) {
            m_doctorCache.remove(doctorId);
        }
        return success;
    }
    
    return false;
}

Doctor HospitalDataManager::getDoctor(const QString& doctorId) const
{
    // Check if doctor is in cache
    if (m_doctorCache.contains(doctorId)) {
        return m_doctorCache[doctorId];
    }
    
    // Read from file
    QString filePath = m_doctorsDirectory + "/" + doctorId + ".json";
    QJsonObject doctorJson = readJsonFromFile(filePath);
    
    if (!doctorJson.isEmpty()) {
        Doctor doctor = Doctor::fromJson(doctorJson);
        m_doctorCache[doctorId] = doctor;
        return doctor;
    }
    
    // Return empty doctor if not found
    return Doctor();
}

QList<Doctor> HospitalDataManager::getAllDoctors() const
{
    QList<Doctor> doctors;
    
    QDir dir(m_doctorsDirectory);
    QStringList filters;
    filters << "*.json";
    
    QStringList doctorFiles = dir.entryList(filters, QDir::Files);
    for (const QString& fileName : doctorFiles) {
        QString doctorId = fileName;
        doctorId.chop(5); // Remove ".json"
        
        Doctor doctor = getDoctor(doctorId);
        if (!doctor.getId().isEmpty()) {
            doctors.append(doctor);
        }
    }
    
    return doctors;
}

bool HospitalDataManager::isDoctorRegistered(const QString& username, const QString& password) const
{
    // For simplicity, we'll use a sample doctor account for demo
    if (username == "doctor" && password == "password") {
        return true;
    }
    
    // In real implementation, you would search the doctors directory for matching credentials
    return false;
}

bool HospitalDataManager::isPatientRegistered(const QString& username, const QString& password) const
{
    // For simplicity, we'll use a sample patient account for demo
    if (username == "patient" && password == "password") {
        return true;
    }
    
    // In real implementation, you would search the patients directory for matching credentials
    return false;
}

QString HospitalDataManager::getDataDirectory() const
{
    return m_dataDirectory;
}

bool HospitalDataManager::createDirectoryIfNotExists(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        bool success = dir.mkpath(".");
        if (!success) {
            qWarning() << "Failed to create directory:" << path;
        }
        return success;
    }
    return true;
}

QString HospitalDataManager::generateUniqueId() const
{
    // Generate a UUID
    QString uuid = QUuid::createUuid().toString();
    
    // Remove the curly braces
    uuid.remove(0, 1);
    uuid.chop(1);
    
    // Get the first 8 characters for a shorter ID
    return uuid.left(8);
}

bool HospitalDataManager::writeJsonToFile(const QString& filePath, const QJsonObject& jsonObj) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    
    if (file.write(jsonData) == -1) {
        qWarning() << "Failed to write JSON data to file:" << filePath;
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

QJsonObject HospitalDataManager::readJsonFromFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Not considering this a warning since it's normal when checking for files
        return QJsonObject();
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "Failed to parse JSON data from file:" << filePath;
        return QJsonObject();
    }
    
    return doc.object();
}