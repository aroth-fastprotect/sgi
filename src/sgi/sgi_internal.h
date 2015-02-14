#pragma once

#include "sgi/plugins/SGIPluginInterface.h"

class QLibrary;

namespace sgi {

class SGIPlugin_internal
{
public:
    static SGIPluginInterface * create(SGIPluginHostInterface * hostInterface);
    static const char * PluginName;
    static std::string itemTypeName(SGIItemType type);

private:
    class Plugin;
};

} // namespace sgi
