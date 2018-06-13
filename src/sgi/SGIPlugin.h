#pragma once

#include "sgi/plugins/SGIPluginInterface.h"

class QObject;
namespace osg {
    class Referenced;
}
namespace osgDB {
    class Options;
}

namespace sgi {

class SGIHostItemBase;
class SGIItemBase;

typedef osg::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;

class ISceneGraphDialog;
class IObjectLoggerDialog;
class IContextMenu;
class IContextMenuQt;
class IObjectTreeItem;
class IContextMenuItem;
class IContextMenuAction;
class IHostCallback;
class ISettingsDialog;
class ISettingsDialogInfo;

class SGIPluginInfo : public ISGIPluginInfo
{
public:
    SGIPluginInfo()
        : pluginName()
        , pluginFilename()
        , pluginInterface(nullptr)
        , writePrettyHTMLInterface(nullptr)
        , objectInfoInterface(nullptr)
        , objectTreeInterface(nullptr)
        , objectLoggerInterface(nullptr)
        , contextMenuInterface(nullptr)
        , settingsDialogInterface(nullptr)
        , guiAdapterInterface(nullptr)
        , convertToImage(nullptr)
        , _pluginScore(0)
        , errorMessage()
    {
    }
    SGIPluginInfo(const SGIPluginInfo & rhs)
        : pluginName(rhs.pluginName)
        , pluginFilename(rhs.pluginFilename)
        , pluginInterface(rhs.pluginInterface)
        , writePrettyHTMLInterface(rhs.writePrettyHTMLInterface)
        , objectInfoInterface(rhs.objectInfoInterface)
        , objectTreeInterface(rhs.objectTreeInterface)
        , objectLoggerInterface(rhs.objectLoggerInterface)
        , contextMenuInterface(rhs.contextMenuInterface)
        , settingsDialogInterface(rhs.settingsDialogInterface)
        , guiAdapterInterface(rhs.guiAdapterInterface)
        , convertToImage(rhs.convertToImage)
        , _pluginScore(rhs._pluginScore)
        , errorMessage(rhs.errorMessage)
    {
    }
    virtual unsigned pluginScore() const { return _pluginScore; }
    bool isInternalPlugin() const;
public:
    std::string                             pluginName;
    std::string                             pluginFilename;
    osg::ref_ptr<SGIPluginInterface>        pluginInterface;
    SGIPluginInterface::WritePrettyHTML*    writePrettyHTMLInterface;
    SGIPluginInterface::ObjectInfo*         objectInfoInterface;
    SGIPluginInterface::ObjectTree*         objectTreeInterface;
    SGIPluginInterface::ObjectLogger*       objectLoggerInterface;
    SGIPluginInterface::ContextMenu*        contextMenuInterface;
    SGIPluginInterface::SettingsDialog*     settingsDialogInterface;
    SGIPluginInterface::GUIAdapter*         guiAdapterInterface;
    SGIPluginInterface::ConvertToImage*     convertToImage;
    unsigned                                _pluginScore;
    std::string                             errorMessage;
};


class SGIPlugins : public osg::Referenced
{
protected:
    SGIPlugins();
    virtual ~SGIPlugins();

    void destruct();

public:
    enum PluginType {
        PluginTypeModel = 0,
    };

    typedef SGIPluginInfo PluginInfo;
    typedef std::list<PluginInfo> PluginInfoList;
    typedef std::pair<std::string, std::string> PluginFileName;
    typedef std::list<PluginFileName> PluginFileNameList;

    bool getPlugins(PluginInfoList & pluginList);
    PluginFileNameList listAllAvailablePlugins(PluginType pluginType=PluginTypeModel);

public:
    SGIPluginHostInterface * hostInterface();
	IHostCallback * defaultHostCallback();
	IHostCallback * hostCallback();
	void setHostCallback(IHostCallback * callback);

    bool generateItem(osg::ref_ptr<SGIItemBase> & item, const SGIHostItemBase * object);

    void writePrettyHTML(std::basic_ostream<char>& os, const SGIHostItemBase * item, bool table=true);
    void writePrettyHTML(std::basic_ostream<char>& os, const SGIItemBase * item, bool table=true);

    bool getObjectName(std::string & name, const SGIHostItemBase * item, bool full=true);
    bool getObjectName(std::string & name, const SGIItemBase * item, bool full=true);
    bool getObjectDisplayName(std::string & name, const SGIHostItemBase * item, bool full=true);
    bool getObjectDisplayName(std::string & name, const SGIItemBase * item, bool full=true);
    bool getObjectTypename(std::string & name, const SGIHostItemBase * item, bool full=true);
    bool getObjectTypename(std::string & name, const SGIItemBase * item, bool full=true);
    bool getObjectSuggestedFilename(std::string & filename, const SGIHostItemBase * object);
    bool getObjectSuggestedFilename(std::string & filename, const SGIItemBase * object);
    bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIHostItemBase * object);
    bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIItemBase * object);
    bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIHostItemBase * object);
    bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIItemBase * object);

    bool getObjectPath(SGIItemBasePtrPath & path, const SGIHostItemBase * object);
    bool getObjectPath(SGIItemBasePtrPath & path, const SGIItemBase * item);

    ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * item, IHostCallback * callback=nullptr);
    ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback=nullptr);

    bool createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object);
    bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item);
    bool getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object);
    bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item);
    bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object);
    bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item);

    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * item, IHostCallback * callback=nullptr);
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback=nullptr);
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback=nullptr);

    IContextMenu * createContextMenu(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback=nullptr);
    IContextMenu * createContextMenu(QWidget *parent, SGIItemBase * item, IHostCallback * callback=nullptr);
    IContextMenuQt * createContextMenu(QWidget *parent, QObject * item, IHostCallback * callback=nullptr);

    IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback);
    IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback);

    bool objectTreeBuildTree(IObjectTreeItem * treeItem, SGIItemBase * item);
    bool objectTreeBuildRootTree(IObjectTreeItem * treeItem, SGIItemBase * item);
    bool contextMenuPopulate(IContextMenuItem * menuItem, SGIItemBase * item, bool onlyRootItem);
    bool contextMenuExecute(IContextMenuAction * menuAction, SGIItemBase * item);

    bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, const SGIHostItemBase * object, ISettingsDialogInfo * info=nullptr);
    bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, SGIItemBase * item, ISettingsDialogInfo * info=nullptr);

    bool writeObjectFile(bool & result, const SGIHostItemBase * item, const std::string & filename, const SGIItemBase* options=nullptr);
    bool writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options=nullptr);

    bool parentWidget(QWidgetPtr & widget, const SGIHostItemBase * item);
    bool parentWidget(QWidgetPtr & widget, SGIItemBase * item);

    bool convertToImage(ImagePtr & image, const SGIHostItemBase * item);
    bool convertToImage(ImagePtr & image, const SGIItemBase * item);

public:
    static SGIPlugins * instance(bool erase=false);
	static void shutdown();

private:
	static SGIPlugins * instanceImpl(bool erase = false, bool autoCreate=true);

private:
    class SGIPluginsImpl;
    SGIPluginsImpl *    _impl;
};

} // namespace sgi
