#pragma once

#include <opencv2/opencv.hpp>
#include <fdeep/fdeep.hpp>
#include <string>
#include <memory>

class ModelInference {
public:
    // Constructor that takes a model path (but will not load the model for now)
    ModelInference(const std::string& modelPath);
    ~ModelInference();
    
    // Predict using the model (returns probabilities for each class)
    std::vector<float> predict(const cv::Mat& image);
    
    // Helper to preprocess images for the model
    fdeep::tensor preprocessImage(const cv::Mat& image);

private:
    // Using a unique_ptr to make the model optional
    std::unique_ptr<fdeep::model> model_;
    bool modelLoaded_;
    std::string modelPath_;
};