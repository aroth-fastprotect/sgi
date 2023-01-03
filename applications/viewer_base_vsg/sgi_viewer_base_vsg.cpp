#include "sgi_viewer_base_vsg.h"

#include <vsg/utils/CommandLine.h>
#include <vsg/io/Logger.h>


void initializeNotifyLevels(vsg::CommandLine & arguments)
{
    std::string osgnotifylevel;
#ifdef SGI_USE_OSGEARTH
    std::string osgearthnotifylevel;
#endif
    if (arguments.read("--debug"))
    {
        osgnotifylevel = "debug";
#ifdef SGI_USE_OSGEARTH
        osgearthnotifylevel = "debug";
#endif
    }
    else
    {
        if (!arguments.read("--osgdebug", osgnotifylevel))
            osgnotifylevel.clear();
#ifdef SGI_USE_OSGEARTH
        if (!arguments.read("--earthdebug", osgearthnotifylevel))
            osgearthnotifylevel.clear();
#endif
    }
    if (!osgnotifylevel.empty())
    {
        /*
        vsg::NotifySeverity level = severityFromString(osgnotifylevel);
        if (level >= 0)
            osg::setNotifyLevel(level);
*/
    }
#ifdef SGI_USE_OSGEARTH
    if (!osgearthnotifylevel.empty())
    {
        /*
        osg::NotifySeverity level = severityFromString(osgearthnotifylevel);
        if (level >= 0)
            osgEarth::setNotifyLevel(level);
*/
    }
#endif
}

sgi_CommonHelper::sgi_CommonHelper(vsg::CommandLine& args)
    : glprofile(GLContextProfileNone)
    , glversion()
    , addSceneGraphInspector(true)
    , showSceneGraphInspector(true)
{
    args.read("--glver", glversion);
    if (args.read("--core"))
        glprofile = GLContextProfileCore;
    if (args.read("--compat"))
        glprofile = GLContextProfileCompatibility;

    if (args.read("--nosgi"))
        addSceneGraphInspector = false;
    if (args.read("--hidesgi"))
        showSceneGraphInspector = false;
}

sgi_CommonHelper::sgi_CommonHelper(const sgi_CommonHelper & rhs)
    : glprofile(rhs.glprofile)
    , glversion(rhs.glversion)
    , addSceneGraphInspector(rhs.addSceneGraphInspector)
    , showSceneGraphInspector(rhs.showSceneGraphInspector)
{
}

sgi_CommonHelper::~sgi_CommonHelper()
{
}

sgi_MapNodeHelper::sgi_MapNodeHelper(vsg::CommandLine& args)
    : sgi_CommonHelper(args)
    , m_errorMessages()
    , m_files()
    , _onlyImages(false)
    , _addKeyDumper(false)
    , _addMouseDumper(false)
    , _movieTrackMouse(false)
    , _viewpointNum(-1)
    , _viewpointName()
{
}

sgi_MapNodeHelper::sgi_MapNodeHelper(const sgi_MapNodeHelper & rhs)
    : sgi_CommonHelper(rhs)
{
}

sgi_MapNodeHelper::~sgi_MapNodeHelper()
{
}


std::string
sgi_MapNodeHelper::usage() const
{
    std::stringstream ss;
    ss  << "  --osgdebug <level>            : set OSG_NOTIFY_LEVEL to specified level\n";

    ss  << "  --nosgi                       : do not add SceneGraphInspector handle\n"
       << "  --hidesgi                     : do not show SceneGraphInspector after loading\n"
       << "  --keys                        : enable keyboard event logging\n"
       << "  --mouse                       : enable mouse event logging\n"
       << "  --track-mouse                 : adjust animation speed by mouse moves\n"
        ;

    return ss.str();
}

std::string sgi_MapNodeHelper::errorMessages() const
{
    return m_errorMessages.str();
}

vsg::Group* sgi_MapNodeHelper::load(
    vsg::CommandLine& args
    )
{
    return nullptr;
}


void sgi_MapNodeHelper::setupEventHandlers(vsg::Viewer* viewer, vsg::Group * root)
{
#if 0
    // add the state manipulator
    view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));

    // add the thread model handler
    view->addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    view->addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    view->addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    view->addEventHandler(new osgViewer::HelpHandler(_usageMessage));

    // add the record camera path handler
    view->addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    view->addEventHandler(new osgViewer::LODScaleHandler);

    // add the screen capture handler
    view->addEventHandler(new osgViewer::ScreenCaptureHandler);

    if (_addKeyDumper)
        view->addEventHandler(new KeyboardDumpHandler);
    if (_addMouseDumper)
        view->addEventHandler(new MouseDumpHandler);

    // pass the model to the MovieEventHandler so it can pick out ImageStream's to manipulate.
    MovieEventHandler* meh = new MovieEventHandler();
    meh->set(root);

    if (_movieTrackMouse)
        meh->setTrackMouse(true);

    view->addEventHandler(meh);
#endif
}

