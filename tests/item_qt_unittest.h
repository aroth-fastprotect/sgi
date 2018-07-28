#pragma once

#include <QtTest/QTest>

class item_qt_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void autoLoadQt();
    void hostItemPaintDevice();
    void qImage();
};
