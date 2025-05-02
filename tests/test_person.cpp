#include <catch2/catch_all.hpp>
#include "person.h"
#include <QJsonObject>

// Tests for the Person base class
TEST_CASE("Person base class functionality", "[person]") {
    // Create a Person instance
    Person person("P-1234", "John Doe", "john.doe@example.com");
    
    SECTION("Constructor and getters") {
        REQUIRE(person.getId() == "P-1234");
        REQUIRE(person.getName() == "John Doe");
        REQUIRE(person.getContactInfo() == "john.doe@example.com");
    }
    
    SECTION("Setters") {
        person.setId("P-5678");
        person.setName("Jane Doe");
        person.setContactInfo("jane.doe@example.com");
        
        REQUIRE(person.getId() == "P-5678");
        REQUIRE(person.getName() == "Jane Doe");
        REQUIRE(person.getContactInfo() == "jane.doe@example.com");
    }
    
    SECTION("JSON conversion") {
        QJsonObject json = person.toJson();
        
        REQUIRE(json["id"].toString() == "P-1234");
        REQUIRE(json["name"].toString() == "John Doe");
        REQUIRE(json["contactInfo"].toString() == "john.doe@example.com");
    }
}

// Test default constructor
TEST_CASE("Person default constructor", "[person]") {
    Person person;
    
    REQUIRE(person.getId() == "");
    REQUIRE(person.getName() == "");
    REQUIRE(person.getContactInfo() == "");
}

// Test polymorphism with virtual functions 
TEST_CASE("Person virtual function behavior", "[person]") {
    // Create a Person pointer to a Person object
    std::unique_ptr<Person> person = std::make_unique<Person>("P-1234", "John Doe", "john.doe@example.com");
    
    // Virtual toJson function should work through pointers
    QJsonObject json = person->toJson();
    
    REQUIRE(json["id"].toString() == "P-1234");
    REQUIRE(json["name"].toString() == "John Doe");
    REQUIRE(json["contactInfo"].toString() == "john.doe@example.com");
    
    // Test that virtual destructor properly cleans up
    // This is implicitly tested when the unique_ptr goes out of scope
}