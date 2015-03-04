#pragma once

#include "SGIItemBase.h"
#include <map>
#include <sstream>

class QWidget;

namespace osg {
    class View;
}

namespace sgi {

class SGIHostItemBase;

class SGIItemBase;
typedef osg::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;

class ISceneGraphDialog;
class ISceneGraphDialogInfo;
typedef osg::ref_ptr<ISceneGraphDialog> ISceneGraphDialogPtr;
typedef osg::ref_ptr<ISceneGraphDialogInfo> ISceneGraphDialogInfoPtr;

class IContextMenu;
class IContextMenuInfo;
typedef osg::ref_ptr<IContextMenuInfo> IContextMenuInfoPtr;
typedef osg::ref_ptr<IContextMenu> IContextMenuPtr;

class IObjectLogger;
class IObjectLoggerDialog;
class IObjectLoggerDialogInfo;

typedef osg::ref_ptr<IObjectLogger> IObjectLoggerPtr;
typedef osg::ref_ptr<IObjectLoggerDialog> IObjectLoggerDialogPtr;
typedef osg::ref_ptr<IObjectLoggerDialogInfo> IObjectLoggerDialogInfoPtr;

class ISettingsDialog;
class ISettingsDialogInfo;
typedef osg::ref_ptr<ISettingsDialog> ISettingsDialogPtr;
typedef osg::ref_ptr<ISettingsDialogInfo> ISettingsDialogInfoPtr;

class IObjectTreeItem : public osg::Referenced
{
public:
    virtual IObjectTreeItem * root() = 0;
    virtual IObjectTreeItem * parent() = 0;
    virtual IObjectTreeItem * addChild(const std::string & name, SGIItemBase * item) = 0;
    virtual IObjectTreeItem * addChild(const std::string & name, const SGIHostItemBase * item) = 0;
    virtual IObjectTreeItem * findChild(const std::string & name) = 0;
    virtual IObjectTreeItem * addChildIfNotExists(const std::string & name, SGIItemBase * item) = 0;
    virtual IObjectTreeItem * addChildIfNotExists(const std::string & name, const SGIHostItemBase * hostitem) = 0;
    virtual void setSelected(bool select) = 0;
    virtual bool isSelected() const = 0;
    virtual SGIItemBase * item() = 0;
    virtual void expand() = 0;
    virtual void collapse() = 0;
};
typedef osg::ref_ptr<IObjectTreeItem> IObjectTreeItemPtr;

class IContextMenuItem : public osg::Referenced
{
public:
    virtual bool addSimpleAction(unsigned actionId, const std::string & name, const SGIHostItemBase * item, osg::Referenced * userData=NULL) = 0;
    virtual bool addSimpleAction(unsigned actionId, const std::string & name, SGIItemBase * item, osg::Referenced * userData=NULL) = 0;
    virtual bool addBoolAction(unsigned actionId, const std::string & name, SGIItemBase * item, bool state, osg::Referenced * userData=NULL) = 0;
    virtual bool addModeAction(const std::string & name, int mode, osg::Referenced * userData=NULL) = 0;
    virtual IContextMenuItem * addMenu(const std::string & name, SGIItemBase * item, osg::Referenced * userData=NULL) = 0;
    virtual IContextMenuItem * addMenu(const std::string & name, const SGIHostItemBase * item, osg::Referenced * userData=NULL) = 0;
    virtual IContextMenuItem * addModeMenu(unsigned actionId, const std::string & name, SGIItemBase * item, int currentMode, osg::Referenced * userData=NULL) = 0;
    virtual IContextMenuItem * getOrCreateMenu(const std::string & name, osg::Referenced * userData=NULL) = 0;
    virtual void addSeparator() = 0;
};
typedef osg::ref_ptr<IContextMenuItem> IContextMenuItemPtr;

class IContextMenuAction : public osg::Referenced
{
public:
    virtual unsigned actionId() = 0;
    virtual IContextMenu * menu() = 0;
    virtual SGIItemBase * item() = 0;
    virtual unsigned mode() = 0;
    virtual bool state() = 0;
    virtual osg::Referenced * userData() = 0;
    virtual const osg::Referenced * userData() const = 0;
    virtual osg::Referenced * modeUserData() = 0;
    virtual const osg::Referenced * modeUserData() const = 0;
};
typedef osg::ref_ptr<IContextMenuAction> IContextMenuActionPtr;

typedef QWidget * QWidgetPtr;

class SGIPluginHostInterface
{
public:
    virtual bool generateItem(osg::ref_ptr<SGIItemBase> & item, const SGIHostItemBase * object) = 0;

    virtual bool writePrettyHTML(std::basic_ostream<char>& os, const SGIHostItemBase * object, bool table=true) = 0;
    virtual bool writePrettyHTML(std::basic_ostream<char>& os, const SGIItemBase * item, bool table=true) = 0;

    virtual bool getObjectName(std::string & name, const SGIHostItemBase * object, bool full=true) = 0;
    virtual bool getObjectName(std::string & name, const SGIItemBase * item, bool full=true) = 0;
    virtual bool getObjectDisplayName(std::string & displayName, const SGIHostItemBase * object, bool full=true) = 0;
    virtual bool getObjectDisplayName(std::string & displayName, const SGIItemBase * item, bool full=true) = 0;
    virtual bool getObjectTypename(std::string & name, const SGIHostItemBase * object, bool full=true) = 0;
    virtual bool getObjectTypename(std::string & name, const SGIItemBase * item, bool full=true) = 0;
    virtual bool getObjectSuggestedFilename(std::string & filename, const SGIHostItemBase * object) = 0;
    virtual bool getObjectSuggestedFilename(std::string & filename, const SGIItemBase * item) = 0;
    virtual bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIHostItemBase * object) = 0;
    virtual bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIItemBase * item) = 0;
    virtual bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIHostItemBase * object) = 0;
    virtual bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIItemBase * item) = 0;
    virtual bool getObjectPath(SGIItemBasePtrPath & path, const SGIHostItemBase * object) = 0;
    virtual bool getObjectPath(SGIItemBasePtrPath & path, const SGIItemBase * item) = 0;
    virtual bool writeObjectFile(bool & result, const SGIHostItemBase * object, const std::string & filename, const SGIItemBase* options) = 0;
    virtual bool writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options) = 0;

    virtual IContextMenu * createContextMenu(QWidget *parent, const SGIHostItemBase * object, IContextMenuInfo * info=NULL) = 0;
    virtual IContextMenu * createContextMenu(QWidget *parent, SGIItemBase * item, IContextMenuInfo * info=NULL) = 0;

    virtual ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * object, ISceneGraphDialogInfo * info=NULL) = 0;
    virtual ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, SGIItemBase * item, ISceneGraphDialogInfo * info=NULL) = 0;

    virtual bool createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object) = 0;
    virtual bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item) = 0;
    virtual bool getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object) = 0;
    virtual bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item) = 0;
    virtual bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object) = 0;
    virtual bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item) = 0;

    virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IObjectLoggerDialogInfo * info=NULL) = 0;
    virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * object, IObjectLoggerDialogInfo * info=NULL) = 0;
    virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IObjectLoggerDialogInfo * info=NULL) = 0;

    virtual bool objectTreeBuildTree(IObjectTreeItem * treeItem, SGIItemBase * item) = 0;
    virtual bool objectTreeBuildRootTree(IObjectTreeItem * treeItem, SGIItemBase * item) = 0;
    virtual bool contextMenuPopulate(IContextMenuItem * menuItem, const SGIHostItemBase * object, bool onlyRootItem=true) = 0;
    virtual bool contextMenuPopulate(IContextMenuItem * menuItem, SGIItemBase * item, bool onlyRootItem=true) = 0;
    virtual bool contextMenuExecute(IContextMenuAction * menuAction, SGIItemBase * item) = 0;

    virtual bool openSettingsDialog(ISettingsDialogPtr & dialog, const SGIHostItemBase * object, ISettingsDialogInfo * info=NULL) = 0;
    virtual bool openSettingsDialog(ISettingsDialogPtr & dialog, SGIItemBase * item, ISettingsDialogInfo * info=NULL) = 0;

    enum InputDialogStringEncoding { InputDialogStringEncodingSystem, InputDialogStringEncodingUTF8, InputDialogStringEncodingUTF16, InputDialogStringEncodingASCII };
    virtual bool inputDialogString(QWidget *parent, std::string & text, const std::string & label, const std::string & windowTitle, InputDialogStringEncoding encoding=InputDialogStringEncodingSystem, SGIItemBase * item=NULL) = 0;
    virtual bool inputDialogText(QWidget *parent, std::string & text, const std::string & label, const std::string & windowTitle, InputDialogStringEncoding encoding=InputDialogStringEncodingSystem, SGIItemBase * item=NULL) = 0;
    virtual bool inputDialogInteger(QWidget *parent, int & number, const std::string & label, const std::string & windowTitle, int minNumber, int maxNumber, int step=1, SGIItemBase * item=NULL) = 0;
    virtual bool inputDialogDouble(QWidget *parent, double & number, const std::string & label, const std::string & windowTitle, double minNumber, double maxNumber, int decimals=1, SGIItemBase * item=NULL) = 0;
    virtual bool inputDialogBitmask(QWidget *parent, unsigned & number, const std::string & label, const std::string & windowTitle, SGIItemBase * item=NULL) = 0;
    virtual bool inputDialogColor(QWidget *parent, Color & color, const std::string & label, const std::string & windowTitle, SGIItemBase * item=NULL) = 0;
    enum InputDialogFilenameType { InputDialogFilenameOpen, InputDialogFilenameSave };
    virtual bool inputDialogFilename(QWidget *parent, InputDialogFilenameType type, std::string & filename, const std::vector<std::string> & filters, const std::string & windowTitle, SGIItemBase * item=NULL) = 0;
    virtual bool inputDialogImage(QWidget *parent, Image & image, const std::string & label, const std::string & windowTitle, SGIItemBase * item=NULL) = 0;

    template<typename VALUE_TYPE>
    bool inputDialogValueAsString(QWidget *parent, VALUE_TYPE & v, const std::string & label, const std::string & windowTitle, InputDialogStringEncoding encoding=InputDialogStringEncodingSystem, SGIItemBase * item=NULL)
    {
        bool ret;
        std::string value_as_string;
        {
            std::stringstream ss;
            ss << v;
            value_as_string = ss.str();
        }
        ret = inputDialogString(parent, value_as_string, label, windowTitle, encoding, item);
        if(ret)
        {
            std::stringstream ss(value_as_string);
            ss >> v;
        }
        return ret;
    }

    virtual bool setView(SGIItemBase * view, const SGIItemBase * item, double animationTime = -1.0) = 0;
    virtual bool setView(const SGIHostItemBase * view, const SGIItemBase * item, double animationTime = -1.0) = 0;
    virtual bool setView(const SGIHostItemBase * view, const SGIHostItemBase * item, double animationTime = -1.0) = 0;

    virtual bool registerNamedEnum(const std::string & enumname, const std::string & description=std::string(), bool bitmask=false) = 0;
    virtual bool registerNamedEnumValue(const std::string & enumname, int value, const std::string & valuename) = 0;
    virtual bool registerNamedEnumValues(const std::string & enumname, const std::map<int, std::string> & values) = 0;
    virtual bool namedEnumValueToString(const std::string & enumname, std::string & text, int value) = 0;
};

class SGIPluginInterface : public osg::Object
{
public:
    SGIPluginInterface(SGIPluginHostInterface * hostInterface)
        : osg::Object()
    {
        _hostInterface = hostInterface;
    }
    SGIPluginInterface(const SGIPluginInterface & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Object(rhs, copyop)
    {
    }
    virtual ~SGIPluginInterface()
    {
    }

    virtual unsigned getPluginScore() = 0;

    virtual bool generateItem(const SGIHostItemBase * object, SGIItemBasePtr & item) = 0;

    class WritePrettyHTML
    {
    public:
        virtual bool writePrettyHTML(std::basic_ostream<char>& os, const SGIItemBase * object, bool table) = 0;
    };
    virtual WritePrettyHTML * getWritePrettyHTML() = 0;

    class ObjectInfo
    {
    public:
        virtual bool getObjectName(std::string & name, const SGIItemBase * object, bool full) = 0;
        virtual bool getObjectDisplayName(std::string & name, const SGIItemBase * object, bool full) = 0;
        virtual bool getObjectTypename(std::string & name, const SGIItemBase * object, bool full) = 0;
        virtual bool getObjectPath(SGIItemBasePtrPath & path, const SGIItemBase * object) = 0;
        virtual bool getObjectSuggestedFilename(std::string & filename, const SGIItemBase * object) = 0;
        virtual bool getObjectSuggestedFilenameExtension(std::string & ext, const SGIItemBase * object) = 0;
        virtual bool getObjectFilenameFilters(std::vector<std::string> & filters, const SGIItemBase * object) = 0;
        virtual bool writeObjectFile(bool & result, SGIItemBase * item, const std::string & filename, const SGIItemBase* options) = 0;
    };
    virtual ObjectInfo * getObjectInfo() = 0;

    class ObjectTree
    {
    public:
        virtual bool buildRootTree(IObjectTreeItem * treeItem, SGIItemBase * object) = 0;
        virtual bool buildTree(IObjectTreeItem * treeItem, SGIItemBase * object) = 0;
    };
    virtual ObjectTree * getObjectTree() = 0;

    class ObjectLogger
    {
    public:
        virtual bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * object) = 0;
        virtual bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * object) = 0;
        virtual bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * object) = 0;
    };
    virtual ObjectLogger * getObjectLogger() = 0;

    class ContextMenu
    {
    public:
        virtual bool populate(IContextMenuItem * menuItem, SGIItemBase * object) = 0;
        virtual bool execute(IContextMenuAction * menuAction, SGIItemBase * object) = 0;
    };
    virtual ContextMenu * getContextMenu() = 0;

    class SettingsDialog
    {
    public:
        virtual bool create(ISettingsDialogPtr & dialog, SGIItemBase * object, ISettingsDialogInfo * info) = 0;
    };
    virtual SettingsDialog * getSettingsDialog() = 0;

    class GUIAdapter
    {
    public:
        virtual bool parentWidget(QWidgetPtr & widget, SGIItemBase * item) = 0;
        virtual bool setView(SGIItemBase * view, const SGIItemBase * object, double animationTime) = 0;
        virtual bool repaint() = 0;
    };
    virtual GUIAdapter * getGUIAdapter() = 0;

    static SGIPluginHostInterface * hostInterface() { return _hostInterface; }

protected:
    static SGIPluginHostInterface * _hostInterface;
};

} // namespace sgi
