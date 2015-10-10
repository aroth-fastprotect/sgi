#pragma once

#include <QtTest/QTest>

class item_unittest : public QObject
{
    Q_OBJECT
private slots:
    void verifyRefPtr();
    void insertItemSingle();
    void insertItemTwoChains();
    void insertItemSameScore();

    void autoLoadOsg();
    void autoLoadQt();

    void write_html();
};
