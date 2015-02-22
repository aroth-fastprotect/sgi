#pragma once
#include "SGIHostItemBase.h"

namespace sgi {
namespace support {

class SGIHostItemOsg : public SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> >
{
public:
    SGIHostItemOsg(osg::Referenced * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> >(object, userData, flags)
        {}
    SGIHostItemOsg(const osg::Referenced * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> >(const_cast<osg::Referenced*>(object), userData, flags)
        {}
};
} // namespace support

typedef support::SGIHostItemOsg SGIHostItemOsg;

} // namespace sgi
