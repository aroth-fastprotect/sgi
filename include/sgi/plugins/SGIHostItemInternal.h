// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

namespace sgi {

namespace details {
    class Referenced;
}

typedef SGIItemInfoSharedPtr<details::Referenced, details::ref_ptr<details::Referenced> > SGIItemInfoInternal;
typedef SGIHostItemImpl<SGIItemInfoInternal> SGIHostItemInternal;

} // namespace sgi
