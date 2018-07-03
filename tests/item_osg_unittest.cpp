#include "item_osg_unittest.h"

#include <QDebug>
#include <QDialog>
#include <QMenu>

#if defined(__linux__)
#include <dlfcn.h>
#include <osgDB/Version>
#endif

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

#ifdef __linux__
namespace {
    std::string getOSGDBModuleFilename()
    {
        std::string ret;
        Dl_info info;
        const char* (* addr) () = osgDBGetVersion;
        if(dladdr(reinterpret_cast<const void*>(addr), &info) != 0)
        {
            ret = info.dli_fname;
        }
        return ret;
    }
    std::string getOSGDBModuleDirectory()
    {
        std::string modulefilename = getOSGDBModuleFilename();
        return std::string(modulefilename, 0, modulefilename.find_last_of('/'));
    }
}
#endif

void item_osg_unittest::initTestCase()
{
#ifdef __linux__
    osgDB::Registry * registry = osgDB::Registry::instance();
    registry->initLibraryFilePathList();
    osgDB::FilePathList libdirs = registry->getLibraryFilePathList();
    libdirs.push_back(getOSGDBModuleDirectory());
    registry->setLibraryFilePathList(libdirs);
#endif
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
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), false);

    auto lib = sgi::autoload::Osg::sgiLibrary();
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), true);

    SGIItemBasePtr item;
    SGIHostItemOsg hostItem(lib);
    sgi::generateItem<sgi::autoload::Osg>(item, &hostItem);
    QVERIFY(item.valid());
    sgi::IContextMenuPtr ctxIface = sgi::createContextMenu<sgi::autoload::Osg>(nullptr, item.get());
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

    sgi::shutdown<sgi::autoload::Osg>();

    sgi::autoload::Osg::sgiLibraryUnload();
    QCOMPARE(sgi::autoload::Osg::sgiLibraryLoaded(), false);
    QCOMPARE(sgi::SGIItemBase::getTotalItemCount(), 0u);
}

QTEST_MAIN(item_osg_unittest)
