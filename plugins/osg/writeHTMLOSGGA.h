#pragma once

namespace osgGA {
    class EventHandler;
    class GUIEventHandler;
    class GUIEventAdapter;
    class CameraManipulator;
}

namespace osgUtil {
    class SceneView;
}

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE(osgGA::EventHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgGA::GUIEventHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgGA::GUIEventAdapter)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgGA::CameraManipulator)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgUtil::SceneView)

} // namespace osg_plugin
} // namespace sgi
