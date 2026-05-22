#include <QApplication> // controls the event loop
#include <QIcon> // sets the app icon
#include "mainwindow.h" // import mainwindow.h

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/app_icon.png")); // 

    MainWindow window; // create a window object
    window.showMaximized(); // scale to monitor size
    return app.exec();
}