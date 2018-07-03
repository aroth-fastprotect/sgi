#include "stdafx.h"
#include <sgi/details/Referenced>
#include <sgi/details/Observer>
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
    namespace details {

Referenced::Referenced()
    : _observerSet(nullptr)
    , _refCount(0)
{
}

Referenced::Referenced(const Referenced&)
    : _observerSet(nullptr)
    , _refCount(0)
{
}

Referenced::~Referenced()
{
    assert(_refCount==0);

    // signal observers that we are being deleted.
    signalObserversAndDelete(true, false);

    // delete the ObserverSet
    ObserverSet* observerSet = _observerSet;
    if (observerSet) observerSet->unref();
}

ObserverSet* Referenced::getOrCreateObserverSet() const
{
    ObserverSet* observerSet = _observerSet;
    while (nullptr == observerSet)
    {
        ObserverSet* newObserverSet = new ObserverSet(this);
        newObserverSet->ref();

        _observerSet.store(newObserverSet);
        observerSet = _observerSet;
    }
    return observerSet;
}

void Referenced::addObserver(Observer* observer) const
{
    getOrCreateObserverSet()->addObserver(observer);
}

void Referenced::removeObserver(Observer* observer) const
{
    getOrCreateObserverSet()->removeObserver(observer);
}

void Referenced::signalObserversAndDelete(bool signalDelete, bool doDelete) const
{
    ObserverSet* observerSet = _observerSet;

    if (observerSet && signalDelete)
    {
        observerSet->signalObjectDeleted(const_cast<Referenced*>(this));
    }

    if (doDelete)
    {
        //if (_refCount!=0)
        //    OSG_NOTICE<<"Warning Referenced::signalObserversAndDelete(,,) doing delete with _refCount="<<_refCount<<std::endl;

        delete this;
    }
}

int Referenced::unref_nodelete() const
{
    return --_refCount;
}

    } // namespace details
} // namespace sgi

