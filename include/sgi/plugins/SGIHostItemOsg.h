// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

namespace sgi {

typedef SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> > SGIHostItemOsg;

} // namespace sgi
