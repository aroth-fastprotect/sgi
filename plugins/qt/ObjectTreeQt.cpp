#include <ostream>
#include <QThread>
#include <QMetaProperty>
#include <QCoreApplication>
#include <QApplication>
#include <QClipboard>
#include <QWidget>
#include <QDesktopWidget>

#include "ObjectTreeQt.h"
#include "SGIItemQt"

#include <sgi/SGIItemInternal>
#include <sgi/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/helpers/string>

#include "qt_string_helpers"

namespace sgi {
namespace qt_plugin {

OBJECT_TREE_BUILD_IMPL_REGISTER(QMetaObject)
OBJECT_TREE_BUILD_IMPL_REGISTER(QObject)
OBJECT_TREE_BUILD_IMPL_REGISTER(QWidget)
OBJECT_TREE_BUILD_IMPL_REGISTER(QCoreApplication)
OBJECT_TREE_BUILD_IMPL_REGISTER(QApplication)
OBJECT_TREE_BUILD_IMPL_REGISTER(QPaintDevice)
OBJECT_TREE_BUILD_IMPL_REGISTER(QImage)

bool objectTreeBuildImpl<QMetaObject>::build(IObjectTreeItem * treeItem)
{
    QMetaObject * object = getObject<QMetaObject, SGIItemQtMeta>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const QMetaObject * super = object->superClass();
            if(super)
            {
                SGIHostItemQtMeta superItem(const_cast<QMetaObject*>(super));
                std::string name = std::string("Super (") + super->className() + ')';
                treeItem->addChild(name, &superItem);
            }
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QObject>::build(IObjectTreeItem * treeItem)
{
    QObject * object = getObject<QObject, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const QMetaObject * metaObject = object->metaObject();
            if(metaObject)
            {
                SGIHostItemQtMeta metaObjectItem(const_cast<QMetaObject*>(metaObject));
                treeItem->addChild("MetaObject", &metaObjectItem);
            }

            QObject * parent = object->parent();
            if(parent)
            {
                SGIHostItemQt parentItem(parent);
                treeItem->addChild("Parent", &parentItem);
            }
            const QObjectList & children = object->children ();
            unsigned numChild = children.size();
            if(numChild)
                treeItem->addChild(helpers::str_plus_count("Childs", numChild), cloneItem<SGIItemQt>(SGIItemTypeChilds));
            //treeItem->addChild("Methods", cloneItem<SGIItemQt>(SGIItemTypeMethods));
            const QThread * thread = object->thread();
            if(thread && object != thread)
            {
                SGIHostItemQt threadItem(const_cast<QThread*>(thread));
                treeItem->addChild("Thread", &threadItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            const QObjectList & children = object->children ();
            for(QObjectList::const_iterator it = children.begin(); it != children.end(); it++)
            {
                QObject * child = *it;
                SGIHostItemQt childItem(child);
                treeItem->addChild(std::string(), &childItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeMethods:
        {
            ret = true;
        }
        break;
    }

    return ret;
}

bool objectTreeBuildImpl<QWidget>::build(IObjectTreeItem * treeItem)
{
    QWidget * object = getObject<QWidget, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->parentWidget() != object->parent())
            {
                SGIHostItemQt parentWidget(object->parentWidget());
                if(parentWidget.hasObject())
                    treeItem->addChild("ParentWidget", &parentWidget);
            }
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QCoreApplication>::build(IObjectTreeItem * treeItem)
{
    QCoreApplication * object = getObject<QCoreApplication, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QApplication>::build(IObjectTreeItem * treeItem)
{
    QApplication * object = getObject<QApplication, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemQt activeWindow(object->activeWindow());
            if(activeWindow.hasObject())
                treeItem->addChild("ActiveWindow", &activeWindow);

            SGIHostItemQt desktop(object->desktop());
            if(desktop.hasObject())
                treeItem->addChild("Desktop", &desktop);

            SGIHostItemQt clipboard(object->clipboard());
            if(clipboard.hasObject())
                treeItem->addChild("Clipboard", &clipboard);
            
            QWidgetList allWidgets = object->allWidgets();
            if(!allWidgets.isEmpty())
                treeItem->addChild(helpers::str_plus_count("Widgets", allWidgets.size()), cloneItem<SGIItemQt>(SGIItemTypeWidgets));
        }
        break;
    case SGIItemTypeWidgets:
        {
            QWidgetList allWidgets = object->allWidgets();
            for(QWidgetList::const_iterator it = allWidgets.begin(); it != allWidgets.end(); it++)
            {
                SGIHostItemQt widgetItem(*it);
                treeItem->addChild(std::string(), &widgetItem);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QPaintDevice>::build(IObjectTreeItem * treeItem)
{
    QPaintDevice * object = getObject<QPaintDevice, SGIItemQtPaintDevice>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = true;
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QImage>::build(IObjectTreeItem * treeItem)
{
    QImage * object = getObject<QImage, SGIItemQtPaintDevice>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_REGISTER(ISceneGraphDialog)

struct QApplicationSingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        SGIHostItemQt hostItem(QApplication::instance());
        hostInterface->generateItem(item, &hostItem);
        return item.release();
    }
};

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = static_cast<ISceneGraphDialog*>(item<SGIItemInternal>()->object());

    SGIHostItemOsg hostItem(new SGIProxyItemT<QApplicationSingleton>(_hostInterface, "QApplication"));
    treeItem->addChild(std::string(), &hostItem);
    return true;
}
} // namespace qt_plugin
} // namespace sgi
