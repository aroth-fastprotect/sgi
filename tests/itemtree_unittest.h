#pragma once

#include <QtTest/QTest>

class itemtree_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_no_object();
    void test_qobject();
};
