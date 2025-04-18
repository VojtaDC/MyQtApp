#include "mainwindow.h"
#include <QDebug>
#include <QFont>
#include <QStandardItemModel>
#include <QScrollArea>
#include <QDateTime>
#include <QInputDialog>
#include <QFormLayout>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QDialog>
#include <QHeaderView>    // voor horizontalHeader()
#include <QComboBox>      // voor QComboBox in je forms
#include <algorithm>      // voor std::max_element
#include <vector>

MainWindow::MainWindow(UserType userType, const QString &username, QWidget *parent)
    : QMainWindow(parent), userType(userType), username(username), 
      modelInference(new ModelInference("epoch_30.json")), hospitalData(new HospitalDataManager())
{
    setWindowTitle("Hospital Management System");
    setMinimumSize(800, 600);
    
    // Initialize the hospital data system
    initializeHospitalData();
    
    setupUI();
    customizeForUserType();
}

MainWindow::~MainWindow()
{
    delete modelInference;
    delete hospitalData;
}

void MainWindow::initializeHospitalData()
{
    if (!hospitalData->initialize()) {
        QMessageBox::warning(this, "Data Initialization Error",
            "Failed to initialize hospital data system. Some features may not work correctly.");
    }
    
    // Create sample data for testing if none exists
    if (hospitalData->getAllPatients().isEmpty()) {
        // Add some sample patients
        Patient patient1("P001", "John Doe", "1980-05-15", "Male", "john.doe@email.com");
        patient1.addMedicalRecord("2024-04-10: Initial consultation - Patient complains of chest pain");
        patient1.addMedicalRecord("2024-04-15: X-ray taken - Results pending");
        hospitalData->savePatient(patient1);
        
        Patient patient2("P002", "Jane Smith", "1975-10-22", "Female", "jane.smith@email.com");
        patient2.addMedicalRecord("2024-04-05: Regular checkup - Blood pressure slightly elevated");
        hospitalData->savePatient(patient2);
    }
    
    if (hospitalData->getAllDoctors().isEmpty()) {
        // Add some sample doctors
        Doctor doctor1("D001", "Dr. Robert Johnson", "Cardiology", "robert.johnson@hospital.com");
        hospitalData->saveDoctor(doctor1);
        
        Doctor doctor2("D002", "Dr. Maria Garcia", "Radiology", "maria.garcia@hospital.com");
        hospitalData->saveDoctor(doctor2);
    }
    
    // For demonstration purposes, set the current user
    if (userType == UserType::Patient) {
        currentPatient = hospitalData->getPatient("P001");
    } else {
        currentDoctor = hospitalData->getDoctor("D001");
    }
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Welcome message
    welcomeLabel = new QLabel(this);
    QFont titleFont = welcomeLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    welcomeLabel->setFont(titleFont);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    
    // Set welcome message based on user type
    QString welcomeText = (userType == UserType::Doctor) ? 
        "Welcome, Dr. " + username : "Welcome, " + username;
    welcomeLabel->setText(welcomeText);
    
    // Tab widget for different sections
    tabWidget = new QTabWidget(this);
    
    // Setup specific UIs
    setupImageAnalysisUI();
    
    if (userType == UserType::Doctor) {
        setupDoctorUI();
    } else {
        setupPatientUI();
    }
    
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addWidget(tabWidget);
}

void MainWindow::setupImageAnalysisUI()
{
    // Create widget for analysis tab
    analysisWidget = new QWidget();
    analysisLayout = new QVBoxLayout(analysisWidget);
    
    // Image display
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(400, 400);
    imageLabel->setText("No image loaded");
    imageLabel->setStyleSheet("border: 1px solid #cccccc;");
    
    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loadButton = new QPushButton("Load X-Ray Image", this);
    predictButton = new QPushButton("Analyze Image", this);
    predictButton->setEnabled(false);
    
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(predictButton);
    
    // Result label
    resultLabel = new QLabel(this);
    resultLabel->setAlignment(Qt::AlignCenter);
    QFont resultFont = resultLabel->font();
    resultFont.setPointSize(14);
    resultLabel->setFont(resultFont);
    
    // Add widgets to layout
    analysisLayout->addWidget(imageLabel);
    analysisLayout->addLayout(buttonLayout);
    analysisLayout->addWidget(resultLabel);
    
    // Add to tab widget
    tabWidget->addTab(analysisWidget, "X-Ray Analysis");
    
    // Connect signals
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(predictButton, &QPushButton::clicked, this, &MainWindow::runPrediction);
}

void MainWindow::setupDoctorUI()
{
    // Create widget for patient management tab
    patientManagementWidget = new QWidget();
    patientManagementLayout = new QVBoxLayout(patientManagementWidget);
    
    // Search area
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchPatientEdit = new QLineEdit(this);
    searchPatientEdit->setPlaceholderText("Enter patient name or ID");
    searchPatientButton = new QPushButton("Search", this);
    searchLayout->addWidget(searchPatientEdit, 1);
    searchLayout->addWidget(searchPatientButton);
    
    // Patients table view
    patientsTableView = new QTableView(this);
    patientsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    patientsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    patientsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Patient action buttons
    QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
    viewPatientButton = new QPushButton("View Details", this);
    addPatientButton = new QPushButton("Add New Patient", this);
    updatePatientButton = new QPushButton("Update Patient", this);
    addRecordButton = new QPushButton("Add Medical Record", this);
    
    actionButtonsLayout->addWidget(viewPatientButton);
    actionButtonsLayout->addWidget(addPatientButton);
    actionButtonsLayout->addWidget(updatePatientButton);
    actionButtonsLayout->addWidget(addRecordButton);
    
    // Disable buttons until a patient is selected
    viewPatientButton->setEnabled(false);
    updatePatientButton->setEnabled(false);
    addRecordButton->setEnabled(false);
    
    // Add everything to the layout
    patientManagementLayout->addLayout(searchLayout);
    patientManagementLayout->addWidget(patientsTableView, 1);
    patientManagementLayout->addLayout(actionButtonsLayout);
    
    // Add to tab widget
    tabWidget->addTab(patientManagementWidget, "Patient Management");
    
    // Set up the initial table data
    QList<Patient> patients = hospitalData->getAllPatients();
    QStandardItemModel *model = new QStandardItemModel(patients.size(), 4, this);
    model->setHorizontalHeaderLabels({"ID", "Name", "Date of Birth", "Gender"});
    
    for (int i = 0; i < patients.size(); ++i) {
        const Patient &patient = patients[i];
        model->setItem(i, 0, new QStandardItem(patient.getId()));
        model->setItem(i, 1, new QStandardItem(patient.getName()));
        model->setItem(i, 2, new QStandardItem(patient.getDateOfBirth()));
        model->setItem(i, 3, new QStandardItem(patient.getGender()));
    }
    
    patientsTableView->setModel(model);
    patientsTableView->horizontalHeader()->setStretchLastSection(true);
    patientsTableView->resizeColumnsToContents();
    
    // Connect signals
    connect(searchPatientButton, &QPushButton::clicked, this, &MainWindow::searchPatients);
    connect(viewPatientButton, &QPushButton::clicked, this, &MainWindow::viewPatientDetails);
    connect(addPatientButton, &QPushButton::clicked, this, &MainWindow::addNewPatient);
    connect(updatePatientButton, &QPushButton::clicked, this, &MainWindow::updatePatientRecord);
    connect(addRecordButton, &QPushButton::clicked, this, &MainWindow::addMedicalRecord);
    
    // Enable action buttons when a patient is selected
    connect(patientsTableView->selectionModel(), &QItemSelectionModel::selectionChanged, [this]() {
        bool hasSelection = patientsTableView->selectionModel()->hasSelection();
        viewPatientButton->setEnabled(hasSelection);
        updatePatientButton->setEnabled(hasSelection);
        addRecordButton->setEnabled(hasSelection);
    });
}

void MainWindow::setupPatientUI()
{
    // Create widget for patient profile tab
    patientProfileWidget = new QWidget();
    patientProfileLayout = new QVBoxLayout(patientProfileWidget);
    
    // Patient information
    QFont headerFont;
    headerFont.setPointSize(14);
    headerFont.setBold(true);
    
    QLabel *infoHeader = new QLabel("Patient Information", this);
    infoHeader->setFont(headerFont);
    
    patientInfoLabel = new QLabel(this);
    patientInfoLabel->setWordWrap(true);
    patientInfoLabel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    
    QLabel *historyHeader = new QLabel("Medical History", this);
    historyHeader->setFont(headerFont);
    
    medicalHistoryLabel = new QLabel(this);
    medicalHistoryLabel->setWordWrap(true);
    medicalHistoryLabel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    
    // Use scroll area for medical history
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(medicalHistoryLabel);
    
    // Add widgets to layout
    patientProfileLayout->addWidget(infoHeader);
    patientProfileLayout->addWidget(patientInfoLabel);
    patientProfileLayout->addSpacing(20);
    patientProfileLayout->addWidget(historyHeader);
    patientProfileLayout->addWidget(scrollArea, 1);
    
    // Add to tab widget
    tabWidget->addTab(patientProfileWidget, "My Profile");
    
    // Display current patient information
    if (!currentPatient.getId().isEmpty()) {
        QString infoText = "<p><b>Name:</b> " + currentPatient.getName() + "</p>";
        infoText += "<p><b>ID:</b> " + currentPatient.getId() + "</p>";
        infoText += "<p><b>Date of Birth:</b> " + currentPatient.getDateOfBirth() + "</p>";
        infoText += "<p><b>Gender:</b> " + currentPatient.getGender() + "</p>";
        infoText += "<p><b>Contact:</b> " + currentPatient.getContactInfo() + "</p>";
        
        patientInfoLabel->setText(infoText);
        
        QString historyText = "";
        QList<QString> medicalHistory = currentPatient.getMedicalHistory();
        if (medicalHistory.isEmpty()) {
            historyText = "No medical records available.";
        } else {
            for (const QString &record : medicalHistory) {
                historyText += "<p>• " + record + "</p>";
            }
        }
        
        medicalHistoryLabel->setText(historyText);
    } else {
        patientInfoLabel->setText("Patient information not available.");
        medicalHistoryLabel->setText("No medical records available.");
    }
}

void MainWindow::customizeForUserType()
{
    // Set window title based on user type
    if (userType == UserType::Doctor) {
        setWindowTitle("Hospital Management System - Doctor Dashboard");
    } else {
        setWindowTitle("Hospital Management System - Patient Dashboard");
    }
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open X-Ray Image",
                                                   "", "Image Files (*.png *.jpg *.jpeg)");
    
    if (!fileName.isEmpty()) {
        currentImagePath = fileName;
        currentImage = cv::imread(fileName.toStdString());
        
        if (!currentImage.empty()) {
            // Convert to QPixmap and display
            cv::Mat rgbImage;
            cv::cvtColor(currentImage, rgbImage, cv::COLOR_BGR2RGB);
            
            QImage qimg(rgbImage.data, rgbImage.cols, rgbImage.rows, 
                       rgbImage.step, QImage::Format_RGB888);
            
            QPixmap pixmap = QPixmap::fromImage(qimg);
            
            // Scale pixmap while keeping aspect ratio
            pixmap = pixmap.scaled(imageLabel->width(), imageLabel->height(),
                                  Qt::KeepAspectRatio, Qt::SmoothTransformation);
            
            imageLabel->setPixmap(pixmap);
            
            // Enable prediction button
            predictButton->setEnabled(true);
        } else {
            QMessageBox::warning(this, "Image Loading Error",
                               "Failed to load the selected image.");
        }
    }
}

void MainWindow::runPrediction()
{
    if (currentImage.empty()) {
        QMessageBox::warning(this, "Error", "No image loaded to analyze");
        return;
    }
    
    // Perform prediction
    try {
        std::vector<float> probabilities = modelInference->predict(currentImage);
        
        // Find the class with highest probability
        auto max_it = std::max_element(probabilities.begin(), probabilities.end());
        size_t max_idx = std::distance(probabilities.begin(), max_it);
        float confidence = *max_it * 100.0f;
        
        // Classes corresponding to the model output
        std::string classes[] = {"Normal", "COVID-19", "Viral Pneumonia", "Lung Opacity"};
        std::string result = classes[max_idx] + " (Confidence: " + 
                             std::to_string(static_cast<int>(confidence)) + "%)";
        
        resultLabel->setText("Analysis Result: " + QString::fromStdString(result));
        
        // Add to medical records if appropriate
        if (userType == UserType::Doctor) {
            // Get selected patient if any
            QModelIndexList selection = patientsTableView->selectionModel()->selectedRows();
            if (!selection.isEmpty()) {
                QModelIndex index = selection.at(0);
                QString patientId = index.sibling(index.row(), 0).data().toString();
                
                // Confirmation dialog
                QMessageBox::StandardButton reply = QMessageBox::question(this, 
                    "Add to Medical Record", 
                    "Would you like to add this analysis result to the patient's medical record?",
                    QMessageBox::Yes | QMessageBox::No);
                
                if (reply == QMessageBox::Yes) {
                    Patient patient = hospitalData->getPatient(patientId);
                    QString record = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") + 
                                   ": X-Ray Analysis: " + QString::fromStdString(result);
                    patient.addMedicalRecord(record);
                    hospitalData->savePatient(patient);
                    
                    QMessageBox::information(this, "Record Added", 
                                          "Analysis result added to patient's medical record.");
                }
            }
        }
    } catch (const std::exception &e) {
        resultLabel->setText("Error: Failed to analyze the image");
        QMessageBox::critical(this, "Analysis Error", 
                            QString("An error occurred during analysis: %1").arg(e.what()));
    }
}

void MainWindow::searchPatients()
{
    QString searchTerm = searchPatientEdit->text().trimmed();
    
    if (searchTerm.isEmpty()) {
        // If search term is empty, show all patients
        QList<Patient> patients = hospitalData->getAllPatients();
        QStandardItemModel *model = new QStandardItemModel(patients.size(), 4, this);
        model->setHorizontalHeaderLabels({"ID", "Name", "Date of Birth", "Gender"});
        
        for (int i = 0; i < patients.size(); ++i) {
            const Patient &patient = patients[i];
            model->setItem(i, 0, new QStandardItem(patient.getId()));
            model->setItem(i, 1, new QStandardItem(patient.getName()));
            model->setItem(i, 2, new QStandardItem(patient.getDateOfBirth()));
            model->setItem(i, 3, new QStandardItem(patient.getGender()));
        }
        
        patientsTableView->setModel(model);
    } else {
        // Search for matching patients
        QList<Patient> matchingPatients = hospitalData->searchPatients(searchTerm);
        
        QStandardItemModel *model = new QStandardItemModel(matchingPatients.size(), 4, this);
        model->setHorizontalHeaderLabels({"ID", "Name", "Date of Birth", "Gender"});
        
        for (int i = 0; i < matchingPatients.size(); ++i) {
            const Patient &patient = matchingPatients[i];
            model->setItem(i, 0, new QStandardItem(patient.getId()));
            model->setItem(i, 1, new QStandardItem(patient.getName()));
            model->setItem(i, 2, new QStandardItem(patient.getDateOfBirth()));
            model->setItem(i, 3, new QStandardItem(patient.getGender()));
        }
        
        patientsTableView->setModel(model);
    }
    
    patientsTableView->resizeColumnsToContents();
}

void MainWindow::viewPatientDetails()
{
    // Get selected patient
    QModelIndexList selection = patientsTableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a patient first");
        return;
    }
    
    QModelIndex index = selection.at(0);
    QString patientId = index.sibling(index.row(), 0).data().toString();
    
    Patient patient = hospitalData->getPatient(patientId);
    if (patient.getId().isEmpty()) {
        QMessageBox::warning(this, "Error", "Patient not found");
        return;
    }
    
    // Create and display patient details dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Patient Details - " + patient.getName());
    dialog.setMinimumSize(500, 400);
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    
    // Patient information
    QFont headerFont;
    headerFont.setPointSize(14);
    headerFont.setBold(true);
    
    QLabel *infoHeader = new QLabel("Patient Information", &dialog);
    infoHeader->setFont(headerFont);
    
    QString infoText = "<p><b>Name:</b> " + patient.getName() + "</p>";
    infoText += "<p><b>ID:</b> " + patient.getId() + "</p>";
    infoText += "<p><b>Date of Birth:</b> " + patient.getDateOfBirth() + "</p>";
    infoText += "<p><b>Gender:</b> " + patient.getGender() + "</p>";
    infoText += "<p><b>Contact:</b> " + patient.getContactInfo() + "</p>";
    
    QLabel *infoLabel = new QLabel(infoText, &dialog);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    
    QLabel *historyHeader = new QLabel("Medical History", &dialog);
    historyHeader->setFont(headerFont);
    
    QString historyText = "";
    QList<QString> medicalHistory = patient.getMedicalHistory();
    if (medicalHistory.isEmpty()) {
        historyText = "No medical records available.";
    } else {
        for (const QString &record : medicalHistory) {
            historyText += "<p>• " + record + "</p>";
        }
    }
    
    QLabel *historyLabel = new QLabel(historyText, &dialog);
    historyLabel->setWordWrap(true);
    historyLabel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(historyLabel);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    
    dialogLayout->addWidget(infoHeader);
    dialogLayout->addWidget(infoLabel);
    dialogLayout->addSpacing(20);
    dialogLayout->addWidget(historyHeader);
    dialogLayout->addWidget(scrollArea, 1);
    dialogLayout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    dialog.exec();
}

void MainWindow::addNewPatient()
{
    // Create dialog for adding new patient
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Patient");
    dialog.setMinimumWidth(400);
    
    QFormLayout *formLayout = new QFormLayout();
    
    QLineEdit *nameEdit = new QLineEdit(&dialog);
    QLineEdit *dobEdit = new QLineEdit(&dialog);
    dobEdit->setPlaceholderText("YYYY-MM-DD");
    
    QComboBox *genderCombo = new QComboBox(&dialog);
    genderCombo->addItems({"Male", "Female", "Other"});
    
    QLineEdit *contactEdit = new QLineEdit(&dialog);
    
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Date of Birth:", dobEdit);
    formLayout->addRow("Gender:", genderCombo);
    formLayout->addRow("Contact Info:", contactEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        // Validate inputs
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Validation Error", "Patient name is required");
            return;
        }
        
        // Create and save new patient
        Patient newPatient;
        newPatient.setName(nameEdit->text().trimmed());
        newPatient.setDateOfBirth(dobEdit->text().trimmed());
        newPatient.setGender(genderCombo->currentText());
        newPatient.setContactInfo(contactEdit->text().trimmed());
        
        if (hospitalData->savePatient(newPatient)) {
            QMessageBox::information(&dialog, "Success", "Patient added successfully");
            dialog.accept();
            
            // Refresh patients table
            searchPatients();
        } else {
            QMessageBox::critical(&dialog, "Error", "Failed to add patient");
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    dialog.exec();
}

void MainWindow::updatePatientRecord()
{
    // Get selected patient
    QModelIndexList selection = patientsTableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a patient first");
        return;
    }
    
    QModelIndex index = selection.at(0);
    QString patientId = index.sibling(index.row(), 0).data().toString();
    
    Patient patient = hospitalData->getPatient(patientId);
    if (patient.getId().isEmpty()) {
        QMessageBox::warning(this, "Error", "Patient not found");
        return;
    }
    
    // Create dialog for updating patient
    QDialog dialog(this);
    dialog.setWindowTitle("Update Patient");
    dialog.setMinimumWidth(400);
    
    QFormLayout *formLayout = new QFormLayout();
    
    QLineEdit *nameEdit = new QLineEdit(patient.getName(), &dialog);
    QLineEdit *dobEdit = new QLineEdit(patient.getDateOfBirth(), &dialog);
    
    QComboBox *genderCombo = new QComboBox(&dialog);
    genderCombo->addItems({"Male", "Female", "Other"});
    genderCombo->setCurrentText(patient.getGender());
    
    QLineEdit *contactEdit = new QLineEdit(patient.getContactInfo(), &dialog);
    
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Date of Birth:", dobEdit);
    formLayout->addRow("Gender:", genderCombo);
    formLayout->addRow("Contact Info:", contactEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        // Validate inputs
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Validation Error", "Patient name is required");
            return;
        }
        
        // Update patient
        patient.setName(nameEdit->text().trimmed());
        patient.setDateOfBirth(dobEdit->text().trimmed());
        patient.setGender(genderCombo->currentText());
        patient.setContactInfo(contactEdit->text().trimmed());
        
        if (hospitalData->savePatient(patient)) {
            QMessageBox::information(&dialog, "Success", "Patient updated successfully");
            dialog.accept();
            
            // Refresh patients table
            searchPatients();
        } else {
            QMessageBox::critical(&dialog, "Error", "Failed to update patient");
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    dialog.exec();
}

void MainWindow::addMedicalRecord()
{
    // Get selected patient
    QModelIndexList selection = patientsTableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a patient first");
        return;
    }
    
    QModelIndex index = selection.at(0);
    QString patientId = index.sibling(index.row(), 0).data().toString();
    QString patientName = index.sibling(index.row(), 1).data().toString();
    
    Patient patient = hospitalData->getPatient(patientId);
    if (patient.getId().isEmpty()) {
        QMessageBox::warning(this, "Error", "Patient not found");
        return;
    }
    
    // Create dialog for adding medical record
    QDialog dialog(this);
    dialog.setWindowTitle("Add Medical Record - " + patientName);
    dialog.setMinimumSize(500, 300);
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    
    QLabel *dateLabel = new QLabel("Date: " + QDateTime::currentDateTime().toString("yyyy-MM-dd"), &dialog);
    QLabel *instructionLabel = new QLabel("Enter medical record details:", &dialog);
    
    QTextEdit *recordEdit = new QTextEdit(&dialog);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    
    dialogLayout->addWidget(dateLabel);
    dialogLayout->addWidget(instructionLabel);
    dialogLayout->addWidget(recordEdit, 1);
    dialogLayout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        // Validate inputs
        if (recordEdit->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Validation Error", "Medical record cannot be empty");
            return;
        }
        
        // Add medical record
        QString recordText = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") + 
                           ": " + recordEdit->toPlainText().trimmed();
        
        patient.addMedicalRecord(recordText);
        
        if (hospitalData->savePatient(patient)) {
            QMessageBox::information(&dialog, "Success", "Medical record added successfully");
            dialog.accept();
        } else {
            QMessageBox::critical(&dialog, "Error", "Failed to add medical record");
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    dialog.exec();
}