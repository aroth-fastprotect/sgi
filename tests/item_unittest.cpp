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
#include <sgi/plugins/SGIHostItemQt.h>

#include <sstream>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}


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

void item_unittest::initTestCase()
{
}

void item_unittest::cleanupTestCase()
{
}

void item_unittest::verifyRefPtr()
{
    SGIItemBasePtr item, item2, item3;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    item = new TestItem;
    QCOMPARE(TestItem::getTotalItemCount(), 1u);
    item = nullptr;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);

    item = new TestItem;
    item2 = new TestItem;
    item3 = new TestItem;
    QCOMPARE(TestItem::getTotalItemCount(), 3u);
    item->insertAfter(item2);
    item2->insertAfter(item3);
    item2 = nullptr;
    item3 = nullptr;
    QCOMPARE((int)item->listSize(), 2);
    QCOMPARE(TestItem::getTotalItemCount(), 3u);
    item = nullptr;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);

}

void item_unittest::insertItemSingle()
{
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    {
        SGIItemBasePtr first = new TestItem(100);
        QCOMPARE(first->listSize(), (size_t)0);
        SGIItemBasePtr second = new TestItem(50);
        QCOMPARE(second->listSize(), (size_t)0);

        SGIItemBasePtr front;
        first->insertByScore(second, front);

        QVERIFY(first->score() > first->nextBase()->score());
        QCOMPARE(first->listSize(), (size_t)1);
    }
    {
        SGIItemBasePtr first = new TestItem(100);
        SGIItemBasePtr second = new TestItem(50);

        SGIItemBasePtr front;
        first->insertByScore(second, front);

        QVERIFY(first->score() > first->nextBase()->score());
    }
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::insertItemTwoChains()
{
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
                prev->insertBefore(item);
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
                prev->insertBefore(item);
            prev = item;
        }
        secondChain = prev;
        QVERIFY(secondChain->isListValid());
        QCOMPARE(secondChain->listSize(), (size_t)numItems);
    }
    SGIItemBasePtr newFront;
    firstChain->insertByScore(secondChain, newFront);
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
                prev->insertBefore(item);
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
                prev->insertBefore(item);
            prev = item;
        }
        secondChain = prev;
        QVERIFY(secondChain->isListValid());
        QCOMPARE(secondChain->listSize(), (size_t)numItems);
    }
    SGIItemBasePtr newFront;
    firstChain->insertByScore(secondChain, newFront);
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

void item_unittest::writePrettyHTML()
{
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    SGIItemBasePtr item = new TestItem;
    std::stringstream ss;
    sgi::writePrettyHTML<sgi::autoload::Qt>(ss, item);
    // TestItem is unknown to SGI so the resulting string must be empty
    QVERIFY(ss.str().empty());
    item = nullptr;
    QCOMPARE(TestItem::getTotalItemCount(), 0u);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    SGIHostItemQt hostItem(lib);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::writePrettyHTML<sgi::autoload::Qt>(ss, item);
    QVERIFY(!ss.str().empty());
    //qDebug() << ss.str();

    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::sceneGraphDialog()
{
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    SGIHostItemQt hostItem(lib);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::ISceneGraphDialogPtr dlgIface = sgi::showSceneGraphDialog<sgi::autoload::Qt>(nullptr, item);
    QVERIFY(dlgIface != nullptr);
    QDialog * dlg = dlgIface->getDialog();
    dlg->show();
    dlg->close();

    dlgIface = nullptr;
    item = nullptr;

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::contextMenu()
{
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    SGIHostItemQt hostItem(lib);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::IContextMenuPtr ctxIface = sgi::createContextMenu<sgi::autoload::Qt>(nullptr, item);
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

	sgi::shutdown<sgi::autoload::Qt>();

    sgi::autoload::Qt::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::contextMenuQt()
{
	QCOMPARE(TestItem::getTotalItemCount(), 0u);
	QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

	auto lib = sgi::autoload::Qt::sgiLibrary();
	QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

	SGIItemBasePtr item;
	SGIHostItemQt hostItem(lib);
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
	QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

void item_unittest::imagePreviewDialog()
{
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Qt::sgiLibrary();
    QCOMPARE(sgi::autoload::Qt::sgiLibraryLoaded(), true);

    QPixmap * pm = new QPixmap(QLatin1String("/usr/share/pixmaps/putty.png"));

    SGIItemBasePtr item;
    //SGIHostItemQtPaintDevice hostItem(pm);
    //sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);
    SGIHostItemQt hostItem(lib);
    sgi::generateItem<sgi::autoload::Qt>(item, &hostItem);

    QVERIFY(item.valid());
    sgi::IImagePreviewDialogPtr dlgIface = sgi::showImagePreviewDialog<sgi::autoload::Qt>(nullptr, item);
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
    QCOMPARE(TestItem::getTotalItemCount(), 0u);
}

QTEST_MAIN(item_unittest)
