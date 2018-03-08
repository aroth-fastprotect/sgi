// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemFallback.h"

namespace sgi {
namespace support {

class Fallback
{
public:
    typedef void object_type;
    typedef SGIHostItemFallback host_item_type;
};

} // namespace support
} // namespace sgi
