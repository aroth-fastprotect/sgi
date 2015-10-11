#pragma once

#include <QtTest/QTest>

class item_unittest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void verifyRefPtr();
    void insertItemSingle();
    void insertItemTwoChains();
    void insertItemSameScore();

    void autoLoadOsg();
    void autoLoadQt();

    void writePrettyHTML();

    void sceneGraphDialog();
    void contextMenu();
    void imagePreviewDialog();
};
