#include <QApplication>
#include <QMessageBox>
#include <iostream>

#include "usertypedialog.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "hospitaldatamanager.h"

int main(int argc, char** argv) {
    try {
        // Initialize Qt application
        QApplication app(argc, argv);
        
        // Set application information for data storage
        app.setOrganizationName("HospitalApp");
        app.setApplicationName("HospitalManagementSystem");
        
        // Initialize hospital data manager
        HospitalDataManager hospitalData;
        if (!hospitalData.initialize()) {
            std::cerr << "Warning: Failed to initialize hospital data system." << std::endl;
        }
        
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
        loginDialog.setDataManager(&hospitalData);
        if (loginDialog.exec() != QDialog::Accepted) {
            // User cancelled the login
            return 0;
        }
        
        // Get login credentials
        QString username = loginDialog.getUsername();
        
        // Finally, show the main application window
        MainWindow mainWindow(selectedType, username);
        mainWindow.show();
        
        // Run the application
        return app.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Critical Error", 
                             QString("An unhandled error occurred: %1").arg(e.what()));
        return 1;
    }
}