#include "item_qt_unittest.h"

#include <QDebug>
#include <QDialog>
#include <QMenu>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>
#include <sgi/WritePrettyHTML>
#include <sgi/SceneGraphDialog>
#include <sgi/ContextMenu>
#include <sgi/ImagePreviewDialog>
#include <sgi/Shutdown>
#include <sgi/AutoLoadQt>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIItemQt>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}

void item_qt_unittest::initTestCase()
{
}

void item_qt_unittest::cleanupTestCase()
{
}

void item_qt_unittest::autoLoadQt()
{
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    auto lib = sgi::autoload::Qt::instance();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);
    QVERIFY(sgi::autoload::Qt::sgiLibrary() != nullptr);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryName(), std::string(SGI_LIBRARY_NAME));
    QVERIFY(sgi::autoload::Qt::sgiLibraryError().empty());
    QVERIFY(!sgi::autoload::Qt::sgiLibraryFilename().empty());
    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
}

void item_qt_unittest::hostItemPaintDevice()
{
    QImage * image = new QImage(QSize(64, 64), QImage::Format_ARGB32);
    SGIHostItemQtPaintDevice hostitem(image);
    QVERIFY(hostitem.hasObject());

    SGIHostItemQtPaintDevice hostitem2 = hostitem;
    QVERIFY(hostitem2.hasObject());

}

void item_qt_unittest::qImage()
{
    QImage * image = new QImage(QSize(64, 64), QImage::Format_ARGB32);
    SGIHostItemQtPaintDevice hostitem(image);
    QVERIFY(hostitem.hasObject());
    SGIItemBasePtr item;
    QVERIFY(sgi::generateItem<autoload::Qt>(item, &hostitem));
    QVERIFY(item.valid());

    item = nullptr;
    QVERIFY(!item.valid());

}

void item_qt_unittest::qIcon()
{
    SGIItemBasePtr item;
    {
        QPixmap pixmap(32, 32);
        QIcon icon(pixmap);
        auto sizes = icon.availableSizes();
        QVERIFY(!sizes.empty());
        SGIHostItemQtIcon hostitem(icon);
        QVERIFY(hostitem.hasObject());
        QVERIFY(sgi::generateItem<autoload::Qt>(item, &hostitem));
    }

    QVERIFY(item.valid());

    SGIItemQtIcon* qitem = dynamic_cast<SGIItemQtIcon*>(item.get());
    QVERIFY(qitem != nullptr);

    QIcon * icon = qitem->objectAs<QIcon>();
    auto sizes = icon->availableSizes();
    QVERIFY(!sizes.empty());

    item = nullptr;
    QVERIFY(!item.valid());

}

QTEST_MAIN(item_qt_unittest)
