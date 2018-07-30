#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventHandler>
#include <sstream>

namespace osg {
    class ImageStream;
}

namespace osgViewer {
    class View;
    class GraphicsWindow;
}

#ifdef SGI_USE_OSGEARTH
namespace osgEarth {
    class MapNode;
    namespace Util {
        class MapNodeHelper;
        namespace Controls {
            class Control;
            class Container;
            class LabelControl;
        }
    }
}
#endif // WITH_OSGEARTH

class QObject;
class QWidget;

namespace std {

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::NotifySeverity & t);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::BoundingSphere & bs);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::BoundingBox & bbox);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::EventType & t);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::MouseButtonMask & m);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::KeySymbol & k);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::ModKeyMask & m);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::PointerData& pd);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter& ea);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIActionAdapter& aa);
} // namespace std

osg::NotifySeverity severityFromString(const std::string & input);
void initializeNotifyLevels(osg::ArgumentParser & arguments);
QWidget * getWidgetForGraphicsWindow(osgViewer::GraphicsWindow * gw, QWidget * parent=nullptr);
bool createWidgetForGraphicsWindow(osg::GraphicsContext * ctx, QObject * parent = nullptr);

class KeyboardDumpHandler : public osgGA::GUIEventHandler
{
public:
    KeyboardDumpHandler() {}
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) override;
};

class MouseDumpHandler : public osgGA::GUIEventHandler
{
public:
    MouseDumpHandler() {}
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) override;
};


class MovieEventHandler : public osgGA::GUIEventHandler
{
public:
    static osg::ImageStream* s_imageStream;
    MovieEventHandler() :_playToggle(true), _trackMouse(false) {}

    void setMouseTracking(bool track) { _trackMouse = track; }
    bool getMouseTracking() const { return _trackMouse; }

    void set(osg::Node* node);

    void setTrackMouse(bool tm);
    bool getTrackMouse() const { return _trackMouse; }

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor* nv);

    virtual void getUsage(osg::ApplicationUsage& usage) const;

    typedef std::vector< osg::observer_ptr<osg::ImageStream> > ImageStreamList;

    struct ImageStreamPlaybackSpeedData {
        double fps;
        unsigned char* lastData;
        double timeStamp, lastOutput;

        ImageStreamPlaybackSpeedData() : fps(0), lastData(nullptr), timeStamp(0), lastOutput(0) {}

    };

    typedef std::vector< ImageStreamPlaybackSpeedData > ImageStreamPlayBackSpeedList;

protected:

    virtual ~MovieEventHandler() {}

    class FindImageStreamsVisitor;

    bool            _playToggle;
    bool            _trackMouse;
    ImageStreamList _imageStreamList;
    ImageStreamPlayBackSpeedList _imageStreamPlayBackSpeedList;

};


/**
* Parses a set of built-in example arguments. Any Controls created by parsing
* command-line parameters will appear in the lower-left corner of the display.
*/
class sgi_MapNodeHelper
{
public:
    sgi_MapNodeHelper();
    ~sgi_MapNodeHelper();

    /**
    * Loads a map file and processes all the built-in example command line
    * arguments and XML externals.
    */
    osg::Group* load(
        osg::ArgumentParser& args,
        osgViewer::View*     view
#ifdef SGI_USE_OSGEARTH
        , osgEarth::Util::Controls::Container* userContainer = nullptr
#endif
    );

    void parse(
#ifdef SGI_USE_OSGEARTH
        osgEarth::MapNode*   mapNode,
#endif // WITH_OSGEARTH
        osg::ArgumentParser& args,
        osgViewer::View*     view,
        osg::Group*          parentGroup
#ifdef SGI_USE_OSGEARTH
        , osgEarth::Util::Controls::LabelControl* userLabel
#endif
    );

#ifdef SGI_USE_OSGEARTH
    /**
    * Takes an existing map node and processes all the built-in example command
    * line arguments and mapNode externals.
    */
    void parse(
        osgEarth::MapNode*   mapNode,
        osg::ArgumentParser& args,
        osgViewer::View*     view,
        osg::Group*          parentGroup = nullptr,
        osgEarth::Util::Controls::Container* userContainer = nullptr
    );
#endif // WITH_OSGEARTH

    /**
    * Configures a view with a stock set of event handlers that are useful
    * for demos, and performs some other common view configuration for osgEarth.
    */
    void configureView(osgViewer::View* view) const;

    void setupInitialPosition(osgViewer::View* view) const;

    osg::Group * setupRootGroup(osg::Group * root);
    osg::Group * setupLight(osg::Group * root);

    /**
    * Returns a usage string
    */
    std::string usage() const;

    std::string errorMessages() const;

    const std::vector<std::string> & files() const {
        return m_files;
    }
    bool onlyImages() const { return _onlyImages; }

private:
    void setupEventHandlers(osgViewer::View* view, osg::Group * root);

private:
    std::stringstream m_errorMessages;
    std::vector<std::string> m_files;
#ifdef SGI_USE_OSGEARTH
    osgEarth::Util::MapNodeHelper * _mapNodeHelper;
    bool _useOELighting;
#endif
    osg::ApplicationUsage * _usageMessage;
    bool _onlyImages;
    bool _addKeyDumper;
    bool _addMouseDumper;
    bool _movieTrackMouse;
    int _viewpointNum;
    std::string _viewpointName;
};
