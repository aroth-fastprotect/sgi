#pragma once

#include <QtTest/QTest>

class item_osg_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void autoLoadOsg();
    void contextMenuOsg();
};
