// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

namespace osg {
    class Referenced;
}

namespace sgi {

typedef SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> > SGIHostItemInternal;

} // namespace sgi
