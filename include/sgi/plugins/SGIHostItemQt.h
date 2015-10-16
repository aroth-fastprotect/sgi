// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

class QObject;
struct QMetaObject;
class QPaintDevice;
class QSurface;

namespace osg {
    class Referenced;
}

namespace sgi {
namespace support {

class SGIHostItemQt : public SGIHostItemImpl<QObject>
{
public:
    SGIHostItemQt(QObject * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QObject>(object, userData, flags)
        {}
    SGIHostItemQt(const QObject * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QObject>(const_cast<QObject*>(object), userData, flags)
        {}
};
class SGIHostItemQtMeta : public SGIHostItemImpl<QMetaObject>
{
public:
    SGIHostItemQtMeta(QMetaObject * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QMetaObject>(object, userData, flags)
        {}
    SGIHostItemQtMeta(const QMetaObject * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QMetaObject>(const_cast<QMetaObject*>(object), userData, flags)
        {}
};

class SGIHostItemQtPaintDevice : public SGIHostItemImpl<QPaintDevice>
{
public:
    SGIHostItemQtPaintDevice(QPaintDevice * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QPaintDevice>(object, userData, flags)
        {}
    SGIHostItemQtPaintDevice(const QPaintDevice * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QPaintDevice>(const_cast<QPaintDevice*>(object), userData, flags)
        {}
};

class SGIHostItemQtSurface : public SGIHostItemImpl<QSurface>
{
public:
    SGIHostItemQtSurface(QSurface * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QSurface>(object, userData, flags)
        {}
    SGIHostItemQtSurface(const QSurface * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QSurface>(const_cast<QSurface*>(object), userData, flags)
        {}
};

class SGIHostItemQtIcon : public SGIHostItemImpl<QIcon>
{
public:
    SGIHostItemQtIcon(QIcon * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QIcon>(object, userData, flags)
        {}
    SGIHostItemQtIcon(const QIcon * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QIcon>(const_cast<QIcon*>(object), userData, flags)
        {}
    SGIHostItemQtIcon(const QIcon & object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<QIcon>(const_cast<QIcon*>(&object), userData, flags)
        {}
};

} // namespace support

typedef support::SGIHostItemQt SGIHostItemQt;
typedef support::SGIHostItemQtMeta SGIHostItemQtMeta;
typedef support::SGIHostItemQtPaintDevice SGIHostItemQtPaintDevice;
typedef support::SGIHostItemQtSurface SGIHostItemQtSurface;
typedef support::SGIHostItemQtIcon SGIHostItemQtIcon;

} // namespace sgi
