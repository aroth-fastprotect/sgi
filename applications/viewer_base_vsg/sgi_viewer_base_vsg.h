#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <vsg/core/Object.h>

class QObject;
class QWidget;

namespace vsg {
    class Viewer;
    class Window;
    class CommandLine;
    class Node;
    class Group;
}

void initializeNotifyLevels(vsg::CommandLine& arguments);

class sgi_CommonHelper
{
public:
    sgi_CommonHelper(vsg::CommandLine& args);
    sgi_CommonHelper(const sgi_CommonHelper & rhs);
    ~sgi_CommonHelper();


    enum GLContextProfile {
        GLContextProfileNone,
        GLContextProfileCore,
        GLContextProfileCompatibility,
    };

public:
    GLContextProfile glprofile;
    std::string glversion;
    bool addSceneGraphInspector;
    bool showSceneGraphInspector;

};

/**
* Parses a set of built-in example arguments. Any Controls created by parsing
* command-line parameters will appear in the lower-left corner of the display.
*/
class sgi_MapNodeHelper : public sgi_CommonHelper
{
public:
    sgi_MapNodeHelper(vsg::CommandLine& args);
    sgi_MapNodeHelper(const sgi_MapNodeHelper & rhs);
    ~sgi_MapNodeHelper();


    bool load(
        vsg::ref_ptr<vsg::Group> root,
        vsg::CommandLine& args
        );

    /**
    * Returns a usage string
    */
    std::string usage() const;

    std::string errorMessages() const;

    const std::vector<std::string> & files() const {
        return m_files;
    }
    bool onlyImages() const { return _onlyImages; }

    void setupEventHandlers(vsg::Viewer* viewer, vsg::Group * root);

    void setupInitialPosition(vsg::Viewer* viewer) const;

private:
    std::stringstream m_errorMessages;
    std::vector<std::string> m_files;
    bool _onlyImages;
    bool _addKeyDumper;
    bool _addMouseDumper;
    bool _movieTrackMouse;
    int _viewpointNum;
    std::string _viewpointName;
};
