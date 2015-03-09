#pragma once

#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>

QT_BEGIN_NAMESPACE
class Ui_TileInspectorDialog;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace osgEarth {
    class TileSource;
    class TerrainLayer;
}

namespace sgi {
class SGIPluginHostInterface;
class SGIItemOsg;

class IObjectTreeItem;
typedef osg::ref_ptr<IObjectTreeItem> IObjectTreeItemPtr;

namespace osgearth_plugin {

class TileInspectorDialog : public QDialog
{
	Q_OBJECT

public:
                                TileInspectorDialog(QWidget * parent, SGIItemOsg * item, ISettingsDialogInfo * info=NULL);
	virtual				        ~TileInspectorDialog();

public:
    ISettingsDialog *           dialogInterface() { return _interface; }

public slots:
    void                        refresh();
    void                        proxySaveScript();
    void                        reloadTree();
    void                        updateMetaData();
    void                    onItemExpanded(QTreeWidgetItem * item);
    void                    onItemCollapsed(QTreeWidgetItem * item);
    void                    onItemClicked(QTreeWidgetItem * item, int column);
    void                    onItemActivated(QTreeWidgetItem * item, int column);
    void                    onItemContextMenu(QPoint pt);

public:
    void                        requestRedraw();
    
public:
    enum NUM_NEIGHBORS 
    {
        NUM_NEIGHBORS_NONE = 0,
        NUM_NEIGHBORS_CROSS,
        NUM_NEIGHBORS_IMMEDIATE,
    };
protected:
    bool                    buildTree(IObjectTreeItem * treeItem, SGIItemBase * item);

    void                    triggerRepaint();
    SGIHostItemBase *       getView();
    bool                    showSceneGraphDialog(SGIItemBase * item);
    bool                    showSceneGraphDialog(const SGIHostItemBase * item);

    bool                    newInstance(SGIItemBase * item);
    bool                    newInstance(const SGIHostItemBase * item);

protected:
    class ContextMenuCallback;
    class SceneGraphDialogInfo;
    class ObjectLoggerDialogImpl;

private:
    osgEarth::TileSource *          getTileSource() const;
    osgEarth::TerrainLayer *        getTerrainLayer() const;

private:
	Ui_TileInspectorDialog *	    ui;
    SGIPluginHostInterface *        _hostInterface;
    ISettingsDialogPtr              _interface;
    ISettingsDialogInfoPtr          _info;
    IObjectTreeItemPtr              _treeRoot;
    osg::ref_ptr<IContextMenu>          _contextMenu;
    osg::ref_ptr<ContextMenuCallback>   _contextMenuCallback;
    osg::ref_ptr<SGIItemOsg>        _item;
    SGIItemBasePtrVector            _tiles;
};

} // namespace osgearth_plugin
} // namespace sgi
