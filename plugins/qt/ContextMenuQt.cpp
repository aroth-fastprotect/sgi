#include "stdafx.h"
#include "ContextMenuQt.h"
#include <sgi/plugins/SGIHostItemQt.h>
#include "SGIItemQt"
#include "MenuActionQt.h"
#include <QMetaClassInfo>
#include <QWidget>
#include <QWindow>
#include <QSystemTrayIcon>
#include <QLayout>
#include <sgi/helpers/string>
#include <sgi/helpers/qt>

namespace sgi {

namespace qt_plugin {

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QObject)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QWidget)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QWindow)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QMetaObject)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QPaintDevice)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QImage)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QIcon)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QSystemTrayIcon)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QLayout)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QWindow)

using namespace sgi::qt_helpers;

bool contextMenuPopulateImpl<QObject>::populate(IContextMenuItem * menuItem)
{
    QObject * object = getObject<QObject,SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            menuItem->addSimpleAction(MenuActionObjectInfo, "Object info...", _item);
            menuItem->addSimpleAction(MenuActionDumpObjectInfo, "Dump object info", _item);
            menuItem->addSimpleAction(MenuActionDumpObjectTree, "Dump object tree", _item);

            SGIHostItemQtMeta metaObject(object->metaObject());
            if(metaObject.hasObject())
                menuItem->addMenu("Meta object", &metaObject); 

            menuItem->addMenu("Properties", cloneItem<SGIItemQt>(SGIItemTypeProperties, ~0u));
            menuItem->addMenu("Methods", cloneItem<SGIItemQt>(SGIItemTypeMethods, ~0u));

            SGIHostItemQt parent(object->parent());
            if(parent.hasObject())
                menuItem->addMenu("Parent", &parent);

            const QObjectList & children = object->children();
            if(!children.empty())
                menuItem->addMenu(helpers::str_plus_count("Childs", children.size()), cloneItem<SGIItemQt>(SGIItemTypeChilds));
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            const QObjectList & children = object->children();
            for(QObjectList::const_iterator it = children.begin(); it != children.end(); it++)
            {
                SGIHostItemQt childItem(*it);
                menuItem->addMenu(std::string(), &childItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeProperties:
        {
			const QMetaObject * metaObject = object->metaObject();
			while (metaObject)
			{
				int propertyOffset = metaObject->propertyOffset();
				int propertyCount = metaObject->propertyCount();
				std::vector<std::pair<int, const char*> > properties(propertyCount - propertyOffset);
				for (int i = propertyOffset; i < propertyCount; ++i)
				{
					QMetaProperty metaproperty = metaObject->property(i);
					properties[i - propertyOffset] = std::make_pair(i, metaproperty.name());
				}
				std::sort(properties.begin(), properties.end(), [](std::pair<int, const char*> const & lhs, std::pair<int, const char*> const & rhs) { return qstricmp(lhs.second, rhs.second) < -1; });
				for(const std::pair<int, const char*> & prop : properties)
				{
					if(_item->number() != ~0u && prop.first != _item->number())
						continue;
					QMetaProperty metaproperty = metaObject->property(prop.first);
					QVariant value = object->property(prop.second);
					bool isSubMenu = false;

					std::stringstream ss;
					ss << metaObject->className() << "::" << prop.second << "=";

					if ((QMetaType::Type)value.type() == QMetaType::QObjectStar || value.canConvert<QObject*>())
					{
						QObject* o = value.value<QObject*>();
						if (o)
						{
							SGIHostItemQt item(o);
							menuItem->addMenu(std::string(), &item);
							isSubMenu = true;
						}
						else
							ss << "QObject*(nullptr)";
					}
					else if (value.type() == QVariant::Icon)
						ss << "QIcon()";
					else if(value.type() == QVariant::Palette)
						ss << "QPalette()";
					else
						writeVariant(ss, value, &metaproperty);

					if(!isSubMenu)
						menuItem->addSimpleAction(MenuActionObjectModifyProperty, ss.str(), _item, new ReferencedDataString(prop.second));
				}
				metaObject = metaObject->superClass();
			}

            ret = true;
        }
        break;
    case SGIItemTypeMethods:
        {
            const QMetaObject * metaObject = object->metaObject();
            while(metaObject)
            {
                int methodOffset = metaObject->methodOffset();
                int methodCount = metaObject->methodCount();
                std::vector<QMetaMethod> methods(methodCount - methodOffset);
                for (int i = methodOffset; i<methodCount; ++i)
                    methods[i - methodOffset] = metaObject->method(i);
                std::sort(methods.begin(), methods.end(), [](const QMetaMethod & lhs, const QMetaMethod & rhs) { return qstricmp(lhs.name(), rhs.name()) < -1; });
                for (const QMetaMethod & method : methods)
                {
                    if (method.name() == QString("deleteLater") || method.name().at(0) == QChar('_') || method.methodType() == QMetaMethod::Signal)
                        continue;

                    std::stringstream ss;
                    ss << metaObject->className() << "::" << method.methodSignature().toStdString();
                    menuItem->addSimpleAction(MenuActionObjectMethodInvoke, ss.str(), _item, new ReferencedDataMetaMethod(method));
                }
                metaObject = metaObject->superClass();
            }
            ret = true;
        }
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QWidget>::populate(IContextMenuItem * menuItem)
{
    QWidget * object = getObjectMulti<QWidget, SGIItemQt,SGIItemQtPaintDevice>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionWidgetGrab, "Grab widget", _item);
            menuItem->addSimpleAction(MenuActionWidgetHighlight, "Highlight", _item);
            menuItem->addBoolAction(MenuActionWidgetSetVisibility, object->isVisible() ? "Hide" : "Show", _item, object->isVisible());
            menuItem->addBoolAction(MenuActionWidgetSetEnabled, "Enabled", _item, object->isEnabled());
            menuItem->addBoolAction(MenuActionWidgetSetAcceptDrops, "Accept drops", _item, object->acceptDrops());
            menuItem->addBoolAction(MenuActionWidgetSetAutoFillBackground, "Auto fill background", _item, object->autoFillBackground());

            SGIHostItemQt layout(object->layout());
            if(layout.hasObject())
                menuItem->addMenu("Layout", &layout);

/*
            if(object->parentWidget() != object->parent())
            {
                SGIHostItemQt parentWidget(object->parentWidget());
                if(parentWidget.hasObject())
                    menuItem->addMenu("ParentWidget", &parentWidget);
            }*/
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QWindow>::populate(IContextMenuItem * menuItem)
{
    QWindow * object = getObjectMulti<QWindow, SGIItemQt, SGIItemQtSurface>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(menuItem);
		if (ret)
		{
		}
		break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<QMetaObject>::populate(IContextMenuItem * menuItem)
{
    QMetaObject * object = getObject<QMetaObject,SGIItemQtMeta>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            menuItem->addSimpleAction(MenuActionObjectInfo, "Object info...", _item);

            SGIHostItemQtMeta superClass(object->superClass());
            if(superClass.hasObject())
                menuItem->addMenu(helpers::str_plus_info("Super class", object->superClass()->className()), &superClass);
            ret = true;
        }
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QPaintDevice>::populate(IContextMenuItem * menuItem)
{
    QPaintDevice * object = getObject<QPaintDevice,SGIItemQtPaintDevice>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            menuItem->addSimpleAction(MenuActionObjectInfo, "Object info...", _item);
            ret = true;
        }
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QImage>::populate(IContextMenuItem * menuItem)
{
    QImage * object = getObject<QImage,SGIItemQtPaintDevice>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionImagePreview, "Preview...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QIcon>::populate(IContextMenuItem * menuItem)
{
    QIcon * object = getObject<QIcon,SGIItemQtIcon>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        menuItem->addSimpleAction(MenuActionImagePreview, "Preview...", _item);
        menuItem->addSimpleAction(MenuActionIconFromTheme, "From theme...", _item);
        ret = true;
        break;
    default:
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QSystemTrayIcon>::populate(IContextMenuItem * menuItem)
{
    QSystemTrayIcon * object = getObject<QSystemTrayIcon,SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            SGIHostItemQt contextMenu(object->contextMenu());
            if (contextMenu.hasObject())
                menuItem->addMenu("ContextMenu", &contextMenu);
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QLayout>::populate(IContextMenuItem * menuItem)
{
    QLayout * object = getObject<QLayout,SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            menuItem->addMenu(helpers::str_plus_count("Items", object->count()), cloneItem<SGIItemQt>(SGIItemTypeLayoutItem, ~0u));
        }
        break;
    case SGIItemTypeLayoutItem:
        {
            if (itemNumber() == ~0u)
            {
                for (int n = 0; n < object->count(); ++n)
                {
                    auto* item = object->itemAt(n);
                    menuItem->addMenu(qt_helpers::getObjectNameAndType(item), cloneItem<SGIItemQt>(SGIItemTypeLayoutItem, n));
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
                        menuItem->addMenu(std::string(), &item);
                    }
                    else if (QLayout * l = item->layout())
                    {
                        SGIHostItemQt item(l);
                        menuItem->addMenu(std::string(), &item);
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
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<QWindow>::populate(IContextMenuItem* menuItem)
{
	QWindow* qobject = getObject<QWindow, SGIItemQt, DynamicCaster>();
	QSurface* surface = getObject<QSurface, SGIItemQtSurface, DynamicCaster>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(menuItem);
		if (ret)
		{
			SGIHostItemQt parent(qobject ? qobject->parent() : nullptr);
			if (parent.hasObject())
				menuItem->addMenu("Parent", &parent);

			// add the opposite base-type to the tree
			if (qobject)
			{
				QSurface* s = dynamic_cast<QSurface*>(qobject);
				SGIHostItemQtSurface item(s);
				if (item.hasObject())
					menuItem->addMenu("Surface", &item);
			}
		}
		break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}
} // namespace qt_plugin
} // namespace sgi
