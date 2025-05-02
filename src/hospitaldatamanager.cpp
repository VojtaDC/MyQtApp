#include "hospitaldatamanager.h"
#include <QJsonDocument>
#include <QFile>
#include <QUuid>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <thread>

HospitalDataManager::HospitalDataManager()
    : m_patientIdRegex("[a-zA-Z0-9]{8}")  // Updated regex to accept 8-character alphanumeric IDs
{
    // Set up directories for data storage with std::filesystem
    m_dataDirectory = std::filesystem::path(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString()) / "HospitalData";
    m_patientsDirectory = m_dataDirectory / "patients";
    m_doctorsDirectory = m_dataDirectory / "doctors";
    m_tempDirectory = std::filesystem::temp_directory_path() / "HospitalAppTemp";
}

HospitalDataManager::~HospitalDataManager()
{
    // Clean up temporary files if they exist
    try {
        if (std::filesystem::exists(m_tempDirectory)) {
            std::filesystem::remove_all(m_tempDirectory);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Failed to clean up temporary directory:" << e.what();
    }
}

bool HospitalDataManager::initialize()
{
    try {
        // Create all required directories
        if (!createDirectoryIfNotExists(m_dataDirectory) ||
            !createDirectoryIfNotExists(m_patientsDirectory) ||
            !createDirectoryIfNotExists(m_doctorsDirectory) ||
            !createDirectoryIfNotExists(m_tempDirectory)) {
            return false;
        }
        
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Filesystem error during initialization:" << e.what();
        return false;
    }
}

bool HospitalDataManager::savePatient(const Patient& patient)
{
    QString patientId = patient.getId();
    
    // If ID is empty, generate a new one
    if (patientId.isEmpty()) {
        patientId = generateUniqueId();
        const_cast<Patient&>(patient).setId(patientId);
    }
    
    // Validate patient ID format using regex
    if (!validatePatientId(patientId)) {
        qWarning() << "Invalid patient ID format:" << patientId;
        return false;
    }
    
    // Convert patient to JSON
    QJsonObject patientJson = patient.toJson();
    
    // Use std::filesystem path for file operations
    std::filesystem::path filePath = m_patientsDirectory / (patientId.toStdString() + ".json");
    
    if (!writeJsonToFile(filePath, patientJson)) {
        return false;
    }
    
    // Update cache with thread safety
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        m_patientCache[patientId] = patient;
    }
    
    return true;
}

bool HospitalDataManager::deletePatient(const QString& patientId)
{
    std::filesystem::path filePath = m_patientsDirectory / (patientId.toStdString() + ".json");
    
    try {
        bool success = std::filesystem::exists(filePath) && 
                      std::filesystem::remove(filePath);
        
        if (success) {
            // Remove from cache with thread safety
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_patientCache.erase(patientId);
        }
        
        return success;
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Error deleting patient:" << e.what();
        return false;
    }
}

Patient HospitalDataManager::getPatient(const QString& patientId) const
{
    // Check if patient is in cache with thread safety
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        auto it = m_patientCache.find(patientId);
        if (it != m_patientCache.end()) {
            return it->second;
        }
    }
    
    // Read from file
    std::filesystem::path filePath = m_patientsDirectory / (patientId.toStdString() + ".json");
    QJsonObject patientJson = readJsonFromFile(filePath);
    
    if (!patientJson.isEmpty()) {
        Patient patient = Patient::fromJson(patientJson);
        
        // Update cache with thread safety
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        m_patientCache[patientId] = patient;
        
        return patient;
    }
    
    // Return empty patient if not found
    return Patient();
}

QList<Patient> HospitalDataManager::getAllPatients() const
{
    QList<Patient> patients;
    
    try {
        // Using std::filesystem to iterate through directory
        for (const auto& entry : std::filesystem::directory_iterator(m_patientsDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string patientIdStr = entry.path().stem().string();
                QString patientId = QString::fromStdString(patientIdStr);
                
                Patient patient = getPatient(patientId);
                if (!patient.getId().isEmpty()) {
                    patients.append(patient);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Error accessing patients directory:" << e.what();
    }
    
    return patients;
}

QList<Patient> HospitalDataManager::searchPatients(const QString& searchTerm) const
{
    QList<Patient> allPatients = getAllPatients();
    QList<Patient> matchingPatients;
    
    QString lowerSearchTerm = searchTerm.toLower();
    
    // Filter patients based on search term
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
    
    // Use std::filesystem path for file operations
    std::filesystem::path filePath = m_doctorsDirectory / (doctorId.toStdString() + ".json");
    
    if (!writeJsonToFile(filePath, doctorJson)) {
        return false;
    }
    
    // Update cache with thread safety
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        m_doctorCache[doctorId] = doctor;
    }
    
    return true;
}

bool HospitalDataManager::deleteDoctor(const QString& doctorId)
{
    std::filesystem::path filePath = m_doctorsDirectory / (doctorId.toStdString() + ".json");
    
    try {
        bool success = std::filesystem::exists(filePath) && 
                      std::filesystem::remove(filePath);
        
        if (success) {
            // Remove from cache with thread safety
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_doctorCache.erase(doctorId);
        }
        
        return success;
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Error deleting doctor:" << e.what();
        return false;
    }
}

Doctor HospitalDataManager::getDoctor(const QString& doctorId) const
{
    // Check if doctor is in cache with thread safety
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        auto it = m_doctorCache.find(doctorId);
        if (it != m_doctorCache.end()) {
            return it->second;
        }
    }
    
    // Read from file
    std::filesystem::path filePath = m_doctorsDirectory / (doctorId.toStdString() + ".json");
    QJsonObject doctorJson = readJsonFromFile(filePath);
    
    if (!doctorJson.isEmpty()) {
        Doctor doctor = Doctor::fromJson(doctorJson);
        
        // Update cache with thread safety
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        m_doctorCache[doctorId] = doctor;
        
        return doctor;
    }
    
    // Return empty doctor if not found
    return Doctor();
}

QList<Doctor> HospitalDataManager::getAllDoctors() const
{
    QList<Doctor> doctors;
    
    try {
        // Using std::filesystem to iterate through directory
        for (const auto& entry : std::filesystem::directory_iterator(m_doctorsDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string doctorIdStr = entry.path().stem().string();
                QString doctorId = QString::fromStdString(doctorIdStr);
                
                Doctor doctor = getDoctor(doctorId);
                if (!doctor.getId().isEmpty()) {
                    doctors.append(doctor);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Error accessing doctors directory:" << e.what();
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

std::filesystem::path HospitalDataManager::getDataDirectory() const
{
    return m_dataDirectory;
}

bool HospitalDataManager::validatePatientId(const QString& patientId) const
{
    // Using std::regex to validate patient ID format (8-character alphanumeric ID)
    return std::regex_match(patientId.toStdString(), m_patientIdRegex);
}

bool HospitalDataManager::createDirectoryIfNotExists(const std::filesystem::path& path)
{
    try {
        if (!std::filesystem::exists(path)) {
            return std::filesystem::create_directories(path);
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Failed to create directory:" << QString::fromStdString(path.string())
                   << "Error:" << e.what();
        return false;
    }
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

// Directory operations - tail recursive implementation
bool HospitalDataManager::saveDirectory(const std::filesystem::path& sourcePath, 
                                       const std::filesystem::path& destPath)
{
    if (!std::filesystem::exists(sourcePath) || !std::filesystem::is_directory(sourcePath)) {
        qWarning() << "Source path does not exist or is not a directory:" 
                  << QString::fromStdString(sourcePath.string());
        return false;
    }
    
    // Create the destination directory if it doesn't exist
    if (!createDirectoryIfNotExists(destPath)) {
        return false;
    }
    
    // Start the recursive process with an empty stack
    std::vector<std::filesystem::path> pendingPaths;
    return saveDirectoryRecursive(sourcePath, destPath, pendingPaths);
}

bool HospitalDataManager::saveDirectoryRecursive(const std::filesystem::path& sourcePath,
                                                const std::filesystem::path& destPath,
                                                std::vector<std::filesystem::path>& pendingPaths)
{
    try {
        // Process the current directory
        for (const auto& entry : std::filesystem::directory_iterator(sourcePath)) {
            const auto& entryPath = entry.path();
            auto relativePath = entryPath.lexically_relative(sourcePath);
            auto targetPath = destPath / relativePath;
            
            if (entry.is_directory()) {
                // Create corresponding directory in destination
                if (!createDirectoryIfNotExists(targetPath)) {
                    return false;
                }
                
                // Add directory to the pending stack for tail recursion
                pendingPaths.push_back(entryPath);
            } else if (entry.is_regular_file()) {
                // Copy the file
                std::filesystem::copy_file(entryPath, targetPath, 
                                          std::filesystem::copy_options::overwrite_existing);
            }
        }
        
        // Tail recursion: if there are pending directories, process the next one
        if (!pendingPaths.empty()) {
            auto nextDir = pendingPaths.back();
            pendingPaths.pop_back();
            return saveDirectoryRecursive(nextDir, destPath, pendingPaths);
        }
        
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Error during directory save operation:" << e.what();
        return false;
    }
}

// Metadata processing function using regex and stream reading
bool HospitalDataManager::loadMetadata(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        qWarning() << "Failed to open metadata file:" << QString::fromStdString(filePath);
        return false;
    }
    
    // Define regex patterns for parsing
    std::regex headerPattern("^\\s*([\\w\\s]+)\\s*:\\s*(.+)\\s*$");
    std::regex dataRowPattern("^\\s*(\\d+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*$");
    
    std::string line;
    std::unordered_map<std::string, std::string> metadata;
    
    // Process the file line by line
    while (std::getline(file, line)) {
        std::smatch matches;
        
        // Try to match header information
        if (std::regex_match(line, matches, headerPattern)) {
            if (matches.size() == 3) {
                std::string key = matches[1].str();
                std::string value = matches[2].str();
                metadata[key] = value;
            }
        }
        // Try to match data rows
        else if (std::regex_match(line, matches, dataRowPattern)) {
            if (matches.size() == 4) {
                std::string id = matches[1].str();
                std::string description = matches[2].str();
                std::string value = matches[3].str();
                
                // Process the metadata entry
                // In a real application, you would store this in a data structure
                qDebug() << "Parsed metadata:" << QString::fromStdString(id)
                        << QString::fromStdString(description) 
                        << QString::fromStdString(value);
            }
        }
    }
    
    return !metadata.empty();
}

bool HospitalDataManager::writeJsonToFile(const std::filesystem::path& filePath, const QJsonObject& jsonObj) const
{
    QFile file(QString::fromStdString(filePath.string()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << QString::fromStdString(filePath.string());
        return false;
    }
    
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    
    if (file.write(jsonData) == -1) {
        qWarning() << "Failed to write JSON data to file:" << QString::fromStdString(filePath.string());
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

QJsonObject HospitalDataManager::readJsonFromFile(const std::filesystem::path& filePath) const
{
    QFile file(QString::fromStdString(filePath.string()));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Not considering this a warning since it's normal when checking for files
        return QJsonObject();
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "Failed to parse JSON data from file:" << QString::fromStdString(filePath.string());
        return QJsonObject();
    }
    
    return doc.object();
}

// Template function for calculating statistics over any range
template<class Range>
auto HospitalDataManager::calculateStatistics(const Range& range) const -> std::pair<double, double>
{
    using value_type = typename std::remove_reference<decltype(*std::begin(range))>::type;
    
    // Make sure the range isn't empty
    if (std::begin(range) == std::end(range)) {
        return {0.0, 0.0};
    }
    
    // Calculate sum using std::accumulate
    double sum = std::accumulate(std::begin(range), std::end(range), 0.0,
                               [](double acc, const value_type& val) {
                                   return acc + static_cast<double>(val);
                               });
    
    // Calculate size
    size_t size = std::distance(std::begin(range), std::end(range));
    
    // Calculate mean
    double mean = sum / size;
    
    // Calculate variance
    double variance = 0.0;
    for (auto it = std::begin(range); it != std::end(range); ++it) {
        double diff = static_cast<double>(*it) - mean;
        variance += diff * diff;
    }
    variance /= size;
    
    // Return mean and standard deviation
    return {mean, std::sqrt(variance)};
}