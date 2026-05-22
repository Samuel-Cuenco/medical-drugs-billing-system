#include "helpers.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

void Helpers::loadUI(QWidget* widget) {
    if (!widget) return;

    QFile file(":/mainwindow.css"); 
    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString loadedStyleSheet = stream.readAll();
        
        widget->setStyleSheet(loadedStyleSheet);
        
        file.close();
    } else {
        // If the path is wrong or the file is missing, this prints to your console
        qDebug() << "Error: Could not find the CSS file in resources (:/mainwindow.css)";
    }
}