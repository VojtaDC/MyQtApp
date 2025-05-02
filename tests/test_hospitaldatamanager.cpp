#include <catch2/catch_all.hpp>
#include "hospitaldatamanager.h"
#include <filesystem>
#include <fstream>
#include <random>
#include <algorithm>
#include <vector>

// Create a temporary directory for testing
std::filesystem::path createTempTestDir() {
    std::filesystem::path testPath = std::filesystem::temp_directory_path() / "hospital_test_dir";
    std::filesystem::create_directories(testPath);
    return testPath;
}

// Generate a random string for unique file/directory names
std::string randomString(size_t length) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, chars.size() - 1);
    
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += chars[distrib(gen)];
    }
    return result;
}

// Create a test directory structure with files
void createTestDirectoryStructure(const std::filesystem::path& basePath) {
    // Create nested directories
    std::filesystem::create_directories(basePath / "dir1" / "subdir1");
    std::filesystem::create_directories(basePath / "dir2" / "subdir2");
    
    // Create some test files
    std::ofstream file1(basePath / "file1.txt");
    file1 << "Test file 1 content";
    file1.close();
    
    std::ofstream file2(basePath / "dir1" / "file2.txt");
    file2 << "Test file 2 content";
    file2.close();
    
    std::ofstream file3(basePath / "dir1" / "subdir1" / "file3.txt");
    file3 << "Test file 3 content";
    file3.close();
    
    std::ofstream file4(basePath / "dir2" / "subdir2" / "file4.txt");
    file4 << "Test file 4 content";
    file4.close();
}

// Tests for the HospitalDataManager class
TEST_CASE("HospitalDataManager basic functionality", "[hospitaldatamanager]") {
    // Create a temporary directory for testing
    auto testDir = createTempTestDir() / randomString(10);
    std::filesystem::create_directories(testDir);
    
    SECTION("Initialization creates required directories") {
        HospitalDataManager manager;
        
        // Verify initialization works
        REQUIRE(manager.initialize());
        
        // Verify data directory path is returned correctly
        auto dataDir = manager.getDataDirectory();
        REQUIRE(!dataDir.empty());
        
        // Check if directories were created
        REQUIRE(std::filesystem::exists(dataDir));
        REQUIRE(std::filesystem::is_directory(dataDir));
    }
    
    // Clean up
    std::filesystem::remove_all(testDir);
}

TEST_CASE("HospitalDataManager patient operations", "[hospitaldatamanager]") {
    // Create test patients
    Patient patient1("P-1234", "John Smith", "1980-01-01", "Male", "john@example.com");
    Patient patient2("P-5678", "Jane Doe", "1990-02-15", "Female", "jane@example.com");
    
    HospitalDataManager manager;
    REQUIRE(manager.initialize());
    
    SECTION("Save and retrieve patients") {
        // Save patients
        REQUIRE(manager.savePatient(patient1));
        REQUIRE(manager.savePatient(patient2));
        
        // Retrieve patients
        Patient retrieved1 = manager.getPatient("P-1234");
        Patient retrieved2 = manager.getPatient("P-5678");
        
        // Verify retrieved data
        REQUIRE(retrieved1.getId() == "P-1234");
        REQUIRE(retrieved1.getName() == "John Smith");
        REQUIRE(retrieved1.getDateOfBirth() == "1980-01-01");
        REQUIRE(retrieved1.getGender() == "Male");
        REQUIRE(retrieved1.getContactInfo() == "john@example.com");
        
        REQUIRE(retrieved2.getId() == "P-5678");
        REQUIRE(retrieved2.getName() == "Jane Doe");
    }
    
    SECTION("Get all patients") {
        // Save patients
        REQUIRE(manager.savePatient(patient1));
        REQUIRE(manager.savePatient(patient2));
        
        // Get all patients
        auto patients = manager.getAllPatients();
        
        // Check that we got both patients
        REQUIRE(patients.size() >= 2);  // May be >= if previous tests left patients
        
        // Find our test patients
        bool found1 = false, found2 = false;
        for (const auto& patient : patients) {
            if (patient.getId() == "P-1234") found1 = true;
            if (patient.getId() == "P-5678") found2 = true;
        }
        
        REQUIRE(found1);
        REQUIRE(found2);
    }
    
    SECTION("Delete patient") {
        // Save patient
        REQUIRE(manager.savePatient(patient1));
        
        // Verify patient exists
        REQUIRE(!manager.getPatient("P-1234").getId().isEmpty());
        
        // Delete patient
        REQUIRE(manager.deletePatient("P-1234"));
        
        // Verify patient no longer exists
        REQUIRE(manager.getPatient("P-1234").getId().isEmpty());
    }
    
    SECTION("Search patients") {
        // Save patients
        REQUIRE(manager.savePatient(patient1));
        REQUIRE(manager.savePatient(patient2));
        
        // Search by name
        auto results1 = manager.searchPatients("John");
        REQUIRE(results1.size() >= 1);
        bool foundJohn = false;
        for (const auto& patient : results1) {
            if (patient.getId() == "P-1234") foundJohn = true;
        }
        REQUIRE(foundJohn);
        
        // Search by another name
        auto results2 = manager.searchPatients("Jane");
        REQUIRE(results2.size() >= 1);
        bool foundJane = false;
        for (const auto& patient : results2) {
            if (patient.getId() == "P-5678") foundJane = true;
        }
        REQUIRE(foundJane);
    }
}

TEST_CASE("HospitalDataManager std::filesystem features", "[hospitaldatamanager][filesystem]") {
    // Create temporary test directories
    auto testBaseDir = createTempTestDir() / randomString(10);
    auto sourceDir = testBaseDir / "source";
    auto destDir = testBaseDir / "destination";
    
    // Create test directory structure
    createTestDirectoryStructure(sourceDir);
    
    HospitalDataManager manager;
    
    SECTION("Recursive directory saving") {
        // Copy directory structure using our tail-recursive implementation
        REQUIRE(manager.saveDirectory(sourceDir, destDir));
        
        // Verify that all directories and files were copied
        REQUIRE(std::filesystem::exists(destDir / "file1.txt"));
        REQUIRE(std::filesystem::exists(destDir / "dir1" / "file2.txt"));
        REQUIRE(std::filesystem::exists(destDir / "dir1" / "subdir1" / "file3.txt"));
        REQUIRE(std::filesystem::exists(destDir / "dir2" / "subdir2" / "file4.txt"));
        
        // Verify file content was copied correctly
        std::ifstream file(destDir / "file1.txt");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        REQUIRE(content == "Test file 1 content");
    }
    
    // Clean up
    std::filesystem::remove_all(testBaseDir);
}

TEST_CASE("HospitalDataManager regex validation", "[hospitaldatamanager][regex]") {
    HospitalDataManager manager;
    
    SECTION("Patient ID validation") {
        // Valid patient IDs (P-NNNN format)
        REQUIRE(manager.validatePatientId("P-1234"));
        REQUIRE(manager.validatePatientId("P-5678"));
        REQUIRE(manager.validatePatientId("P-0000"));
        REQUIRE(manager.validatePatientId("P-9999"));
        
        // Invalid patient IDs
        REQUIRE_FALSE(manager.validatePatientId("P1234"));  // Missing hyphen
        REQUIRE_FALSE(manager.validatePatientId("P-123"));   // Too few digits
        REQUIRE_FALSE(manager.validatePatientId("P-12345")); // Too many digits
        REQUIRE_FALSE(manager.validatePatientId("X-1234"));  // Wrong prefix
        REQUIRE_FALSE(manager.validatePatientId("p-1234"));  // Lowercase prefix
    }
}

// Test the template function for statistics calculation
TEST_CASE("Template statistics calculation", "[hospitaldatamanager][template]") {
    // Create a test instance using protected method exposure via friend class
    class TestableHospitalDataManager : public HospitalDataManager {
    public:
        template<class Range>
        auto testCalculateStatistics(const Range& range) const {
            return this->calculateStatistics(range);
        }
    };
    
    TestableHospitalDataManager manager;
    
    SECTION("Vector of integers") {
        std::vector<int> values = {2, 4, 6, 8, 10};
        auto [mean, stddev] = manager.testCalculateStatistics(values);
        
        REQUIRE(mean == Approx(6.0));
        REQUIRE(stddev == Approx(3.16227766).epsilon(0.00001));
    }
    
    SECTION("Vector of floats") {
        std::vector<float> values = {1.5f, 2.5f, 3.5f, 4.5f, 5.5f};
        auto [mean, stddev] = manager.testCalculateStatistics(values);
        
        REQUIRE(mean == Approx(3.5));
        REQUIRE(stddev == Approx(1.58113883).epsilon(0.00001));
    }
    
    SECTION("Array of doubles") {
        std::array<double, 4> values = {10.0, 20.0, 30.0, 40.0};
        auto [mean, stddev] = manager.testCalculateStatistics(values);
        
        REQUIRE(mean == Approx(25.0));
        REQUIRE(stddev == Approx(12.9099445).epsilon(0.00001));
    }
    
    SECTION("Empty container") {
        std::vector<int> values;
        auto [mean, stddev] = manager.testCalculateStatistics(values);
        
        REQUIRE(mean == 0.0);
        REQUIRE(stddev == 0.0);
    }
}