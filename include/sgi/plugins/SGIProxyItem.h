// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once

#include "SGIItemBase.h"

namespace sgi {

class SGIPluginHostInterface;

class SGIProxyItemBase : public SGIItemBase
{
public:
    SGIProxyItemBase(SGIPluginHostInterface * hostInterface = NULL, SGIItemBase * realItem = NULL);
    SGIProxyItemBase(SGIPluginHostInterface * hostInterface, 
        const std::string & name, 
        const std::string & typeName = std::string(), 
        const std::string & displayName = std::string(), 
        SGIItemBase * realItem = NULL);
    SGIProxyItemBase(const SGIProxyItemBase & rhs);

    SGIItemBase * realItem(bool getInstance = false);
    const SGIItemBase * realItem(bool getInstance = false) const;

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
