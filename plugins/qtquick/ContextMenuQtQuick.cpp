#include "stdafx.h"
#include "ContextMenuQtQuick.h"
#include <sgi/plugins/SGIItemQt>
#include "SGIItemQtQuick"
#include "MenuActionQtQuick.h"
#include <QtQml/QQmlContext>

namespace sgi {

namespace qtquick_plugin {

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(QQmlContext)

bool contextMenuPopulateImpl<QQmlContext>::populate(IContextMenuItem * menuItem)
{
    QQmlContext* object = getObject<QQmlContext,SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

} // namespace qtquick_plugin
} // namespace sgi
