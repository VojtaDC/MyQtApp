#include "mainwindow.h"
#include <QDebug>
#include <QFont>

MainWindow::MainWindow(UserType userType, const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , modelInference(nullptr)
    , userType(userType)
    , username(username)
{
    setupUI();
    customizeForUserType();
    
    // Initialize the model - COMMENTED OUT TO TEMPORARILY DISABLE JSON LOADING
    /*
    try {
        modelInference = new ModelInference("../epoch_30.json");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load model: %1").arg(e.what()));
    }
    */
    
    // Display a message that model loading is disabled
    resultLabel->setText("Model loading is temporarily disabled");
}

MainWindow::~MainWindow()
{
    delete modelInference;
}

void MainWindow::setupUI()
{
    // Create central widget and layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Welcome label
    welcomeLabel = new QLabel("", this);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(14);
    welcomeFont.setBold(true);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    
    // Image display
    imageLabel = new QLabel(this);
    imageLabel->setMinimumSize(400, 400);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("No image loaded");
    imageLabel->setStyleSheet("QLabel { background-color : #f0f0f0; }");
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loadButton = new QPushButton("Load Image", this);
    predictButton = new QPushButton("Run Prediction", this);
    predictButton->setEnabled(false);  // Disable until image is loaded
    
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(predictButton);
    
    // Result label
    resultLabel = new QLabel("Load an image and run prediction", this);
    resultLabel->setAlignment(Qt::AlignCenter);
    
    // Add widgets to main layout
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(resultLabel);
    
    // Connect signals
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(predictButton, &QPushButton::clicked, this, &MainWindow::runPrediction);
    
    // Window settings
    setWindowTitle("COVID-19 X-Ray Classification");
    resize(600, 550);
}

void MainWindow::customizeForUserType()
{
    // Adjust UI based on user type
    if (userType == UserType::Doctor) {
        welcomeLabel->setText(QString("Welcome Dr. %1 - Doctor Mode").arg(username));
        welcomeLabel->setStyleSheet("QLabel { color: #0066cc; }");
        
        // Doctors have full access to all features
        loadButton->setEnabled(true);
        predictButton->setText("Diagnose");
        
    } else if (userType == UserType::Patient) {
        welcomeLabel->setText(QString("Welcome %1 - Patient Mode").arg(username));
        welcomeLabel->setStyleSheet("QLabel { color: #009933; }");
        
        // Patients may have limited functionality
        loadButton->setEnabled(true); 
        // We could disable certain features for patients if needed
    }
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    
    if (!fileName.isEmpty()) {
        currentImagePath = fileName;
        
        // Load image with OpenCV
        currentImage = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
        
        if (!currentImage.empty()) {
            // Convert to Qt format for display
            cv::Mat rgbImage;
            cv::cvtColor(currentImage, rgbImage, cv::COLOR_BGR2RGB);
            
            // Qt6 changes: Use QImage constructor with bits per line instead of step
            QImage qImage(
                rgbImage.data,
                rgbImage.cols,
                rgbImage.rows,
                static_cast<int>(rgbImage.step),
                QImage::Format_RGB888
            );
            
            // Display image
            QPixmap pixmap = QPixmap::fromImage(qImage);
            imageLabel->setPixmap(pixmap.scaled(
                imageLabel->width(), 
                imageLabel->height(), 
                Qt::KeepAspectRatio, 
                Qt::SmoothTransformation
            ));
            
            // Enable prediction
            predictButton->setEnabled(true);
            
            // Show different message based on user type
            if (userType == UserType::Doctor) {
                resultLabel->setText("Ready for diagnosis");
            } else {
                resultLabel->setText("Ready for analysis");
            }
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open image!"));
        }
    }
}

void MainWindow::runPrediction()
{
    if (currentImage.empty()) {
        QMessageBox::warning(this, tr("Error"), tr("No image loaded!"));
        return;
    }
    
    // Display a message since model loading is disabled
    QString message = "Model prediction is temporarily disabled.\n"
                      "The image was loaded successfully, but the model needs to be fixed.\n"
                      "Sample classes: COVID, Lung_Opacity, Normal, Viral Pneumonia";
                      
    // Add user-type specific message
    if (userType == UserType::Doctor) {
        message += "\n\nDoctor Mode: Detailed diagnostic information would be shown here.";
    } else {
        message += "\n\nPatient Mode: Simplified report would be shown here.";
    }
    
    resultLabel->setText(message);
    
    /*
    // Original code - commented out
    if (currentImage.empty() || !modelInference) {
        QMessageBox::warning(this, tr("Error"), tr("No image loaded or model not initialized!"));
        return;
    }
    
    try {
        // Get predictions
        std::vector<float> predictions = modelInference->predict(currentImage);
        
        if (predictions.empty()) {
            resultLabel->setText("Error: Model returned empty predictions");
            return;
        }
        
        // Find the class with highest probability
        size_t maxClass = 0;
        float maxProb = predictions[0];
        for (size_t i = 1; i < predictions.size(); ++i) {
            if (predictions[i] > maxProb) {
                maxProb = predictions[i];
                maxClass = i;
            }
        }
        
        // Class labels
        std::vector<std::string> classLabels = {"COVID", "Lung_Opacity", "Normal", "Viral Pneumonia"};
        QString resultText;
        
        if (maxClass < classLabels.size()) {
            resultText = QString("Predicted class: %1 (Confidence: %2%)").arg(
                QString::fromStdString(classLabels[maxClass])).arg(
                QString::number(maxProb * 100.0, 'f', 2)
            );
            
            // Display all probabilities
            resultText += "\n\nProbabilities:\n";
            for (size_t i = 0; i < predictions.size() && i < classLabels.size(); ++i) {
                resultText += QString("%1: %2%\n").arg(
                    QString::fromStdString(classLabels[i])).arg(
                    QString::number(predictions[i] * 100.0, 'f', 2)
                );
            }
        } else {
            resultText = "Unexpected model output format";
        }
        
        // Update result label
        resultLabel->setText(resultText);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Error"), tr("Prediction failed: %1").arg(e.what()));
    }
    */
}