#include "stdafx.h"
#include "sgi_internal.h"

#include "sgi/Export"
#include "sgi/plugins/SGIPluginImpl.h"

#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIItemQt>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/SGIImage.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/SGIItemInternal>
#include "SGIPlugin.h"
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ObjectLoggerDialog>
#include <sgi/plugins/SettingsDialog>

#include "sgi/helpers/rtti"
#include "sgi/helpers/qt"

#include "QtProxy.h"
#include "ImageGLWidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define OBJECTTREE_PROXYITEM_GET_INSTANCE true
#define CONTEXTMENU_PROXYITEM_GET_INSTANCE true
#define WRITE_PRETTY_HTML_PROXYITEM_GET_INSTANCE true
#define GET_OBJECT_NAME_PROXYITEM_GET_INSTANCE false
#define GET_OBJECT_TYPE_PROXYITEM_GET_INSTANCE false
#define GET_OBJECT_DISPLAYNAME_PROXYITEM_GET_INSTANCE false

sgi::SGIPluginHostInterface * sgi::SGIPluginInterface::_hostInterface = nullptr;

SGI_OBJECT_INFO_BEGIN(sgi::details::Referenced)
    sgi::SGIPlugins,
    sgi::ISceneGraphDialog,
    sgi::IContextMenu,
    sgi::IObjectLoggerDialog,
    sgi::ISettingsDialog,
    sgi::ISettingsDialogInfo,
    sgi::ReferencedInternalItemData,
    sgi::ReferencedInternalInfoData,
    sgi::Image,
    sgi::SGIItemHolder,
    sgi::details::Object
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(sgi::details::Object)
    sgi::SGIItemBase
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(sgi::SGIItemBase)
    sgi::SGIProxyItemBase
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QObject)
    QWidget,
    sgi::QtProxy
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QWidget)
    QOpenGLWidget
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QOpenGLWidget)
    sgi::ImageGLWidget
SGI_OBJECT_INFO_END()


namespace sgi {

namespace internal_plugin {

enum MenuAction {
    MenuActionNone = -1,
    MenuActionObjectInfo,
    MenuActionSGIAbout,
};


enum SettingsDialog {
    SettingsDialogNone = -1,
    SettingsDialogAbout,
};

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(QObject)
GENERATE_IMPL_NO_ACCEPT(QWidget)
GENERATE_IMPL_NO_ACCEPT(QOpenGLWidget)

WRITE_PRETTY_HTML_IMPL_TEMPLATE()
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(details::Referenced)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIPlugins)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Image)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIItemHolder)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(sgi::ImageGLWidget)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(sgi::QtProxy)

bool writePrettyHTMLImpl<details::Referenced>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    details::Referenced * object = getObject<details::Referenced,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>this</td><td>" << std::hex << (void*)object << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>typename</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;
            os << "<tr><td>refCount</td><td>" << (object?object->referenceCount():0) << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // no more forwarding
        break;
    }
    return ret;
}

#define HAS_SUPPORT(__pp_name) \
    os << "<tr><td>" << #__pp_name << "</td><td>" << ((SGI_HAS_ ## __pp_name ## _SUPPORT) ?"true":"false") << "</td></tr>" << std::endl

bool writePrettyHTMLImpl<SGIPlugins>::process(std::basic_ostream<char>& os)
{
    SGIPlugins * object = getObject<SGIPlugins,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'' width='100%'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>libraryName</td><td>" << sgiGetLibraryName() << "</td></tr>" << std::endl;
            os << "<tr><td>moduleFilename</td><td>" << sgiGetLibraryModuleFilename() << "</td></tr>" << std::endl;
            os << "<tr><td>version</td><td>" << sgiGetVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>.so version</td><td>" << sgiGetSOVersion() << "</td></tr>" << std::endl;
            HAS_SUPPORT(LOG4CPLUS);
            HAS_SUPPORT(OSGQT);
            HAS_SUPPORT(OSGEARTH);
            HAS_SUPPORT(FFMPEG);
            HAS_SUPPORT(GAMMARAY);

            os << "<tr><td>plugins</td><td><ul>";
            SGIPlugins::PluginInfoList plugins;
            if(object->getPlugins(plugins))
            {
                for(SGIPlugins::PluginInfoList::const_iterator it = plugins.begin(); it != plugins.end(); it++)
                {
                    const SGIPluginInfo & info = *it;
                    os << "<li>" << info.pluginName();
                    if(info.isInternalPlugin())
                        os << " <i>(internal)</i>";
                    else if(!info.pluginFilename().empty())
                        os << " from " << info.pluginFilename();
                    os << "</li>";
                }
            }
            os << "</ul></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    case SGIItemTypePlugins:
        {
            os << "<ul>";
            SGIPlugins::PluginInfoList plugins;
            if(object->getPlugins(plugins))
            {
                for(SGIPlugins::PluginInfoList::const_iterator it = plugins.begin(); it != plugins.end(); it++)
                {
                    const SGIPluginInfo & info = *it;
                    os << "<li>" << info.pluginName() << "<br/>";
                    os << "<table border=\'1\' align=\'left\' width='100%'>";
                    os << "<tr><td>filename</td><td>" << info.pluginFilename() << "</td></tr>";
                    os << "<tr><td>score</td><td>" << info.pluginScore() << "</td></tr>";
                    os << "<tr><td>error</td><td>" << info.errorMessage << "</td></tr>";
                    os << "<tr><td>WritePrettyHTML</td><td>" << (void*)info.writePrettyHTMLInterface << "</td></tr>";
                    os << "<tr><td>ObjectInfo</td><td>" << (void*)info.objectInfoInterface << "</td></tr>";
                    os << "<tr><td>ObjectTree</td><td>" << (void*)info.objectTreeInterface << "</td></tr>";
                    os << "<tr><td>ObjectLogger</td><td>" << (void*)info.objectLoggerInterface << "</td></tr>";
                    os << "<tr><td>ContextMenu</td><td>" << (void*)info.contextMenuInterface << "</td></tr>";
                    os << "<tr><td>SettingsDialog</td><td>" << (void*)info.settingsDialogInterface << "</td></tr>";
                    os << "<tr><td>GUIAdapter</td><td>" << (void*)info.guiAdapterInterface << "</td></tr>";
                    os << "<tr><td>ConvertToImage</td><td>" << (void*)info.convertToImage << "</td></tr>";
                    os << "</table></li>";
                }
            }
            os << "</ul>";
        }
        break;
    case SGIItemTypeBackendPlugins:
        {
            os << "Plugin directories:<ul>";
            for(const auto & s : object->pluginDirectories())
                os << "<li>" << s << "</li>";

            os << "</ul>Available plugins<ul>";
            SGIPlugins::PluginFileNameList plugins = object->listAllAvailablePlugins();
            for(SGIPlugins::PluginFileNameList::const_iterator it = plugins.begin(); it != plugins.end(); it++)
            {
                os << "<li>" << it->first;
                os << " from " << it->second;
                os << "</li>";
            }
            os << "</ul>";
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<ReferencedInternalItemData>::process(std::basic_ostream<char>& os)
{
    ReferencedInternalItemData * object = getObject<ReferencedInternalItemData,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const SGIItemBasePtr & data = object->data().item;
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            SGIProxyItemBase * proxyObject = dynamic_cast<SGIProxyItemBase*>(data.get());

            os << "<tr><td>this</td><td>" << (void*)data.get() << "</td></tr>" << std::endl;
            os << "<tr><td>type</td><td>" << helpers::getRTTITypename_html(data.get()) << "</td></tr>" << std::endl;
            os << "<tr><td>proxy</td><td>" << (proxyObject?"true":"false") << "</td></tr>" << std::endl;
            if(proxyObject)
            {
                os << "<tr><td>proxyName</td><td>" << proxyObject->name() << "</td></tr>" << std::endl;
                os << "<tr><td>proxyDisplayName</td><td>" << proxyObject->displayName() << "</td></tr>" << std::endl;
                os << "<tr><td>proxyTypeName</td><td>" << proxyObject->typeName() << "</td></tr>" << std::endl;
                os << "<tr><td>realItem</td><td>" << helpers::getRTTIObjectNameAndType_html(proxyObject->realItem(false)) << "</td></tr>" << std::endl;
            }

            os << "<tr><td>itemType</td><td>" << enumValueToString(data->type()) << "</td></tr>" << std::endl;
            os << "<tr><td>flags</td><td>0x" << std::hex << data->flags() << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>score</td><td>" << data->score() << "</td></tr>" << std::endl;
            os << "<tr><td>typeName</td><td>" << helpers::demangle_html(data->typeName()) << "</td></tr>" << std::endl;
            os << "<tr><td>number</td><td>" << data->number() << "</td></tr>" << std::endl;
            os << "<tr><td>userData</td><td>" << helpers::getRTTIObjectNameAndType_html(data->userDataPtr()) << "</td></tr>" << std::endl;
            os << "<tr><td>root</td><td>" << (void*)data->rootBase() << "</td></tr>" << std::endl;
            os << "<tr><td>prev</td><td>" << (void*)data->previousBase() << "</td></tr>" << std::endl;
            os << "<tr><td>next</td><td>" << (void*)data->nextBase() << "</td></tr>" << std::endl;

            const SGIPluginInfo * pluginInfo = static_cast<const SGIPluginInfo *>(data->pluginInfo());
            os << "<tr><td>plugin</td><td>" << pluginInfo->pluginName() << "</td></tr>" << std::endl;
            os << "<tr><td>pluginScore</td><td>" << pluginInfo->pluginScore() << "</td></tr>" << std::endl;

            os << "<tr><td>item chain</td><td><ul>";
            SGIItemType initialType = data->type();
            {
                SGIItemBase* item = data->rootBase();
                while(item)
                {
                    const SGIPluginInfo * pluginInfo = static_cast<const SGIPluginInfo *>(item->pluginInfo());
                    os << "<li>#" << item->score() << "&nbsp;";
                    if(initialType != item->type())
                        os << enumValueToString(item->type()) << "&nbsp;";
                    if(item == data.get())
                        os << "(this)";
                    else
                        os << (void*)item;
                    os << "&nbsp;<b>" << helpers::demangle_html(item->typeName()) << "</b>";
                    if(pluginInfo)
                    {
                        os << "&nbsp;<i>"
                           << pluginInfo->pluginName() << ':' << pluginInfo->pluginScore()
                           << "</i>";
                    }
                    os << "</li>";
                    item = item->nextBase();
                }
            }
            os << "</ul></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<ReferencedInternalInfoData>::process(std::basic_ostream<char>& os)
{
    ReferencedInternalInfoData * object = getObject<ReferencedInternalInfoData,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            InternalInfoData::CommandId command = object->data().command;
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>command</td><td>" << command << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<SGIProxyItemBase>::process(std::basic_ostream<char>& os)
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    bool ret = false;
    if(WRITE_PRETTY_HTML_PROXYITEM_GET_INSTANCE)
    {
        SGIItemBase * realObject = object ? object->realItem(true) : nullptr;
        if(realObject)
            ret = _hostInterface->writePrettyHTML(os, realObject);
        else
        {
            os <<  "<i>nullptr</i>";
            ret = true;
        }
    }
    else
    {
        switch(itemType())
        {
        case SGIItemTypeObject:
            {
                if(_table)
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                ret = callNextHandler(os);

                if(object)
                {
                    os << "<tr><td>proxyName</td><td>" << object->name() << "</td></tr>" << std::endl;
                    os << "<tr><td>proxyDisplayName</td><td>" << object->displayName() << "</td></tr>" << std::endl;
                    os << "<tr><td>proxyTypeName</td><td>" << object->typeName() << "</td></tr>" << std::endl;
                    os << "<tr><td>realItem</td><td>" << helpers::getRTTIObjectNameAndType_html(object->realItem(false)) << "</td></tr>" << std::endl;
                }

                if(_table)
                    os << "</table>" << std::endl;
            }
            break;
        case SGIItemTypeProxyRealItem:
            {
                SGIItemBase * realObject = object ? object->realItem(true) : nullptr;
                if(realObject)
                    ret = _hostInterface->writePrettyHTML(os, realObject);
                else
                {
                    os <<  "<i>nullptr</i>";
                    ret = true;
                }
            }
            break;
        default:
            ret = callNextHandler(os);
            break;
        }
    }
    return ret;
}

bool writePrettyHTMLImpl<SGIItemHolder>::process(std::basic_ostream<char>& os)
{
    SGIItemHolder * object = getObject<SGIItemHolder, SGIItemInternal>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            callNextHandler(os);

            if (_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        callNextHandler(os);
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<Image>::process(std::basic_ostream<char>& os)
{
    Image * object = getObject<Image, SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            callNextHandler(os);

            os << "<tr><td>width</td><td>" << object->width() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->height() << "</td></tr>" << std::endl;
            os << "<tr><td>allocatedWidth</td><td>" << object->allocatedWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>allocatedHeight</td><td>" << object->allocatedHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>depth</td><td>" << object->depth() << "</td></tr>" << std::endl;
            os << "<tr><td>pitch</td><td>" << object->pitch(0) << "," << object->pitch(1) << "," << object->pitch(2) << "," << object->pitch(3) << "</td></tr>" << std::endl;
            os << "<tr><td>planeOffset</td><td>" << object->planeOffset(0) << "," << object->planeOffset(1) << "," << object->planeOffset(2) << "," << object->planeOffset(3) << "</td></tr>" << std::endl;
            os << "<tr><td>scale</td><td>" << object->hscale() << '/' << object->vscale() << "</td></tr>" << std::endl;
            os << "<tr><td>pixelSize</td><td>" << object->horizontalPixelSize() << '/' << object->verticalPixelSize() << "</td></tr>" << std::endl;
            os << "<tr><td>origin</td><td>" << Image::originToString(object->origin()) << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << Image::imageFormatToString(object->format()) << "</td></tr>" << std::endl;
            os << "<tr><td>dataType</td><td>" << Image::dataTypeToString(object->dataType()) << "</td></tr>" << std::endl;
            os << "<tr><td>bitsPerPixel</td><td>" << object->bitsPerPixel() << "</td></tr>" << std::endl;
            os << "<tr><td>data</td><td>" << object->data() << "</td></tr>" << std::endl;
            os << "<tr><td>length</td><td>" << object->length() << "</td></tr>" << std::endl;
            os << "<tr><td>originalImage</td><td>" << helpers::getRTTIObjectNameAndType_html(object->originalImage()) << "</td></tr>" << std::endl;
            os << "<tr><td>originalImageQt</td><td>" << helpers::getRTTIObjectNameAndType_html(object->originalImageQt()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        callNextHandler(os);
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<ISceneGraphDialog>::process(std::basic_ostream<char>& os)
{
    ISceneGraphDialog * object = getObject<ISceneGraphDialog,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>dialog</td><td>" << helpers::getRTTIObjectNameAndType_html(object->getDialog()) << "</td></tr>" << std::endl;
            os << "<tr><td>visible</td><td>" << (object->isVisible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>host callback</td><td>" << helpers::getRTTIObjectNameAndType_html(object->getHostCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>toolsMenu</td><td>" << helpers::getRTTIObjectNameAndType_html(object->toolsMenu()) << "</td></tr>" << std::endl;
            os << "<tr><td>selectedItem</td><td>" << helpers::getRTTIObjectNameAndType_html(object->selectedItem()) << "</td></tr>" << std::endl;
            os << "<tr><td>rootItem</td><td>" << helpers::getRTTIObjectNameAndType_html(object->rootItem()) << "</td></tr>" << std::endl;
            os << "<tr><td>item</td><td>" << helpers::getRTTIObjectNameAndType_html(object->item()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        callNextHandler(os);
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<sgi::ImageGLWidget>::process(std::basic_ostream<char>& os)
{
    sgi::ImageGLWidget * object = getObject<sgi::ImageGLWidget,SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>hasError</td><td>" << (object->hasError() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>image</td><td>" << helpers::getRTTIObjectNameAndType_html(object->image()) << "</td></tr>" << std::endl;
            os << "<tr><td>backgroundColor</td><td>" << object->backgroundColor() << "</td></tr>" << std::endl;
            os << "<tr><td>colorFilterFragment</td><td><pre>" << object->colorFilterFragment() << "</pre></td></tr>" << std::endl;
            os << "<tr><td>colorFilterVertex</td><td><pre>" << object->colorFilterVertex() << "</pre></td></tr>" << std::endl;

            os << "<tr><td>vertexBuffer</td><td>" << helpers::getRTTIObjectNameAndType_html(object->vertexBuffer()) << "</td></tr>" << std::endl;
            os << "<tr><td>vao</td><td>" << helpers::getRTTIObjectNameAndType_html(object->vao()) << "</td></tr>" << std::endl;
            os << "<tr><td>program</td><td>" << helpers::getRTTIObjectNameAndType_html(object->program()) << "</td></tr>" << std::endl;
            os << "<tr><td>texture</td><td>" << helpers::getRTTIObjectNameAndType_html(object->texture()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        callNextHandler(os);
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<sgi::QtProxy>::process(std::basic_ostream<char>& os)
{
    sgi::QtProxy * object = getObject<sgi::QtProxy,SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>ptr</td><td>" << (void*)object << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        callNextHandler(os);
        break;
    }
    return true;
}

GET_OBJECT_NAME_IMPL_TEMPLATE()
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(details::Referenced)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(sgi::SGIPlugins)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(sgi::QtProxy)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(Image)

std::string getObjectNameImpl<details::Referenced>::process()
{
    details::Referenced * object = getObject<details::Referenced,SGIItemInternal>();
    return helpers::getRTTIObjectNameAndType(object);
}

std::string getObjectNameImpl<ReferencedInternalItemData>::process()
{
    return "ReferencedInternalItemData";
}

std::string getObjectNameImpl<ReferencedInternalInfoData>::process()
{
    return "ReferencedInternalInfoData";
}

std::string getObjectNameImpl<SGIPlugins>::process()
{
    return "SGIPlugins";
}

std::string getObjectNameImpl<QtProxy>::process()
{
    return "SGIQtProxy";
}

std::string getObjectNameImpl<Image>::process()
{
    return "SGIImage";
}

std::string getObjectNameImpl<SGIProxyItemBase>::process()
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    SGIItemBase * realObject = object ? object->realItem(GET_OBJECT_NAME_PROXYITEM_GET_INSTANCE) : nullptr;
    std::string ret;
    switch(itemType())
    {
    case SGIItemTypeProxyRealItem:
        if(realObject)
        {
            if(!_hostInterface->getObjectName(ret, realObject))
                ret = object->name();
        }
        else
            ret = object->name();
        break;
    default:
        ret = object->name();
        break;
    }
    return ret;
}

GET_OBJECT_TYPE_IMPL_TEMPLATE()
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(details::Referenced)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(sgi::SGIPlugins)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(sgi::QtProxy)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(Image)

std::string getObjectTypeImpl<details::Referenced>::process()
{
    details::Referenced * object = getObject<details::Referenced, SGIItemInternal>();
    return helpers::getRTTITypename(object);
}

std::string getObjectTypeImpl<ReferencedInternalItemData>::process()
{
    return "sgi::ReferencedInternalItemData";
}

std::string getObjectTypeImpl<ReferencedInternalInfoData>::process()
{
    return "sgi::ReferencedInternalInfoData";
}

std::string getObjectTypeImpl<SGIPlugins>::process()
{
    return "sgi::SGIPlugins";
}

std::string getObjectTypeImpl<QtProxy>::process()
{
    return "sgi::QtProxy";
}

std::string getObjectTypeImpl<Image>::process()
{
    return "sgi::Image";
}

std::string getObjectTypeImpl<SGIProxyItemBase>::process()
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    SGIItemBase * realObject = object->realItem(GET_OBJECT_TYPE_PROXYITEM_GET_INSTANCE);
    std::string ret;
    if(realObject)
    {
        if(!_hostInterface->getObjectTypename(ret, realObject))
            ret = object->typeName();
    }
    else
        ret = object->typeName();
    if(ret.empty())
        ret = "sgi::SGIProxyItemBase";
    return ret;
}

GET_OBJECT_DISPLAYNAME_IMPL_TEMPLATE()
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(details::Referenced)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(sgi::SGIPlugins)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(sgi::QtProxy)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(Image)

std::string getObjectDisplayNameImpl<details::Referenced>::process()
{
    details::Referenced * object = getObject<details::Referenced, SGIItemInternal>();
    return helpers::getRTTIObjectNameAndType(object);
}

std::string getObjectDisplayNameImpl<ReferencedInternalItemData>::process()
{
    return "ReferencedInternalItemData";
}

std::string getObjectDisplayNameImpl<ReferencedInternalInfoData>::process()
{
    return "ReferencedInternalInfoData";
}

std::string getObjectDisplayNameImpl<SGIPlugins>::process()
{
    return "SGIPlugins";
}

std::string getObjectDisplayNameImpl<QtProxy>::process()
{
    return "SGIQtProxy";
}

std::string getObjectDisplayNameImpl<SGIProxyItemBase>::process()
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    SGIItemBase * realObject = object->realItem(GET_OBJECT_DISPLAYNAME_PROXYITEM_GET_INSTANCE);
    std::string ret;
    if(realObject)
    {
        if(!_hostInterface->getObjectDisplayName(ret, realObject))
            ret = object->displayName();
    }
    else
        ret = object->displayName();
    if(ret.empty())
        ret = object->name();
    return ret;
}

std::string getObjectDisplayNameImpl<Image>::process()
{
    return "SGIImage";
}
OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(SGIPlugins)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(sgi::ImageGLWidget)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(sgi::QtProxy)

bool objectTreeBuildImpl<ReferencedInternalItemData>::build(IObjectTreeItem * treeItem)
{
    ReferencedInternalItemData * object = getObject<ReferencedInternalItemData,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const SGIItemBasePtr & data = object->data().item;
            SGIHostItemInternal holder(data->holder());
            if (holder.hasObject())
                treeItem->addChild("Holder", &holder);

            if(data->nextBase())
            {
                SGIHostItemInternal next(new ReferencedInternalItemData(data->nextBase()));
                if(next.hasObject())
                    treeItem->addChild("Next", &next);
            }
            if(data->previousBase())
            {
                SGIHostItemInternal previous(new ReferencedInternalItemData(data->previousBase()));
                if(previous.hasObject())
                    treeItem->addChild("Previous", &previous);
            }
            SGIItemBasePtr rootBase = data->rootBase();
            if(rootBase.valid() && rootBase.get() != data.get())
            {
                SGIHostItemInternal root(new ReferencedInternalItemData(rootBase.get()));
                if(root.hasObject())
                    treeItem->addChild("Root", &root);
            }
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<ReferencedInternalInfoData>::build(IObjectTreeItem * treeItem)
{
    ReferencedInternalInfoData * object = getObject<ReferencedInternalInfoData,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<SGIPlugins>::build(IObjectTreeItem * treeItem)
{
    SGIPlugins * object = getObject<SGIPlugins,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        {
            ret = true;
            treeItem->addChild("Plugins", cloneItem<SGIItemQt>(SGIItemTypePlugins, ~0u));
            treeItem->addChild("Backend plugins", cloneItem<SGIItemQt>(SGIItemTypeBackendPlugins, ~0u));
        }
        break;
    case SGIItemTypePlugins:
        ret = true;
        break;
    case SGIItemTypeBackendPlugins:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<sgi::QtProxy>::build(IObjectTreeItem * treeItem)
{
    sgi::QtProxy * object = getObject<sgi::QtProxy,SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        {
            SGIHostItemInternal plugins(SGIPlugins::instance());
            treeItem->addChild("Plugins", &plugins);
        }
        break;
    case SGIItemTypePlugins:
        ret = true;
        break;
    case SGIItemTypeBackendPlugins:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<SGIProxyItemBase>::build(IObjectTreeItem * treeItem)
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    bool ret;
    bool getInstance = (itemType() == SGIItemTypeProxyRealItem)?true:OBJECTTREE_PROXYITEM_GET_INSTANCE;
    SGIItemBase * realObject = object->realItem(getInstance);
    if(realObject)
        ret = _hostInterface->objectTreeBuildTree(treeItem, realObject);
    else
    {
        bool ret = false;
        switch(itemType())
        {
        case SGIItemTypeObject:
            {
                ret = callNextHandler(treeItem);
                if(!OBJECTTREE_PROXYITEM_GET_INSTANCE)
                {
                    treeItem->addChild("Real item", _item->clone<SGIItemInternal>(SGIItemTypeProxyRealItem));
                    ret = true;
                }
            }
            break;
        case SGIItemTypeProxyRealItem:
            ret = false;
            break;
        default:
            ret = callNextHandler(treeItem);
            break;
        }
        return ret;
    }
    return ret;
}

bool objectTreeBuildImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = getObject<ISceneGraphDialog,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            callNextHandler(treeItem);

            SGIHostItemQt dialog(object->getDialog());
            if(dialog.hasObject())
                treeItem->addChild("Dialog", &dialog);

            SGIHostItemInternal hostCallback(object->getHostCallback());
            if(hostCallback.hasObject())
                treeItem->addChild("Host callback", &hostCallback);

            SGIHostItemInternal toolsMenu(object->toolsMenu());
            if(toolsMenu.hasObject())
                treeItem->addChild("Tools menu", &toolsMenu);

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<sgi::ImageGLWidget>::build(IObjectTreeItem * treeItem)
{
    sgi::ImageGLWidget * object = getObject<sgi::ImageGLWidget,SGIItemQt>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemInternal image(object->image());
            if(image.hasObject())
                treeItem->addChild("Image", &image);

            SGIHostItemQt vao(object->vao());
            if(vao.hasObject())
                treeItem->addChild("VAO", &vao);

            SGIHostItemQt program(object->program());
            if(program.hasObject())
                treeItem->addChild("Program", &program);

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

CONTEXT_MENU_POPULATE_IMPL_TEMPLATE()
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(SGIPlugins)

bool contextMenuPopulateImpl<SGIProxyItemBase>::populate(IContextMenuItem * menuItem)
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    bool ret;
    bool getInstance = (itemType() == SGIItemTypeProxyRealItem)?true:CONTEXTMENU_PROXYITEM_GET_INSTANCE;
    SGIItemBase * realObject = object->realItem(getInstance);
    if(realObject)
        ret = _hostInterface->contextMenuPopulate(menuItem, realObject);
    else
    {
        switch(itemType())
        {
        case SGIItemTypeObject:
            {
                ret = callNextHandler(menuItem);
                if(!CONTEXTMENU_PROXYITEM_GET_INSTANCE)
                {
                    menuItem->addMenu("Real item", _item->clone<SGIItemInternal>(SGIItemTypeProxyRealItem));
                    ret = true;
                }
            }
            break;
        case SGIItemTypeProxyRealItem:
            ret = false;
            break;
        default:
            ret = callNextHandler(menuItem);
            break;
        }
    }
    return ret;
}

bool contextMenuPopulateImpl<SGIPlugins>::populate(IContextMenuItem * menuItem)
{
    SGIPlugins * object = getObject<SGIPlugins, SGIItemInternal>();
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            callNextHandler(menuItem);
            menuItem->addSimpleAction(MenuActionSGIAbout, "About SGI", _item);
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

ACTION_HANDLER_IMPL_TEMPLATE()
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionSGIAbout)

bool actionHandlerImpl<MenuActionSGIAbout>::execute()
{
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogAbout, menu()->parentWidget(), hostCallback());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if (ret)
    {
        if (dialog.valid())
            dialog->show();
    }
    return ret;
}

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()
SETTINGS_DIALOG_CREATE_IMPL_DECLARE_AND_REGISTER(SettingsDialogAbout)

bool settingsDialogCreateImpl<SettingsDialogAbout>::execute(ISettingsDialogPtr & dialog)
{
    //return openDialog<CameraSettings>(dialog);
    return false;
}

OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = getObject<ISceneGraphDialog,SGIItemInternal>();

    // no need to use a proxy node here, because the plugin instance is already created anyway
    SGIHostItemInternal hostItem(SGIPlugins::instance());
    treeItem->addChild(std::string(), &hostItem);

    treeItem->addChild(std::string("ISceneGraphDialog"), _item.get());
    return true;
}

typedef generateItemImplT<generateItemAcceptImpl, SGIItemInternal, SGIItemQt> generateItemImpl;

typedef SGIPluginImplementationT<       generateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectDisplayNameImpl,
                                        getObjectTypeImpl,
                                        defaultPluginGetObjectPathImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        defaultPluginWriteObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl,
                                        settingsDialogCreateImpl,
                                        defaultPluginGuiAdapterSetViewImpl
                                        >
    SGIPluginImpl;

} // namespace internal_plugin

class SGIPlugin_internal::Plugin : public internal_plugin::SGIPluginImpl
{
public:
    static Plugin * s_instance;
    Plugin(SGIPluginHostInterface * hostInterface=nullptr)
        : internal_plugin::SGIPluginImpl(hostInterface)
    {
        s_instance = this;
        SGIITEMTYPE_NAME(SGIItemTypeInvalid);
        SGIITEMTYPE_NAME(SGIItemTypeDummy);
        SGIITEMTYPE_NAME(SGIItemTypeTreeRoot);
        SGIITEMTYPE_NAME(SGIItemTypeObject);
        SGIITEMTYPE_NAME(SGIItemTypeChilds);
        SGIITEMTYPE_NAME(SGIItemTypeParents);
        SGIITEMTYPE_NAME(SGIItemTypeObservers);
        SGIITEMTYPE_NAME(SGIItemTypeStatistics);
        SGIITEMTYPE_NAME(SGIItemTypeCallbacks);
        SGIITEMTYPE_NAME(SGIItemTypeSettings);
        SGIITEMTYPE_NAME(SGIItemTypeObjectLogger);
    }
    Plugin(const Plugin & rhs)
        : internal_plugin::SGIPluginImpl(rhs)
    {
    }

    SGI_Object(sgi, Plugin)
};

SGIPlugin_internal::Plugin * SGIPlugin_internal::Plugin::s_instance = nullptr;
const char * SGIPlugin_internal::PluginName = "_sgi_internal";

SGIPluginInterface * SGIPlugin_internal::create(SGIPluginHostInterface * hostInterface)
{
    return new Plugin(hostInterface);
}

} // namespace sgi
