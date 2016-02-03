#pragma once
#include <osgViewer/ViewerEventHandlers>

namespace sgi {
	namespace osg_plugin {

class osgViewerStatsHandlerAccess : public osgViewer::StatsHandler
{
public:
	osg::Switch * getSwitch() { return _switch.get(); }
    const osg::Switch * getSwitch() const { return _switch.get(); }
};

	} // namespace osg_plugin
} // namespace sgi
