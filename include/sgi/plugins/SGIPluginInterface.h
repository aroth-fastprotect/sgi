// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

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
typedef osg::ref_ptr<ISceneGraphDialog> ISceneGraphDialogPtr;

class IContextMenu;
typedef osg::ref_ptr<IContextMenu> IContextMenuPtr;

class IObjectLogger;
class IObjectLoggerDialog;

typedef osg::ref_ptr<IObjectLogger> IObjectLoggerPtr;
typedef osg::ref_ptr<IObjectLoggerDialog> IObjectLoggerDialogPtr;

class ISettingsDialog;
class ISettingsDialogInfo;
typedef osg::ref_ptr<ISettingsDialog> ISettingsDialogPtr;
typedef osg::ref_ptr<ISettingsDialogInfo> ISettingsDialogInfoPtr;

class IImagePreviewDialog;
typedef osg::ref_ptr<IImagePreviewDialog> IImagePreviewDialogPtr;

class IObjectTreeItem;
typedef osg::ref_ptr<IObjectTreeItem> IObjectTreeItemPtr;
typedef std::vector<IObjectTreeItemPtr> IObjectTreeItemPtrList;

class IHostCallback;
typedef osg::ref_ptr<IHostCallback> IHostCallbackPtr;

class IContextMenuItem;
typedef osg::ref_ptr<IContextMenuItem> IContextMenuItemPtr;

class IContextMenuAction;
typedef osg::ref_ptr<IContextMenuAction> IContextMenuActionPtr;

typedef QWidget * QWidgetPtr;

class SGIPluginHostInterface
{
public:
	virtual IHostCallback * defaultHostCallback() = 0;
	virtual IHostCallback * hostCallback() = 0;
	virtual void setHostCallback(IHostCallback * callback) = 0;

    virtual bool generateItem(SGIItemBasePtr & item, const SGIHostItemBase * object) = 0;

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

    virtual IContextMenu * createContextMenu(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback=NULL) = 0;
    virtual IContextMenu * createContextMenu(QWidget *parent, SGIItemBase * item, IHostCallback * callback=NULL) = 0;

    virtual ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback=NULL) = 0;
    virtual ISceneGraphDialog * showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback=NULL) = 0;

    virtual bool createObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object) = 0;
    virtual bool createObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item) = 0;
    virtual bool getObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object) = 0;
    virtual bool getObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item) = 0;
    virtual bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, const SGIHostItemBase * object) = 0;
    virtual bool getOrCreateObjectLogger(IObjectLoggerPtr & logger, SGIItemBase * item) = 0;

    virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback=NULL) = 0;
    virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback=NULL) = 0;
    virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback=NULL) = 0;

    virtual IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, const SGIHostItemBase * object, IHostCallback * callback=NULL) = 0;
    virtual IImagePreviewDialog * showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback=NULL) = 0;

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
    virtual bool inputDialogInteger64(QWidget *parent, int64_t & number, const std::string & label, const std::string & windowTitle, int64_t minNumber, int64_t maxNumber, int step=1, SGIItemBase * item=NULL) = 0;
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
    virtual bool setView(SGIItemBase * view, const SGIHostItemBase * item, double animationTime = -1.0) = 0;
    virtual bool setView(const SGIHostItemBase * view, const SGIHostItemBase * item, double animationTime = -1.0) = 0;

    virtual bool registerNamedEnum(const std::string & enumname, const std::string & description=std::string(), bool bitmask=false) = 0;
    virtual bool registerNamedEnumValue(const std::string & enumname, int value, const std::string & valuename) = 0;
    virtual bool registerNamedEnumValues(const std::string & enumname, const std::map<int, std::string> & values) = 0;
    virtual bool namedEnumValueToString(const std::string & enumname, std::string & text, int value) = 0;

    virtual bool convertToImage(ImagePtr & image, const SGIHostItemBase * object) = 0;
    virtual bool convertToImage(ImagePtr & image, const SGIItemBase * item) = 0;
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

    virtual void shutdown() = 0;

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
    class ConvertToImage
    {
    public:
        virtual bool convert(ImagePtr & image, const SGIItemBase * object) = 0;
    };
    virtual ConvertToImage * getConvertToImage() = 0;

    static SGIPluginHostInterface * hostInterface() { return _hostInterface; }

protected:
    static SGIPluginHostInterface * _hostInterface;
};

} // namespace sgi
