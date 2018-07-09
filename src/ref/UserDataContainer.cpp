#include "stdafx.h"
#include <sgi/details/UserDataContainer>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
    namespace details {

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// UserDataContainer
//
UserDataContainer::UserDataContainer():
    Object()
{
}

UserDataContainer::UserDataContainer(const UserDataContainer& udc):
    Object(udc)
{
}

Object* UserDataContainer::getUserObject(const std::string& name, unsigned int startPos)
{
     return getUserObject(getUserObjectIndex(name, startPos));
}

const Object* UserDataContainer::getUserObject(const std::string& name, unsigned int startPos) const
{
     return getUserObject(getUserObjectIndex(name, startPos));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// DefaultUserDataContainer
//
DefaultUserDataContainer::DefaultUserDataContainer()
{
}

DefaultUserDataContainer::DefaultUserDataContainer(const DefaultUserDataContainer& udc):
    UserDataContainer(udc)
{
    _userData = udc._userData;
    _descriptionList = udc._descriptionList;
    for(ObjectList::const_iterator itr = udc._objectList.begin();
        itr != udc._objectList.end();
        ++itr)
    {
        _objectList.push_back(itr->get());
    }
}

void DefaultUserDataContainer::setUserData(Referenced* obj)
{
    _userData = obj;
}

Referenced* DefaultUserDataContainer::getUserData()
{
    return _userData.get();
}

const Referenced* DefaultUserDataContainer::getUserData() const
{
    return _userData.get();
}

unsigned int DefaultUserDataContainer::addUserObject(Object* obj)
{
    // make sure that the object isn't already in the container
    unsigned int i = getUserObjectIndex(obj);
    if (i<_objectList.size())
    {
        // object already in container so just return.
        return i;
    }

    unsigned int pos = _objectList.size();

    // object not already on user data container so add it in.
    _objectList.push_back(obj);

    return pos;
}

void DefaultUserDataContainer::removeUserObject(unsigned int i)
{
    if (i<_objectList.size())
    {
        _objectList.erase(_objectList.begin()+i);
    }
}

void DefaultUserDataContainer::setUserObject(unsigned int i, Object* obj)
{
    if (i<_objectList.size())
    {
        _objectList[i] = obj;
    }
}

Object* DefaultUserDataContainer::getUserObject(unsigned int i)
{
    if (i<_objectList.size())
    {
        return _objectList[i].get();
    }
    return 0;
}

const Object* DefaultUserDataContainer::getUserObject(unsigned int i) const
{
    if (i<_objectList.size())
    {
        return _objectList[i].get();
    }
    return 0;
}

unsigned int DefaultUserDataContainer::getNumUserObjects() const
{
    return _objectList.size();
}

unsigned int DefaultUserDataContainer::getUserObjectIndex(const Object* obj, unsigned int startPos) const
{
    for(unsigned int i = startPos; i < _objectList.size(); ++i)
    {
        if (_objectList[i]==obj) return i;
    }
    return _objectList.size();
}

unsigned int DefaultUserDataContainer::getUserObjectIndex(const std::string& name, unsigned int startPos) const
{
    for(unsigned int i = startPos; i < _objectList.size(); ++i)
    {
        Object* obj = _objectList[i].get();
        if (obj && obj->getName()==name) return i;
    }
    return _objectList.size();
}

void DefaultUserDataContainer::setDescriptions(const DescriptionList& descriptions)
{
    _descriptionList = descriptions;
}

UserDataContainer::DescriptionList& DefaultUserDataContainer::getDescriptions()
{
    return _descriptionList;
}

const UserDataContainer::DescriptionList& DefaultUserDataContainer::getDescriptions() const
{
    return _descriptionList;
}

unsigned int DefaultUserDataContainer::getNumDescriptions() const
{
    return _descriptionList.size();
}

void DefaultUserDataContainer::addDescription(const std::string& desc)
{
    _descriptionList.push_back(desc);
}

    } // namespace details
} // namespace sgi

