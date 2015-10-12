#include "stdafx.h"
#include "entrypoint.h"
#include "SGIPlugin.h"

extern "C" {

static unsigned g_sgiModuleInitCount = 0;

const char* sgiGetVersion()
{
    return SGI_VERSION_STR;
}

const char* sgiGetSOVersion()
{
    return SGI_SOVERSION_STR;
}

const char* sgiGetLibraryName()
{
    return "SGI Library";
}

sgi::IHostCallback * sgi_defaultHostCallback()
{
	return sgi::SGIPlugins::instance()->defaultHostCallback();
}

void sgi_setHostCallback(sgi::IHostCallback * callback)
{
	sgi::SGIPlugins::instance()->setHostCallback(callback);
}

bool sgi_generateItem(osg::ref_ptr<sgi::SGIItemBase> & item, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->generateItem(item, object);
}

bool sgi_parentWidget(sgi::QWidgetPtr & widget, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->parentWidget(widget, object);
}

bool sgi_parentWidgetItem(sgi::QWidgetPtr & widget, sgi::SGIItemBase * item)
{
    return sgi::SGIPlugins::instance()->parentWidget(widget, item);
}

void sgi_shutdown()
{
    sgi::SGIPlugins::instance()->shutdown();
    sgi::SGIPlugins::instance(true);
}

sgi::ISceneGraphDialog * sgi_showSceneGraphDialog(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->showSceneGraphDialog(parent, object, callback);
}

sgi::ISceneGraphDialog * sgi_showSceneGraphDialogItem(QWidget *parent, sgi::SGIItemBase * item, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->showSceneGraphDialog(parent, item, callback);
}

sgi::IObjectLoggerDialog* sgi_showObjectLoggerDialog(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->showObjectLoggerDialog(parent, object, callback);
}

sgi::IObjectLoggerDialog* sgi_showObjectLoggerDialogItem(QWidget *parent, sgi::SGIItemBase * item, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->showObjectLoggerDialog(parent, item, callback);
}

sgi::IImagePreviewDialog * sgi_showImagePreviewDialog(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->showImagePreviewDialog(parent, object, callback);
}

sgi::IImagePreviewDialog * sgi_showImagePreviewDialogItem(QWidget *parent, sgi::SGIItemBase * item, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->showImagePreviewDialog(parent, item, callback);
}

sgi::IContextMenu * sgi_createContextMenu(QWidget *parent, const sgi::SGIHostItemBase * object, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->createContextMenu(parent, object, callback);
}

sgi::IContextMenu * sgi_createContextMenuItem(QWidget *parent, sgi::SGIItemBase * item, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->createContextMenu(parent, item, callback);
}

sgi::IContextMenuQt * sgi_createContextMenuItemQt(QWidget *parent, QObject * item, sgi::IHostCallback * callback)
{
    return sgi::SGIPlugins::instance()->createContextMenu(parent, item, callback);
}

void sgi_writePrettyHTML(std::basic_ostream<char>& os, const sgi::SGIHostItemBase * object, bool table)
{
    sgi::SGIPlugins::instance()->writePrettyHTML(os, object, table);
}

void sgi_writePrettyHTMLItem(std::basic_ostream<char>& os, const sgi::SGIItemBase * item, bool table)
{
    sgi::SGIPlugins::instance()->writePrettyHTML(os, item, table);
}

bool sgi_getObjectName(std::string & name, const sgi::SGIHostItemBase * object, bool full)
{
    return sgi::SGIPlugins::instance()->getObjectName(name, object, full);
}

bool sgi_getObjectDisplayName(std::string & name, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->getObjectDisplayName(name, object);
}

bool sgi_getObjectTypename(std::string & name, const sgi::SGIHostItemBase * object, bool full)
{
    return sgi::SGIPlugins::instance()->getObjectTypename(name, object, full);
}

bool sgi_getObjectSuggestedFilename(std::string & filename, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->getObjectSuggestedFilename(filename, object);
}
bool sgi_getObjectSuggestedFilenameExtension(std::string & ext, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->getObjectSuggestedFilenameExtension(ext, object);
}
bool sgi_getObjectFilenameFilters(std::vector<std::string> & filters, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->getObjectFilenameFilters(filters, object);
}
bool sgi_getObjectPath(sgi::SGIItemBasePtrPath & path, const sgi::SGIHostItemBase * object)
{
    return sgi::SGIPlugins::instance()->getObjectPath(path, object);
}

} // extern "C"
