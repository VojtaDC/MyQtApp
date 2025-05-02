#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>
#include <optional>
#include <memory>

// Class implementing a thread-safe producer/consumer buffer for X-ray images
class XRayBuffer {
public:
    // Buffer with configurable maximum size
    explicit XRayBuffer(size_t maxBufferSize = 5);
    ~XRayBuffer();

    // Add an image to the buffer (producer)
    void push(cv::Mat image);
    
    // Get the next image from the buffer (consumer)
    // Returns std::nullopt if buffer is closed and empty
    std::optional<cv::Mat> pop();
    
    // Close the buffer (no more items will be accepted)
    void close();
    
    // Check if buffer is empty
    bool isEmpty() const;
    
    // Check if buffer is full
    bool isFull() const;
    
    // Get current buffer size
    size_t size() const;
    
    // Process images in background with a given function
    // Returns immediately, processing happens asynchronously
    void processInBackground(std::function<void(const cv::Mat&)> processingFunction);
    
    // Cancel ongoing background processing
    void cancelBackgroundProcessing();

private:
    // Background processing thread function
    void backgroundWorker(std::function<void(const cv::Mat&)> processingFunction);
    
    mutable std::mutex m_mutex;
    std::condition_variable m_condNotEmpty;
    std::condition_variable m_condNotFull;
    std::deque<cv::Mat> m_buffer;
    size_t m_maxSize;
    std::atomic<bool> m_closed;
    
    // Background processing control - replaced std::jthread with std::thread
    std::thread m_workerThread;
    std::atomic<bool> m_stopRequested;
    bool m_threadRunning;  // Flag to track if thread is running
};