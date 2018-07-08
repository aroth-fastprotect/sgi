#include "stdafx.h"
#include <sgi/details/Object>
#include <sgi/details/UserDataContainer>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
    namespace details {

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Object
//
Object::Object() 
    :Referenced(), _userDataContainer(nullptr)
{
}

Object::Object(const Object& obj)
    : Referenced()
    , _name(obj._name)
    , _userDataContainer(0)
{
    if (obj._userDataContainer)
    {
        setUserDataContainer(obj._userDataContainer);
    }
}

Object::~Object()
{
    if (_userDataContainer) _userDataContainer->unref();
}

void Object::setUserDataContainer(UserDataContainer* udc)
{
    if (_userDataContainer == udc) return;

    if (_userDataContainer) _userDataContainer->unref();

    _userDataContainer = udc;

    if (_userDataContainer) _userDataContainer->ref();
}

UserDataContainer* Object::getOrCreateUserDataContainer()
{
    if (!_userDataContainer) setUserDataContainer(new DefaultUserDataContainer());
    return _userDataContainer;
}

void Object::setUserData(Referenced* obj)
{
    if (getUserData()==obj) return;

    getOrCreateUserDataContainer()->setUserData(obj);
}

Referenced* Object::getUserData()
{
    return _userDataContainer ? _userDataContainer->getUserData() : 0;
}

const Referenced* Object::getUserData() const
{
    return _userDataContainer ? _userDataContainer->getUserData() : 0;
}

    } // namespace details
} // namespace sgi

