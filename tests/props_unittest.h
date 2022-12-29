#pragma once

#include <QtTest/QTest>

class props_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void registerProps();
};
