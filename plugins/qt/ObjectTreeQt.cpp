#include "stdafx.h"
#include <ostream>
#include <QThread>
#include <QMetaProperty>
#include <QCoreApplication>
#include <QApplication>
#include <QClipboard>
#include <QWidget>
#include <QWindow>
#include <QSurface>
#include <QDesktopWidget>
#include <QOpenGLContext>
#include <QOpenGLWidget>
#ifdef WITH_QTOPENGL
#include <QGLWidget>
#endif // WITH_QTOPENGL

#include "ObjectTreeQt.h"
#include "SGIItemQt"
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/helpers/string>
#include <sgi/helpers/qt>
#include <sgi/helpers/qt_widgetwindow>

namespace sgi {
namespace qt_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QMetaObject)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QObject)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QWidget)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QWidgetWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QSurface)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QCoreApplication)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QApplication)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QPaintDevice)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QImage)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLContext)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLWidget)
#ifdef WITH_QTOPENGL
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QGLWidget)
#endif // WITH_QTOPENGL

using namespace sgi::qt_helpers;

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
            treeItem->addChild("Methods", cloneItem<SGIItemQt>(SGIItemTypeMethods, ~0u));
            const QThread * thread = object->thread();
            if(thread && object != thread)
            {
                SGIHostItemQt threadItem(const_cast<QThread*>(thread));
                treeItem->addChild("Thread", &threadItem);
            }

            while(metaObject)
            {
                int propertyOffset = metaObject->propertyOffset();
                int propertyCount = metaObject->propertyCount();
                for (int i=propertyOffset; i<propertyCount; ++i)
                {
                    QMetaProperty metaproperty = metaObject->property(i);
                    const char *name = metaproperty.name();
                    const char *typeName = metaproperty.typeName();
                    QVariant value = object->property(name);

                    bool isQObject = value.canConvert<QObject*>();
                    if(isQObject)
                    {
                        std::stringstream ss;
                        ss << metaObject->className() << "::" << name;
                        SGIHostItemQt item(value.value<QObject*>());
                        treeItem->addChild(ss.str(), &item);
                    }
                }
                metaObject = metaObject->superClass();
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
            if (_item->number() == ~0u)
            {
                const QMetaObject * metaObject = object->metaObject();
                while (metaObject)
                {
                    int methodOffset = metaObject->methodOffset();
                    int methodCount = metaObject->methodCount();
                    for (int i = methodOffset; i < methodCount; ++i)
                    {
                        QMetaMethod method = metaObject->method(i);
                        treeItem->addChild(method.name().toStdString(), cloneItem<SGIItemQt>(SGIItemTypeMethods, i));
                    }
                    metaObject = metaObject->superClass();
                }
            }
            else
            {
            }
            ret = true;
        }
        break;
    }

    return ret;
}

bool objectTreeBuildImpl<QWidget>::build(IObjectTreeItem * treeItem)
{
	QWidget * object = getObjectMulti<QWidget, SGIItemQt, SGIItemQtPaintDevice>();
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

            SGIHostItemQt windowHandle(object->windowHandle());
            if(windowHandle.hasObject())
                treeItem->addChild("WindowHandle", &windowHandle);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QWindow>::build(IObjectTreeItem * treeItem)
{
    QWindow * object = getObjectMulti<QWindow, SGIItemQt, SGIItemQtSurface>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Format", cloneItem<SGIItemQt>(SGIItemTypeSurfaceFormat));
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QWidgetWindow>::build(IObjectTreeItem * treeItem)
{
	QWidgetWindow * object = getObjectMulti<QWidgetWindow, SGIItemQt, SGIItemQtSurface>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
			SGIHostItemQt widget(object->widget());
			if (widget.hasObject())
				treeItem->addChild("Widget", &widget);
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<QSurface>::build(IObjectTreeItem * treeItem)
{
    QSurface * object = getObject<QSurface, SGIItemQtSurface>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Format", cloneItem<SGIItemQt>(SGIItemTypeSurfaceFormat));
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

            SGIHostItemQtIcon windowIcon(object->windowIcon());
            if(windowIcon.hasObject())
                treeItem->addChild("WindowIcon", &windowIcon);

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

bool objectTreeBuildImpl<QOpenGLContext>::build(IObjectTreeItem * treeItem)
{
    QOpenGLContext * object = getObject<QOpenGLContext, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Extensions", cloneItem<SGIItemQt>(SGIItemTypeContextExtensions));
            treeItem->addChild("Surface", cloneItem<SGIItemQt>(SGIItemTypeSurface));
            SGIHostItemQt shareContext(object->shareContext());
            if (shareContext.hasObject())
                treeItem->addChild("ShareContext", &shareContext);

            SGIHostItemQt shareGroup(object->shareGroup());
            if (shareGroup.hasObject())
                treeItem->addChild("ShareGroup", &shareGroup);
        }
        break;
    case SGIItemTypeContextExtensions:
        ret = true;
        break;
    case SGIItemTypeSurface:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QOpenGLWidget>::build(IObjectTreeItem * treeItem)
{
    QOpenGLWidget * object = getObject<QOpenGLWidget, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemQt context(object->context());
            if (context.hasObject())
                treeItem->addChild("Context", &context);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}


#ifdef WITH_QTOPENGL
bool objectTreeBuildImpl<QGLWidget>::build(IObjectTreeItem * treeItem)
{
    QGLWidget * object = getObject<QGLWidget, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Context", cloneItem<SGIItemQt>(SGIItemTypeContext));
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif // WITH_QTOPENGL


OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

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

    SGIHostItemInternal hostItem(new SGIProxyItemT<QApplicationSingleton>(_hostInterface, "QApplication"));
    treeItem->addChild(std::string(), &hostItem);
    return true;
}
} // namespace qt_plugin
} // namespace sgi
