#pragma once

#include <QtTest/QTest>

class variadic_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void checkCompiler();
};
