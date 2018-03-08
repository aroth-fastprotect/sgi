// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

namespace osg {
    class Referenced;
}

namespace sgi {

namespace support {
class SGIHostItemInternal : public SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> >
{
public:
    SGIHostItemInternal(osg::Referenced * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> >(object, userData, flags)
        {}
    SGIHostItemInternal(const osg::Referenced * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osg::Referenced, osg::ref_ptr<osg::Referenced> >(const_cast<osg::Referenced*>(object), userData, flags)
        {}
};
} // namespace support

typedef support::SGIHostItemInternal SGIHostItemInternal;

} // namespace sgi
