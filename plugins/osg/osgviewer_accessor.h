#pragma once
#include <osgViewer/ViewerEventHandlers>

namespace sgi {
	namespace osg_plugin {

class ViewerBaseAccess : public osgViewer::ViewerBase
{
public:
    bool requestRedraw() const { return _requestRedraw; }
    bool requestContinousUpdate() const { return _requestContinousUpdate; }
};

class osgViewerStatsHandlerAccess : public osgViewer::StatsHandler
{
public:
	osg::Switch * getSwitch() { return _switch.get(); }
    const osg::Switch * getSwitch() const { return _switch.get(); }
};

	} // namespace osg_plugin
} // namespace sgi
