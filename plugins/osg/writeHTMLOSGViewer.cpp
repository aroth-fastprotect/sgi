#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSG.h"

// osgViewer headers
#include <osgViewer/CompositeViewer>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>

#ifdef SGI_USE_OSGQT
// osgQt headers
#include <osgQt/GraphicsWindowQt>
#endif

#include "SGIItemOsg"
#include <sgi/helpers/osg>
#include "osgviewer_accessor.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::ViewerBase)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::CompositeViewer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::View)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::GraphicsWindow)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::Renderer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::Scene)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::HelpHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::StatsHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::WindowSizeHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::ThreadingHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::RecordCameraPathHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::LODScaleHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::ToggleSyncToVBlankHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::ScreenCaptureHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgViewer::InteractiveImageHandler)

#ifdef SGI_USE_OSGQT
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgQt::GraphicsWindowQt)
#endif

using namespace sgi::osg_helpers;

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osgViewer::ViewerBase::ThreadingModel t)
{
    switch(t)
    {
    case osgViewer::ViewerBase::SingleThreaded: os << "SingleThreaded"; break;
    case osgViewer::ViewerBase::CullDrawThreadPerContext: os << "CullDrawThreadPerContext"; break;
    case osgViewer::ViewerBase::DrawThreadPerContext: os << "DrawThreadPerContext"; break;
    case osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext: os << "CullThreadPerCameraDrawThreadPerContext"; break;
    case osgViewer::ViewerBase::AutomaticSelection: os << "AutomaticSelection"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osgViewer::ViewerBase::FrameScheme t)
{
    switch(t)
    {
    case osgViewer::ViewerBase::ON_DEMAND: os << "ON_DEMAND"; break;
    case osgViewer::ViewerBase::CONTINUOUS: os << "CONTINUOUS"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osgViewer::ViewerBase>::process(std::basic_ostream<char>& os)
{
    ViewerBaseAccess * object = static_cast<ViewerBaseAccess*>(getObject<osgViewer::ViewerBase, SGIItemOsg, DynamicCaster>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining node properties
            os << "<tr><td>threadingModel</td><td>" << object->getThreadingModel() << "</td></tr>" << std::endl;
            os << "<tr><td>bestThreadingModel</td><td>" << object->suggestBestThreadingModel() << "</td></tr>" << std::endl;
            os << "<tr><td>threadsRunning</td><td>" << (object->areThreadsRunning()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>done</td><td>" << (object->done()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>keyEventSetsDone</td><td>" << object->getKeyEventSetsDone() << "</td></tr>" << std::endl;
            os << "<tr><td>quitEventSetsDone</td><td>" << object->getQuitEventSetsDone() << "</td></tr>" << std::endl;
            os << "<tr><td>releaseContextAtEndOfFrameHint</td><td>" << (object->getReleaseContextAtEndOfFrameHint()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>runFrameScheme</td><td>" << object->getRunFrameScheme() << "</td></tr>" << std::endl;
            os << "<tr><td>runMaxFrameRate</td><td>" << object->getRunMaxFrameRate() << "</td></tr>" << std::endl;
            os << "<tr><td>requestRedraw</td><td>" << (object->requestRedraw()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>requestContinousUpdate</td><td>" << (object->requestContinousUpdate()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseScenes:
        {
            osgViewer::ViewerBase::Scenes list;
            object->getScenes(list);
            os << "<ol>";
            for(auto it = list.begin(); it != list.end(); it++)
            {
                os << "<li>" << getObjectNameAndType(*it) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseViews:
        {
            osgViewer::ViewerBase::Views list;
            object->getViews(list);
            os << "<ol>";
            for(auto it = list.begin(); it != list.end(); it++)
            {
                os << "<li>" << getObjectNameAndType(*it) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseCameras:
        {
            osgViewer::ViewerBase::Cameras list;
            object->getCameras(list);
            os << "<ol>";
            for(auto it = list.begin(); it != list.end(); it++)
            {
                os << "<li>" << getObjectNameAndType(*it) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseContexts:
        {
            osgViewer::ViewerBase::Contexts list;
            object->getContexts(list);
            os << "<ol>";
            for(auto it = list.begin(); it != list.end(); it++)
            {
                os << "<li>" << getObjectNameAndType(*it) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::CompositeViewer>::process(std::basic_ostream<char>& os)
{
    osgViewer::CompositeViewer * object = dynamic_cast<osgViewer::CompositeViewer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining osgViewer::ViewerBase properties

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::View>::process(std::basic_ostream<char>& os)
{
    bool brief = true;
    osgViewer::View * object = dynamic_cast<osgViewer::View*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::View properties first
            callNextHandler(os);

            // add remaining osgViewer::View properties
            os << "<tr><td>start tick</td><td>" << object->getStartTick() << "</td></tr>" << std::endl;
            os << "<tr><td>scene data</td><td>" << getObjectNameAndType(object->getSceneData(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>database pager</td><td>" << getObjectNameAndType(object->getDatabasePager(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>image pager</td><td>" << getObjectNameAndType(object->getImagePager(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>event queue</td><td>" << getObjectNameAndType(object->getEventQueue()) << "</td></tr>" << std::endl;
            os << "<tr><td>camera manipulator</td><td>" << getObjectNameAndType(object->getCameraManipulator(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>display settings</td><td>" << getObjectNameAndType(object->getDisplaySettings()) << "</td></tr>" << std::endl;
            os << "<tr><td>fusion distance mode</td><td>" << object->getFusionDistanceMode() << "</td></tr>" << std::endl;
            os << "<tr><td>fusion distance</td><td>" << object->getFusionDistanceValue() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add view callbacks first
            callNextHandler(os);

            // add osgViewer::View callbacks (event handlers) now
            os << "<tr><td>event handlers</td><td><ol>" << std::endl;
            const osgViewer::View::EventHandlers& handlers = object->getEventHandlers();
            for(osgViewer::View::EventHandlers::const_iterator it = handlers.begin(); it != handlers.end(); it++)
            {
                os << "<li>" << getObjectNameAndType((*it).get(), true) << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDevices:
        {
            os << "<ol>";
            const osgViewer::View::Devices& devices = object->getDevices();
            for(osgViewer::View::Devices::const_iterator it = devices.begin(); it != devices.end(); it++)
            {
                const osgGA::Device * device = *it;
                os << "<li>" << getObjectNameAndType(device, true) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeEventHandlers:
        {
            os << "<ol>";
            const osgViewer::View::EventHandlers& eventHandlers = object->getEventHandlers();
            for(osgViewer::View::EventHandlers::const_iterator it = eventHandlers.begin(); it != eventHandlers.end(); it++)
            {
                os << "<li>" << getObjectNameAndType(*it, true) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::GraphicsWindow>::process(std::basic_ostream<char>& os)
{
    osgViewer::GraphicsWindow * object = static_cast<osgViewer::GraphicsWindow*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::GraphicsContext properties first
            callNextHandler(os);

            // add remaining osgViewer::GraphicsWindow properties
            os << "<tr><td>window name</td><td>" << object->getWindowName() << "</td></tr>" << std::endl;
            os << "<tr><td>windowDecoration</td><td>" << (object->getWindowDecoration()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>syncToVBlank</td><td>" << (object->getSyncToVBlank()?"true":"false") << "</td></tr>" << std::endl;

            int x = 0, y = 0, width = 0, height = 0;
            object->getWindowRectangle(x, y, width, height);
            os << "<tr><td>x</td><td>" << x << "</td></tr>" << std::endl;
            os << "<tr><td>y</td><td>" << y << "</td></tr>" << std::endl;
            os << "<tr><td>width</td><td>" << width << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << height << "</td></tr>" << std::endl;
            os << "<tr><td>aspect ratio</td><td>" << ((float)width/(float)height) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::Renderer>::process(std::basic_ostream<char>& os)
{
    osgViewer::Renderer * object = dynamic_cast<osgViewer::Renderer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::GraphicsOperation properties first
            callNextHandler(os);

            // add remaining osgViewer::Renderer properties
            os << "<tr><td>done</td><td>" << (object->getDone()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>graphicsThreadDoesCull</td><td>" << (object->getGraphicsThreadDoesCull()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>compileOnNextDraw</td><td>" << (object->getCompileOnNextDraw()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>cameraRequiresSetUp</td><td>" << (object->getCameraRequiresSetUp()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::Scene>::process(std::basic_ostream<char>& os)
{
    osgViewer::Scene * object = static_cast<osgViewer::Scene*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::Scene properties

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::HelpHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::HelpHandler * object = getObject<osgViewer::HelpHandler,SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::HelpHandler properties
            os << "<tr><td>camera</td><td>" << getObjectNameAndType(object->getCamera(), true) << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::StatsHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::StatsHandler * object = getObject<osgViewer::StatsHandler,SGIItemOsg, DynamicCaster>();
    osgViewerStatsHandlerAccess * access = static_cast<osgViewerStatsHandlerAccess*>(object);
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::StatsHandler properties
            os << "<tr><td>camera</td><td>" << getObjectNameAndType(object->getCamera(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>switch</td><td>" << getObjectNameAndType(access->getSwitch(), true) << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::WindowSizeHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::WindowSizeHandler * object = getObject<osgViewer::WindowSizeHandler,SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::WindowSizeHandler properties

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::ThreadingHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::ThreadingHandler * object = getObject<osgViewer::ThreadingHandler,SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::ThreadingHandler properties

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::RecordCameraPathHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::RecordCameraPathHandler * object = getObject<osgViewer::RecordCameraPathHandler,SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::RecordCameraPathHandler properties

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::LODScaleHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::LODScaleHandler * object = getObject<osgViewer::LODScaleHandler,SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::LODScaleHandler properties

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::ToggleSyncToVBlankHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::ToggleSyncToVBlankHandler * object = getObject<osgViewer::ToggleSyncToVBlankHandler,SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::ToggleSyncToVBlankHandler properties

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<osgViewer::ScreenCaptureHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::ScreenCaptureHandler * object = getObject<osgViewer::ScreenCaptureHandler, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
    {
        if (_table)
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

        callNextHandler(os);

        // add remaining osgViewer::ScreenCaptureHandler properties

        if (_table)
            os << "</table>" << std::endl;
        ret = true;
    }
    break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgViewer::InteractiveImageHandler>::process(std::basic_ostream<char>& os)
{
    osgViewer::InteractiveImageHandler * object = getObject<osgViewer::InteractiveImageHandler,SGIItemOsg,DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add remaining osgViewer::InteractiveImageHandler properties

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#ifdef SGI_USE_OSGQT
bool writePrettyHTMLImpl<osgQt::GraphicsWindowQt>::process(std::basic_ostream<char>& os)
{
    osgQt::GraphicsWindowQt * object = static_cast<osgQt::GraphicsWindowQt*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgViewer::GraphicsWindow properties first
            callNextHandler(os);

            // add remaining osgQt::GraphicsWindowQt properties
            os << "<tr><td>GL widget</td><td>" << (void*)object->getGLWidget() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}
#endif

} // namespace osg_plugin
} // namespace sgi

