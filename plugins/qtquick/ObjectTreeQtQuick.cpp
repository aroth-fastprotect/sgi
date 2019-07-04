#include "stdafx.h"
#include <QtQml/QQmlContext>

#include <QtQuick/QQuickItem>
#include <QtQuickWidgets/QQuickWidget>
#include <QtQml/QQmlEngine>

#include "ObjectTreeQtQuick.h"
#include "SGIItemQtQuick"
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIItemQt>
#include <sgi/helpers/string>
#include <sgi/helpers/qt>

namespace sgi {
namespace qtquick_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QQmlContext)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QQmlEngine)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(QQuickWidget)

using namespace sgi::qt_helpers;

bool objectTreeBuildImpl<QQmlContext>::build(IObjectTreeItem * treeItem)
{
    QQmlContext* object = getObject<QQmlContext, SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemQt parentContext(object->parentContext());
            if (parentContext.hasObject())
                treeItem->addChild("ParentContext", &parentContext);

            SGIHostItemQt engine(object->engine());
            if (engine.hasObject())
                treeItem->addChild("Engine", &engine);

            SGIHostItemQt contextObject(object->contextObject());
            if (contextObject.hasObject())
                treeItem->addChild("ContextObject", &contextObject);

        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QQmlEngine>::build(IObjectTreeItem* treeItem)
{
    QQmlEngine* object = getObject<QQmlEngine, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemQt rootContext(object->rootContext());
            if (rootContext.hasObject())
                treeItem->addChild("RootContext", &rootContext);

#if QT_CONFIG(qml_network)
            SGIHostItemQt networkAccessManager((QObject*)object->networkAccessManager());
            if (networkAccessManager.hasObject())
                treeItem->addChild("NetworkAccessManager", &networkAccessManager);

            SGIHostItemQt networkAccessManagerFactory((QObject*)object->networkAccessManagerFactory());
            if (networkAccessManagerFactory.hasObject())
                treeItem->addChild("NetworkAccessManagerFactory", &networkAccessManagerFactory);
#endif

            SGIHostItemQt urlInterceptor((QObject*)object->urlInterceptor());
            if (urlInterceptor.hasObject())
                treeItem->addChild("UrlInterceptor", &urlInterceptor);

            SGIHostItemQt incubationController((QObject*)object->incubationController());
            if (incubationController.hasObject())
                treeItem->addChild("IncubationController", &incubationController);

        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<QQuickWidget>::build(IObjectTreeItem* treeItem)
{
    QQuickWidget* object = getObject<QQuickWidget, SGIItemQt>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemQt engine(object->engine());
            if(engine.hasObject())
                treeItem->addChild("Engine", &engine);

            SGIHostItemQt rootContext(object->rootContext());
            if (rootContext.hasObject())
                treeItem->addChild("RootContext", &rootContext);

            SGIHostItemQt rootObject(object->rootObject());
            if (rootObject.hasObject())
                treeItem->addChild("RootObject", &rootObject);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

} // namespace qtquick_plugin
} // namespace sgi
