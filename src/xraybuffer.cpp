#include "xraybuffer.h"
#include <QDebug>

XRayBuffer::XRayBuffer(size_t maxBufferSize)
    : m_maxSize(maxBufferSize), m_closed(false), m_stopRequested(false), m_threadRunning(false)
{
}

XRayBuffer::~XRayBuffer()
{
    // Make sure to stop background processing and close the buffer
    cancelBackgroundProcessing();
    close();
}

void XRayBuffer::push(cv::Mat image)
{
    // Use RAII lock_guard for thread safety
    std::unique_lock<std::mutex> lock(m_mutex);
    
    // Wait until buffer is not full or closed
    m_condNotFull.wait(lock, [this]() { 
        return (m_buffer.size() < m_maxSize) || m_closed; 
    });
    
    // Don't accept new items if buffer is closed
    if (m_closed) {
        return;
    }
    
    // Add item to buffer
    m_buffer.push_back(std::move(image));
    
    // Notify consumers that buffer is not empty
    m_condNotEmpty.notify_one();
}

std::optional<cv::Mat> XRayBuffer::pop()
{
    // Use RAII unique_lock for thread safety and condition variable
    std::unique_lock<std::mutex> lock(m_mutex);
    
    // Wait until buffer is not empty or closed
    m_condNotEmpty.wait(lock, [this]() { 
        return !m_buffer.empty() || m_closed; 
    });
    
    // If buffer is empty and closed, return std::nullopt
    if (m_buffer.empty()) {
        return std::nullopt;
    }
    
    // Get item from buffer
    cv::Mat image = std::move(m_buffer.front());
    m_buffer.pop_front();
    
    // Notify producers that buffer is not full
    m_condNotFull.notify_one();
    
    return image;
}

void XRayBuffer::close()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_closed = true;
    }
    
    // Notify all waiting threads
    m_condNotEmpty.notify_all();
    m_condNotFull.notify_all();
}

bool XRayBuffer::isEmpty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_buffer.empty();
}

bool XRayBuffer::isFull() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_buffer.size() >= m_maxSize;
}

size_t XRayBuffer::size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_buffer.size();
}

void XRayBuffer::processInBackground(std::function<void(const cv::Mat&)> processingFunction)
{
    // Cancel any existing background processing
    cancelBackgroundProcessing();
    
    // Reset stop flag
    m_stopRequested = false;
    m_threadRunning = true;
    
    // Start background worker thread using std::thread (C++11/17 compatible)
    m_workerThread = std::thread([this, processingFunction]() {
        this->backgroundWorker(processingFunction);
    });
}

void XRayBuffer::cancelBackgroundProcessing()
{
    // Signal worker to stop
    m_stopRequested = true;
    
    // Wake up the worker thread if it's waiting
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Don't actually close the buffer, just notify waiting threads
    }
    m_condNotEmpty.notify_all();
    
    // Wait for thread to finish if it's running
    if (m_threadRunning && m_workerThread.joinable()) {
        m_workerThread.join();
        m_threadRunning = false;
    }
}

void XRayBuffer::backgroundWorker(std::function<void(const cv::Mat&)> processingFunction)
{
    while (!m_stopRequested) {
        // Try to get an image from the buffer
        auto optImage = pop();
        
        // Exit if buffer is closed and empty
        if (!optImage) {
            break;
        }
        
        // Process the image
        try {
            processingFunction(optImage.value());
        } catch (const std::exception& e) {
            qWarning() << "Exception in background worker:" << e.what();
        }
    }
    
    // Mark thread as no longer running
    m_threadRunning = false;
}