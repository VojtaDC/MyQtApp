#include <catch2/catch_all.hpp>
#include "xraybuffer.h"
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>

// Tests for the XRayBuffer class
TEST_CASE("XRayBuffer basic functionality", "[xraybuffer]") {
    XRayBuffer buffer(3); // Buffer with max size 3
    
    SECTION("Buffer starts empty") {
        REQUIRE(buffer.isEmpty());
        REQUIRE(!buffer.isFull());
        REQUIRE(buffer.size() == 0);
    }
    
    SECTION("Push and pop operations") {
        // Create test image
        cv::Mat testImage(10, 10, CV_8UC1, cv::Scalar(255));
        
        // Push an item
        buffer.push(testImage);
        
        REQUIRE(!buffer.isEmpty());
        REQUIRE(!buffer.isFull());
        REQUIRE(buffer.size() == 1);
        
        // Push more items
        buffer.push(testImage);
        buffer.push(testImage);
        
        REQUIRE(!buffer.isEmpty());
        REQUIRE(buffer.isFull());
        REQUIRE(buffer.size() == 3);
        
        // Pop items
        auto item1 = buffer.pop();
        REQUIRE(item1.has_value());
        REQUIRE(item1->size() == testImage.size());
        
        auto item2 = buffer.pop();
        REQUIRE(item2.has_value());
        REQUIRE(item2->size() == testImage.size());
        
        auto item3 = buffer.pop();
        REQUIRE(item3.has_value());
        REQUIRE(item3->size() == testImage.size());
        
        REQUIRE(buffer.isEmpty());
        REQUIRE(!buffer.isFull());
        REQUIRE(buffer.size() == 0);
    }
    
    SECTION("Closing buffer") {
        cv::Mat testImage(10, 10, CV_8UC1, cv::Scalar(255));
        
        // Push some items
        buffer.push(testImage);
        buffer.push(testImage);
        
        // Close the buffer
        buffer.close();
        
        // Pop existing items should work
        auto item1 = buffer.pop();
        REQUIRE(item1.has_value());
        
        auto item2 = buffer.pop();
        REQUIRE(item2.has_value());
        
        // Pop on empty closed buffer should return nullopt
        auto item3 = buffer.pop();
        REQUIRE(!item3.has_value());
        
        // Push to closed buffer should not add items
        buffer.push(testImage);
        REQUIRE(buffer.isEmpty());
    }
}

TEST_CASE("XRayBuffer thread-safety", "[xraybuffer][threads]") {
    SECTION("Multiple producers and consumers") {
        XRayBuffer buffer(10);
        
        constexpr int numProducers = 3;
        constexpr int numConsumers = 2;
        constexpr int itemsPerProducer = 10;
        std::atomic<int> totalItemsProduced{0};
        std::atomic<int> totalItemsConsumed{0};
        
        // Flag to signal consumers to stop when all expected items are consumed
        std::atomic<bool> producingComplete{false};
        
        // Start producer threads
        std::vector<std::thread> producers;
        for (int p = 0; p < numProducers; ++p) {
            producers.push_back(std::thread([&, p]() {
                for (int i = 0; i < itemsPerProducer; ++i) {
                    // Create a unique test image
                    cv::Mat testImage(10, 10, CV_8UC1, cv::Scalar(p * 50 + i));
                    
                    buffer.push(testImage);
                    totalItemsProduced++;
                    
                    // Small random delay to simulate real-world conditions
                    std::this_thread::sleep_for(std::chrono::milliseconds(1 + (std::rand() % 5)));
                }
            }));
        }
        
        // Start consumer threads
        std::vector<std::thread> consumers;
        for (int c = 0; c < numConsumers; ++c) {
            consumers.push_back(std::thread([&]() {
                while (true) {
                    auto image = buffer.pop();
                    
                    if (image) {
                        // Successfully got an image
                        totalItemsConsumed++;
                        
                        // Random processing delay
                        std::this_thread::sleep_for(std::chrono::milliseconds(1 + (std::rand() % 10)));
                    } else {
                        // Buffer is closed and empty
                        break;
                    }
                    
                    // Check if we've consumed all expected items and producers are done
                    if (producingComplete && totalItemsConsumed >= totalItemsProduced) {
                        break;
                    }
                }
            }));
        }
        
        // Wait for producers to finish
        for (auto& t : producers) {
            t.join();
        }
        
        // Signal that production is complete
        producingComplete = true;
        
        // Wait a bit to ensure consumers have a chance to process remaining items
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Close the buffer to signal consumers to exit when empty
        buffer.close();
        
        // Wait for consumers to finish
        for (auto& t : consumers) {
            t.join();
        }
        
        // Verify all items were processed
        REQUIRE(totalItemsProduced == numProducers * itemsPerProducer);
        REQUIRE(totalItemsConsumed == totalItemsProduced);
        REQUIRE(buffer.isEmpty());
    }
}

TEST_CASE("XRayBuffer background processing", "[xraybuffer][async]") {
    XRayBuffer buffer(5);
    
    // Track processed images
    std::atomic<int> processedCount{0};
    std::mutex resultsMutex;
    std::vector<int> processedValues;
    
    // Start background processing
    buffer.processInBackground([&](const cv::Mat& img) {
        // Extract the value from test image (we're using the scalar value to identify images)
        int value = img.at<uint8_t>(0, 0);
        
        {
            std::lock_guard<std::mutex> lock(resultsMutex);
            processedValues.push_back(value);
        }
        
        processedCount++;
        
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    // Push images with different values
    for (int i = 1; i <= 10; ++i) {
        cv::Mat testImage(10, 10, CV_8UC1, cv::Scalar(i * 10));
        buffer.push(testImage);
    }
    
    // Wait for processing to complete
    while (processedCount < 10) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Cancel background processing
    buffer.cancelBackgroundProcessing();
    
    // Verify all images were processed
    REQUIRE(processedCount == 10);
    
    // Check that all values were processed (order might vary)
    std::lock_guard<std::mutex> lock(resultsMutex);
    REQUIRE(processedValues.size() == 10);
    
    // Sort the values for deterministic comparison
    std::sort(processedValues.begin(), processedValues.end());
    for (int i = 0; i < 10; ++i) {
        REQUIRE(processedValues[i] == (i + 1) * 10);
    }
}