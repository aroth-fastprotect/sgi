#pragma once
#include "SGIHostItemBase.h"

namespace sgi {
namespace support {

class SGIHostItemFallback : public SGIHostItemImpl<void>
{
public:
    SGIHostItemFallback(void * object)
        : SGIHostItemImpl<void>(object)
        {}
};
} // namespace support

typedef support::SGIHostItemFallback SGIHostItemFallback;

} // namespace sgi
