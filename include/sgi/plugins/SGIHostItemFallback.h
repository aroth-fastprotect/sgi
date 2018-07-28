// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

namespace sgi {

typedef SGIItemInfoPlainPtr<void,void*, details::copy_object,details::do_not_delete> SGIItemInfoFallback;
typedef SGIHostItemImpl<SGIItemInfoFallback> SGIHostItemFallback;

} // namespace sgi
