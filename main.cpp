#include "general.h"
#include "mainwindow.h"
#include "aboutwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //Create main objects
    MainWindow mainWindow;
    AboutWindow aboutWindow;

    //Connect objets
    QObject::connect(&mainWindow, SIGNAL(displayAboutWindow()), &aboutWindow, SLOT(show()));

    //Display main window
    mainWindow.show();

    return app.exec();
}
