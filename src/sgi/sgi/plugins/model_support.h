#pragma once
#include "SGIHostItemModel.h"

namespace terra3d {
    class ModelObject;
}

namespace sgi {
namespace support {

class Model
{
public:
    typedef terra3d::ModelObject object_type;
    typedef SGIHostItemModelObject host_item_type;
};

} // namespace support
} // namespace sgi
