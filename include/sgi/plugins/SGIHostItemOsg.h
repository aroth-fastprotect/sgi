// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"
#include <osg/Referenced>
#include <osg/ref_ptr>

namespace sgi {

typedef SGIItemInfoSharedPtr<osg::Referenced, osg::ref_ptr<osg::Referenced> > SGIItemInfoOsg;
typedef SGIHostItemImpl<SGIItemInfoOsg> SGIHostItemOsg;

} // namespace sgi
