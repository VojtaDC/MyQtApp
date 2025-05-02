#include <catch2/catch_all.hpp>
#include "doctor.h"
#include "person.h"
#include <QJsonObject>
#include <memory>

// Tests for the Doctor derived class
TEST_CASE("Doctor class functionality", "[doctor]") {
    // Create a Doctor instance
    Doctor doctor("D-1234", "Dr. Smith", "Cardiology", "dr.smith@hospital.com");
    
    SECTION("Constructor and getters") {
        // Test base class getters
        REQUIRE(doctor.getId() == "D-1234");
        REQUIRE(doctor.getName() == "Dr. Smith");
        REQUIRE(doctor.getContactInfo() == "dr.smith@hospital.com");
        
        // Test derived class getter
        REQUIRE(doctor.getSpecialization() == "Cardiology");
    }
    
    SECTION("Setters") {
        // Test base class setters
        doctor.setId("D-5678");
        doctor.setName("Dr. Johnson");
        doctor.setContactInfo("dr.johnson@hospital.com");
        
        // Test derived class setter
        doctor.setSpecialization("Neurology");
        
        REQUIRE(doctor.getId() == "D-5678");
        REQUIRE(doctor.getName() == "Dr. Johnson");
        REQUIRE(doctor.getContactInfo() == "dr.johnson@hospital.com");
        REQUIRE(doctor.getSpecialization() == "Neurology");
    }
    
    SECTION("JSON conversion (overridden toJson)") {
        QJsonObject json = doctor.toJson();
        
        // Base class fields
        REQUIRE(json["id"].toString() == "D-1234");
        REQUIRE(json["name"].toString() == "Dr. Smith");
        REQUIRE(json["contactInfo"].toString() == "dr.smith@hospital.com");
        
        // Derived class field
        REQUIRE(json["specialization"].toString() == "Cardiology");
    }
    
    SECTION("fromJson static method") {
        QJsonObject json;
        json["id"] = "D-9876";
        json["name"] = "Dr. Williams";
        json["specialization"] = "Pediatrics";
        json["contactInfo"] = "dr.williams@hospital.com";
        
        Doctor reconstructed = Doctor::fromJson(json);
        
        REQUIRE(reconstructed.getId() == "D-9876");
        REQUIRE(reconstructed.getName() == "Dr. Williams");
        REQUIRE(reconstructed.getSpecialization() == "Pediatrics");
        REQUIRE(reconstructed.getContactInfo() == "dr.williams@hospital.com");
    }
}

// Test default constructor
TEST_CASE("Doctor default constructor", "[doctor]") {
    Doctor doctor;
    
    // Base class fields
    REQUIRE(doctor.getId() == "");
    REQUIRE(doctor.getName() == "");
    REQUIRE(doctor.getContactInfo() == "");
    
    // Derived class field
    REQUIRE(doctor.getSpecialization() == "");
}

// Test polymorphism (Doctor as a Person)
TEST_CASE("Doctor polymorphism through Person base", "[doctor][polymorphism]") {
    // Create a Doctor but reference it through a Person pointer
    std::unique_ptr<Person> person = std::make_unique<Doctor>("D-1234", "Dr. Smith", "Cardiology", "dr.smith@hospital.com");
    
    // Base class getters should work
    REQUIRE(person->getId() == "D-1234");
    REQUIRE(person->getName() == "Dr. Smith");
    REQUIRE(person->getContactInfo() == "dr.smith@hospital.com");
    
    // The overridden toJson should include the specialization
    QJsonObject json = person->toJson();
    REQUIRE(json["id"].toString() == "D-1234");
    REQUIRE(json["name"].toString() == "Dr. Smith");
    REQUIRE(json["contactInfo"].toString() == "dr.smith@hospital.com");
    REQUIRE(json["specialization"].toString() == "Cardiology");
}