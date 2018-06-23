#pragma once

#include <QtTest/QTest>

class item_osgearth_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void checkRefItems();
    void checkConfigItems();
    void checkConfigOptionsItems();
};
