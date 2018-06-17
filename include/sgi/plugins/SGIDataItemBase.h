// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIItemBase.h"
#include "SGIPluginInterface.h"
#include <list>

namespace sgi {

class SGIPluginHostInterface;

class SGIDataFieldBase : public details::Referenced
{
public:
    SGIDataFieldBase() : details::Referenced() {}
    SGIDataFieldBase(const SGIDataFieldBase & rhs) : details::Referenced(rhs) {}

    virtual std::string toString(SGIPluginHostInterface * hostInterface) const = 0;
};

typedef details::ref_ptr<SGIDataFieldBase> SGIDataFieldBasePtr;
typedef std::vector<SGIDataFieldBasePtr> SGIDataFieldBaseVector;

template<typename DATATYPE>
class SGIDataFieldT : public SGIDataFieldBase
{
public:
    SGIDataFieldT(const DATATYPE & value=DATATYPE())
        : SGIDataFieldBase(), _value(value) {}
    SGIDataFieldT(const SGIDataFieldT & rhs)
        : SGIDataFieldBase(rhs), _value(rhs._value) {}
    ~SGIDataFieldT() override {}

    const DATATYPE & value() const { return _value; }
    DATATYPE & value() { return _value; }

    bool operator<(const SGIDataFieldT & rhs) const { return _value < rhs._value; }
    bool operator==(const SGIDataFieldT & rhs) const { return _value == rhs._value; }
    bool operator!=(const SGIDataFieldT & rhs) const { return _value != rhs._value; }
    bool operator==(const DATATYPE & rhs) const { return _value == rhs; }
    bool operator!=(const DATATYPE & rhs) const { return _value != rhs; }

    virtual std::string toString(SGIPluginHostInterface * hostInterface) const override;

protected:
    DATATYPE _value;
};

typedef SGIDataFieldT<std::string> SGIDataFieldString;
typedef SGIDataFieldT<int> SGIDataFieldInteger;
typedef SGIDataFieldT<unsigned> SGIDataFieldUnsigned;
typedef SGIDataFieldT<bool> SGIDataFieldBool;
typedef SGIDataFieldT<SGIItemBasePtr> SGIDataFieldItemBasePtr;

template<>
inline std::string SGIDataFieldT<std::string>::toString(SGIPluginHostInterface * /*hostInterface*/) const
{
    return _value;
}

template<>
inline std::string SGIDataFieldT<int>::toString(SGIPluginHostInterface * /*hostInterface*/) const
{
    std::stringstream ss;
    ss << _value;
    return ss.str();
}

template<>
inline std::string SGIDataFieldT<unsigned>::toString(SGIPluginHostInterface * /*hostInterface*/) const
{
    std::stringstream ss;
    ss << _value;
    return ss.str();
}

template<>
inline std::string SGIDataFieldT<bool>::toString(SGIPluginHostInterface * /*hostInterface*/) const
{
    std::stringstream ss;
    ss << ((_value)?"true":"false");
    return ss.str();
}

template<>
inline std::string SGIDataFieldT<SGIItemBasePtr>::toString(SGIPluginHostInterface * hostInterface) const
{
    std::string ret;
    if(!hostInterface->getObjectDisplayName(ret, _value.get(), true))
        ret = "N/A";
    return ret;
}

class SGI_IMPL_EXPORT SGIDataItemBase : public details::Referenced
{
public:
    SGIDataItemBase(SGIItemBase * item=nullptr, size_t numFields=0);
    SGIDataItemBase(const SGIDataItemBase & rhs);
    virtual ~SGIDataItemBase();

    template<typename DATAITEMTYPE>
    DATAITEMTYPE * as()
    {
        return dynamic_cast<DATAITEMTYPE*>(this);
    }
    template<typename DATAITEMTYPE>
    const DATAITEMTYPE * as() const
    {
        return dynamic_cast<const DATAITEMTYPE*>(this);
    }

    size_t numFields() const;
    SGIDataFieldBase * getField(unsigned n) const;
    void setField(unsigned n, SGIDataFieldBase * field);
    void addField(SGIDataFieldBase * field);

protected:
    SGIItemBasePtr          _item;
    SGIDataFieldBaseVector  _fields;
};
typedef details::ref_ptr<SGIDataItemBase> SGIDataItemBasePtr;
typedef std::vector<SGIDataItemBasePtr> SGIDataItemBasePtrVector;
typedef std::list<SGIDataItemBasePtr> SGIDataItemBasePtrList;

} // namespace sgi

