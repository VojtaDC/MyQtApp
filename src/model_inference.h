#pragma once

#include <opencv2/opencv.hpp>
#include <fdeep/fdeep.hpp>
#include <string>

class ModelInference {
public:
    ModelInference(const std::string& modelPath);
    ~ModelInference();
    
    // Predict using the model (returns class index or probability pair)
    std::vector<float> predict(const cv::Mat& image);
    
    // Helper to preprocess images for the model
    fdeep::tensor preprocessImage(const cv::Mat& image);

private:
    fdeep::model model_;
};