// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemQt.h"

namespace sgi {
namespace support {

class Qt
{
public:
    typedef QObject object_type;
    typedef SGIHostItemQt host_item_type;
};

class QtMeta
{
public:
    typedef QMetaObject object_type;
    typedef SGIHostItemQtMeta host_item_type;
};

} // namespace support
} // namespace sgi
