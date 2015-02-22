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

class IContextMenuInfoQt;

class SGIHostItemBase;
class SGIItemBase;

typedef osg::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;

class ISceneGraphDialog;
class ISceneGraphDialogInfo;
class IObjectLoggerDialog;
class IObjectLoggerDialogInfo;
class IContextMenu;
class IContextMenuInfo;
class IContextMenuQt;
class IContextMenuInfoQt;
class IObjectTreeItem;
class IContextMenuItem;
class IContextMenuAction;
class ISettingsDialog;
class ISettingsDialogInfo;

class SGIPluginInfo : public ISGIPluginInfo
{
public:
    SGIPluginInfo()
        : pluginName()
        , pluginFilename()
        , pluginInterface(NULL)
        , pluginUIInterface(NULL)
        , writePrettyHTMLInterface(NULL)
        , objectInfoInterface(NULL)
        , objectTreeInterface(NULL)
        , objectLoggerInterface(NULL)
        , contextMenuInterface(NULL)
        , settingsDialogInterface(NULL)
        , guiAdapterInterface(NULL)
        , _pluginScore(0)
    {
    }
    SGIPluginInfo(const SGIPluginInfo & rhs)
        : pluginName(rhs.pluginName)
        , pluginFilename(rhs.pluginFilename)
        , pluginInterface(rhs.pluginInterface)
        , pluginUIInterface(rhs.pluginUIInterface)
        , writePrettyHTMLInterface(rhs.writePrettyHTMLInterface)
        , objectInfoInterface(rhs.objectInfoInterface)
        , objectTreeInterface(rhs.objectTreeInterface)
        , objectLoggerInterface(rhs.objectLoggerInterface)
        , contextMenuInterface(rhs.contextMenuInterface)
        , settingsDialogInterface(rhs.settingsDialogInterface)
        , guiAdapterInterface(rhs.guiAdapterInterface)
        , _pluginScore(rhs._pluginScore)
    {
    }
    virtual unsigned pluginScore() const { return _pluginScore; }
public:
    std::string                             pluginName;
    std::string                             pluginFilename;
    osg::ref_ptr<SGIPluginInterface>        pluginInterface;
    osg::ref_ptr<SGIPluginInterface>        pluginUIInterface;
    SGIPluginInterface::WritePrettyHTML*    writePrettyHTMLInterface;
    SGIPluginInterface::ObjectInfo*         objectInfoInterface;
    SGIPluginInterface::ObjectTree*         objectTreeInterface;
    SGIPluginInterface::ObjectLogger*       objectLoggerInterface;
    SGIPluginInterface::ContextMenu*        contextMenuInterface;
    SGIPluginInterface::SettingsDialog*     settingsDialogInterface;
    SGIPluginInterface::GUIAdapter*         guiAdapterInterface;
    unsigned                                _pluginScore;
};


class SGIPlugins : public osg::Referenced
{
protected:
    SGIPlugins();
    virtual ~SGIPlugins();

    void destruct();

public:
    typedef SGIPluginInfo PluginInfo;
    typedef std::list<PluginInfo> PluginInfoList;
    bool getPlugins(PluginInfoList & pluginList);

public:
    SGIPluginHostInterface * hostInterface();

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

    ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * item, ISceneGraphDialogInfo * info=NULL);
    ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, SGIItemBase * item, ISceneGraphDialogInfo * info=NULL);

    bool createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object);
    bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item);
    bool getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object);
    bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item);
    bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object);
    bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item);

    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * item, IObjectLoggerDialogInfo * info=NULL);
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IObjectLoggerDialogInfo * info=NULL);
    IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IObjectLoggerDialogInfo * info=NULL);

    IContextMenu * createContextMenu(QWidget *parent, const SGIHostItemBase * object, IContextMenuInfo * info=NULL);
    IContextMenu * createContextMenu(QWidget *parent, SGIItemBase * item, IContextMenuInfo * info=NULL);
    IContextMenuQt * createContextMenu(QWidget *parent, QObject * item, IContextMenuInfoQt * info=NULL);

    bool objectTreeBuildTree(IObjectTreeItem * treeItem, SGIItemBase * item);
    bool objectTreeBuildRootTree(IObjectTreeItem * treeItem, SGIItemBase * item);
    bool contextMenuPopulate(IContextMenuItem * menuItem, SGIItemBase * item, bool onlyRootItem);
    bool contextMenuExecute(IContextMenuAction * menuAction, SGIItemBase * item);

    bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, const SGIHostItemBase * object, ISettingsDialogInfo * info=NULL);
    bool openSettingsDialog(osg::ref_ptr<ISettingsDialog> & dialog, SGIItemBase * item, ISettingsDialogInfo * info=NULL);

    bool writeObjectFile(bool & result, const SGIHostItemBase * item, const std::string & filename, const SGIItemBase* options=NULL);
    bool writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options=NULL);

    bool parentWidget(QWidgetPtr & widget, const SGIHostItemBase * item);
    bool parentWidget(QWidgetPtr & widget, SGIItemBase * item);

public:
    static SGIPlugins * instance(bool erase=false);

private:
    class SGIPluginsImpl;
    SGIPluginsImpl *    _impl;
};

} // namespace sgi
