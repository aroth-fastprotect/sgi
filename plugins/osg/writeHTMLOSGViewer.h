#pragma once

namespace osgViewer {
    class ViewerBase;
    class CompositeViewer;
    class View;
    class GraphicsWindow;
    class Renderer;
    class Scene;
    class HelpHandler;
    class StatsHandler;
    class WindowSizeHandler;
    class ThreadingHandler;
    class RecordCameraPathHandler;
    class LODScaleHandler;
    class ToggleSyncToVBlankHandler;
    class ScreenCaptureHandler;
    class InteractiveImageHandler;
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
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::HelpHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::StatsHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::WindowSizeHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::ThreadingHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::RecordCameraPathHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::LODScaleHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::ToggleSyncToVBlankHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::ScreenCaptureHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgViewer::InteractiveImageHandler)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgQt::GraphicsWindowQt)

} // namespace osg_plugin
} // namespace sgi
