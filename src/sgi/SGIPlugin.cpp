#include "stdafx.h"
#include "SGIPlugin.h"
#include "sgi/plugins/SGIItemBase.h"
#include "sgi/plugins/SGIPluginImpl.h"
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/PluginQuery>
//#include <osg/Notify>

#include <QInputDialog>
#include <QFileDialog>
#include <QColorDialog>

#include "QtProxy.h"
#include "sgi_internal.h"
#include "QTextDialog.h"
#include "DoubleInputDialog.h"
#include <sgi/plugins/ImagePreviewDialog>
#include <sgi/plugins/ObjectLoggerDialog>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ContextMenu>
#include <sgi/helpers/qt>

using namespace sgi;
using namespace sgi::qt_helpers;

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const SGIItemBase * item)
{
    const SGIPluginInfo * pluginInfo = (const SGIPluginInfo* )item->pluginInfo();
    return os << '{' << (void*)item << '/' << pluginInfo->pluginName
        << ";type=" << item->type()
        << ";typeName=" << item->typeName()
        << '}';
}

namespace {
	class DisableLibraryLoadErrors 
	{
	public:
		DisableLibraryLoadErrors()
		{
#ifdef _WIN32
			_oldErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif
		}
		~DisableLibraryLoadErrors()
		{
#ifdef _WIN32
			::SetErrorMode(_oldErrorMode);
#endif
		}
	private:
#ifdef _WIN32
		unsigned _oldErrorMode;
#endif
	};
}

class SGIPlugins::SGIPluginsImpl
{
public:
    typedef std::pair<std::string, std::string> PluginFileName;
    typedef std::list<PluginFileName> PluginFileNameList;

    enum PluginType {
        PluginTypeModel = 0,
        PluginTypeUI
    };

    SGIPluginsImpl()
        : _pluginsLoaded(false)
        , _hostInterface(this)
		, _defaultHostCallback(new DefaultHostCallback(this))
    {
        {
            const std::string& value_type = details::StaticTypeName<sgi::SGIItemType>::name();
            registerNamedEnum(value_type, "SGIItemType", false);
        }

        loadInternalPlugin();
        PluginFileNameList pluginFiles = listAllAvailablePlugins();
        for(PluginFileNameList::const_iterator it = pluginFiles.begin(); it != pluginFiles.end(); it++)
        {
            const std::string & pluginName = (*it).first;
            const std::string & file = (*it).second;
            //std::cout << "Found plugin " << pluginName << ": " << file << std::endl;
        }

    }
    ~SGIPluginsImpl()
    {
    }

    static SGIPluginsImpl* instance(bool erase=false)
    {
        static SGIPluginsImpl* s_impl = new SGIPluginsImpl;
        if (erase)
        {
            delete s_impl;
            s_impl = 0;
        }

        return s_impl; // will return NULL on erase
    }

    SGIPluginHostInterface * hostInterface()
    {
        return &_hostInterface;
    }
	IHostCallback * defaultHostCallback()
	{
		return _defaultHostCallback.get();
	}
	IHostCallback * hostCallback()
	{
		if (_hostCallback.valid())
			return _hostCallback.get();
		else
			return _defaultHostCallback.get();
	}
	void setHostCallback(IHostCallback * callback)
	{
		if (callback)
			_hostCallback = callback;
		else
			_hostCallback = NULL;
	}

    class HostInterface : public SGIPluginHostInterface
    {
    public:
        HostInterface(SGIPluginsImpl * impl)
            : _impl(impl)
            {
            }
		IHostCallback * defaultHostCallback() 
		{
			return _impl->defaultHostCallback();
		}
		IHostCallback * hostCallback()
		{
			return _impl->hostCallback();
		}
		void setHostCallback(IHostCallback * callback)
		{
			return _impl->setHostCallback(callback);
		}
        bool generateItem(osg::ref_ptr<SGIItemBase> & item, const SGIHostItemBase * object)
        {
            return _impl->generateItem(item, object);
        }
        bool writePrettyHTML(std::basic_ostream<char>& os, const SGIItemBase * item, bool table)
        {
            return _impl->writePrettyHTML(os, item, table);
        }
        bool writePrettyHTML(std::basic_ostream<char>& os, const SGIHostItemBase * object, bool table)
        {
            return _impl->writePrettyHTML(os, object, table);
        }
        bool getObjectName(std::string & name, const SGIHostItemBase * object, bool full)
        {
            return _impl->getObjectName(name, object, full);
        }
        bool getObjectName(std::string & name, const SGIItemBase * item, bool full)
        {
            return _impl->getObjectName(name, item, full);
        }
        bool getObjectDisplayName(std::string & name, const SGIHostItemBase * object, bool full)
        {
            return _impl->getObjectDisplayName(name, object, full);
        }
        bool getObjectDisplayName(std::string & name, const SGIItemBase * item, bool full)
        {
            return _impl->getObjectDisplayName(name, item, full);
        }
        bool getObjectTypename(std::string & name, const SGIHostItemBase * object, bool full)
        {
            return _impl->getObjectTypename(name, object, full);
        }
        bool getObjectTypename(std::string & name, const SGIItemBase * item, bool full)
        {
            return _impl->getObjectTypename(name, item, full);
        }
        bool getObjectSuggestedFilename(std::string & filename, const SGIHostItemBase * object)
        {
            return _impl->getObjectSuggestedFilename(filename, object);
        }
        bool getObjectSuggestedFilename(std::string & filename, const SGIItemBase * item)
        {
            return _impl->getObjectSuggestedFilename(filename, item);
        }
        bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIHostItemBase * object)
        {
            return _impl->getObjectSuggestedFilenameExtension(ext, object);
        }
        bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIItemBase * item)
        {
            return _impl->getObjectSuggestedFilenameExtension(ext, item);
        }
        bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIHostItemBase * object)
        {
            return _impl->getObjectFilenameFilters(filters, object);
        }
        bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIItemBase * item)
        {
            return _impl->getObjectFilenameFilters(filters, item);
        }
        bool getObjectPath(SGIItemBasePtrPath & path, const SGIHostItemBase * object)
        {
            return _impl->getObjectPath(path, object);
        }
        bool getObjectPath(SGIItemBasePtrPath & path, const SGIItemBase * item)
        {
            return _impl->getObjectPath(path, item);
        }
        bool writeObjectFile(bool & result, const SGIHostItemBase * object, const std::string & filename, const SGIItemBase* options)
        {
            return _impl->writeObjectFile(result, object, filename, options);
        }
        bool writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options)
        {
            return _impl->writeObjectFile(result, item, filename, options);
        }
        IContextMenu * createContextMenu(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
        {
            return _impl->createContextMenu(parent, object, callback);
        }
        IContextMenu * createContextMenu(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
        {
            return _impl->createContextMenu(parent, item, callback);
        }
        ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
        {
            return _impl->showSceneGraphDialog(parent, object, callback);
        }
        ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
        {
            return _impl->showSceneGraphDialog(parent, item, callback);
        }
        bool createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
        {
            return _impl->createObjectLogger(logger, object);
        }
        bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
        {
            return _impl->createObjectLogger(logger, item);
        }
        bool getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
        {
            return _impl->getObjectLogger(logger, object);
        }
        bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
        {
            return _impl->getObjectLogger(logger, item);
        }
        bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
        {
            return _impl->getOrCreateObjectLogger(logger, object);
        }
        bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
        {
            return _impl->getOrCreateObjectLogger(logger, item);
        }
        IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback)
        {
            return _impl->showObjectLoggerDialog(parent, logger, callback);
        }
        IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
        {
            return _impl->showObjectLoggerDialog(parent, object, callback);
        }
        IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
        {
            return _impl->showObjectLoggerDialog(parent, item, callback);
        }
        bool objectTreeBuildTree(IObjectTreeItem * treeItem, SGIItemBase * item)
        {
            return _impl->objectTreeBuildTree(treeItem, item);
        }
        bool objectTreeBuildRootTree(IObjectTreeItem * treeItem, SGIItemBase * item)
        {
            return _impl->objectTreeBuildRootTree(treeItem, item);
        }
        bool contextMenuPopulate(IContextMenuItem * menuItem, const SGIHostItemBase * object, bool onlyRootItem)
        {
            return _impl->contextMenuPopulate(menuItem, object, onlyRootItem);
        }
        bool contextMenuPopulate(IContextMenuItem * menuItem, SGIItemBase * item, bool onlyRootItem)
        {
            return _impl->contextMenuPopulate(menuItem, item, onlyRootItem);
        }
        bool contextMenuExecute(IContextMenuAction * menuAction, SGIItemBase * item)
        {
            return _impl->contextMenuExecute(menuAction, item);
        }
        IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
        {
            return _impl->showImagePreviewDialog(parent, item, callback);
        }
        IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
        {
            return _impl->showImagePreviewDialog(parent, object, callback);
        }
        bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, const SGIHostItemBase * object, ISettingsDialogInfo * info)
        {
            return _impl->openSettingsDialog(dialog, object, info);
        }
        bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, SGIItemBase * item, ISettingsDialogInfo * info)
        {
            return _impl->openSettingsDialog(dialog, item, info);
        }
        bool inputDialogString(QWidget *parent, std::string & text, const std::string & label, const std::string & windowTitle, InputDialogStringEncoding encoding, SGIItemBase * item)
        {
            return _impl->inputDialogString(parent, text, label, windowTitle, encoding, item);
        }
        bool inputDialogText(QWidget *parent, std::string & text, const std::string & label, const std::string & windowTitle, InputDialogStringEncoding encoding, SGIItemBase * item)
        {
            return _impl->inputDialogText(parent, text, label, windowTitle, encoding, item);
        }
        bool inputDialogInteger(QWidget *parent, int & number, const std::string & label, const std::string & windowTitle, int minNumber, int maxNumber, int step, SGIItemBase * item)
        {
            return _impl->inputDialogInteger(parent, number, label, windowTitle, minNumber, maxNumber, step, item);
        }
        bool inputDialogDouble(QWidget *parent, double & number, const std::string & label, const std::string & windowTitle, double minNumber, double maxNumber, int decimals, SGIItemBase * item)
        {
            return _impl->inputDialogDouble(parent, number, label, windowTitle, minNumber, maxNumber, decimals, item);
        }
        bool inputDialogBitmask(QWidget *parent, unsigned & number, const std::string & label, const std::string & windowTitle, SGIItemBase * item)
        {
            return _impl->inputDialogBitmask(parent, number, label, windowTitle, item);
        }
        bool inputDialogColor(QWidget *parent, Color & color, const std::string & label, const std::string & windowTitle, SGIItemBase * item)
        {
            return _impl->inputDialogColor(parent, color, label, windowTitle, item);
        }
        bool inputDialogFilename(QWidget *parent, InputDialogFilenameType type, std::string & filename, const std::vector<std::string> & filters, const std::string & windowTitle, SGIItemBase * item)
        {
            return _impl->inputDialogFilename(parent, type, filename, filters, windowTitle, item);
        }
        bool inputDialogImage(QWidget *parent, Image & image, const std::string & label, const std::string & windowTitle, SGIItemBase * item)
        {
            return _impl->inputDialogImage(parent, image, label, windowTitle, item);
        }
        bool setView(SGIItemBase * view, const SGIItemBase * item, double animationTime = -1.0)
        {
            return _impl->setView(view, item, animationTime);
        }
        bool setView(const SGIHostItemBase * view, const SGIItemBase * item, double animationTime = -1.0)
        {
            return _impl->setView(view, item, animationTime);
        }
        bool setView(SGIItemBase * view, const SGIHostItemBase * item, double animationTime = -1.0)
        {
            return _impl->setView(view, item, animationTime);
        }
        bool setView(const SGIHostItemBase * view, const SGIHostItemBase * item, double animationTime = -1.0)
        {
            return _impl->setView(view, item, animationTime);
        }
        bool registerNamedEnum(const std::string & enumname, const std::string & description=std::string(), bool bitmask=false)
        {
            return _impl->registerNamedEnum(enumname, description, bitmask);
        }
        bool registerNamedEnumValue(const std::string & enumname, int value, const std::string & valuename)
        {
            return _impl->registerNamedEnumValue(enumname, value, valuename);
        }
        bool registerNamedEnumValues(const std::string & enumname, const std::map<int, std::string> & values)
        {
            return _impl->registerNamedEnumValues(enumname, values);
        }
        bool namedEnumValueToString(const std::string & enumname, std::string & text, int value)
        {
            return _impl->namedEnumValueToString(enumname, text, value);
        }

    private:
        SGIPluginsImpl * _impl;
    };

	class DefaultHostCallback : public IHostCallback
	{
	public:
		DefaultHostCallback(SGIPluginsImpl * impl)
			: _impl(impl) {}
		~DefaultHostCallback()
		{

		}

		IContextMenu *          contextMenu(QWidget * parent, const SGIItemBase * item) override
		{
			if (_contextMenu.valid())
			{
				_contextMenu->setObject(const_cast<SGIItemBase*>(item));
			}
			else
				_contextMenu = _impl->createContextMenu(parent, const_cast<SGIItemBase*>(item), this);
			return _contextMenu.get();
		}
		IContextMenu *          contextMenu(QWidget * parent, const SGIHostItemBase * item) override
		{
			if (_contextMenu.valid())
			{
				_contextMenu->setObject(item);
			}
			else
				_contextMenu = _impl->createContextMenu(parent, item, this);
			return _contextMenu.get();
		}
		ISceneGraphDialog *     showSceneGraphDialog(QWidget * parent, SGIItemBase * item) override
		{
            if (_dialog.valid())
                _dialog->setObject(item);
            else
                _dialog = _impl->showSceneGraphDialog(parent, item, this);
            if (_dialog.valid())
                _dialog->show();
            return _dialog.get();
		}
		ISceneGraphDialog *     showSceneGraphDialog(QWidget * parent, const SGIHostItemBase * item) override
		{
            if (_dialog.valid())
                _dialog->setObject(item);
            else
                _dialog = _impl->showSceneGraphDialog(parent, item, this);
            if (_dialog.valid())
                _dialog->show();
            return _dialog.get();
		}
		IObjectLoggerDialog *   showObjectLoggerDialog(QWidget * parent, SGIItemBase * item) override
		{
            if (!_loggerDialog.valid())
                _loggerDialog = _impl->showObjectLoggerDialog(parent, item, this);
            if (!_loggerDialog.valid())
                _loggerDialog->show();
            return _loggerDialog.get();
		}
		IObjectLoggerDialog *   showObjectLoggerDialog(QWidget * parent, const SGIHostItemBase * item) override
		{
            if (!_loggerDialog.valid())
                _loggerDialog = _impl->showObjectLoggerDialog(parent, item, this);
            if (!_loggerDialog.valid())
                _loggerDialog->show();
            return _loggerDialog.get();
		}
        IObjectLoggerDialog *   showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger) override
        {
            if (!_loggerDialog.valid())
                _loggerDialog = _impl->showObjectLoggerDialog(parent, logger, this);
            if (!_loggerDialog.valid())
                _loggerDialog->show();
            return _loggerDialog.get();
        }
		IImagePreviewDialog *   showImagePreviewDialog(QWidget * parent, SGIItemBase * item) override
		{
            if (_imagePreviewDialog.valid())
                _imagePreviewDialog->setObject(item);
            else
                _imagePreviewDialog = _impl->showImagePreviewDialog(parent, item, this);
            if(_imagePreviewDialog.valid())
                _imagePreviewDialog->show();
			return _imagePreviewDialog.get();
		}
		IImagePreviewDialog *   showImagePreviewDialog(QWidget * parent, const SGIHostItemBase * item) override
		{
			if (_imagePreviewDialog.valid())
				_imagePreviewDialog->setObject(item);
			else
				_imagePreviewDialog = _impl->showImagePreviewDialog(parent, item, this);
            if(_imagePreviewDialog.valid())
                _imagePreviewDialog->show();
			return _imagePreviewDialog.get();
		}
		virtual ReferencedPickerBase *  createPicker(PickerType type, float x, float y) override
		{
			return NULL;
		}
		void triggerRepaint() override
		{
			/* NOP */
		}
		SGIItemBase * getView() override
		{
			return NULL;
		}
        virtual QWidget * getFallbackParentWidget() override
        {
            return NULL;
        }
		virtual void shutdown() override
		{
			_contextMenu = NULL;
			_dialog = NULL;
			_loggerDialog = NULL;
			_imagePreviewDialog = NULL;
		}

	private:
		SGIPluginsImpl * _impl;
		sgi::IContextMenuPtr _contextMenu;
		sgi::ISceneGraphDialogPtr _dialog;
		sgi::IObjectLoggerDialogPtr _loggerDialog;
		sgi::IImagePreviewDialogPtr _imagePreviewDialog;
	};

    static std::string createLibraryNameForPlugin(const std::string& name)
    {
        return ".sgi_" + name + "_plugin";
    }

    static std::string createLibraryNameForPluginUI(const std::string& name)
    {
        return ".sgiui_" + name + "_plugin";
    }

    const PluginInfo * loadInternalPlugin()
    {
        PluginMap::iterator it = _plugins.find(SGIPlugin_internal::PluginName);
        if(it == _plugins.end())
        {
            PluginInfo info;
            info.pluginName = SGIPlugin_internal::PluginName;
            info.pluginInterface = SGIPlugin_internal::create(&_hostInterface);
            if (info.pluginInterface)
            {
                info.writePrettyHTMLInterface = info.pluginInterface->getWritePrettyHTML();
                info.objectInfoInterface = info.pluginInterface->getObjectInfo();
                info.objectTreeInterface = info.pluginInterface->getObjectTree();
                info.objectLoggerInterface = info.pluginInterface->getObjectLogger();
                info.contextMenuInterface = info.pluginInterface->getContextMenu();
                info.settingsDialogInterface = info.pluginInterface->getSettingsDialog();
                if(info.settingsDialogInterface)
                    info.pluginUIInterface = info.pluginInterface;
            }
            it = _plugins.insert(PluginMap::value_type(info.pluginName, info)).first;
        }
        return &it->second;
    }

    /// @brief load the given plugin
    /// @note only loads the model plugin (not the UI)
    /// @param name internal name of the plugin to load without any prefix or suffix
    /// @param filename optional filename of the library to load
    /// @return pointer to plugin info struct
    const PluginInfo * loadPlugin(const std::string & name, const std::string & filename=std::string())
    {
        PluginMap::iterator it = _plugins.find(name);
        if(it == _plugins.end())
        {
            PluginInfo info;
            info.pluginName = name;

            if(!_pluginLoadOpts.valid())
            {
                osg::ref_ptr<osgDB::Options> defaultOpts = osgDB::Registry::instance()->getOptions();
                if(defaultOpts.valid())
                    _pluginLoadOpts = static_cast<osgDB::Options*>(osgDB::Registry::instance()->getOptions()->clone(osg::CopyOp::SHALLOW_COPY));
                else
                    _pluginLoadOpts = new osgDB::Options();
                _pluginLoadOpts->setPluginData("hostInterface", &_hostInterface);
            }

            std::string pluginFilename = createLibraryNameForPlugin(name);
			{
				DisableLibraryLoadErrors disable_load_errors;
				osgDB::ReaderWriter::ReadResult result = osgDB::Registry::instance()->readObject(pluginFilename, _pluginLoadOpts.get(), false);
				info.pluginInterface = (SGIPluginInterface*)result.getObject();
			}
            if (info.pluginInterface)
            {
                info.pluginFilename = filename;
                info._pluginScore = info.pluginInterface->getPluginScore();
                info.writePrettyHTMLInterface = info.pluginInterface->getWritePrettyHTML();
                info.objectInfoInterface = info.pluginInterface->getObjectInfo();
                info.objectTreeInterface = info.pluginInterface->getObjectTree();
                info.objectLoggerInterface = info.pluginInterface->getObjectLogger();
                info.contextMenuInterface = info.pluginInterface->getContextMenu();
                info.settingsDialogInterface = info.pluginInterface->getSettingsDialog();
                info.guiAdapterInterface = info.pluginInterface->getGUIAdapter();
                if(info.settingsDialogInterface)
                    info.pluginUIInterface = info.pluginInterface;
            }
            it = _plugins.insert(PluginMap::value_type(name, info)).first;
        }
        return &it->second;
    }

    bool loadPluginUI(PluginInfo & info)
    {
        bool ret = false;
        if(!info.pluginUIInterface)
        {
			std::string pluginFilename = createLibraryNameForPluginUI(info.pluginName);
			{
				DisableLibraryLoadErrors disable_load_errors;
				osgDB::ReaderWriter::ReadResult result = osgDB::Registry::instance()->readObject(pluginFilename, _pluginLoadOpts.get(), false);
				info.pluginUIInterface = (SGIPluginInterface*)result.getObject();
			}
            if (info.pluginUIInterface)
            {
                info.settingsDialogInterface = info.pluginUIInterface->getSettingsDialog();
                ret = (info.settingsDialogInterface != NULL);
            }
        }
        else
            ret = true;
        return ret;
    }

    PluginFileNameList listAllAvailablePlugins(PluginType pluginType=PluginTypeModel)
    {
        PluginFileNameList ret;
        std::string postfix = OSG_LIBRARY_POSTFIX;
        size_t postfix_len = postfix.length();

        osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins();
        for(osgDB::FileNameList::iterator itr = plugins.begin();
            itr != plugins.end();
            ++itr)
        {
            std::string basename = osgDB::getSimpleFileName(*itr);
            std::string::size_type pos;
            switch(pluginType)
            {
            case PluginTypeModel: pos = basename.find("osgdb_sgi_"); break;
            case PluginTypeUI: pos = basename.find("osgdb_sgiui_"); break;
            default: pos = std::string::npos; break;
            }
            if (pos == std::string::npos)
                continue;

            std::string::size_type posSuffix = basename.find("_plugin"); 
            if(posSuffix == std::string::npos)
                continue;

            std::string pluginName;
            switch(pluginType)
            {
            case PluginTypeModel: pluginName = std::string(basename.begin()+pos+10,basename.begin()+posSuffix); break;
            case PluginTypeUI: pluginName = std::string(basename.begin()+pos+12,basename.begin()+posSuffix); break;
            default: break;
            }

            if(!pluginName.empty())
            {
                std::string basename_no_ext = osgDB::getNameLessAllExtensions(basename);
                std::string expectedFilename;
                switch(pluginType)
                {
                case PluginTypeModel: expectedFilename = std::string("osgdb_sgi_") + pluginName + std::string("_plugin") + postfix; break;
                case PluginTypeUI: expectedFilename = std::string("osgdb_sgiui_") + pluginName + std::string("_plugin") + postfix; break;
                default: pos = std::string::npos; break;
                }
                if(basename_no_ext == expectedFilename)
                {
                    const std::string & pluginLibraryFile = *itr;
                    PluginFileName plugin(pluginName, pluginLibraryFile);
                    ret.push_back(plugin);
                }
            }
        }

        return ret;
    }

    bool getPlugins(PluginInfoList & pluginList)
    {
        pluginList.clear();
        for(PluginMap::const_iterator it = _plugins.begin(); it != _plugins.end(); it++)
        {
            const PluginInfo & info = it->second;
            pluginList.push_back(info);
        }
        return true;
    }

    class AutoPluginLoader
    {
    public:
        enum PluginMatch {
            PluginMatchExact,
            PluginMatchStartsWith,
            PluginMatchAll,
        };
        AutoPluginLoader(SGIPluginsImpl * impl, const char * name, PluginMatch match=PluginMatchExact)
        {
            switch(match)
            {
            case PluginMatchExact:
                impl->loadPlugin(name);
                break;
            case PluginMatchStartsWith:
                {
                    unsigned name_len = strlen(name);
                    PluginFileNameList pluginFiles = impl->listAllAvailablePlugins(SGIPlugins::SGIPluginsImpl::PluginTypeModel);
                    for(PluginFileNameList::const_iterator it = pluginFiles.begin(); it != pluginFiles.end(); it++)
                    {
                        const PluginFileName & plugin = *it;
                        const std::string & pluginName = plugin.first;
                        const std::string & pluginFilename = plugin.second;
                        if(pluginName.compare(0, name_len, name) == 0)
                        {
                            //std::cout << "Load plugin " << pluginName << " from " << pluginFilename << std::endl;
                            impl->loadPlugin(pluginName, pluginFilename);
                        }
                        else
                            std::cout << "Ignore plugin " << pluginName << std::endl;
                    }
                }
                break;
            case PluginMatchAll:
                {
                    PluginFileNameList pluginFiles = impl->listAllAvailablePlugins(SGIPlugins::SGIPluginsImpl::PluginTypeModel);
                    for(PluginFileNameList::const_iterator it = pluginFiles.begin(); it != pluginFiles.end(); it++)
                    {
                        const PluginFileName & plugin = *it;
                        const std::string & pluginName = plugin.first;
                        const std::string & pluginFilename = plugin.second;
                        //std::cout << "Load plugin " << pluginName << " from " << pluginFilename << std::endl;
                        impl->loadPlugin(pluginName, pluginFilename);
                    }
                }
                break;
            }
        }
    };

    bool generateItem(SGIItemBasePtr & item, const SGIHostItemBase * object)
    {
        bool ret = false;
        if(!_pluginsLoaded)
        {
            AutoPluginLoader loader(this, NULL, AutoPluginLoader::PluginMatchAll );
            _pluginsLoaded = true;
        }
        for(PluginMap::const_iterator it = _plugins.begin(); it != _plugins.end(); it++)
        {
            const PluginInfo & itPluginInfo = it->second;
            if(itPluginInfo.pluginInterface)
            {
                SGIItemBasePtr newItem;
                bool ok = itPluginInfo.pluginInterface->generateItem(object, newItem);
                if(ok && newItem.valid())
                {
                    //std::cout << "got item " << (void*)newItem.get() << " with score=" << newItem->score() << " from " << itPluginInfo.pluginName << std::endl;
                    // always remember which plugin generated the item
                    newItem->setPluginInfo(&itPluginInfo);
                    // copy the flags from the host item to the just generated item
                    newItem->setFlags(object->flags());

                    if(!item.valid())
                        item = newItem;
                    else
                    {
                        SGIItemBasePtr front;
                        item->insertByScore(newItem.get(), front);
                        item = front;
                    }
                    ret = true;
                }
                else
                {
                    //std::cout << "no item from " << itPluginInfo.pluginName << std::endl;
                }
            }
            else
            {
                //std::cout << "no item from " << itPluginInfo.pluginName << " because no interface" << std::endl;
            }
        }
        if(ret && item.valid())
        {
            // mark the first item as the root item (to automatically created tables for HTML)
            item->setFlag(SGIItemFlagRoot, true);
        }
        return ret;
    }

    bool writePrettyHTML(std::basic_ostream<char>& os, const SGIHostItemBase * object, bool table)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return writePrettyHTML(os, item, table);
        else
            return false;
    }
    bool writePrettyHTML(std::basic_ostream<char>& os, const SGIItemBase * item, bool table)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->writePrettyHTMLInterface)
            {
                ret = pluginInfo->writePrettyHTMLInterface->writePrettyHTML(os, item, table);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectName(std::string & name, const SGIHostItemBase * object, bool full)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectName(name, item, full);
        else
            return false;
    }
    bool getObjectName(std::string & name, const SGIItemBase * item, bool full)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectName(name, item, full);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectDisplayName(std::string & name, const SGIHostItemBase * object, bool full)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectDisplayName(name, item, full);
        else
            return false;
    }
    bool getObjectDisplayName(std::string & name, const SGIItemBase * item, bool full)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectDisplayName(name, item, full);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectTypename(std::string & name, const SGIHostItemBase * object, bool full=true)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectTypename(name, item, full);
        else
            return false;
    }
    bool getObjectTypename(std::string & name, const SGIItemBase * item, bool full=true)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectTypename(name, item, full);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectSuggestedFilename(std::string & filename, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectSuggestedFilename(filename, item);
        else
            return false;
    }
    bool getObjectSuggestedFilename(std::string & filename, const SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectSuggestedFilename(filename, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectSuggestedFilenameExtension(ext, item);
        else
            return false;
    }
    bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectSuggestedFilenameExtension(ext, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectFilenameFilters(filters, item);
        else
            return false;
    }
    bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectFilenameFilters(filters, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }

    bool getObjectPath(SGIItemBasePtrPath & path, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectPath(path, item);
        else
            return false;
    }
    bool getObjectPath(SGIItemBasePtrPath & path, const SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectInfoInterface)
            {
                ret = pluginInfo->objectInfoInterface->getObjectPath(path, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool writeObjectFile(bool & result, const SGIHostItemBase * object, const std::string & filename, const SGIItemBase* options)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return writeObjectFile(result, item, filename, options);
        else
            return false;
    }
    bool writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo)
            {
                if(pluginInfo && pluginInfo->objectInfoInterface)
                {
                    ret = pluginInfo->objectInfoInterface->writeObjectFile(result, item, filename, options);
                }
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }

    IContextMenu * createContextMenu(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return createContextMenu(parent, item, callback);
        else
            return NULL;
    }
    IContextMenu * createContextMenu(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
    {
        return QtProxy::instance()->createContextMenu(parent, item, true, callback?callback:_defaultHostCallback.get());
    }
    IContextMenuQt * createContextMenu(QWidget *parent, QObject * item, IHostCallback * callback)
    {
        return QtProxy::instance()->createContextMenu(parent, item, true, callback?callback:_defaultHostCallback.get());
    }

    ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return showSceneGraphDialog(parent, item, callback);
        else
            return NULL;
    }
    ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
    {
        return QtProxy::instance()->showSceneGraphDialog(parent, item, callback?callback:_defaultHostCallback.get());
    }
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return showObjectLoggerDialog(parent, item, callback);
        else
            return NULL;
    }
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
    {
        return QtProxy::instance()->showObjectLoggerDialog(parent, item, callback?callback:_defaultHostCallback.get());
    }
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback)
    {
        return QtProxy::instance()->showObjectLoggerDialog(parent, logger, callback?callback:_defaultHostCallback.get());
    }

    IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
    {
        return QtProxy::instance()->showImagePreviewDialog(parent, item, callback?callback:_defaultHostCallback.get());
    }
    IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return showImagePreviewDialog(parent, item, callback);
        else
            return NULL;
    }


    bool objectTreeBuildTree(IObjectTreeItem * treeItem, SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->objectTreeInterface)
            {
                ret = pluginInfo->objectTreeInterface->buildTree(treeItem, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool objectTreeBuildRootTree(IObjectTreeItem * treeItem, SGIItemBase * item)
    {
        bool overall_ret = false;

        for(PluginMap::const_iterator it = _plugins.begin(); it != _plugins.end(); it++)
        {
            const PluginInfo & itPluginInfo = it->second;
            if(itPluginInfo.pluginInterface && itPluginInfo.objectTreeInterface)
            {
                bool ret = itPluginInfo.objectTreeInterface->buildRootTree(treeItem, item);
                if(ret)
                    overall_ret = true;
            }
        }
        return overall_ret;
    }

    bool contextMenuPopulate(IContextMenuItem * menuItem, const SGIHostItemBase * object, bool onlyRootItem)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return contextMenuPopulate(menuItem, item, onlyRootItem);
        else
            return false;
    }
    bool contextMenuPopulate(IContextMenuItem * menuItem, SGIItemBase * item, bool onlyRootItem)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->contextMenuInterface)
            {
                ret = pluginInfo->contextMenuInterface->populate(menuItem, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && ((onlyRootItem && !ret) || !onlyRootItem));
        if(!onlyRootItem)
            ret = false;
        return ret;
    }

    bool contextMenuExecute(IContextMenuAction * menuAction, SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo && pluginInfo->contextMenuInterface)
            {
                ret = pluginInfo->contextMenuInterface->execute(menuAction, item);
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, const SGIHostItemBase * object, ISettingsDialogInfo * info)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return openSettingsDialog(dialog, item, info);
        else
            return false;
    }
    bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, SGIItemBase * item, ISettingsDialogInfo * info)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo)
            {
                if(loadPluginUI(*const_cast<PluginInfo *>(pluginInfo)) &&
                    pluginInfo->settingsDialogInterface)
                {
                    ret = pluginInfo->settingsDialogInterface->create(dialog, item, info);
                }
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    static QString copyStringFromStdString(const std::string & text, SGIPluginHostInterface::InputDialogStringEncoding encoding)
    {
        QString ret;
        switch(encoding)
        {
        default:
        case SGIPluginHostInterface::InputDialogStringEncodingSystem:
            ret = QString::fromLocal8Bit(text.data(), text.size());
            break;
        case SGIPluginHostInterface::InputDialogStringEncodingUTF8:
            ret = QString::fromUtf8(text.data(), text.size());
            break;
        case SGIPluginHostInterface::InputDialogStringEncodingUTF16:
            ret = QString::fromUtf16((const ushort*)text.data(), text.size());
            break;
        case SGIPluginHostInterface::InputDialogStringEncodingASCII:
            ret = QString::fromLatin1(text.data(), text.size());
            break;
        }
        return ret;
    }
    static std::string copyStringToStdString(const QString & text, SGIPluginHostInterface::InputDialogStringEncoding encoding)
    {
        std::string ret;
        QByteArray qba;
        switch(encoding)
        {
        default:
        case SGIPluginHostInterface::InputDialogStringEncodingSystem:
            qba = text.toLocal8Bit();
            break;
        case SGIPluginHostInterface::InputDialogStringEncodingUTF8:
            qba = text.toUtf8();
            break;
        case SGIPluginHostInterface::InputDialogStringEncodingUTF16:
            qba = QByteArray((const char*)text.utf16(), text.size() * sizeof(ushort));
            break;
        case SGIPluginHostInterface::InputDialogStringEncodingASCII:
            qba = text.toLatin1();
            break;
        }
        ret.assign(qba.constData(), qba.size());
        return ret;
    }
    bool inputDialogString(QWidget *parent, std::string & text, const std::string & label, const std::string & windowTitle, SGIPluginHostInterface::InputDialogStringEncoding encoding, SGIItemBase * item)
    {
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        QString oldText = copyStringFromStdString(text, encoding);
        bool ok = false;
        QString newText = QInputDialog::getText(parent, qwindowTitle, fromLocal8Bit(label), QLineEdit::Normal, oldText, &ok);
        if(ok)
        {
            text = copyStringToStdString(newText, encoding);
        }
        return ok;
    }
    bool inputDialogText(QWidget *parent, std::string & text, const std::string & label, const std::string & windowTitle, SGIPluginHostInterface::InputDialogStringEncoding encoding, SGIItemBase * item)
    {
        bool ok = false;
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        
        QString qtext = copyStringFromStdString(text, encoding);
        QTextDialog dialog(parent);
        dialog.setWindowTitle(qwindowTitle);
        dialog.setButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
        dialog.setLabel(fromLocal8Bit(label));
        dialog.setText(fromLocal8Bit(text));
        dialog.setReadOnly(false);
        if(dialog.exec() == QDialog::Accepted)
        {
            text = copyStringToStdString(dialog.text(), encoding);
            ok = true;
        }
        return ok;
    }
    bool inputDialogInteger(QWidget *parent, int & number, const std::string & label, const std::string & windowTitle, int minNumber, int maxNumber, int step, SGIItemBase * item)
    {
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        bool ok = false;
        int newNumber = QInputDialog::getInt(parent, qwindowTitle, fromLocal8Bit(label), number, minNumber, maxNumber, step, &ok);
        if(ok)
        {
            number = newNumber;
        }
        return ok;
    }
    bool inputDialogDouble(QWidget *parent, double & number, const std::string & label, const std::string & windowTitle, double minNumber, double maxNumber, int decimals, SGIItemBase * item)
    {
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        bool ok = false;
        DoubleInputDialog dlg(parent);
        dlg.setWindowTitle(qwindowTitle);
        dlg.setLabel(fromLocal8Bit(label));
        dlg.setRange(minNumber, maxNumber);
        dlg.setDecimals(decimals);
        dlg.setValue(number);
        ok = (dlg.exec() == QDialog::Accepted);
        if(ok)
        {
            number = dlg.value();
        }
        return ok;
    }
    bool inputDialogBitmask(QWidget *parent, unsigned & number, const std::string & label, const std::string & windowTitle, SGIItemBase * item)
    {
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        QString oldText = "0x" + QString::number(number, 16);
        bool ok = false;
        QString newText = QInputDialog::getText(parent, qwindowTitle, fromLocal8Bit(label), QLineEdit::Normal, oldText, &ok);
        if(ok)
        {
            bool isHexNumber = false;
            if(newText.startsWith("0x"))
            {
                isHexNumber = true;
                newText = newText.mid(2);
            }
            if(newText.endsWith("h"))
            {
                isHexNumber = true;
                newText.chop(1);
            }

            if(isHexNumber)
                number = newText.toUInt(&ok, 16);
            else
                number = newText.toUInt(&ok, 10);
        }
        return ok;
    }
    bool inputDialogColor(QWidget *parent, Color & color, const std::string & label, const std::string & windowTitle, SGIItemBase * item)
    {
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        bool ok = false;
        QColor oldColor;
        oldColor.setRgbF(color.r, color.g, color.b, color.a);
        QColor newColor = QColorDialog::getColor(oldColor, parent, qwindowTitle, QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
        ok = newColor.isValid();
        if(ok)
        {
            color.r = (float)newColor.redF();
            color.g = (float)newColor.greenF();
            color.b = (float)newColor.blueF();
            color.a = (float)newColor.alphaF();
        }
        return ok;
    }
    bool inputDialogFilename(QWidget *parent, SGIPluginHostInterface::InputDialogFilenameType type, std::string & filename, const std::vector<std::string> & filters, const std::string & windowTitle, SGIItemBase * item)
    {
        static QString lastDir;
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        QString qfilters;
        if(filters.empty())
        {
            if(item)
            {
                std::vector<std::string> itemFilters;
                getObjectFilenameFilters(itemFilters, item);
            }
        }
        else
        {
            for(std::vector<std::string>::const_iterator it = filters.begin(); it != filters.end(); it++)
            {
                if(qfilters.isEmpty())
                    qfilters = fromLocal8Bit(*it);
                else
                    qfilters.append(QString(";;") + fromLocal8Bit(*it));
            }
        }
        QString * selectedFilter = NULL;
        QString oldFilename = fromLocal8Bit(filename);
        QString newFilename;
        bool ok = false;
        QFileDialog::Options dialogFlags = QFileDialog::DontResolveSymlinks;
        switch(type)
        {
        case SGIPluginHostInterface::InputDialogFilenameOpen:
            newFilename = QFileDialog::getOpenFileName(parent, qwindowTitle, oldFilename, qfilters, selectedFilter, dialogFlags);
            ok = !newFilename.isNull();
            break;
        case SGIPluginHostInterface::InputDialogFilenameSave:
            newFilename = QFileDialog::getSaveFileName(parent, qwindowTitle, oldFilename, qfilters, selectedFilter, dialogFlags);
            ok = !newFilename.isNull();
            break;
        default:
            ok = false;
            break;
        }
        if(ok)
        {
            filename = toLocal8Bit(newFilename);
        }
        return ok;

    }
    bool inputDialogImage(QWidget *parent, Image & image, const std::string & label, const std::string & windowTitle, SGIItemBase * item)
    {
        QString qwindowTitle;
        if(item)
        {
            std::string objectDisplayName;
            getObjectDisplayName(objectDisplayName, item, true);
            qwindowTitle = fromLocal8Bit(windowTitle) + QString(" (%1)").arg(fromLocal8Bit(objectDisplayName));
        }
        else
            qwindowTitle = fromLocal8Bit(windowTitle);
        bool ok = false;
        /*
        QColor oldColor;
        oldColor.setRgbF(color.r, color.g, color.b, color.a);
        QColor newColor = QColorDialog::getColor(oldColor, parent, qwindowTitle, QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
        ok = newColor.isValid();
        if(ok)
        {
            color.r = (float)newColor.redF();
            color.g = (float)newColor.greenF();
            color.b = (float)newColor.blueF();
            color.a = (float)newColor.alphaF();
        }
        */
        return ok;
    }
    bool setView(SGIItemBase * view, const SGIItemBase * item, double animationTime = -1.0)
    {
        bool ret = false;
        while(view != NULL && !ret)
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )view->pluginInfo();
            if(pluginInfo)
            {
                if(pluginInfo && pluginInfo->guiAdapterInterface)
                {
                    ret = pluginInfo->guiAdapterInterface->setView(view, item, animationTime);
                }
            }
            view = view->nextBase();
        }
        return ret;
    }
    bool setView(const SGIHostItemBase * view, const SGIItemBase * item, double animationTime = -1.0)
    {
        osg::ref_ptr<SGIItemBase> viewItem;
        if(generateItem(viewItem, view))
            return setView(viewItem.get(), item, animationTime);
        else
            return false;
    }
    bool setView(SGIItemBase * view, const SGIHostItemBase * object, double animationTime = -1.0)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return setView(view, item, animationTime);
        else
            return false;
    }
    bool setView(const SGIHostItemBase * view, const SGIHostItemBase * object, double animationTime = -1.0)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return setView(view, item, animationTime);
        else
            return false;
    }

    bool createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return createObjectLogger(logger, item);
        else
            return false;
    }
    bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo)
            {
                if(pluginInfo && pluginInfo->objectLoggerInterface)
                {
                    ret = pluginInfo->objectLoggerInterface->createObjectLogger(logger, item);
                }
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getObjectLogger(logger, item);
        else
            return false;
    }
    bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo)
            {
                if(pluginInfo && pluginInfo->objectLoggerInterface)
                {
                    ret = pluginInfo->objectLoggerInterface->getObjectLogger(logger, item);
                }
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return getOrCreateObjectLogger(logger, item);
        else
            return false;
    }
    bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo)
            {
                if(pluginInfo && pluginInfo->objectLoggerInterface)
                {
                    ret = pluginInfo->objectLoggerInterface->getOrCreateObjectLogger(logger, item);
                }
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    bool parentWidget(QWidgetPtr & widget, const SGIHostItemBase * object)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(generateItem(item, object))
            return parentWidget(widget, item);
        else
            return false;
    }

    bool parentWidget(QWidgetPtr & widget, SGIItemBase * item)
    {
        bool ret = false;
        do
        {
            const PluginInfo * pluginInfo = (const PluginInfo * )item->pluginInfo();
            if(pluginInfo)
            {
                if(pluginInfo && pluginInfo->guiAdapterInterface)
                {
                    ret = pluginInfo->guiAdapterInterface->parentWidget(widget, item);
                }
            }
            item = item->nextBase();
        }
        while(item != NULL && !ret);
        return ret;
    }
    void shutdown()
    {
        for(auto it = _plugins.begin(); it != _plugins.end(); ++it)
        {
            PluginInfo & pluginInfo = it->second;
            if(pluginInfo.pluginInterface)
                pluginInfo.pluginInterface->shutdown();
        }
		_plugins.clear();
        _pluginsLoaded = false;
        _namedEnums.clear();
		if (_hostCallback.valid())
			_hostCallback->shutdown();
		if(_defaultHostCallback.valid())
			_defaultHostCallback->shutdown();
		_defaultHostCallback = NULL;
	}

    bool registerNamedEnum(const std::string & enumname, const std::string & description, bool bitmask)
    {
        NamedEnumType::const_iterator it = _namedEnums.find(enumname);
        bool ret = (it == _namedEnums.end());
        if(ret)
        {
            EnumType et;
            et.description = description;
            et.bitmask = bitmask;
            _namedEnums[enumname] = et;
        }
        return ret;
    }
    bool registerNamedEnumValue(const std::string & enumname, int value, const std::string & valuename)
    {
        NamedEnumType::iterator it = _namedEnums.find(enumname);
        bool ret = (it != _namedEnums.end());
        if(ret)
        {
            EnumType & et = it->second;
            NamedEnumValues::const_iterator itv = et.values.find(value);
            ret = (itv == et.values.end());
            if(ret)
                et.values[value] = valuename;
        }
        return ret;
    }
    bool registerNamedEnumValues(const std::string & enumname, const std::map<int, std::string> & values)
    {
        NamedEnumType::iterator it = _namedEnums.find(enumname);
        bool ret = (it != _namedEnums.end());
        if(ret)
        {
            EnumType & et = it->second;
            for(NamedEnumValues::const_iterator input_itv = values.begin(); input_itv != values.end(); input_itv++)
                et.values[input_itv->first] = input_itv->second;
        }
        return ret;
    }
    bool namedEnumValueToString(const std::string & enumname, std::string & text, int value)
    {
        NamedEnumType::iterator it = _namedEnums.find(enumname);
        bool ret = (it != _namedEnums.end());
        if(ret)
        {
            EnumType & et = it->second;
            if(et.bitmask)
            {
                std::stringstream ss;
                if((unsigned)value == 0u)
                {
                    NamedEnumValues::const_iterator itv = et.values.find(0);
                    if((itv != et.values.end()))
                        ss << itv->second;
                    else
                        ss << "zero";
                }
                else if((unsigned)value == ~0u)
                {
                    NamedEnumValues::const_iterator itv = et.values.find((int)(~0u));
                    if((itv != et.values.end()))
                        ss << itv->second;
                    else
                        ss << "all";
                }
                else
                {
                    bool first = true;
                    for(unsigned n = 0; n < 32; n++)
                    {
						unsigned bitfield_value = (1 << n);
                        if(((unsigned)value) & bitfield_value)
                        {
                            if(!first)
                                ss << '|';
                            NamedEnumValues::const_iterator itv = et.values.find((int)bitfield_value);
                            if((itv != et.values.end()))
                                ss << itv->second;
                            else
                                ss << std::hex << "0x" << bitfield_value;
                            first = false;
                        }
                    }
                }
                text = ss.str();
            }
            else
            {
                NamedEnumValues::const_iterator itv = et.values.find(value);
                if((itv != et.values.end()))
                    text = itv->second;
                else
                {
                    std::stringstream ss;
                    ss << value;
                    text = ss.str();
                }
            }
        }
        else
        {
            std::stringstream ss;
            ss << enumname << '(' << (int)value << ')';
            text = ss.str();
        }
        return ret;
    }

    typedef std::map<std::string, PluginInfo> PluginMap;
    typedef std::map<int, std::string> NamedEnumValues;
    struct EnumType {
        std::string description;
        NamedEnumValues values;
        bool bitmask;
    };
    typedef std::map<std::string, EnumType> NamedEnumType;

private:
    bool _pluginsLoaded;
    PluginMap   _plugins;
    osg::ref_ptr<osgDB::Options> _pluginLoadOpts;
    HostInterface _hostInterface;
	IHostCallbackPtr _defaultHostCallback;
	IHostCallbackPtr _hostCallback;
    NamedEnumType _namedEnums;
};

SGIPlugins* SGIPlugins::instance(bool erase)
{
	return instanceImpl(erase);
}

SGIPlugins * SGIPlugins::instanceImpl(bool erase, bool autoCreate)
{
	static osg::ref_ptr<SGIPlugins> s_plugins = NULL;
	if (erase)
	{
		if(s_plugins.valid())
			s_plugins->destruct();
		s_plugins = 0;
	}
	else if (autoCreate)
	{
		if (!s_plugins.valid())
			s_plugins = new SGIPlugins;
	}
	return s_plugins.get(); // will return NULL on erase
}

void SGIPlugins::shutdown()
{
	// check for an existing SGIPlugins  instance
	SGIPlugins * p = instanceImpl(false, false);
	if (p)
	{
		p->_impl->shutdown();
	}
	// now erase the SGIPlugins instance (if any)
	instanceImpl(true, false);
}

SGIPlugins::SGIPlugins()
    : _impl(SGIPluginsImpl::instance())
{
}

SGIPlugins::~SGIPlugins()
{
    destruct();
}

void SGIPlugins::destruct()
{
    if(_impl)
    {
        // delete the SGIPluginsImpl instance
        SGIPluginsImpl::instance(true);
        _impl = NULL;
    }
}

SGIPluginHostInterface * SGIPlugins::hostInterface()
{
    return _impl->hostInterface();
}

IHostCallback * SGIPlugins::defaultHostCallback()
{
	return _impl->defaultHostCallback();
}

IHostCallback * SGIPlugins::hostCallback()
{
	return _impl->hostCallback();
}

void SGIPlugins::setHostCallback(IHostCallback * callback)
{
	_impl->setHostCallback(callback);
}

bool SGIPlugins::generateItem(osg::ref_ptr<SGIItemBase> & item, const SGIHostItemBase * object)
{
    return _impl->generateItem(item, object);
}

void SGIPlugins::writePrettyHTML(std::basic_ostream<char>& os, const SGIItemBase * item, bool table)
{
    _impl->writePrettyHTML(os, item, table);
}

void SGIPlugins::writePrettyHTML(std::basic_ostream<char>& os, const SGIHostItemBase * object, bool table)
{
    _impl->writePrettyHTML(os, object, table);
}

bool SGIPlugins::getObjectName(std::string & name, const SGIItemBase * item, bool full)
{
    return _impl->getObjectName(name, item, full);
}

bool SGIPlugins::getObjectName(std::string & name, const SGIHostItemBase * object, bool full)
{
    return _impl->getObjectName(name, object, full);
}

bool SGIPlugins::getObjectDisplayName(std::string & name, const SGIHostItemBase * object, bool full)
{
    return _impl->getObjectDisplayName(name, object, full);
}

bool SGIPlugins::getObjectDisplayName(std::string & name, const SGIItemBase * item, bool full)
{
    return _impl->getObjectDisplayName(name, item, full);
}

bool SGIPlugins::getObjectTypename(std::string & name, const SGIItemBase * item, bool full)
{
    return _impl->getObjectTypename(name, item, full);
}

bool SGIPlugins::getObjectTypename(std::string & name, const SGIHostItemBase * object, bool full)
{
    return _impl->getObjectTypename(name, object, full);
}

bool SGIPlugins::getObjectSuggestedFilename(std::string & filename, const SGIHostItemBase * object)
{
    return _impl->getObjectSuggestedFilename(filename, object);
}

bool SGIPlugins::getObjectSuggestedFilename(std::string & filename, const SGIItemBase * object)
{
    return _impl->getObjectSuggestedFilename(filename, object);
}

bool SGIPlugins::getObjectSuggestedFilenameExtension(std::string & ext, const SGIHostItemBase * object)
{
    return _impl->getObjectSuggestedFilenameExtension(ext, object);
}

bool SGIPlugins::getObjectSuggestedFilenameExtension(std::string & ext, const SGIItemBase * object)
{
    return _impl->getObjectSuggestedFilenameExtension(ext, object);
}

bool SGIPlugins::getObjectFilenameFilters(std::vector<std::string> & filters, const SGIHostItemBase * object)
{
    return _impl->getObjectFilenameFilters(filters, object);
}

bool SGIPlugins::getObjectFilenameFilters(std::vector<std::string> & filters, const SGIItemBase * object)
{
    return _impl->getObjectFilenameFilters(filters, object);
}

bool SGIPlugins::getObjectPath(SGIItemBasePtrPath & path, const SGIHostItemBase * object)
{
    return _impl->getObjectPath(path, object);
}

bool SGIPlugins::getObjectPath(SGIItemBasePtrPath & path, const SGIItemBase * item)
{
    return _impl->getObjectPath(path, item);
}

ISceneGraphDialog * SGIPlugins::showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    return _impl->showSceneGraphDialog(parent, item, callback);
}

ISceneGraphDialog * SGIPlugins::showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
{
    return _impl->showSceneGraphDialog(parent, object, callback);
}

bool SGIPlugins::createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
{
    return _impl->createObjectLogger(logger, object);
}

bool SGIPlugins::createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
{
    return _impl->createObjectLogger(logger, item);
}

bool SGIPlugins::getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
{
    return _impl->getObjectLogger(logger, object);
}

bool SGIPlugins::getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
{
    return _impl->getObjectLogger(logger, item);
}

bool SGIPlugins::getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object)
{
    return _impl->getOrCreateObjectLogger(logger, object);
}

bool SGIPlugins::getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item)
{
    return _impl->getOrCreateObjectLogger(logger, item);
}

IObjectLoggerDialog * SGIPlugins::showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
{
    return _impl->showObjectLoggerDialog(parent, object, callback);
}

IObjectLoggerDialog * SGIPlugins::showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback)
{
    return _impl->showObjectLoggerDialog(parent, logger, callback);
}

IObjectLoggerDialog * SGIPlugins::showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    return _impl->showObjectLoggerDialog(parent, item, callback);
}

IContextMenu * SGIPlugins::createContextMenu(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
{
    return _impl->createContextMenu(parent, object, callback);
}

IContextMenu * SGIPlugins::createContextMenu(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    return _impl->createContextMenu(parent, item, callback);
}

IContextMenuQt * SGIPlugins::createContextMenu(QWidget *parent, QObject * item, IHostCallback * callback)
{
    return _impl->createContextMenu(parent, item, callback);
}

IImagePreviewDialog * SGIPlugins::showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    return _impl->showImagePreviewDialog(parent, item, callback);
}

IImagePreviewDialog * SGIPlugins::showImagePreviewDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback)
{
    return _impl->showImagePreviewDialog(parent, object, callback);
}


bool SGIPlugins::objectTreeBuildTree(IObjectTreeItem * treeItem, SGIItemBase * item)
{
    return _impl->objectTreeBuildTree(treeItem, item);
}

bool SGIPlugins::objectTreeBuildRootTree(IObjectTreeItem * treeItem, SGIItemBase * item)
{
    return _impl->objectTreeBuildRootTree(treeItem, item);
}

bool SGIPlugins::contextMenuPopulate(IContextMenuItem * menuItem, SGIItemBase * item, bool onlyRootItem)
{
    return _impl->contextMenuPopulate(menuItem, item, onlyRootItem);
}

bool SGIPlugins::contextMenuExecute(IContextMenuAction * menuAction, SGIItemBase * item)
{
    return _impl->contextMenuExecute(menuAction, item);
}

bool SGIPlugins::openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, const SGIHostItemBase * object, ISettingsDialogInfo * info)
{
    return _impl->openSettingsDialog(dialog, object, info);
}

bool SGIPlugins::openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, SGIItemBase * item, ISettingsDialogInfo * info)
{
    return _impl->openSettingsDialog(dialog, item, info);
}

bool SGIPlugins::writeObjectFile(bool & result, const SGIHostItemBase * item, const std::string & filename, const SGIItemBase* options)
{
    return _impl->writeObjectFile(result, item, filename, options);
}

bool SGIPlugins::writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options)
{
    return _impl->writeObjectFile(result, item, filename, options);
}

bool SGIPlugins::getPlugins(PluginInfoList & pluginList)
{
    return _impl->getPlugins(pluginList);
}

bool SGIPlugins::parentWidget(QWidgetPtr & widget, const SGIHostItemBase * item)
{
    return _impl->parentWidget(widget, item);
}

bool SGIPlugins::parentWidget(QWidgetPtr & widget, SGIItemBase * item)
{
    return _impl->parentWidget(widget, item);
}
