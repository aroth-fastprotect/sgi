#pragma once

#include <QDialog>
#include <osg/ref_ptr>
#include <sgi/plugins/SGISettingsDialogImpl>

QT_BEGIN_NAMESPACE
class Ui_TileInspectorDialog;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace osg {
    class Referenced;
}

namespace osgEarth {
    class TileSource;
    class TerrainLayer;
    class TileKey;
}

namespace sgi {
class SGIPluginHostInterface;

class IObjectTreeItem;
typedef details::ref_ptr<IObjectTreeItem> IObjectTreeItemPtr;

class IObjectTreeImpl;
typedef details::ref_ptr<IObjectTreeImpl> IObjectTreeImplPtr;
class IContextMenu;
typedef details::ref_ptr<IContextMenu> IContextMenuPtr;

typedef SGIItemInfoSharedPtr<osg::Referenced, osg::ref_ptr<osg::Referenced> > SGIItemInfoOsg;
typedef SGIHostItemImpl<SGIItemInfoOsg> SGIHostItemOsg;
typedef SGIItemHolderT<SGIItemInfoOsg> SGIItemHolderOsg;
typedef SGIItemT<SGIHostItemOsg, SGIItemHolderOsg> SGIItemOsg;

namespace osgearth_plugin {

class TileKeyList;
class TileKeySet;

class TileInspectorDialog : public SettingsQDialogImpl
{
	Q_OBJECT

public:
    TileInspectorDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
    ~TileInspectorDialog() override;

public slots:
    void                        refresh();
    void                        layerChanged(int index);
    void                        layerSourceChanged(int index);
    void                        proxySaveScript();
    void                        reloadTree();
    void                        updateMetaData();
    void                        loadData();
    void                        reloadSelectedItem();
	void                        takePositionFromCamera();
    void                        loadFromFile();
    void                        takeFromDataExtents();
    void                        takeFromDataExtentsUnion();
    
public:
    enum LAYER_DATA_SOURCE
    {
        LayerDataSourceNone = -1,
        LayerDataSourceLayer,
        LayerDataSourceTileSource,
        LayerDataSourceCache,
    };
    void                    addTileKey(const osgEarth::TileKey & key);
    void                    addTileKeys(osgEarth::TileSource * tileSource, const TileKeyList & tiles, bool append=false);
    void                    addTileKeys(osgEarth::TileSource * tileSource, const TileKeySet & tiles, bool append=false);
    void                    addTileKeys(const TileKeyList & tiles, bool append=false);
    void                    addTileKeys(const TileKeySet & tiles, bool append=false);
protected:
    void                    triggerRepaint();
    SGIItemBase *           getView();

    void                    setNodeInfo(const SGIItemBase * item);

    void                    itemContextMenu(IObjectTreeItem * treeItem, IContextMenuPtr & contextMenu);

    void                    updateLayerContextMenu();

protected:
    class ObjectTreeImpl;

private:
	Ui_TileInspectorDialog *	    ui;
    ISettingsDialogPtr _interface;
    IObjectTreeItemPtr              _treeRoot;
    IObjectTreeImplPtr              _treeImpl;
    IContextMenuPtr                 _contextMenu;
    IContextMenuPtr                 _layerContextMenu;
    details::ref_ptr<SGIItemOsg>    _item;
    SGIItemBasePtrVector            _tiles;
};

} // namespace osgearth_plugin
} // namespace sgi
