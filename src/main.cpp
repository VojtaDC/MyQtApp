#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "usertypedialog.h"
#include "logindialog.h"

int main(int argc, char** argv) {
    try {
        // Initialize Qt application
        QApplication app(argc, argv);
        
        // First, show the user type selection dialog
        UserTypeDialog userTypeDialog;
        if (userTypeDialog.exec() != QDialog::Accepted) {
            // User cancelled the dialog
            return 0;
        }
        
        // Get the selected user type
        UserType selectedType = userTypeDialog.getUserType();
        
        // Then show the login dialog with the selected user type
        LoginDialog loginDialog(selectedType);
        if (loginDialog.exec() != QDialog::Accepted) {
            // User cancelled the login
            return 0;
        }
        
        // If login was successful, get the username and show the main window
        QString username = loginDialog.getUsername();
        
        // Create and show the main window with user type and username
        MainWindow mainWindow(selectedType, username);
        mainWindow.show();
        
        // Start the Qt event loop
        return app.exec();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}