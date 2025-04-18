#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QString>
#include <QPixmap>
#include <QMessageBox>
#include <QTableView>
#include <QLineEdit>
#include <QTabWidget>
#include <opencv2/opencv.hpp>
#include "model_inference.h"
#include "usertypedialog.h"
#include "hospitaldatamanager.h"
#include "patient.h"
#include "doctor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(UserType userType, const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void runPrediction();
    
    // Hospital related slots
    void searchPatients();
    void viewPatientDetails();
    void addNewPatient();
    void updatePatientRecord();
    void addMedicalRecord();

private:
    void setupUI();
    void customizeForUserType();
    
    // Hospital UI setup methods
    void setupDoctorUI();
    void setupPatientUI();
    void setupImageAnalysisUI();
    
    // Initialize the hospital data system
    void initializeHospitalData();

    UserType userType;
    QString username;
    
    // Main UI components
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QVBoxLayout *mainLayout;
    QLabel *welcomeLabel;
    
    // Image analysis components
    QWidget *analysisWidget;
    QVBoxLayout *analysisLayout;
    QLabel *imageLabel;
    QPushButton *loadButton;
    QPushButton *predictButton;
    QLabel *resultLabel;
    
    // Doctor interface components
    QWidget *patientManagementWidget;
    QVBoxLayout *patientManagementLayout;
    QLineEdit *searchPatientEdit;
    QPushButton *searchPatientButton;
    QTableView *patientsTableView;
    QPushButton *viewPatientButton;
    QPushButton *addPatientButton;
    QPushButton *updatePatientButton;
    QPushButton *addRecordButton;
    
    // Patient interface components
    QWidget *patientProfileWidget;
    QVBoxLayout *patientProfileLayout;
    QLabel *patientInfoLabel;
    QLabel *medicalHistoryLabel;
    
    cv::Mat currentImage;
    QString currentImagePath;
    ModelInference *modelInference;
    
    // Hospital data management
    HospitalDataManager *hospitalData;
    Patient currentPatient;
    Doctor currentDoctor;
};