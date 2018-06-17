// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIDataItemBase.h>
#include <algorithm>

#ifdef max
#undef max
#endif

namespace sgi {
// some method implementations from SGIItemBase which are only
// used within the SGI base and not in any plugin or caller.

SGIDataItemBase::SGIDataItemBase(SGIItemBase * item, size_t numFields)
    : _item(item), _fields(numFields)
{

}

SGIDataItemBase::SGIDataItemBase(const SGIDataItemBase & rhs)
    : details::Referenced(rhs), _item(rhs._item), _fields(rhs._fields)
{
}

SGIDataItemBase::~SGIDataItemBase()
{

}

size_t SGIDataItemBase::numFields() const
{
    return _fields.size();
}

SGIDataFieldBase * SGIDataItemBase::getField(unsigned n) const
{
    return  (n < _fields.size()) ? (_fields[n].get()) : nullptr;
}
void SGIDataItemBase::setField(unsigned n, SGIDataFieldBase * field)
{
    _fields.resize(std::max(n, static_cast<unsigned>(_fields.size())));
    _fields[n] = field;
}
void SGIDataItemBase::addField(SGIDataFieldBase * field)
{
    unsigned idx = static_cast<unsigned>(_fields.size());
    _fields.resize(idx+1);
    _fields[idx] = field;
}

} // namespace sgi
