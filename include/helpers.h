#pragma once

#include <QWidget>
#include <QStringList>

class Helpers {
public:
    static void loadUI(QWidget* widget);
    static QStringList parseCsvLine(const QString &line);
};