#include <QApplication> // controls the event loop
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/app_icon.png"));

    MainWindow window;
    window.showMaximized(); // scale to monitor size
    return app.exec();
}