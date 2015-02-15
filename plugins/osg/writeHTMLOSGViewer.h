#pragma once

namespace osgViewer {
    class ViewerBase;
    class CompositeViewer;
    class View;
    class GraphicsWindow;
    class Renderer;
    class Scene;
}

namespace osgQt {
    class GraphicsWindowQt;
}

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::ViewerBase)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::CompositeViewer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::View)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::GraphicsWindow)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::Renderer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::Scene)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgQt::GraphicsWindowQt)

} // namespace osg_plugin
} // namespace sgi
