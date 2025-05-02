#include <catch2/catch_all.hpp>
#include "patient.h"
#include "person.h"
#include <QJsonObject>
#include <QJsonArray>
#include <memory>

// Tests for the Patient derived class
TEST_CASE("Patient class functionality", "[patient]") {
    // Create a Patient instance
    Patient patient("P-1234", "Alice Smith", "1990-05-15", "Female", "alice.smith@example.com");
    
    SECTION("Constructor and getters") {
        // Test base class getters
        REQUIRE(patient.getId() == "P-1234");
        REQUIRE(patient.getName() == "Alice Smith");
        REQUIRE(patient.getContactInfo() == "alice.smith@example.com");
        
        // Test derived class getters
        REQUIRE(patient.getDateOfBirth() == "1990-05-15");
        REQUIRE(patient.getGender() == "Female");
        
        // Medical history should be empty at first
        REQUIRE(patient.getMedicalHistory().size() == 0);
    }
    
    SECTION("Setters") {
        // Test base class setters
        patient.setId("P-5678");
        patient.setName("Bob Jones");
        patient.setContactInfo("bob.jones@example.com");
        
        // Test derived class setters
        patient.setDateOfBirth("1985-12-20");
        patient.setGender("Male");
        
        REQUIRE(patient.getId() == "P-5678");
        REQUIRE(patient.getName() == "Bob Jones");
        REQUIRE(patient.getContactInfo() == "bob.jones@example.com");
        REQUIRE(patient.getDateOfBirth() == "1985-12-20");
        REQUIRE(patient.getGender() == "Male");
    }
    
    SECTION("Medical record management") {
        // Add medical records
        patient.addMedicalRecord("Initial visit: Common cold");
        patient.addMedicalRecord("Follow-up: Condition improved");
        
        // Check records were added
        auto records = patient.getMedicalHistory();
        REQUIRE(records.size() == 2);
        REQUIRE(records[0] == "Initial visit: Common cold");
        REQUIRE(records[1] == "Follow-up: Condition improved");
    }
    
    SECTION("JSON conversion (overridden toJson)") {
        // Add a medical record
        patient.addMedicalRecord("Vaccination: COVID-19");
        
        QJsonObject json = patient.toJson();
        
        // Base class fields
        REQUIRE(json["id"].toString() == "P-1234");
        REQUIRE(json["name"].toString() == "Alice Smith");
        REQUIRE(json["contactInfo"].toString() == "alice.smith@example.com");
        
        // Derived class fields
        REQUIRE(json["dateOfBirth"].toString() == "1990-05-15");
        REQUIRE(json["gender"].toString() == "Female");
        
        // Check medical history array
        QJsonArray historyArray = json["medicalHistory"].toArray();
        REQUIRE(historyArray.size() == 1);
        REQUIRE(historyArray[0].toString() == "Vaccination: COVID-19");
    }
    
    SECTION("fromJson static method") {
        QJsonObject json;
        json["id"] = "P-9876";
        json["name"] = "Carol Davis";
        json["dateOfBirth"] = "1978-08-12";
        json["gender"] = "Female";
        json["contactInfo"] = "carol.davis@example.com";
        
        QJsonArray historyArray;
        historyArray.append("Diagnosis: Hypertension");
        historyArray.append("Prescription: Beta blockers");
        json["medicalHistory"] = historyArray;
        
        Patient reconstructed = Patient::fromJson(json);
        
        REQUIRE(reconstructed.getId() == "P-9876");
        REQUIRE(reconstructed.getName() == "Carol Davis");
        REQUIRE(reconstructed.getDateOfBirth() == "1978-08-12");
        REQUIRE(reconstructed.getGender() == "Female");
        REQUIRE(reconstructed.getContactInfo() == "carol.davis@example.com");
        
        auto history = reconstructed.getMedicalHistory();
        REQUIRE(history.size() == 2);
        REQUIRE(history[0] == "Diagnosis: Hypertension");
        REQUIRE(history[1] == "Prescription: Beta blockers");
    }
}

// Test default constructor
TEST_CASE("Patient default constructor", "[patient]") {
    Patient patient;
    
    // Base class fields
    REQUIRE(patient.getId() == "");
    REQUIRE(patient.getName() == "");
    REQUIRE(patient.getContactInfo() == "");
    
    // Derived class fields
    REQUIRE(patient.getDateOfBirth() == "");
    REQUIRE(patient.getGender() == "");
    REQUIRE(patient.getMedicalHistory().size() == 0);
}

// Test polymorphism (Patient as a Person)
TEST_CASE("Patient polymorphism through Person base", "[patient][polymorphism]") {
    // Create a Patient but reference it through a Person pointer
    std::unique_ptr<Person> person = std::make_unique<Patient>(
        "P-1234", "Alice Smith", "1990-05-15", "Female", "alice.smith@example.com");
    
    // Base class getters should work
    REQUIRE(person->getId() == "P-1234");
    REQUIRE(person->getName() == "Alice Smith");
    REQUIRE(person->getContactInfo() == "alice.smith@example.com");
    
    // The overridden toJson should include patient-specific fields
    QJsonObject json = person->toJson();
    REQUIRE(json["id"].toString() == "P-1234");
    REQUIRE(json["name"].toString() == "Alice Smith");
    REQUIRE(json["contactInfo"].toString() == "alice.smith@example.com");
    REQUIRE(json["dateOfBirth"].toString() == "1990-05-15");
    REQUIRE(json["gender"].toString() == "Female");
}

// Test std::span usage in getMedicalHistory
TEST_CASE("Patient medical history span", "[patient][span]") {
    Patient patient("P-1234", "Alice Smith", "1990-05-15", "Female", "alice@example.com");
    
    // Add some medical records
    patient.addMedicalRecord("Record 1");
    patient.addMedicalRecord("Record 2");
    patient.addMedicalRecord("Record 3");
    
    // Get medical history as span
    auto records = patient.getMedicalHistory();
    
    // Verify size and content through the span
    REQUIRE(records.size() == 3);
    REQUIRE(records[0] == "Record 1");
    REQUIRE(records[1] == "Record 2");
    REQUIRE(records[2] == "Record 3");
    
    // Span should be read-only (would not compile if attempting to modify)
    // Verify we can iterate through it
    int count = 0;
    for (const auto& record : records) {
        count++;
        REQUIRE(!record.isEmpty());
    }
    REQUIRE(count == 3);
}