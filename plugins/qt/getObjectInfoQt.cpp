#include "stdafx.h"
#include "getObjectInfoQt.h"
#include <sgi/helpers/qt>
#include <sgi/helpers/rtti>

#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIImage.h>
#include <sgi/plugins/SGIItemQt>

namespace sgi {

namespace qt_plugin {

GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(QObject)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(QMetaObject)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(QPaintDevice)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(QIcon)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(QObject)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(QMetaObject)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(QPaintDevice)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(QIcon)
GET_OBJECT_PATH_IMPL_DECLARE_AND_REGISTER(QObject)
GET_OBJECT_PATH_IMPL_DECLARE_AND_REGISTER(QMetaObject)

CONVERT_TO_IMAGE_CONVERT_IMPL_DECLARE_AND_REGISTER(QImage)
CONVERT_TO_IMAGE_CONVERT_IMPL_DECLARE_AND_REGISTER(QPixmap)

using namespace sgi::qt_helpers;

std::string getObjectNameImpl<QObject>::process()
{
    QObject * object = static_cast<QObject*>(item<SGIItemQt>()->object());
    QString objectName = object->objectName();
    std::string ret;
    if(objectName.isEmpty())
    {
        std::stringstream ss;
        const QMetaObject * metaObject = object->metaObject();
        if(metaObject)
            ss << metaObject->className() << '(' << (void*)object << ')';
        else
            ss << "QObject(" << (void*)object << ')';
        ret = ss.str();
    }
    else
        ret = toUtf8(objectName);
    return ret;
}

std::string getObjectNameImpl<QMetaObject>::process()
{
    QMetaObject * object = static_cast<QMetaObject*>(item<SGIItemQtMeta>()->object());
    return std::string(object->className());
}

std::string getObjectNameImpl<QPaintDevice>::process()
{
    QPaintDevice * object = static_cast<QPaintDevice*>(item<SGIItemQtPaintDevice>()->object());
    std::stringstream ss;
    std::string ret;
    ss << "QPaintDevice(" << (void*)object << ")";
    ret = ss.str();
    return ret;
}

std::string getObjectNameImpl<QIcon>::process()
{
    QIcon * object = static_cast<QIcon*>(item<SGIItemQtIcon>()->object());
    std::stringstream ss;
    std::string ret;
    ss << "QIcon(" << (void*)object << ")";
    ret = ss.str();
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
std::string getObjectTypeImpl<QObject>::process()
{
    QObject * object = static_cast<QObject*>(item<SGIItemQt>()->object());
    std::string ret;
    std::string classname = object->metaObject()->className();
    std::string rtticlass = helpers::getRTTITypename(object);

    if(helpers::is_same_class(classname, rtticlass))
        ret = classname;
    else
        ret = classname + '(' + rtticlass + ')';

    return ret;
}

std::string getObjectTypeImpl<QMetaObject>::process()
{
    QMetaObject * object = static_cast<QMetaObject*>(item<SGIItemQtMeta>()->object());
    std::string ret;
    std::string classname = "QMetaObject";
    std::string rtticlass = helpers::getRTTITypename(object);

    if(helpers::is_same_class(classname, rtticlass))
        ret = classname;
    else
        ret = classname + '(' + rtticlass + ')';
    return ret;
}

std::string getObjectTypeImpl<QPaintDevice>::process()
{
    QPaintDevice * object = static_cast<QPaintDevice*>(item<SGIItemQtPaintDevice>()->object());
    std::string ret;
    std::string classname = "QPaintDevice";
    std::string rtticlass = helpers::getRTTITypename(object);

    if(helpers::is_same_class(classname, rtticlass))
        ret = classname;
    else
        ret = classname + '(' + rtticlass + ')';
    return ret;
}

std::string getObjectTypeImpl<QIcon>::process()
{
    QIcon * object = static_cast<QIcon*>(item<SGIItemQtIcon>()->object());
    std::string ret;
    std::string classname = "QIcon";
    std::string rtticlass = helpers::getRTTITypename(object);

    if(helpers::is_same_class(classname, rtticlass))
        ret = classname;
    else
        ret = classname + '(' + rtticlass + ')';
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------

SGIItemBasePtrPath getObjectPathImpl<QObject>::process()
{
    QObject * object = static_cast<QObject*>(item<SGIItemQt>()->object());
    SGIItemBasePtrPath ret;
    QObject * parent = object;
    while(parent)
    {
        SGIHostItemQt pathHostItem(parent);
        SGIItemBasePtr pathItem;
        if(_hostInterface->generateItem(pathItem, &pathHostItem))
            ret.push_back(pathItem.get());
        parent = parent->parent();
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
}

SGIItemBasePtrPath getObjectPathImpl<QMetaObject>::process()
{
    QMetaObject * object = static_cast<QMetaObject*>(item<SGIItemQtMeta>()->object());
    SGIItemBasePtrPath ret;
    const QMetaObject * parent = object;
    while(parent)
    {
        SGIHostItemQtMeta pathHostItem(parent);
        SGIItemBasePtr pathItem;
        if(_hostInterface->generateItem(pathItem, &pathHostItem))
            ret.push_back(pathItem.get());
        parent = parent->superClass();
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
}

//--------------------------------------------------------------------------------
// convertToImageConvertImpl
//--------------------------------------------------------------------------------
bool convertToImageConvertImpl<QImage>::convert()
{
    QImage * object = getObject<QImage, SGIItemQtPaintDevice>();
    *_image = new sgi::Image(object);
    return true;
}

bool convertToImageConvertImpl<QPixmap>::convert()
{
    QPixmap * object = getObject<QPixmap, SGIItemQtPaintDevice>();
    QImage img = object->toImage();
    *_image = new sgi::Image(&img);
    return true;
}

} // namespace qt_plugin
} // namespace sgi
