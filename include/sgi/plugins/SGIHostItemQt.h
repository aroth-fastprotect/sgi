// kate: syntax C++;
// SGI - Copyright (C) 2012-2019 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"
#include <QtCore/QPointer>

class QObject;
struct QMetaObject;
class QPaintDevice;
class QSurface;
class QIcon;
class QPlatformSurface;

namespace sgi {

typedef SGIItemInfoPlainPtr<QObject> SGIItemInfoQt;
typedef SGIItemInfoPlainPtr<QMetaObject> SGIItemInfoQtMeta;
typedef SGIItemInfoSharedPtrQt<QPaintDevice, QSharedPointer<QPaintDevice> > SGIItemInfoQtPaintDevice;
typedef SGIItemInfoPlainPtr<QSurface> SGIItemInfoQtSurface;
typedef SGIItemInfoPlainPtr<QIcon, QIcon*, details::copy_object, details::delete_operator, details::copy_object> SGIItemInfoQtIcon;
typedef SGIItemInfoPlainPtr<QPlatformSurface> SGIItemInfoQtPlatformSurface;

typedef SGIHostItemImpl<SGIItemInfoQt> SGIHostItemQt;
typedef SGIHostItemImpl<SGIItemInfoQtMeta> SGIHostItemQtMeta;
typedef SGIHostItemImpl<SGIItemInfoQtPaintDevice> SGIHostItemQtPaintDevice;
typedef SGIHostItemImpl<SGIItemInfoQtSurface> SGIHostItemQtSurface;
typedef SGIHostItemImpl<SGIItemInfoQtIcon> SGIHostItemQtIcon;

/*
class SGIHostItemQtIcon : public SGIHostItemImpl<SGIItemInfoQtIcon>
{
public:
    SGIHostItemQtIcon(QIcon * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<SGIItemInfoQtIcon>(object, userData, flags)
        {}
    SGIHostItemQtIcon(const QIcon * object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<SGIItemInfoQtIcon>(const_cast<QIcon*>(object), userData, flags)
        {}
    SGIHostItemQtIcon(const QIcon & object, details::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<SGIItemInfoQtIcon>(const_cast<QIcon*>(&object), userData, flags)
        {}
};
*/
} // namespace sgi
