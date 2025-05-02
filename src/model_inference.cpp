#include "model_inference.h"
#include <iostream>
#include <QDebug>
#include <thread>
#include <chrono>

ModelInference::ModelInference(const std::string& modelPath, QObject* parent) 
    : QObject(parent),
      model_(nullptr),
      m_modelLoaded(false),
      m_modelPath(modelPath),
      m_imageBuffer(std::make_unique<XRayBuffer>(10))  // Buffer for 10 images
{
    qDebug() << "ModelInference created with model path:" << QString::fromStdString(modelPath);
}

ModelInference::~ModelInference()
{
    // Cancel any background processing
    if (m_imageBuffer) {
        m_imageBuffer->cancelBackgroundProcessing();
    }
    
    // Wait for any async loading to complete
    if (m_loadingFuture.valid()) {
        m_loadingFuture.wait();
    }
}

bool ModelInference::isModelLoaded() const
{
    return m_modelLoaded.load();
}

void ModelInference::loadModelAsync()
{
    // Don't start loading if already in progress or loaded
    if (m_loadingFuture.valid() || m_modelLoaded) {
        return;
    }
    
    // Start loading in a background thread
    m_loadingFuture = std::async(std::launch::async, [this]() {
        bool success = this->loadModel();
        emit modelLoadingCompleted(success);
        return;
    });
}

bool ModelInference::loadModel()
{
    try {
        qDebug() << "Loading model from path:" << QString::fromStdString(m_modelPath);
        
        // Progress reporting
        for (int i = 0; i <= 100; i += 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate load time
            emit progressUpdated(i);
        }
        
        // When real model loading is implemented, replace this with actual code:
        // std::unique_ptr<fdeep::model> loadedModel = 
        //     std::make_unique<fdeep::model>(fdeep::load_model(m_modelPath));
        
        // For now, we'll skip actual loading and use dummy data
        qDebug() << "Model loaded successfully (simulated)";
        
        // Update model loaded state
        {
            std::lock_guard<std::mutex> lock(m_modelMutex);
            // Set model_ to a dummy value here if needed
            m_modelLoaded = true;
        }
        
        // Start processing images in background
        processImagesInBackground();
        
        return true;
    } catch (const std::exception& e) {
        qWarning() << "Error loading model:" << e.what();
        m_modelLoaded = false;
        return false;
    }
}

fdeep::tensor ModelInference::preprocessImage(const cv::Mat& image) {
    // Resize image to match model input dimensions
    cv::Mat resizedImg;
    cv::resize(image, resizedImg, cv::Size(128, 128));
    
    // Convert to the format needed by the model
    cv::Mat processedImg;
    if(image.channels() == 3) {
        cv::cvtColor(resizedImg, processedImg, cv::COLOR_BGR2GRAY);
    } else {
        processedImg = resizedImg.clone();
    }
    
    // Convert OpenCV Mat to fdeep::tensor and scale pixel values from [0, 255] to [0, 1]
    return fdeep::tensor_from_bytes(
        processedImg.ptr(),
        static_cast<std::size_t>(processedImg.rows),
        static_cast<std::size_t>(processedImg.cols),
        static_cast<std::size_t>(processedImg.channels()),
        0.0f, 1.0f);
}

std::vector<float> ModelInference::predict(const cv::Mat& image) {
    // Check if model is loaded
    if (!m_modelLoaded) {
        qDebug() << "Model not loaded, returning dummy predictions";
        
        // Create dummy probabilities
        std::vector<float> probabilities = {0.05f, 0.85f, 0.07f, 0.03f};
        
        // Find the class with highest probability for logging
        auto max_it = std::max_element(probabilities.begin(), probabilities.end());
        size_t max_idx = std::distance(probabilities.begin(), max_it);
        
        // Print the "predicted" class for demo purposes
        std::string classes[] = {"Normal", "COVID-19", "Viral Pneumonia", "Lung Opacity"};
        qDebug() << "Dummy prediction:" << QString::fromStdString(classes[max_idx])
                 << "(Confidence:" << *max_it * 100.0f << "%)";
        
        return probabilities;
    }
    
    // Thread-safe access to the model
    std::lock_guard<std::mutex> lock(m_modelMutex);
    
    try {
        // Real prediction logic would be:
        // auto input = preprocessImage(image);
        // auto result = model_->predict({input});
        // const auto& output_tensor = result.front();
        // 
        // std::vector<float> probabilities;
        // const auto& shape = output_tensor.shape();
        // const auto size = shape.depth_ * shape.height_ * shape.width_;
        // 
        // probabilities.reserve(size);
        // for (std::size_t i = 0; i < size; ++i) {
        //     probabilities.push_back(output_tensor.get(fdeep::tensor_pos(i)));
        // }
        
        // For now, return simulated results
        std::vector<float> probabilities = {0.80f, 0.05f, 0.10f, 0.05f};
        return probabilities;
    } catch (const std::exception& e) {
        qWarning() << "Error during prediction:" << e.what();
        return std::vector<float>{0.25f, 0.25f, 0.25f, 0.25f}; // Equal probabilities on error
    }
}

void ModelInference::predictAsync(const cv::Mat& image) {
    // Create a copy of the image to ensure it remains valid in the other thread
    cv::Mat imageCopy = image.clone();
    
    // Process asynchronously
    std::thread([this, imageCopy = std::move(imageCopy)]() mutable {
        std::vector<float> result = this->predict(imageCopy);
        emit predictionCompleted(result);
    }).detach();
}

void ModelInference::queueImage(const cv::Mat& image) {
    if (m_imageBuffer) {
        // Push a copy of the image to the buffer
        m_imageBuffer->push(image.clone());
    }
}

void ModelInference::processImagesInBackground() {
    if (!m_imageBuffer) return;
    
    // Set up a processing function that will emit results when done
    auto processingFunction = [this](const cv::Mat& img) {
        std::vector<float> probabilities = this->predict(img);
        emit predictionCompleted(probabilities);
    };
    
    // Start processing in background
    m_imageBuffer->processInBackground(processingFunction);
}
