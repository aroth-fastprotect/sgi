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

class IObjectTreeImpl;
typedef osg::ref_ptr<IObjectTreeImpl> IObjectTreeImplPtr;

namespace osgearth_plugin {

class TileInspectorDialog : public QDialog
{
	Q_OBJECT

public:
                                TileInspectorDialog(QWidget * parent, SGIItemOsg * item, ISettingsDialogInfo * info=NULL, SGIPluginHostInterface * hostInterface=NULL);
	virtual				        ~TileInspectorDialog();

public:
    ISettingsDialog *           dialogInterface() { return _interface; }

public slots:
    void                        refresh();
    void                        layerChanged(int index);
    void                        proxySaveScript();
    void                        reloadTree();
    void                        updateMetaData();
    void                        loadData();
    void                        reloadSelectedItem();

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
    void                    triggerRepaint();
    SGIHostItemBase *       getView();
    bool                    showSceneGraphDialog(SGIItemBase * item);
    bool                    showSceneGraphDialog(const SGIHostItemBase * item);

    void                    setNodeInfo(const SGIItemBase * item);

    bool                    newInstance(SGIItemBase * item);
    bool                    newInstance(const SGIHostItemBase * item);

    void                    itemContextMenu(IObjectTreeItem * treeItem, IContextMenuPtr & contextMenu);

protected:
    class ContextMenuCallback;
    class SceneGraphDialogInfo;
    class ObjectTreeImpl;

private:
	Ui_TileInspectorDialog *	    ui;
    SGIPluginHostInterface *        _hostInterface;
    ISettingsDialogPtr              _interface;
    ISettingsDialogInfoPtr          _info;
    IObjectTreeItemPtr              _treeRoot;
    IObjectTreeImplPtr              _treeImpl;           
    osg::ref_ptr<IContextMenu>          _contextMenu;
    osg::ref_ptr<ContextMenuCallback>   _contextMenuCallback;
    osg::ref_ptr<SGIItemOsg>        _item;
    SGIItemBasePtrVector            _tiles;
};

} // namespace osgearth_plugin
} // namespace sgi
