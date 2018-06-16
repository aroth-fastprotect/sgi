// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"

class QObject;
struct QMetaObject;
class QPaintDevice;
class QSurface;
class QIcon;

namespace sgi {
typedef SGIHostItemImpl<QObject> SGIHostItemQt;
typedef SGIHostItemImpl<QMetaObject> SGIHostItemQtMeta;
typedef SGIHostItemImpl<QPaintDevice> SGIHostItemQtPaintDevice;
typedef SGIHostItemImpl<QSurface> SGIHostItemQtSurface;
//typedef SGIHostItemImpl<QIcon> SGIHostItemQtIcon;

class SGIHostItemQtIcon : public SGIHostItemImpl<QIcon>
{
public:
    SGIHostItemQtIcon(QIcon * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<QIcon>(object, userData, flags)
        {}
    SGIHostItemQtIcon(const QIcon * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<QIcon>(const_cast<QIcon*>(object), userData, flags)
        {}
    SGIHostItemQtIcon(const QIcon & object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<QIcon>(const_cast<QIcon*>(&object), userData, flags)
        {}
};

} // namespace sgi
