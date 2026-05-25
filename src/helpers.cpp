#include "helpers.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>

void Helpers::loadUI(QWidget* widget) {
    if (!widget) return;

    QFile file(":/mainwindow.css");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString loadedStyleSheet = stream.readAll();

        widget->setStyleSheet(loadedStyleSheet);

        file.close();
    } else {
        // send error if not found
        qDebug() << "Error: Could not find the CSS file in resources (:/mainwindow.css)";
    }
}

QStringList Helpers::parseCsvLine(const QString &line) {
    QStringList fields;
    QString field;
    bool inQuotes = false;

    for (QChar c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.append(field.trimmed());
            field.clear();
        } else {
            field += c;
        }
    }

    fields.append(field.trimmed());
    return fields;
}