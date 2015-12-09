#pragma once

#include <sgi/plugins/SGIPluginMacros.h>

namespace osg {
    class Camera;
    class View;
}

namespace osgGA {
	class CameraManipulator;
	class TrackballManipulator;
}

namespace sgi {
namespace osg_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_TEMPLATE()
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(osg::Camera)
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(osg::View)
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(osgGA::CameraManipulator)

} // namespace osg_plugin
} // namespace sgi
