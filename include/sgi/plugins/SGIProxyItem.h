// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once

#include "SGIItemBase.h"

namespace sgi {

class SGIPluginHostInterface;

class SGIProxyItemBase : public SGIItemBase
{
public:
    SGIProxyItemBase(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * realItem=NULL)
        : _hostInterface(hostInterface), _realItem(realItem) {}
    SGIProxyItemBase(SGIPluginHostInterface * hostInterface, const std::string & name, const std::string & typeName=std::string(), const std::string & displayName=std::string(), SGIItemBase * realItem=NULL)
        : _hostInterface(hostInterface), _name(name), _displayName(displayName), _typeName(typeName)
        , _realItem(realItem) {}
    SGIProxyItemBase(const SGIProxyItemBase & rhs)
        : _hostInterface(rhs._hostInterface), _name(rhs._name), _displayName(rhs._displayName)
        , _typeName(rhs._typeName), _realItem(rhs._realItem) {}

    SGIItemBase * realItem(bool getInstance=false)
    {
        if(!_realItem.valid())
            _realItem = getRealInstance();
        return _realItem;
    }
    const SGIItemBase * realItem(bool getInstance=false) const
    {
        if(!_realItem.valid())
            const_cast<SGIProxyItemBase*>(this)->_realItem = const_cast<SGIProxyItemBase*>(this)->getRealInstance();
        return _realItem;
    }

    template<typename ITEMTYPE>
    ITEMTYPE * realItemAs(bool getInstance=false)
    {
        return dynamic_cast<ITEMTYPE*>(realItem(getInstance));
    }
    template<typename ITEMTYPE>
    const ITEMTYPE * realItemAs(bool getInstance=false) const
    {
        return dynamic_cast<const ITEMTYPE*>(realItem(getInstance));
    }

    const std::string & name() const { return _name; }
    const std::string & displayName() const { return _displayName; }
    const std::string & typeName() const { return _typeName; }

protected:
    virtual SGIItemBase * getRealInstance() = 0;

protected:
    std::string     _name;
    std::string     _displayName;
    std::string     _typeName;
    SGIItemBasePtr _realItem;
    SGIPluginHostInterface * _hostInterface;
};

template<typename GET_REAL_INSTANCE>
class SGIProxyItemT : public SGIProxyItemBase
{
public:
    SGIProxyItemT(SGIPluginHostInterface * hostInterface=NULL, SGIItemBase * realItem=NULL)
        : SGIProxyItemBase(hostInterface, realItem) {}
    SGIProxyItemT(SGIPluginHostInterface * hostInterface, const std::string & name, const std::string & typeName=std::string(), const std::string & displayName=std::string(), SGIItemBase * realItem=NULL)
        : SGIProxyItemBase(hostInterface, name, displayName, typeName, realItem) {}
    SGIProxyItemT(const SGIProxyItemT & rhs)
        : SGIProxyItemBase(rhs) {}

protected:
    virtual SGIItemBase * getRealInstance()
    {
        GET_REAL_INSTANCE f;
        return f(_hostInterface);
    }
};

} // namespace sgi
