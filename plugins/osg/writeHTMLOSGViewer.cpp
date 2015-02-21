#include <ostream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGViewer.h"

// osgViewer headers
#include <osgViewer/CompositeViewer>
#include <osgViewer/Renderer>

// osgQt headers
#include <osgQt/GraphicsWindowQt>

#include "SGIItemOsg"
#include <sgi/helpers/osg>

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgViewer::ViewerBase)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgViewer::CompositeViewer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgViewer::View)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgViewer::GraphicsWindow)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgViewer::Renderer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgViewer::Scene)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgQt::GraphicsWindowQt)

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

namespace {
    class ViewerBaseAccess : public osgViewer::ViewerBase
    {
    public:
        bool requestRedraw() const { return _requestRedraw; }
        bool requestContinousUpdate() const { return _requestContinousUpdate; }
    };
}

bool writePrettyHTMLImpl<osgViewer::ViewerBase>::process(std::basic_ostream<char>& os)
{
    osgViewer::ViewerBase * object = dynamic_cast<osgViewer::ViewerBase*>(item<SGIItemOsg>()->object());
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
            os << "<tr><td>bestThreadingModel</td><td>" << const_cast<osgViewer::ViewerBase *>(object)->suggestBestThreadingModel() << "</td></tr>" << std::endl;
            os << "<tr><td>threadsRunning</td><td>" << (object->areThreadsRunning()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>done</td><td>" << (object->done()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>keyEventSetsDone</td><td>" << object->getKeyEventSetsDone() << "</td></tr>" << std::endl;
            os << "<tr><td>quitEventSetsDone</td><td>" << object->getQuitEventSetsDone() << "</td></tr>" << std::endl;
            os << "<tr><td>releaseContextAtEndOfFrameHint</td><td>" << (object->getReleaseContextAtEndOfFrameHint()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>runFrameScheme</td><td>" << object->getRunFrameScheme() << "</td></tr>" << std::endl;
            os << "<tr><td>runMaxFrameRate</td><td>" << object->getRunMaxFrameRate() << "</td></tr>" << std::endl;

            const ViewerBaseAccess * access = (const ViewerBaseAccess * )object;
            os << "<tr><td>requestRedraw</td><td>" << (access->requestRedraw()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>requestContinousUpdate</td><td>" << (access->requestContinousUpdate()?"true":"false") << "</td></tr>" << std::endl;

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
            os << "<tr><td>scene data</td><td>";
            const osg::Node * sceneData = object->getSceneData();
            if(sceneData)
                os << getObjectName(sceneData) << " (" << getObjectTypename(sceneData) << ")" << std::endl;
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>database pager</td><td>";
            osgDB::DatabasePager * dbpager = object->getDatabasePager();
            if(dbpager)
                os << getObjectName(dbpager) << " (" << getObjectTypename(dbpager) << ")" << std::endl;
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>image pager</td><td>";
            osgDB::ImagePager * imgpager = object->getImagePager();
            if(imgpager)
                os << getObjectName(imgpager) << " (" << getObjectTypename(imgpager) << ")" << std::endl;
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>event queue</td><td>";
            osgGA::EventQueue * evtqueue = object->getEventQueue();
            if(evtqueue)
                os << getObjectName(evtqueue) << " (" << getObjectTypename(evtqueue) << ")" << std::endl;
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>camera manipulator</td><td>";
            osgGA::CameraManipulator * cammani = object->getCameraManipulator();
            if(cammani)
                os << getObjectName(cammani) << " (" << getObjectTypename(cammani) << ")" << std::endl;
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>display settings</td><td>";
            osg::DisplaySettings * dispsettings = object->getDisplaySettings();
            if(dispsettings)
                os << getObjectName(dispsettings) << " (" << getObjectTypename(dispsettings) << ")" << std::endl;
            else
                os << "&lt;null&gt;";
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
                os << "<li>" << getObjectNameAndType((*it).get()) << "</li>" << std::endl;
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
                os << "<li>" << getObjectNameAndType(device) << "</li>" << std::endl;
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
                os << "<li>" << getObjectNameAndType(*it) << "</li>" << std::endl;
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
            if(object)
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

} // namespace osg_plugin
} // namespace sgi

