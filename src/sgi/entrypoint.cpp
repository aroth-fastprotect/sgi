#include "stdafx.h"
#include "entrypoint.h"
#include "SGIPlugin.h"

#if !defined(_WIN32)
#include <dlfcn.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" {

static unsigned g_sgiModuleInitCount = 0;
#ifdef _WIN32
static HMODULE g_sgiModuleHandle = NULL;
#endif // _WIN32

#ifdef _WIN32
/// @brief DLL main entry point
/// @remarks leave this function outside the terra3d namespace
/// @note Welcome to the land of darkness!
///       Within this function, Win32 cannot execute ANY thread related
///       functions (e.g. WaitForSingleObject, CreateThread, etc). This
///       is caused by a dubious implementation by M$. Some people might
///       think about this as a BUG and kindly it IS one (a very famous
///       one).
/// this function simply calls @a initializeCommonCore on load and
/// @a deinitializeCommonCore at unload.
BOOL WINAPI DllMain(__in  HMODULE hModule,
                    __in  DWORD fdwReason,
                    __in  LPVOID lpvReserved
                    )
{
    switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_sgiModuleHandle = hModule;
        break;
    case DLL_PROCESS_DETACH:
        g_sgiModuleHandle = NULL;
        break;
    }
    return TRUE;
}
#endif // _WIN32

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

/// @brief returns the full path of the terra3d-common-core library
/// @remarks on Linux it returns the filename of the current process
/// @return full path
const char * sgiGetLibraryModuleFilename()
{
    static std::string s_cachedFilename;
    if (s_cachedFilename.empty())
    {
#ifdef _WIN32
        char buf[512];
        DWORD len = ::GetModuleFileNameA(g_sgiModuleHandle, buf, _countof(buf));
        s_cachedFilename.assign(buf, len);
#else // _WIN32
        Dl_info info;
        const char * (* addr) () = sgiGetLibraryName;
        if(dladdr((const void*)addr, &info) != 0)
        {
            char buf[PATH_MAX];
            if(realpath(info.dli_fname, buf))
                s_cachedFilename = buf;
            else
                s_cachedFilename = info.dli_fname;
        }
#endif // _WIN32
    }
    return s_cachedFilename.c_str();
}

sgi::IHostCallback * sgi_defaultHostCallback()
{
	return sgi::SGIPlugins::instance()->defaultHostCallback();
}

sgi::IHostCallback * sgi_hostCallback()
{
    return sgi::SGIPlugins::instance()->hostCallback();
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
    sgi::SGIPlugins::shutdown();
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
