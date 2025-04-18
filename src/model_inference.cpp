#include "model_inference.h"
#include <iostream>

ModelInference::ModelInference(const std::string& modelPath) 
    : model_(nullptr), modelLoaded_(false), modelPath_(modelPath)
{
    std::cout << "Model loading temporarily disabled. Will implement later with model path: " << modelPath << std::endl;
    // Note: the model_ unique_ptr is initialized to nullptr, so no model is loaded
}

ModelInference::~ModelInference()
{
    // No explicit cleanup needed - unique_ptr will handle deletion
}

fdeep::tensor ModelInference::preprocessImage(const cv::Mat& image) {
    // Resize image to match model input dimensions (assuming 128x128 from the tutorial)
    cv::Mat resizedImg;
    cv::resize(image, resizedImg, cv::Size(128, 128));
    
    // Convert to the format needed by the model
    // Note: Adjust channels, normalization values based on your specific model requirements
    
    // If your model expects grayscale images:
    cv::Mat processedImg;
    if(image.channels() == 3) {
        cv::cvtColor(resizedImg, processedImg, cv::COLOR_BGR2GRAY);
    } else {
        processedImg = resizedImg.clone();
    }
    
    // Convert OpenCV Mat to fdeep::tensor
    // Scale pixel values from [0, 255] to [0, 1]
    return fdeep::tensor_from_bytes(
        processedImg.ptr(),
        static_cast<std::size_t>(processedImg.rows),
        static_cast<std::size_t>(processedImg.cols),
        static_cast<std::size_t>(processedImg.channels()),
        0.0f, 1.0f);
}

std::vector<float> ModelInference::predict(const cv::Mat& image) {
    // If model loading is disabled, return dummy predictions
    if (!modelLoaded_ || !model_) {
        // For COVID-19 classifications, common classes might be:
        // Normal, COVID-19, Viral Pneumonia, Lung Opacity
        std::cout << "Using dummy prediction values since model loading is disabled" << std::endl;
        
        // Create dummy probabilities for the classes
        // Simulate as if the model detected "COVID-19" with high probability
        std::vector<float> probabilities = {0.05f, 0.85f, 0.07f, 0.03f};
        
        // Find the class with highest probability
        auto max_it = std::max_element(probabilities.begin(), probabilities.end());
        size_t max_idx = std::distance(probabilities.begin(), max_it);
        
        // Print the "predicted" class for demo purposes
        std::string classes[] = {"Normal", "COVID-19", "Viral Pneumonia", "Lung Opacity"};
        std::cout << "Dummy prediction result: " << classes[max_idx] 
                  << " (Confidence: " << *max_it * 100.0f << "%)" << std::endl;
        
        return probabilities;
    }
    
    // This code will never run until model loading is re-enabled
    auto input = preprocessImage(image);
    auto result = model_->predict({input});
    const auto& output_tensor = result.front();
    
    std::vector<float> probabilities;
    const auto& shape = output_tensor.shape();
    const auto size = shape.depth_ * shape.height_ * shape.width_;
    
    probabilities.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        probabilities.push_back(output_tensor.get(fdeep::tensor_pos(i)));
    }
    
    return probabilities;
}
