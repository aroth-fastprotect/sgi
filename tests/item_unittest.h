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

    void autoLoadQt();

    void generateItem();

    void writePrettyHTML();

    void sceneGraphDialog();
    void contextMenuHidden();
    void contextMenu();
	void contextMenuQt();
    void imagePreviewDialog();
};
