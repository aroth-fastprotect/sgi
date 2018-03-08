#include <sgi/details/functionImplBase>
#include <sgi/plugins/SGIPluginInterface.h>

namespace sgi {
namespace details {
functionImplBase::functionImplBase(SGIPluginHostInterface * hostInterface, SGIItemBase * item)
    : _hostInterface(hostInterface)
    , _item(item)
{
}

functionImplBase::functionImplBase(SGIPluginHostInterface * hostInterface, const SGIItemBase * item)
    : _hostInterface(hostInterface)
    , _item(const_cast<SGIItemBase*>(item))
{
}

functionImplBase::~functionImplBase()
{
}

void functionImplBase::namedEnumValueToString(const std::string& value_type, std::string& ret, int value)
{
    _hostInterface->namedEnumValueToString(value_type, ret, (int)value);
}

IHostCallback * functionImplBase::hostCallback() const
{
    return _hostInterface->hostCallback();
}

SGIItemType functionImplBase::itemType() const
{
    return _item->type();
}
unsigned functionImplBase::itemNumber() const
{
    return _item->number();
}

} // namespace details
} // namespace sgi
