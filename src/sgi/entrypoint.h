#pragma once

#include "sgi/Export"

class QWidget;
namespace sgi {
    class ISceneGraphDialogInfo;
    class ISceneGraphDialog;

    class IObjectLoggerDialogInfo;
    class IObjectLoggerDialog;

    class IContextMenu;
    class IContextMenuInfo;
    class IContextMenuQt;
    class IContextMenuInfoQt;

    class SGIHostItemBase;
    class SGIItemBase;

    typedef osg::ref_ptr<SGIItemBase> SGIItemBasePtr;
    typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;

    typedef QWidget * QWidgetPtr;
} // namespace sgi

namespace osgDB {
    class Options;
}

extern "C" {

SGI_EXPORT const char* sgiGetVersion();
SGI_EXPORT const char* sgiGetSOVersion();
SGI_EXPORT const char* sgiGetLibraryName();

SGI_EXPORT void sgi_shutdown();
SGI_EXPORT bool sgi_generateItem(osg::ref_ptr<sgi::SGIItemBase> & item, const sgi::SGIHostItemBase * object);
SGI_EXPORT bool sgi_parentWidget(sgi::QWidgetPtr & widget, const sgi::SGIHostItemBase * object);
SGI_EXPORT bool sgi_parentWidgetItem(sgi::QWidgetPtr & widget, sgi::SGIItemBase * item);
SGI_EXPORT sgi::ISceneGraphDialog * sgi_showSceneGraphDialog(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::ISceneGraphDialogInfo * info);
SGI_EXPORT sgi::ISceneGraphDialog * sgi_showSceneGraphDialogItem(QWidget *parent, sgi::SGIItemBase * item, sgi::ISceneGraphDialogInfo * info);
SGI_EXPORT sgi::IObjectLoggerDialog* sgi_showObjectLoggerDialog(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::IObjectLoggerDialogInfo * info);
SGI_EXPORT sgi::IObjectLoggerDialog* sgi_showObjectLoggerDialogItem(QWidget *parent, sgi::SGIItemBase * item, sgi::IObjectLoggerDialogInfo * info);
SGI_EXPORT sgi::IContextMenu * sgi_createContextMenu(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::IContextMenuInfo * info);
SGI_EXPORT sgi::IContextMenu * sgi_createContextMenuItem(QWidget *parent, sgi::SGIItemBase * item, sgi::IContextMenuInfo * info);
SGI_EXPORT sgi::IContextMenuQt * sgi_createContextMenuItemQt(QWidget *parent, QObject * item, sgi::IContextMenuInfoQt * info);
SGI_EXPORT void sgi_writePrettyHTML(std::basic_ostream<char>& os, const sgi::SGIHostItemBase * object, bool table);
SGI_EXPORT bool sgi_getObjectName(std::string & name, const sgi::SGIHostItemBase * object, bool full);
SGI_EXPORT bool sgi_getObjectDisplayName(std::string & name, const sgi::SGIHostItemBase * object);
SGI_EXPORT bool sgi_getObjectTypename(std::string & name, const sgi::SGIHostItemBase * object, bool full);
SGI_EXPORT bool sgi_getObjectSuggestedFilename(std::string & filename, const sgi::SGIHostItemBase * object);
SGI_EXPORT bool sgi_getObjectSuggestedFilenameExtension(std::string & ext, const sgi::SGIHostItemBase * object);
SGI_EXPORT bool sgi_getObjectFilenameFilters(std::vector<std::string> & filters, const sgi::SGIHostItemBase * object);
SGI_EXPORT bool sgi_getObjectPath(sgi::SGIItemBasePtrPath & path, const sgi::SGIHostItemBase * object);

} // extern "C"
