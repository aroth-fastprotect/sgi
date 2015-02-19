#include "stdafx.h"
#include "getObjectInfoQt.h"
#include "qt_string_helpers"

#include "SGIItemQt"

#include <sgi/helpers/rtti>

namespace sgi {

class SGIItemQt;

namespace qt_plugin {

GET_OBJECT_NAME_IMPL_REGISTER(QObject)
GET_OBJECT_NAME_IMPL_REGISTER(QMetaObject)
GET_OBJECT_NAME_IMPL_REGISTER(QPaintDevice)
GET_OBJECT_TYPE_IMPL_REGISTER(QObject)
GET_OBJECT_TYPE_IMPL_REGISTER(QMetaObject)
GET_OBJECT_TYPE_IMPL_REGISTER(QPaintDevice)
GET_OBJECT_PATH_IMPL_REGISTER(QObject)
GET_OBJECT_PATH_IMPL_REGISTER(QMetaObject)

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
        ret = toLocal8Bit(objectName);
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
        osg::ref_ptr<SGIItemBase> pathItem;
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
        osg::ref_ptr<SGIItemBase> pathItem;
        if(_hostInterface->generateItem(pathItem, &pathHostItem))
            ret.push_back(pathItem.get());
        parent = parent->superClass();
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
}

} // namespace qt_plugin
} // namespace sgi
