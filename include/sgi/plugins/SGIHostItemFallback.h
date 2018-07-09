// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

namespace sgi {

typedef SGIItemInfoPlainPtr<void> SGIItemInfoFallback;
typedef SGIHostItemImpl<SGIItemInfoFallback> SGIHostItemFallback;

} // namespace sgi
