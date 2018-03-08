// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemLog4cplus.h"

namespace sgi {
namespace support {

class Log4cplus
{
public:
    typedef sgi::log4cplus_plugin::Log4cplusObjectBase object_type;
    typedef SGIHostItemLog4cplus host_item_type;
};

} // namespace support
} // namespace sgi
