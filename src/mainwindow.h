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
#include <opencv2/opencv.hpp>
#include "model_inference.h"
#include "usertypedialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(UserType userType, const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void runPrediction();

private:
    void setupUI();
    void customizeForUserType();

    UserType userType;
    QString username;
    
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QLabel *welcomeLabel;
    QLabel *imageLabel;
    QPushButton *loadButton;
    QPushButton *predictButton;
    QLabel *resultLabel;

    cv::Mat currentImage;
    QString currentImagePath;
    ModelInference *modelInference;
};