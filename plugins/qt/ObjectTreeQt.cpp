#include "stdafx.h"
#include <ostream>
#include <QThread>
#include <QMetaProperty>
#include <QCoreApplication>
#include <QApplication>
#include <QAbstractEventDispatcher>
#include <QClipboard>
#include <QWidget>
#include <QWindow>
#include <QSurface>
#include <QStyle>
#include <QBitmap>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLShaderProgram>
#ifdef WITH_QTOPENGL
#include <QGLWidget>
#endif // WITH_QTOPENGL
#include <QLayout>

#include <qpa/qplatformwindow.h>

#include "ObjectTreeQt.h"
#include "SGIItemQt"
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/helpers/string>
#include <sgi/helpers/qt>

namespace sgi {
namespace qt_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QMetaObject)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QObject)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QWidget)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QSurface)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QCoreApplication)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QApplication)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QPaintDevice)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QImage)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLContext)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLWidget)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLShaderProgram)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QOpenGLShader)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QIcon)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QSystemTrayIcon)

#ifdef WITH_QTOPENGL
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QGLWidget)
#endif // WITH_QTOPENGL

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QLayout)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QPlatformSurface)

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
            treeItem->addChild("Properties", cloneItem<SGIItemQt>(SGIItemTypeProperties, ~0u));
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
                    QVariant value = object->property(name);

                    if((QMetaType::Type)value.type() == QMetaType::QObjectStar || value.canConvert<QObject*>())
                    {
                        std::stringstream ss;
                        ss << metaObject->className() << "::" << name;
                        SGIHostItemQt item(value.value<QObject*>());
                        if(item.hasObject())
                            treeItem->addChild(ss.str(), &item);
                    }
                    else if(value.canConvert<QIcon>())
                    {
                        std::stringstream ss;
                        ss << metaObject->className() << "::" << name;
                        SGIHostItemQtIcon item(value.value<QIcon>());
                        if (item.hasObject())
                            treeItem->addChild(ss.str(), &item);
                    }
                    else if(value.canConvert<QBitmap>())
                    {
                        std::stringstream ss;
                        ss << metaObject->className() << "::" << name;
                        SGIHostItemQtPaintDevice item(value.value<QBitmap>());
                        if (item.hasObject())
                            treeItem->addChild(ss.str(), &item);
                    }
                    else if(value.canConvert<QImage>())
                    {
                        std::stringstream ss;
                        ss << metaObject->className() << "::" << name;
                        SGIHostItemQtPaintDevice item(value.value<QImage>());
                        if (item.hasObject())
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
    case SGIItemTypeProperties:
        {
            if (_item->number() == ~0u)
            {
                const QMetaObject * metaObject = object->metaObject();
                while (metaObject)
                {
                    int offset = metaObject->propertyOffset();
                    int count = metaObject->propertyCount();
                    for (int i = offset; i < count; ++i)
                    {
                        QMetaProperty prop = metaObject->property(i);
						QVariant value = object->property(prop.name());
						bool isSubItem = false;
						if ((QMetaType::Type)value.type() == QMetaType::QObjectStar || value.canConvert<QObject*>())
						{
							QObject* o = value.value<QObject*>();
							if (o)
							{
								SGIHostItemQt item(o);
								treeItem->addChild(prop.name(), &item);
								isSubItem = true;
							}
						}

						if(!isSubItem)
							treeItem->addChild(prop.name(), cloneItem<SGIItemQt>(SGIItemTypeProperties, i));
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
            treeItem->addChild("Palette", cloneItemMulti<SGIItemQt, SGIItemQtPaintDevice>(SGIItemTypePalette, ~0u));
            SGIHostItemQt layout(object->layout());
            if(layout.hasObject())
                treeItem->addChild("Layout", &layout);
            SGIHostItemQt style(object->style());
            if(style.hasObject())
                treeItem->addChild("Style", &style);
        }
        break;
    case SGIItemTypePalette:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QWindow>::build(IObjectTreeItem * treeItem)
{
    QWindow * qobject = getObject<QWindow, SGIItemQt, DynamicCaster>();
    QSurface * surface = getObject<QSurface, SGIItemQtSurface, DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Format", cloneItem<SGIItemQt>(SGIItemTypeSurfaceFormat));

			// add the opposite base-type to the tree
			if (qobject)
			{
				QSurface* s = dynamic_cast<QSurface*>(qobject);
				SGIHostItemQtSurface item(s);
				if(item.hasObject())
					treeItem->addChild("Surface", &item);
			}
#if 0
            else if (surface)
            {
				SGIHostItemQt item(surface);
				if (item.hasObject())
					treeItem->addChild("Object", &item);
			}
#endif
        }
        break;
	case SGIItemTypeSurfaceFormat:
		ret = true;
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
		{
			callNextHandler(treeItem);
            treeItem->addChild("Format", cloneItem<SGIItemQtSurface>(SGIItemTypeSurfaceFormat));

			SGIHostItemQtPlatformSurface platform(object->surfaceHandle());
			if(platform.hasObject())
				treeItem->addChild("SurfaceHandle", &platform);
			ret = true;
        }
        break;
	case SGIItemTypeSurfaceFormat:
		ret = true;
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
            treeItem->addChild("Environment", cloneItem<SGIItemQt>(SGIItemTypeSystemEnvironment));

            SGIHostItemQt eventDispatcher(object->eventDispatcher());
            if(eventDispatcher.hasObject())
                treeItem->addChild("EventDispatcher", &eventDispatcher);
        }
        break;
    case SGIItemTypeSystemEnvironment:
        ret = true;
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

            SGIHostItemQt style(object->style());
            if (style.hasObject())
                treeItem->addChild("Style", &style);

            SGIHostItemQt clipboard(object->clipboard());
            if(clipboard.hasObject())
                treeItem->addChild("Clipboard", &clipboard);
            
            QWidgetList allWidgets = object->allWidgets();
            if(!allWidgets.isEmpty())
                treeItem->addChild(helpers::str_plus_count("Widgets", allWidgets.size()), cloneItem<SGIItemQt>(SGIItemTypeWidgets));

            treeItem->addChild("Palette", cloneItem<SGIItemQt>(SGIItemTypePalette));
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
    case SGIItemTypePalette:
        ret = true;
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

bool objectTreeBuildImpl<QOpenGLWindow>::build(IObjectTreeItem * treeItem)
{
    QOpenGLWindow * object = getObject<QOpenGLWindow, SGIItemQt>();
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

bool objectTreeBuildImpl<QOpenGLShaderProgram>::build(IObjectTreeItem * treeItem)
{
    QOpenGLShaderProgram * object = getObject<QOpenGLShaderProgram, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            QList<QOpenGLShader *> shaders = object->shaders();
            treeItem->addChild(helpers::str_plus_count("Shaders", shaders.size()), cloneItem<SGIItemQt>(SGIItemTypeShaderProgramShaders));
        }
        break;
    case SGIItemTypeShaderProgramShaders:
        {
            QList<QOpenGLShader *> shaders = object->shaders();
            for(auto shader : shaders)
            {
                SGIHostItemQt item(shader);
                if (item.hasObject())
                    treeItem->addChild(std::string(), &item);
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

bool objectTreeBuildImpl<QOpenGLShader>::build(IObjectTreeItem * treeItem)
{
    QOpenGLShader * object = getObject<QOpenGLShader, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            treeItem->addChild("Source", cloneItem<SGIItemQt>(SGIItemTypeShaderSourceCode));
            treeItem->addChild("Log", cloneItem<SGIItemQt>(SGIItemTypeShaderLog));
        }
        break;
    case SGIItemTypeShaderSourceCode:
        ret = true;
        break;
    case SGIItemTypeShaderLog:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QIcon>::build(IObjectTreeItem * treeItem)
{
    QIcon * object = getObject<QIcon, SGIItemQtIcon>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        treeItem->addChild("Theme", cloneItem<SGIItemQtIcon>(SGIItemTypeTheme));
        ret = true;
        break;
    case SGIItemTypeTheme:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QSystemTrayIcon>::build(IObjectTreeItem * treeItem)
{
    QSystemTrayIcon * object = getObject<QSystemTrayIcon, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemQt contextMenu(object->contextMenu());
            if (contextMenu.hasObject())
                treeItem->addChild("ContextMenu", &contextMenu);
            ret = true;
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
			if(object->context())
				treeItem->addChild("Context", cloneItem<SGIItemQt>(SGIItemTypeContext));
			if (object->overlayContext())
				treeItem->addChild("OverlayContext", cloneItem<SGIItemQt>(SGIItemTypeOverlayContext));
		}
        break;
	case SGIItemTypeContext:
		{
			auto ctx = object->context();
			if (ctx)
			{
				SGIHostItemQtPaintDevice device(ctx->device());
				treeItem->addChild("Device", &device);
				SGIHostItemQt contextHandle(ctx->contextHandle());
				treeItem->addChild("ContextHandle", &contextHandle);
			}
			ret = true;
		}
		break;
	case SGIItemTypeOverlayContext:
		{
			auto ctx = object->overlayContext();
			if (ctx)
			{
				SGIHostItemQtPaintDevice device(ctx->device());
				treeItem->addChild("Device", &device);
				SGIHostItemQt contextHandle(ctx->contextHandle());
				treeItem->addChild("ContextHandle", &contextHandle);
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
#endif // WITH_QTOPENGL

bool objectTreeBuildImpl<QLayout>::build(IObjectTreeItem* treeItem)
{
    QLayout* object = getObject<QLayout, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            treeItem->addChild(helpers::str_plus_count("Items", object->count()), cloneItem<SGIItemQt>(SGIItemTypeLayoutItem, ~0u));
        }
        break;
    case SGIItemTypeLayoutItem:
        {
            if (itemNumber() == ~0u)
            {
                for (int n = 0; n < object->count(); ++n)
                {
                    auto* item = object->itemAt(n);
                    treeItem->addChild(qt_helpers::getObjectNameAndType(item), cloneItem<SGIItemQt>(SGIItemTypeLayoutItem, n));
                }
            }
            else
            {
                auto* item = object->itemAt(itemNumber());
                if(item)
                {
                    if(QWidget * w = item->widget())
                    {
                        SGIHostItemQt item(w);
                        treeItem->addChild(std::string(), &item);
                    }
                    else if (QLayout * l = item->layout())
                    {
                        SGIHostItemQt item(l);
                        treeItem->addChild(std::string(), &item);
                    }
                    else if (QSpacerItem * s = item->spacerItem())
                    {
                        //SGIHostItemQt item(s);
                        //treeItem->addChild(std::string(), &item);
                    }
                }

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

bool objectTreeBuildImpl<QPlatformSurface>::build(IObjectTreeItem* treeItem)
{
	QPlatformSurface* object = getObject<QPlatformSurface, SGIItemQtPlatformSurface>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = true;
		break;
	}
	return ret;
}

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
