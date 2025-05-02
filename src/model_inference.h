#pragma once

#include <opencv2/opencv.hpp>
#include <fdeep/fdeep.hpp>
#include <string>
#include <memory>
#include <mutex>
#include <future>
#include <functional>
#include <QObject>
#include <QString>
#include "xraybuffer.h"

// ModelInference class that handles X-ray image analysis with background processing
class ModelInference : public QObject {
    Q_OBJECT

public:
    // Constructor that takes a model path
    ModelInference(const std::string& modelPath, QObject* parent = nullptr);
    ~ModelInference();
    
    // Predict using the model (synchronous version)
    std::vector<float> predict(const cv::Mat& image);
    
    // Predict asynchronously, will emit predictionCompleted when done
    void predictAsync(const cv::Mat& image);
    
    // Queue an image for background processing
    void queueImage(const cv::Mat& image);
    
    // Helper to preprocess images for the model
    fdeep::tensor preprocessImage(const cv::Mat& image);
    
    // Load model in background thread
    void loadModelAsync();
    
    // Get model loading status
    bool isModelLoaded() const;

signals:
    // Signal emitted when async prediction is complete
    void predictionCompleted(const std::vector<float>& probabilities);
    
    // Signal emitted to report progress
    void progressUpdated(int percentage);
    
    // Signal emitted when model loading completes
    void modelLoadingCompleted(bool success);

private:
    // Using a unique_ptr to make the model optional
    std::unique_ptr<fdeep::model> model_;
    mutable std::mutex m_modelMutex;
    std::atomic<bool> m_modelLoaded;
    std::string m_modelPath;
    
    // Buffer for background processing
    std::unique_ptr<XRayBuffer> m_imageBuffer;
    
    // Background tasks
    std::future<void> m_loadingFuture;
    
    // Process images from the buffer in background
    void processImagesInBackground();
    
    // Actual model loading logic
    bool loadModel();
};