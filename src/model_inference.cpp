#include "model_inference.h"
#include <iostream>

ModelInference::ModelInference(const std::string& modelPath) : 
    model_(fdeep::load_model(modelPath)) // Initialize model_ in the initializer list
{
    try {
        // Model is already loaded in the initializer list, just log success
        std::cout << "Model loaded successfully from: " << modelPath << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        throw;
    }
}

ModelInference::~ModelInference() {
    // No explicit cleanup needed
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
    // Preprocess the image
    auto input = preprocessImage(image);
    
    // Run prediction with the model
    auto result = model_.predict({input});
    
    // Extract the probabilities from the result
    // Assuming the output is a 1D tensor with probabilities
    const auto& output_tensor = result.front();
    
    // Convert fdeep::tensor to std::vector<float>
    std::vector<float> probabilities;
    const auto& shape = output_tensor.shape();
    
    // Use tensor_shape_size instead of get_shape_size
    const auto size = shape.depth_ * shape.height_ * shape.width_;
    
    probabilities.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        probabilities.push_back(output_tensor.get(fdeep::tensor_pos(i)));
    }
    
    return probabilities;
}