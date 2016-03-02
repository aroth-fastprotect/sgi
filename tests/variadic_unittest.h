#pragma once

#include <QtTest/QTest>

class variadic_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_index_of();
    void test_for_each();
    void test_sizeof();
};
