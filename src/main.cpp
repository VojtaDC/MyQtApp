#include <iostream>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char** argv) {
    try {
        // Initialize Qt application
        QApplication app(argc, argv);
        
        // Create and show the main window
        MainWindow mainWindow;
        mainWindow.show();
        
        // Start the Qt event loop
        return app.exec();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}