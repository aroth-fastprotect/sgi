// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIDataItemBase.h>

namespace sgi {
// some method implementations from SGIItemBase which are only
// used within the SGI base and not in any plugin or caller.

SGIDataItemBase::SGIDataItemBase(SGIItemBase * item, size_t numFields)
    : _item(item), _fields(numFields)
{

}

SGIDataItemBase::SGIDataItemBase(const SGIDataItemBase & rhs)
    : osg::Referenced(rhs), _item(rhs._item), _fields(rhs._fields)
{
}

SGIDataItemBase::~SGIDataItemBase()
{

}

} // namespace sgi
