#include "stdafx.h"
#include "MenuActionQtQuick.h"
#include "SGIItemQtQuick"

#include <iostream>
#include <cassert>
#include <QMetaProperty>
#include <QUrl>

namespace sgi {
namespace qtquick_plugin {

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectInfo)

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    assert(callback);
    callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
    return true;
}
} // namespace qtquick_plugin
} // namespace sgi
