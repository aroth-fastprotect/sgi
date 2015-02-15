#pragma once

#include <sgi/plugins/SGIPluginMacros.h>

namespace osg {
    class Camera;
    class View;
}

namespace sgi {
namespace osg_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_TEMPLATE()
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(osg::Camera)
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(osg::View)

} // namespace osg_plugin
} // namespace sgi
