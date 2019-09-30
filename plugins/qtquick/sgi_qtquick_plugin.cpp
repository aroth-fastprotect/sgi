#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/GenerateItemImpl>
#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SGIItemQt>
#include "SGIItemQtQuick"

#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>
#include <QtQuickWidgets/QQuickWidget>

#include "writeHTMLQtQuick.h"
#include "getObjectInfoQtQuick.h"
#include "ObjectTreeQtQuick.h"
#include "ContextMenuQtQuick.h"
#include "MenuActionQtQuick.h"
#include "SettingsDialogQtQuick.h"

SGI_OBJECT_INFO_BEGIN(QObject)
    QWidget,
    QQmlContext,
    QQmlComponent,
    QJSEngine,
    QQuickItem
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QJSEngine)
    QQmlEngine
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QWidget)
    QQuickWidget
SGI_OBJECT_INFO_END()

namespace sgi {

namespace qtquick_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(QObject)
GENERATE_IMPL_NO_ACCEPT(QWidget)
GENERATE_IMPL_NO_ACCEPT(sgi::details::Referenced)

typedef generateItemImplT<generateItemAcceptImpl, SGIItemInternal, SGIItemQt> generateItemImpl;

typedef SGIPluginImplementationT<       generateItemImpl,
                                        writePrettyHTMLImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectPathImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        defaultPluginWriteObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl,
                                        settingsDialogCreateImpl,
                                        defaultPluginGuiAdapterParentWidgetImpl,
                                        defaultPluginGuiAdapterSetViewImpl,
                                        defaultPluginGetOrCreateLoggerImpl
                                        >
    SGIPluginImpl;

} // namespace qtquick_plugin

class SGIPlugin_qtquick_Implementation : public qtquick_plugin::SGIPluginImpl
{
public:
    SGIPlugin_qtquick_Implementation(SGIPluginHostInterface * hostInterface=nullptr)
        : qtquick_plugin::SGIPluginImpl(hostInterface)
    {
		SGIITEMTYPE_NAME(SGIItemTypeContextProperties);
    }
    SGIPlugin_qtquick_Implementation(const SGIPlugin_qtquick_Implementation& rhs)
        : qtquick_plugin::SGIPluginImpl(rhs)
    {
    }

    SGI_Object(sgi_qtquick, SGIPlugin_qtquick_Implementation)
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(qtquick)
