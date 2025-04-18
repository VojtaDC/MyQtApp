#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include "model_inference.h"
#include "hospitaldatamanager.h"
#include "usertypedialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(UserType userType, const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Image analysis slots
    void on_loadButton_clicked();
    void on_predictButton_clicked();
    
    // Patient management slots (for doctors)
    void on_searchPatientButton_clicked();
    void on_viewPatientButton_clicked();
    void on_addPatientButton_clicked();
    void on_updatePatientButton_clicked();
    void on_addRecordButton_clicked();
    
    // Selection handling
    void onPatientSelectionChanged();
    
    // Menu actions
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    
    void initializeHospitalData();
    void customizeForUserType();
    void setupPatientTable();
    
    // Patient profile display
    void displayPatientProfile();
    
    // User information
    UserType userType;
    QString username;
    
    // Patient and doctor data
    Patient currentPatient;
    Doctor currentDoctor;
    
    // Image processing
    cv::Mat currentImage;
    QString currentImagePath;
    
    // Services
    ModelInference *modelInference;
    HospitalDataManager *hospitalData;
};