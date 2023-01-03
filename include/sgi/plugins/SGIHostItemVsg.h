// kate: syntax C++;
// SGI - Copyright (C) 2012-2023 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"
#include <vsg/core/Object.h>
#include <vsg/core/ref_ptr.h>

namespace sgi {

typedef details::ReferencedDataT< vsg::ref_ptr<vsg::Object> > SGIRefPtrVsg;
typedef SGIItemInfoSharedPtr<vsg::Object, vsg::ref_ptr<vsg::Object> > SGIItemInfoVsg;
typedef SGIHostItemImpl<SGIItemInfoVsg> SGIHostItemVsg;

} // namespace sgi
