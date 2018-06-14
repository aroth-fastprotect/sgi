#include "item_osg_unittest.h"

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
#include <sgi/AutoLoadOsg>
#include <sgi/plugins/SGIHostItemOsg.h>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}

SGIItemOsg

class TestItem : public SGIItemBase
{
public:
    TestItem(unsigned score=0)
        : SGIItemBase(SGIItemTypeInvalid, 0, score, nullptr)
    {
    }

    virtual ~TestItem()
    {
    }
};

void item_osg_unittest::initTestCase()
{
}

void item_osg_unittest::cleanupTestCase()
{
}

void item_osg_unittest::autoLoadOsg()
{
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), false);
    auto lib = sgi::autoload::Osg::instance();
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), true);
    QVERIFY(sgi::autoload::Osg::sgiLibrary() != nullptr);
    QCOMPARE(sgi::autoload::Osg::sgiLibraryName(), std::string(SGI_LIBRARY_NAME));
    QVERIFY(sgi::autoload::Osg::sgiLibraryError().empty());
    QVERIFY(!sgi::autoload::Osg::sgiLibraryFilename().empty());
    sgi::autoload::Osg::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), false);
}

void item_osg_unittest::contextMenuOsg()
{
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Osg::sgiLibrary();
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    SGIHostItemOsg hostItem(lib);
    sgi::generateItem<sgi::autoload::Osg>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::IContextMenuPtr ctxIface = sgi::createContextMenu<sgi::autoload::Osg>(nullptr, item);
    QVERIFY(ctxIface != nullptr);
    QMenu * menu = ctxIface->getMenu();
    menu->show();
    menu->hide();
    // need to run the main event processor to clean up (deleteLater, etc)
    QApplication::processEvents();

    QCOMPARE(getRefCount(ctxIface.get()), 1u);
    // release the menu
    ctxIface = nullptr;
    item = nullptr;

    sgi::shutdown<sgi::autoload::Osg>();

    sgi::autoload::Osg::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), false);
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

QTEST_MAIN(item_osg_unittest)
