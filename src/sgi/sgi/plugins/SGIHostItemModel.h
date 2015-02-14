#pragma once
#include "SGIHostItemBase.h"

namespace osg {
    class Referenced;
}

namespace terra3d {
    class ModelObject;
}

namespace sgi {
namespace support {

class SGIHostItemModelObject : public SGIHostItemImpl<terra3d::ModelObject>
{
public:
    SGIHostItemModelObject(terra3d::ModelObject * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<terra3d::ModelObject>(object, userData, flags)
        {}
    SGIHostItemModelObject(const terra3d::ModelObject * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<terra3d::ModelObject>(const_cast<terra3d::ModelObject*>(object), userData, flags)
        {}
};
} // namespace support

typedef support::SGIHostItemModelObject SGIHostItemModelObject;

} // namespace sgi
