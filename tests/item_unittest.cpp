#include "item_unittest.h"

#include <QDebug>
#include <QDialog>
#include <QMenu>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>
#include <sgi/WritePrettyHTML>
#include <sgi/SceneGraphDialog>
#include <sgi/ContextMenu>
#include <sgi/ContextMenuQt>
#include <sgi/ImagePreviewDialog>
#include <sgi/Shutdown>
#include <sgi/AutoLoadQt>
#include <sgi/LibraryInfo>
#include <sgi/plugins/SGIHostItemInternal.h>

#include <sstream>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}

struct NullType {};
typedef SGIItemInfoPlainPtr<NullType> SGIItemHolderNullType;
typedef SGIItemHolderT<SGIItemHolderNullType> SGIItemHolderNull;
typedef SGIHostItemImpl<NullType> SGIHostItemNull;
typedef SGIItemT<SGIHostItemNull, SGIItemHolderNull> SGIItemNull;


class TestItem : public SGIItemNull
{
    static NullType the_only_null;
public:
    TestItem(unsigned score=0)
        : SGIItemNull(static_cast<HostItemType*>(nullptr), SGIItemTypeInvalid, &the_only_null, 0, score, nullptr)
    {
    }

    ~TestItem() override
    {
    }
};

NullType TestItem::the_only_null;

void item_unittest::initTestCase()
{
}

void item_unittest::cleanupTestCase()
{
}

void item_unittest::verifyRefPtr()
{
    return;
    SGIItemBasePtr item, item2, item3;
    QCOMPARE(SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(SGIItemHolder::getTotalItemCount(), 0u);
    item = new TestItem;
    QCOMPARE(SGIItemBase::getTotalItemCount(), 1u);
    QCOMPARE(SGIItemHolder::getTotalItemCount(), 1u);
    item = nullptr;
    QCOMPARE(SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(SGIItemHolder::getTotalItemCount(), 0u);

    item = new TestItem;
    item2 = new TestItem;
    item3 = new TestItem;
    QCOMPARE(SGIItemBase::getTotalItemCount(), 3u);
    QCOMPARE(SGIItemHolder::getTotalItemCount(), 3u);
    item->insertAfter(item2.get());
    item2->insertAfter(item3.get());
    item2 = nullptr;
    item3 = nullptr;
    QCOMPARE(item->listSize(), 2ul);
    QCOMPARE(SGIItemBase::getTotalItemCount(), 3u);
    QCOMPARE(SGIItemHolder::getTotalItemCount(), 3u);
    item = nullptr;
    QCOMPARE(SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(SGIItemHolder::getTotalItemCount(), 0u);

}

void item_unittest::insertItemSingle()
{
    return;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    {
        SGIItemBasePtr first = new TestItem(100);
        QCOMPARE(first->listSize(), 0ul);
        SGIItemBasePtr second = new TestItem(50);
        QCOMPARE(second->listSize(), 0ul);

        SGIItemBasePtr front;
        first->insertByScore(second.get(), front);

        QVERIFY(first->score() > first->nextBase()->score());
        QCOMPARE(first->listSize(), 1ul);
    }
    {
        SGIItemBasePtr first = new TestItem(100);
        SGIItemBasePtr second = new TestItem(50);

        SGIItemBasePtr front;
        first->insertByScore(second.get(), front);

        QVERIFY(first->score() > first->nextBase()->score());
    }
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::insertItemTwoChains()
{
    return;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    SGIItemBasePtr firstChain;
    SGIItemBasePtr secondChain;
    const unsigned numItems = 50;
    {
        SGIItemBasePtr prev = nullptr;
        for(unsigned n = 0; n <= (numItems * 2); n+=2)
        {
            SGIItemBasePtr item = new TestItem(n);
            if(prev)
                prev->insertBefore(item.get());
            prev = item;
        }
        firstChain = prev;
        QVERIFY(firstChain->isListValid());
        QCOMPARE(firstChain->listSize(), (size_t)numItems);
    }

    {
        SGIItemBasePtr prev = nullptr;
        for(unsigned n = 1; n <= (numItems * 2) + 1; n+=2)
        {
            SGIItemBasePtr item = new TestItem(n);
            if(prev)
                prev->insertBefore(item.get());
            prev = item;
        }
        secondChain = prev;
        QVERIFY(secondChain->isListValid());
        QCOMPARE(secondChain->listSize(), (size_t)numItems);
    }
    SGIItemBasePtr newFront;
    firstChain->insertByScore(secondChain.get(), newFront);
    QCOMPARE(newFront->score(), (numItems * 2) + 1);
    QVERIFY(newFront->isListValid());
    QCOMPARE(newFront->listSize(), (size_t)(2*numItems) + 1);

    firstChain = nullptr;
    secondChain = nullptr;
    newFront = nullptr;

    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::insertItemSameScore()
{
    return;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);

    SGIItemBasePtr firstChain;
    SGIItemBasePtr secondChain;
    const unsigned numItems = 5;
    {
        SGIItemBasePtr prev = nullptr;
        for(unsigned n = 0; n <= (numItems * 2); n+=2)
        {
            SGIItemBasePtr item = new TestItem(17);
            if(prev)
                prev->insertBefore(item.get());
            prev = item;
        }
        firstChain = prev;
        QVERIFY(firstChain->isListValid());
        QCOMPARE(firstChain->listSize(), (size_t)numItems);
    }

    {
        SGIItemBasePtr prev = nullptr;
        for(unsigned n = 1; n <= (numItems * 2) + 1; n+=2)
        {
            SGIItemBasePtr item = new TestItem(17);
            if(prev)
                prev->insertBefore(item.get());
            prev = item;
        }
        secondChain = prev;
        QVERIFY(secondChain->isListValid());
        QCOMPARE(secondChain->listSize(), (size_t)numItems);
    }
    SGIItemBasePtr newFront;
    firstChain->insertByScore(secondChain.get(), newFront);
    QCOMPARE(newFront->score(), 17u);
    QVERIFY(newFront->isListValid());
    QCOMPARE(newFront->listSize(), (size_t)(2*numItems) + 1);

    firstChain = nullptr;
    secondChain = nullptr;
    newFront = nullptr;

    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::autoLoadQt()
{
    return;
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    auto lib = sgi::autoload::Qt::instance();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);
    QVERIFY(sgi::autoload::Qt::sgiLibrary() != nullptr);
    QVERIFY(lib != nullptr);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryName(), std::string(SGI_LIBRARY_NAME));
    QVERIFY(sgi::autoload::Qt::sgiLibraryError().empty());
    QVERIFY(!sgi::autoload::Qt::sgiLibraryFilename().empty());
    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
}

void item_unittest::generateItem()
{
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    QVERIFY(hostItem.hasObject());
    QCOMPARE(hostItem.object(), infoObj);
    SGIItemBasePtr item;
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    QCOMPARE(item->pluginInfo()->pluginName(), std::string("_sgi_internal"));
    item = nullptr;

    sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

void item_unittest::writePrettyHTML()
{
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    SGIItemBasePtr item = new TestItem;
    std::stringstream ss;
    sgi::writePrettyHTML<sgi::autoload::Qt>(ss, item.get());
    // TestItem is unknown to SGI so the resulting string must be empty
    QVERIFY(ss.str().empty());
    item = nullptr;
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::writePrettyHTML<sgi::autoload::Qt>(ss, item.get());
    QVERIFY(!ss.str().empty());
    //qDebug() << ss.str();

    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

void item_unittest::sceneGraphDialog()
{
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::ISceneGraphDialogPtr dlgIface = sgi::showSceneGraphDialog<sgi::autoload::Qt>(nullptr, item.get());
    QVERIFY(dlgIface != nullptr);
    QDialog * dlg = dlgIface->getDialog();
    dlg->show();
    dlg->close();
    // need to run the main event processor to clean up (deleteLater, etc)
    QApplication::processEvents();

    dlgIface = nullptr;
    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

void item_unittest::contextMenuHidden()
{
    return;
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::IContextMenuPtr ctxIface = sgi::createContextMenu<sgi::autoload::Qt>(nullptr, item.get());
    QVERIFY(ctxIface != nullptr);
    // need to run the main event processor to clean up (deleteLater, etc)
    QApplication::processEvents();
    ctxIface->setObject(static_cast<SGIItemBase*>(nullptr));
    // need to run the main event processor to clean up (deleteLater, etc)
    QApplication::processEvents();

    QCOMPARE(getRefCount(ctxIface.get()), 1u);
    // release the menu
    ctxIface = nullptr;
    item = nullptr;

    sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

void item_unittest::contextMenu()
{
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::IContextMenuPtr ctxIface = sgi::createContextMenu<sgi::autoload::Qt>(nullptr, item.get());
    QVERIFY(ctxIface != nullptr);
    QMenu * menu = ctxIface->getMenu();
    menu->show();
    menu->hide();
    // need to run the main event processor to clean up (deleteLater, etc)
    QApplication::processEvents();

    QCOMPARE(getRefCount(ctxIface.get()), 1);
    // release the menu
    ctxIface = nullptr;
    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

void item_unittest::contextMenuQt()
{
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
	QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

	auto lib = sgi::autoload::Qt::sgiLibrary();
	QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

	SGIItemBasePtr item;
    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
	QVERIFY(item.valid());
    sgi::IContextMenuQt * ctxIface = sgi::createContextMenuQt(nullptr, lib);
    QVERIFY(ctxIface != nullptr);
	QMenu * menu = ctxIface->getMenu();
	menu->show();
	menu->hide();
    // need to run the main event processor to clean up (deleteLater, etc)
    QApplication::processEvents();

	// release the menu
    ctxIface = nullptr;
    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

	sgi::autoload::Qt::sgiLibraryUnload();
	QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

void item_unittest::imagePreviewDialog()
{
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    QPixmap * pm = new QPixmap(QLatin1String("/usr/share/pixmaps/putty.png"));

    SGIItemBasePtr item;
    //SGIHostItemQtPaintDevice hostItem(pm);
    //sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    auto infoObj = sgi::libraryInfoObject<sgi::autoload::Qt>();
    SGIHostItemInternal hostItem(infoObj);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);

    QVERIFY(item.valid());
    sgi::IImagePreviewDialogPtr dlgIface = sgi::showImagePreviewDialog<sgi::autoload::Qt>(nullptr, item.get());
    QVERIFY(dlgIface != nullptr);
    QDialog * dlg = dlgIface->getDialog();
    dlg->show();
    dlg->close();
    //dlg->exec();

    dlgIface = nullptr;
    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

QTEST_MAIN(item_unittest)
