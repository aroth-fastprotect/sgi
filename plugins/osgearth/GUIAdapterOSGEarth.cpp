#include "stdafx.h"
#include "GUIAdapterOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>

#include "ContextMenuOSGEarth.h"

#include <osgEarthUtil/EarthManipulator>

namespace sgi {
namespace osgearth_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::EarthManipulator)

bool guiAdapterSetViewImpl<osgEarth::Util::EarthManipulator>::execute()
{
    bool ret = false;
    osgEarth::Util::EarthManipulator * object = getObject<osgEarth::Util::EarthManipulator, SGIItemOsg, DynamicCaster>();

    const NamedViewpoint * vp = getTargetObject<NamedViewpoint, SGIItemOsg>();
    if(vp)
    {
        object->setViewpoint(*vp, _animationTime);
        ret = true;
    }

    return ret;
}

} // namespace osgearth_plugin
} // namespace sgi
