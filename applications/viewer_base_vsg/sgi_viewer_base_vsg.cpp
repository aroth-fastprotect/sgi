#include "sgi_viewer_base_vsg.h"

#include <vsg/utils/CommandLine.h>
#include <vsg/io/Logger.h>

#include <vsg/nodes/Group.h>
#include <vsg/io/FileSystem.h>
#include <vsg/io/ReaderWriter.h>
#include <vsg/io/read.h>

#include <sgi/helpers/vsg_helper_nodes>

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

bool sgi_MapNodeHelper::load(
    vsg::ref_ptr<vsg::Group> root,
    vsg::CommandLine& args
    )
{
    if (args.read("--keys"))
        _addKeyDumper = true;
    if (args.read("--mouse"))
        _addMouseDumper = true;
    if (args.read("--track-mouse"))
        _movieTrackMouse = true;

#ifdef SGI_USE_OSGEARTH
    if (args.read("--no-oe-lighting")) {
      //  _useOELighting = false;
    }
#endif

    if (!args.read("--viewpoint", _viewpointNum))
    {
        _viewpointNum = -1;
        if (!args.read("--viewpoint", _viewpointName))
            _viewpointName.clear();
    }

    //_usageMessage = args.getApplicationUsage();


    auto options = vsg::Options::create();
    //osgDB::Registry * registry = osgDB::Registry::instance();
    bool hasAtLeastOneNode = false;
    bool hasAtLeastOneObject = false;
    bool hasAtLeastOneImage = false;

    // a root node to hold everything:
    bool previousWasOption = false;
    // check if the args only contain the executable name and nothing else
    bool emptyArgs = args.argc() <= 1;
    if(!root)
        root = vsg::Group::create();
    // load all files from the given args
    for (int i = 1; i < args.argc(); )
    {
        bool argRemoved = false;
        // skip all dash arguments (switches, flags, options, etc)
        if (args[i][0] != '-')
        {
            if (!previousWasOption)
            {
                std::string arg = args[i];
                std::string protocol;

                const char * scheme_sep = strstr(args[i], "://");
                if (scheme_sep)
                {
                    std::string schema = std::string(args[i], scheme_sep - args[i]);
                    if (schema == "file")
                        arg = &args[i][7];
                    else
                    {
                        protocol = schema;
                    }
                }

                vsg::Path lowercaseExt = vsg::lowerCaseFileExtension(arg);
                if (!lowercaseExt.empty())
                {
                    auto vsg_obj = vsg::read_cast<vsg::Object>(arg, options);
                    if (vsg_obj)
                    {
                        if (vsg::Node * node = dynamic_cast<vsg::Node*>(vsg_obj.get()))
                        {
                            hasAtLeastOneNode = true;
                            vsg::ref_ptr<vsg::Node> ref_node(node);
                            sgi::vsg_helpers::tagNodeForObjectTree(node, arg);
                            root->addChild(ref_node);
                        }
                        else if (vsg::Image * image = dynamic_cast<vsg::Image*>(vsg_obj.get()))
                        {
                            hasAtLeastOneImage = true;
                            vsg::ref_ptr<vsg::Image> ref_image(image);
                            vsg::ref_ptr<vsg::Node> ref_node(sgi::vsg_helpers::createNodeForImage(image));
                            sgi::vsg_helpers::tagNodeForObjectTree(ref_node.get(), arg);
                            root->addChild(ref_node);
                        }
                        else
                        {
                            hasAtLeastOneObject = true;
                            //osg::UserDataContainer * container = root->getOrCreateUserDataContainer();
                            vsg_obj->setValue("sgi_tree_item", true);
                            //container->addUserObject(vsg_obj);
                        }
                    }
                    args.remove(i, 1);
                    argRemoved = true;
                }
                else
                {
                    // no extension means, we found a parameter to another option.
                    // maybe the window size or something like this.
                }
            }
        }
        if (!argRemoved)
            i++;
    }

    if (!hasAtLeastOneNode && !hasAtLeastOneObject && !hasAtLeastOneImage)
    {
        if(emptyArgs)
            m_errorMessages << "No .earth, 3D model or image file/url specified in the command line. Empty command line arguments." << std::endl;
        else
            m_errorMessages << "No .earth, 3D model or image file/url specified in the command line." << std::endl;
        return false;
    }

    // check if we only got one image and nothing else
    _onlyImages = (!hasAtLeastOneNode && !hasAtLeastOneObject && hasAtLeastOneImage);

    return true;
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

void sgi_MapNodeHelper::setupInitialPosition(vsg::Viewer* viewer) const
{
#if 0
    osgGA::CameraManipulator * manip = view->getCameraManipulator();

#ifdef SGI_USE_OSGEARTH
    osgEarth::Util::EarthManipulator* earth_manip = dynamic_cast<osgEarth::Util::EarthManipulator*>(manip);
#endif
    osgGA::TrackballManipulator* trackball_manip = dynamic_cast<osgGA::TrackballManipulator*>(manip);

#ifdef SGI_USE_OSGEARTH
    if (earth_manip)
    {
        osgEarth::Viewpoint selectedViewpoint;
        bool gotViewpoint = false;

        osg::ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::findMapNode(view->getSceneData());
        if (mapNode.valid())
        {
            // look for external data in the map node:
            const osgEarth::Config& externals = mapNode->externalConfig();
            osgEarth::Config        viewpointsConf = externals.child("viewpoints");

            // backwards-compatibility: read viewpoints at the top level:
            const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
            for (osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i)
                viewpointsConf.add(*i);

            const osgEarth::ConfigSet& children = viewpointsConf.children();
            if (children.size() > 0)
            {
                int currentViewpointNum = 0;
                for (osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i, ++currentViewpointNum)
                {
                    osgEarth::Viewpoint curvp(*i);
                    if (currentViewpointNum == _viewpointNum || (!_viewpointName.empty() && iequals(curvp.name().value(), _viewpointName)))
                    {
                        gotViewpoint = true;
                        selectedViewpoint = curvp;
                    }
                }
            }

        }
        if(gotViewpoint)
            earth_manip->setViewpoint(selectedViewpoint);
    }
    else
#endif // SGI_USE_OSGEARTH
        if (trackball_manip)
        {
            // set clear color to OSG default (violet-blue)
            view->getCamera()->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));

            osg::ref_ptr<osg::Node> root = view->getSceneData();
            osg::BoundingSphere bs = root->getBound();
            float radiusDistanceFactor = 2.5f;
            osg::Group * mainGroup = dynamic_cast<osg::Group*>(root.get());
            if (mainGroup)
            {
                osg::ref_ptr<osg::LOD> firstChildLOD = mainGroup->getNumChildren() ? dynamic_cast<osg::LOD*>(mainGroup->getChild(0)) : nullptr;
                if (firstChildLOD)
                {
                    bs._center = firstChildLOD->getCenter();
                    bs._radius = firstChildLOD->getRadius();
                    // make sure we are inside the first LOD radius
                    radiusDistanceFactor = 0.9f;
                }
            }
            trackball_manip->setCenter(bs.center());
            trackball_manip->setDistance(bs.radius() * radiusDistanceFactor);
        }
#endif
}

