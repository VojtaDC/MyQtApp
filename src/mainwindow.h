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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void runPrediction();

private:
    void setupUI();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QLabel *imageLabel;
    QPushButton *loadButton;
    QPushButton *predictButton;
    QLabel *resultLabel;

    cv::Mat currentImage;
    QString currentImagePath;
    ModelInference *modelInference;
};