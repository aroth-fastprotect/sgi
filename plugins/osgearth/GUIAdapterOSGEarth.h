#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
namespace osgEarth {
    namespace Util {
        class EarthManipulator;
    }
}

namespace sgi {
namespace osgearth_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_TEMPLATE()
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE(osgEarth::Util::EarthManipulator)

} // namespace osgearth_plugin
} // namespace sgi
