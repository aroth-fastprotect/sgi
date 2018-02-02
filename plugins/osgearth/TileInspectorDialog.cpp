#include "stdafx.h"
#include "TileInspectorDialog.h"
#include "TileInspectorDialog.moc"
#include <sgi/plugins/SGIItemOsg>

#include "ui_TileInspectorDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/helpers/qt>
#include <sgi/helpers/osg>

#include <QTextStream>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

#include <osg/CoordinateSystemNode>
#include <osgViewer/View>

#include <osgEarth/Registry>
#include <osgEarth/Viewpoint>
#include <osgEarth/TileKey>
#include <osgEarth/TerrainLayer>
#include <osgEarth/MapNode>
#include <osgEarth/MapFrame>

#include <osgEarthDrivers/vpb/VPBOptions>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>

#include <osgEarth/Version>
#include <osgEarthUtil/TMS>

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ObjectTreeImpl>

#include "ElevationQueryReferenced"
#include "string_helpers.h"
#include "osgearth_accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {

using namespace sgi::qt_helpers;

namespace {

	void addTileKeyChilds(TileKeySet & set, const osgEarth::TileKey & tilekey)
	{
		if (tilekey.getLevelOfDetail() >= 24 || set.size() > 200)
			return;
		for (unsigned q = 0; q < 4; ++q)
		{
			osgEarth::TileKey qchild = tilekey.createChildKey(q);
			set.insert(qchild);
			addTileKeyChilds(set, qchild);
		}
	}
    
    void addTileKeyAndNeighbors(TileKeyList & list, const osgEarth::TileKey & tilekey, TileInspectorDialog::NUM_NEIGHBORS numNeighbors)
    {
        TileKeySet set;
        switch(numNeighbors)
        {
            case TileInspectorDialog::NUM_NEIGHBORS_NONE:
                set.insert(tilekey);
                break;
            case TileInspectorDialog::NUM_NEIGHBORS_CROSS:
                set.insert(tilekey);
                set.insert(tilekey.createNeighborKey(-1,0));
                set.insert(tilekey.createNeighborKey(0,-1));
                set.insert(tilekey.createNeighborKey(1,0));
                set.insert(tilekey.createNeighborKey(0,1));
                break;
            case TileInspectorDialog::NUM_NEIGHBORS_IMMEDIATE:
                set.insert(tilekey);
                set.insert(tilekey.createNeighborKey(-1,0));
                set.insert(tilekey.createNeighborKey(-1,-1));
                set.insert(tilekey.createNeighborKey(0,-1));
                set.insert(tilekey.createNeighborKey(1,-1));
                set.insert(tilekey.createNeighborKey(1,0));
                set.insert(tilekey.createNeighborKey(1,1));
                set.insert(tilekey.createNeighborKey(0,1));
                set.insert(tilekey.createNeighborKey(-1,1));
                break;
			case TileInspectorDialog::NUM_NEIGHBORS_PARENTAL:
				{
					set.insert(tilekey);
					osgEarth::TileKey t = tilekey.createParentKey();
					while (t.valid())
					{
						set.insert(t);
						t = t.createParentKey();
					}
				}
				break;
			case TileInspectorDialog::NUM_NEIGHBORS_PARENTAL_AND_CHILDS:
				{
					set.insert(tilekey);
					osgEarth::TileKey t = tilekey.createParentKey();
					while (t.valid())
					{
						set.insert(t);
						t = t.createParentKey();
					}
					addTileKeyChilds(set, tilekey);
				}
				break;
            case TileInspectorDialog::NUM_NEIGHBORS_CHILDS:
                {
                    set.insert(tilekey);
                    osgEarth::TileKey t = tilekey.createParentKey();
                    while (t.valid())
                    {
                        set.insert(t);
                        t = t.createParentKey();
                    }
                    addTileKeyChilds(set, tilekey);
                }
                break;
		}
        for(TileKeySet::const_iterator it = set.begin(); it != set.end(); it++)
            list.push_back(*it);
    }

    
    TileKeyList tileKeyListfromStringOrGpsCoordinate(QLineEdit * lineEdit, const osgEarth::Profile * profile, int selectedLod, TileInspectorDialog::NUM_NEIGHBORS numNeighbors, bool * ret_ok)
    {
        TileKeyList ret;
		bool ok = false;
		CoordinateResult coordResult = coordinateFromString(lineEdit, profile, selectedLod, &ok);

        if(ok)
        {
            const osgEarth::Profile * used_profile = profile ? profile : osgEarth::Registry::instance()->getGlobalGeodeticProfile();
			const unsigned maximum_lod = 21;
			if (coordResult.geoPoint.isValid())
			{
				osgEarth::GeoPoint geoptProfile = coordResult.geoPoint.transform(used_profile->getSRS());

				if (selectedLod == -1)
				{
					for (unsigned lod = 0; lod < maximum_lod; lod++)
					{
						osgEarth::TileKey tilekey = used_profile->createTileKey(geoptProfile.x(), geoptProfile.y(), lod);
						addTileKeyAndNeighbors(ret, tilekey, (numNeighbors==TileInspectorDialog::NUM_NEIGHBORS_PARENTAL || numNeighbors == TileInspectorDialog::NUM_NEIGHBORS_PARENTAL_AND_CHILDS)? TileInspectorDialog::NUM_NEIGHBORS_NONE:numNeighbors);
					}
				}
				else
				{
					osgEarth::TileKey tilekey = used_profile->createTileKey(geoptProfile.x(), geoptProfile.y(), selectedLod);
					addTileKeyAndNeighbors(ret, tilekey, numNeighbors);
				}
			}
			else if (coordResult.tileKey.valid())
			{
				addTileKeyAndNeighbors(ret, coordResult.tileKey, numNeighbors);
			}
        }

        if(ret_ok)
            *ret_ok = ok;
        return ret;
    }

    static std::string getVPBTerrainTile( const osgEarth::TileKey& key, const osgEarth::Drivers::VPBOptions & options)
    {
        int level = key.getLevelOfDetail();
        unsigned int tile_x, tile_y;
        key.getTileXY( tile_x, tile_y );
        
        //int max_x = (2 << level) - 1;
        int max_y = (1 << level) - 1;
        
        tile_y = max_y - tile_y;
        
        std::string baseNameToUse = options.baseName().value();
        std::string path = options.url().value().full();
        std::string extension = "ive";

        std::stringstream buf;
        if ( options.directoryStructure() == osgEarth::Drivers::VPBOptions::DS_FLAT )
        {
             buf<<path<<"/"<<baseNameToUse<<"_L"<<level<<"_X"<<tile_x/2<<"_Y"<<tile_y/2<<"_subtile."<<extension;
        }
        else
        {
            int psl = options.primarySplitLevel().value();
            int ssl = options.secondarySplitLevel().value();

            if (level<psl)
            {
                buf<<path<<"/"<<baseNameToUse<<"_root_L0_X0_Y0/"<<
                     baseNameToUse<<"_L"<<level<<"_X"<<tile_x/2<<"_Y"<<tile_y/2<<"_subtile."<<extension;

            }
            else if (level<ssl)
            {
                tile_x /= 2;
                tile_y /= 2;

                int split_x = tile_x >> (level - psl);
                int split_y = tile_y >> (level - psl);

                buf<<path<<"/"<<baseNameToUse<<"_subtile_L"<<psl<<"_X"<<split_x<<"_Y"<<split_y<<"/"<<
                     baseNameToUse<<"_L"<<level<<"_X"<<tile_x<<"_Y"<<tile_y<<"_subtile."<<extension;
            }
            else if ( options.directoryStructure() == osgEarth::Drivers::VPBOptions::DS_TASK )
            {
                tile_x /= 2;
                tile_y /= 2;

                int split_x = tile_x >> (level - psl);
                int split_y = tile_y >> (level - psl);

                int secondary_split_x = tile_x >> (level - ssl);
                int secondary_split_y = tile_y >> (level - ssl);

                buf<<path<<"/"<<baseNameToUse<<"_subtile_L"<<psl<<"_X"<<split_x<<"_Y"<<split_y<<"/"<<
                     baseNameToUse<<"_subtile_L"<<ssl<<"_X"<<secondary_split_x<<"_Y"<<secondary_split_y<<"/"<< 
                     baseNameToUse<<"_L"<<level<<"_X"<<tile_x<<"_Y"<<tile_y<<"_subtile."<<extension;
            }
            else
            {
                tile_x /= 2;
                tile_y /= 2;

                int split_x = tile_x >> (level - ssl);
                int split_y = tile_y >> (level - ssl);

                buf<<path<<"/"<<baseNameToUse<<"_subtile_L"<<ssl<<"_X"<<split_x<<"_Y"<<split_y<<"/"<<
                     baseNameToUse<<"_L"<<level<<"_X"<<tile_x<<"_Y"<<tile_y<<"_subtile."<<extension;
            }
        }
        std::string bufStr;
        bufStr = buf.str();
        return bufStr;
    }
    
    static std::string getArcGISTerrainTile( const osgEarth::TileKey& key, const osgEarth::Drivers::ArcGISOptions & options, bool isTiled=true)
    {
        std::string format = options.format().value();
        if(format.empty())
            format = "png";
        std::stringstream buf;

        if ( isTiled )
        {
            int level = key.getLevelOfDetail();
            unsigned int tile_x, tile_y;
            key.getTileXY( tile_x, tile_y );

            buf << "/tile"
                << "/" << level
                << "/" << tile_y
                << "/" << tile_x << "." << format;
        }
        else
        {
            const osgEarth::GeoExtent& ex = key.getExtent();

            buf << std::setprecision(16)
                << "/export"
                << "?bbox=" << ex.xMin() << "," << ex.yMin() << "," << ex.xMax() << "," << ex.yMax()
                << "&format=" << format 
                << "&size=256,256"
                << "&transparent=true"
                << "&f=image";
            //<< "&" << "." << f;
        }
        std::string bufStr;
        bufStr = buf.str();
        return bufStr;
    }
    inline static bool IsSlash(int i) { return i == '/'; }

	osg::Camera * findCamera(SGIItemOsg * item)
	{
		if (!item)
			return NULL;

		osg::Node * node = dynamic_cast<osg::Node *>(item->object());
		if (!node)
			return NULL;

		return osgEarth::findFirstParentOfType<osg::Camera>(node);
	}
	osg::Camera * findCamera(SGIItemBase * item)
	{
		if (!item)
			return NULL;
		return findCamera(dynamic_cast<SGIItemOsg*>(item));
	}
	osgEarth::MapNode * findMapNode(SGIItemOsg * item)
	{
		if (!item)
			return NULL;

		osg::Node * node = dynamic_cast<osg::Node *>(item->object());
		if (!node)
			return NULL;

		return osgEarth::MapNode::findMapNode(node);
	}
	osgEarth::MapNode * findMapNode(SGIItemBase * item)
	{
		if (!item)
			return NULL;
		return findMapNode(dynamic_cast<SGIItemOsg*>(item));
	}
} // namespace

class TileInspectorDialog::ObjectTreeImpl : public IObjectTreeImpl
{
public:
    ObjectTreeImpl(TileInspectorDialog * dialog)
        : _dialog(dialog) {}
public:
    virtual void    itemSelected(IObjectTreeItem * oldItem, IObjectTreeItem * newItem)
    {
        _dialog->setNodeInfo(newItem?newItem->item():NULL);
    }
    virtual void    itemContextMenu(IObjectTreeItem * item, IContextMenuPtr & contextMenu)
    {
        _dialog->itemContextMenu(item, contextMenu);
    }
    virtual void    itemExpanded(IObjectTreeItem * item)
    {
    }
    virtual void    itemCollapsed(IObjectTreeItem * item)
    {
    }
    virtual void    itemActivated(IObjectTreeItem * item)
    {
        _dialog->setNodeInfo(item->item());
    }
    virtual void    itemClicked(IObjectTreeItem * item)
    {
        _dialog->setNodeInfo(item->item());
    }

private:
    TileInspectorDialog * _dialog;
};

namespace {
    osgEarth::Map * getMap(SGIItemOsg * item)
    {
        if(!item)
            return NULL;
        if(osgEarth::Map * map = dynamic_cast<osgEarth::Map*>(item->object()))
            return map;
        else if(osgEarth::MapNode * mapnode = dynamic_cast<osgEarth::MapNode*>(item->object()))
            return mapnode->getMap();
        else
            return NULL;
    }
    osgEarth::TileSource * getTileSource(SGIItemOsg * item)
    {
        if(!item)
            return NULL;
        if(osgEarth::TileSource * tileSource = dynamic_cast<osgEarth::TileSource *>(item->object()))
            return tileSource;
        else if(osgEarth::TerrainLayer * terrainLayer = dynamic_cast<osgEarth::TerrainLayer*>(item->object()))
            return terrainLayer->getTileSource();
        else
            return NULL;
    }
    osgEarth::TerrainLayer * getTerrainLayer(SGIItemOsg * item)
    {
        if(!item)
            return NULL;
        if(osgEarth::TerrainLayer * terrainLayer = dynamic_cast<osgEarth::TerrainLayer *>(item->object()))
            return terrainLayer;
        else
            return NULL;
    }
    osgEarth::CacheBin * getCacheBin(SGIItemOsg * item)
    {
        if (!item)
            return NULL;
        if (osgEarth::TerrainLayer * terrainLayer = dynamic_cast<osgEarth::TerrainLayer *>(item->object()))
        {
            const osgEarth::Profile * p = terrainLayer->getProfile();
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            if (p)
                return terrainLayer->getCacheBin(p);
#endif
        }
        return NULL;
    }
}

TileInspectorDialog::TileInspectorDialog(QWidget * parent, SGIItemOsg * item, ISettingsDialogInfo * info, SGIPluginHostInterface * hostInterface)
	: QDialog(parent)
    , _hostInterface(hostInterface)
    , _item(item)
    , _interface(new SettingsDialogImpl(this))
    , _info(info)
    , _treeImpl(new ObjectTreeImpl(this))
{
    Q_ASSERT(_info != NULL);

	ui = new Ui_TileInspectorDialog;
	ui->setupUi( this );

    QMenu * coordinateButtonMenu = new QMenu(ui->coordinateButton);
    coordinateButtonMenu->addAction(ui->actionCoordinateFromCamera);
    coordinateButtonMenu->addAction(ui->actionCoordinateLoadFromFile);
    coordinateButtonMenu->addAction(ui->actionCoordinateFromDataExtents);
    coordinateButtonMenu->addAction(ui->actionCoordinateFromDataExtentsUnion);
    ui->coordinateButton->setMenu(coordinateButtonMenu);
    ui->coordinateButton->setDefaultAction(ui->actionCoordinateFromCamera);

#ifndef OSGEARTH_WITH_FAST_MODIFICATIONS
    ui->updateMetaDataButton->hide();
#endif
    ui->coordinateStatus->setText(tr("No coordinate or tile key"));

    _treeRoot = new ObjectTreeItem(ui->treeWidget, _treeImpl.get(), _hostInterface);

    osgEarth::Map * map = getMap(_item.get());
    if(map)
    {
        osgEarth::MapFrame frame(map);
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        for(auto it = frame.elevationLayers().begin(); it != frame.elevationLayers().end(); ++it)
        {
            SGIHostItemOsg layer(*it);
            SGIItemBasePtr item;
            if(_hostInterface->generateItem(item, &layer))
            {
                std::string name;
                _hostInterface->getObjectDisplayName(name, item.get());
                ui->layer->addItem(tr("Elevation: %1").arg(fromUtf8(name)), QVariant::fromValue(QtSGIItem(item.get())));
            }
        }
        for(auto it = frame.imageLayers().begin(); it != frame.imageLayers().end(); ++it)
        {
            SGIHostItemOsg layer(*it);
            SGIItemBasePtr item;
            if(_hostInterface->generateItem(item, &layer))
            {
                std::string name;
                _hostInterface->getObjectDisplayName(name, item.get());
                ui->layer->addItem(tr("Image: %1").arg(fromUtf8(name)), QVariant::fromValue(QtSGIItem(item.get())));
            }
        }
#endif
    }
    else
    {
        std::string name;
        _hostInterface->getObjectDisplayName(name, _item.get());
        ui->layer->addItem(fromUtf8(name), QVariant::fromValue(QtSGIItem(_item.get())));
    }

	osgEarth::MapNode * mapnode = findMapNode(_item.get());
	if (!mapnode)
	{
		ui->actionCoordinateFromCamera->setEnabled(false);
		ui->actionCoordinateFromCamera->setToolTip(tr("This function is not available because access to scene graph is not available. Please run the Tile inspector from a osgEarth::MapNode instance."));
	}

    ui->numNeighbors->addItem(tr("None"), QVariant(NUM_NEIGHBORS_NONE) );
    ui->numNeighbors->addItem(tr("Cross (4)"), QVariant(NUM_NEIGHBORS_CROSS) );
    ui->numNeighbors->addItem(tr("Immediate (9)"), QVariant(NUM_NEIGHBORS_IMMEDIATE) );
    ui->numNeighbors->addItem(tr("Childs"), QVariant(NUM_NEIGHBORS_CHILDS));
	ui->numNeighbors->addItem(tr("Parental"), QVariant(NUM_NEIGHBORS_PARENTAL));
	ui->numNeighbors->addItem(tr("Parental&Childs"), QVariant(NUM_NEIGHBORS_PARENTAL_AND_CHILDS));

    ui->layerSource->addItem(tr("Layer"), QVariant(LayerDataSourceLayer));
    ui->layerSource->addItem(tr("Tile source"), QVariant(LayerDataSourceTileSource));
    ui->layerSource->addItem(tr("Cache"), QVariant(LayerDataSourceCache));

    ui->layer->setCurrentIndex(0);

	takePositionFromCamera();
}

TileInspectorDialog::~TileInspectorDialog()
{
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
}

void TileInspectorDialog::reloadTree()
{
    setCursor(Qt::WaitCursor);

    ui->treeWidget->blockSignals(true);
    ui->treeWidget->clear();
    QList<int> panes_sizes;
    int total_width ;
    QLayout * currentLayout = ui->verticalLayout;
    total_width = this->width() - ui->verticalLayout->margin();
    const int tree_width = 3 * total_width / 5;
    const int textbox_width = 2 * total_width / 5;
    panes_sizes.append(tree_width);
    panes_sizes.append(textbox_width);
    ui->splitter->setSizes(panes_sizes);

    total_width = tree_width - 32;
    ui->treeWidget->setColumnWidth(0, 3 * total_width / 4);
    ui->treeWidget->setColumnWidth(1, total_width / 4);

#if 0
    ObjectTreeItem objectTreeRootItem(ui->treeWidget->invisibleRootItem());

    ObjectTreeItem * firstTreeItem = NULL;

    for(SGIItemBasePtrVector::const_iterator it = _tiles.begin(); it != _tiles.end(); it++)
    {
        const SGIItemBasePtr & item = *it;
        ObjectTreeItem * treeItem = (ObjectTreeItem *)objectTreeRootItem.addChild(std::string(), item.get());
        buildTree(treeItem, item.get());
        if(!firstTreeItem)
            firstTreeItem = treeItem;
    }

    if(firstTreeItem)
    {
        firstTreeItem->setSelected(true);
        //onItemActivated(firstTreeItem->treeItem(), 0);
    }
#endif // 0
    ui->treeWidget->blockSignals(false);
    setCursor(Qt::ArrowCursor);
}

SGIItemBase * TileInspectorDialog::getView()
{
    if(_info)
        return _info->getView();
    else
        return NULL;
}

void TileInspectorDialog::triggerRepaint()
{
    if(_info)
        _info->triggerRepaint();
}

void TileInspectorDialog::layerChanged(int index)
{
    QVariant data = ui->layer->itemData(index);
    QtSGIItem qitem = data.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();

    ui->levelOfDetail->clear();
    osgEarth::TileSource * tileSource = getTileSource(item);
    ui->levelOfDetail->addItem(tr("All"), QVariant(-1));
    for(unsigned lod = 0; lod < 23; lod++)
    {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        if(tileSource && tileSource->hasDataAtLOD(lod))
        {
            QString text(tr("LOD%1").arg(lod));
            ui->levelOfDetail->addItem(text, QVariant(lod));
        }
#endif
    }

    refresh();
}

void TileInspectorDialog::layerSourceChanged(int index)
{
    QVariant data = ui->layer->itemData(index);
    QtSGIItem qitem = data.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();

    ui->levelOfDetail->clear();
    osgEarth::TileSource * tileSource = getTileSource(item);
    ui->levelOfDetail->addItem(tr("All"), QVariant(-1));
    for (unsigned lod = 0; lod < 23; lod++)
    {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        if (tileSource && tileSource->hasDataAtLOD(lod))
        {
            QString text(tr("LOD%1").arg(lod));
            ui->levelOfDetail->addItem(text, QVariant(lod));
        }
#endif
    }

    refresh();
}

void TileInspectorDialog::setNodeInfo(const SGIItemBase * item)
{
    std::ostringstream os;
    if(item)
    {
        QImage qimage;
		std::string previewText;
        _hostInterface->writePrettyHTML(os, item);
        const SGIItemOsg * osgitem = dynamic_cast<const SGIItemOsg *>(item);
        if(osgitem)
        {
            const osg::Image * image = dynamic_cast<const osg::Image *>(osgitem->object());
			const osg::HeightField * hf = dynamic_cast<const osg::HeightField*>(osgitem->object());
            if(image)
            {
                osg_helpers::osgImageToQImage(image, &qimage);
            }
			else if (hf)
			{
				std::stringstream os;
				osg_helpers::heightFieldDumpHTML(os, hf);
				previewText = os.str();
			}
            else
            {
                const TileSourceTileKey * tskey = dynamic_cast<const TileSourceTileKey *>(osgitem->object());
                if(tskey)
                {
                    const TileSourceTileKeyData & data = tskey->data();
                    const osg::Image * image = dynamic_cast<const osg::Image *>(data.tileData.get());
					const osg::HeightField * hf = dynamic_cast<const osg::HeightField*>(data.tileData.get());
                    if(image)
                    {
                        osg_helpers::osgImageToQImage(image, &qimage);
                    }
					else if (hf)
					{
						std::stringstream os;
						osg_helpers::heightFieldDumpHTML(os, hf);
						previewText = os.str();
					}
                }
            }
        }
        if(!qimage.isNull())
        {
            ui->previewImage->setText(QString());
            ui->previewImage->setPixmap(QPixmap::fromImage(qimage));
        }
		else if (!previewText.empty())
		{
			ui->previewImage->setPixmap(QPixmap());
			ui->previewImage->setText(fromUtf8(previewText));
		}
        else
        {
            ui->previewImage->setPixmap(QPixmap());
            ui->previewImage->setText(tr("No image/heightfield"));
        }
    }
    else
    {
        ui->previewImage->setPixmap(QPixmap());
        ui->previewImage->setText(tr("No image"));
        os << "<b>item is <i>NULL</i></b>";
    }
    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(fromUtf8(os.str()));
    ui->textEdit->blockSignals(false);
}

void TileInspectorDialog::refresh()
{
    int index = ui->layer->currentIndex();
    LAYER_DATA_SOURCE layerDataSource = (LAYER_DATA_SOURCE)ui->layerSource->itemData(ui->layerSource->currentIndex()).toInt();
    QVariant data = ui->layer->itemData(index);
    QtSGIItem qitem = data.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = (layerDataSource == LayerDataSourceTileSource) ? getTileSource(item) : NULL;
    osgEarth::TerrainLayer * terrainLayer = (layerDataSource == LayerDataSourceLayer) ? getTerrainLayer(item) : NULL;
    osgEarth::CacheBin * cachebin = (layerDataSource == LayerDataSourceCache) ? getCacheBin(item) : NULL;
    const osgEarth::Profile * profile = NULL;
    if (tileSource)
        profile = tileSource->getProfile();
    else if (terrainLayer)
        profile = terrainLayer->getProfile();
    else if (cachebin)
    {
        osgEarth::TerrainLayer * terrainLayer = getTerrainLayer(item);
        if(terrainLayer)
            profile = terrainLayer->getProfile();
    }

    int lod = -1;
    index = ui->levelOfDetail->currentIndex();
    if (index >= 0)
        lod = ui->levelOfDetail->itemData(index).toInt();
    index = ui->numNeighbors->currentIndex();
    NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE;
    if (index >= 0)
        numNeighbors = (NUM_NEIGHBORS)ui->numNeighbors->itemData(index).toInt();

    _treeRoot->clear();

    typedef std::list<std::string> stdstringlist;
    stdstringlist urllist;

    std::string baseurl;
    std::string driver;
    bool invertY = false;
    osgEarth::Config layerConf;
    osgEarth::TileSourceOptions tileSourceOptions;
    if (tileSource)
    {
        const osgEarth::TileSourceOptions & options = tileSource->getOptions();
        tileSourceOptions = options;
        layerConf = options.getConfig();
    }
    else if (terrainLayer)
    {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        const osgEarth::TerrainLayerOptions & options = terrainLayer->getInitialOptions();
        layerConf = options.getConfig();
        driver = options.driver().value().getDriver();
#else
        layerConf = terrainLayer->getConfig();
        driver = layerConf.get("driver");
#endif
    }
    driver = tileSourceOptions.getDriver();
    if (driver == "tms")
    {
        osgEarth::Drivers::TMSOptions tmsopts(tileSourceOptions);
        invertY = tmsopts.tmsType().value() == "google";
    }

    osgEarth::optional<osgEarth::URI> url;
    layerConf.getIfSet("url", url);
    if (url.isSet())
    {
        baseurl = url.value().full();
        std::string::size_type last_slash = baseurl.rfind('/');
        if (last_slash != std::string::npos)
            baseurl.resize(last_slash + 1);
    }

    bool ok = false;
    QString input = ui->coordinate->text();
    TileKeyList tilekeylist = tileKeyListfromStringOrGpsCoordinate(ui->coordinate, profile, lod, numNeighbors, &ok);
    if(ok && !tilekeylist.empty())
    {
        std::ostringstream os;
        std::string baseurl;

        os << "<b>Result for " << input.toStdString() << "</b><br/>";
        os << "Driver: " << driver << "<br/>";
        if(driver == "tms")
        {
            std::string tms_type;
            osgEarth::Drivers::TMSOptions tmsopts(tileSourceOptions);
            osg::ref_ptr<osgEarth::Util::TMS::TileMap> tilemap = osgEarth::Util::TMS::TileMap::create(tileSource, profile);

            os << "Base URL: <a href=\"" << baseurl << "\">"  << baseurl << "</a><br/>";
            os << "TMS type: " << tmsopts.tmsType().value() << "<br/>";
            os << "Format: " << tmsopts.format().value() << "<br/>";
            os << "InvertY: " << (invertY?"true":"false") << "<br/>";
            os << "<ul>";
            for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
            {
                const osgEarth::TileKey & tilekey = *it;
                if(!tileSource->hasData(tilekey))
                    os << "<li>" << tilekey.str() << ": no data</li>" << std::endl;
                else
                {
                    std::string image_url = tilemap->getURL( tilekey, invertY );
                    if(!image_url.empty())
                    {
                        std::string full_url = baseurl + image_url;
                        urllist.push_back(full_url);
                        os << "<li>L" << tilekey.getLOD() << ": <a href=\"" << full_url << "\">" << full_url << "</a></li>" << std::endl;
                    }
                    else
                    {
                        if(tilemap->getMinLevel() != 0 && tilemap->getMinLevel() > tilekey.getLOD())
                            os << "<li>L" << tilekey.getLOD() << ": < minimum " << tilemap->getMinLevel() << "</li>" << std::endl;
                        else if(tilemap->getMaxLevel() != 0 && tilemap->getMaxLevel() < tilekey.getLOD())
                            os << "<li>L" << tilekey.getLOD() << ": > maximum " << tilemap->getMaxLevel() << "</li>" << std::endl;
                        else
                            os << "<li>L" << tilekey.getLOD() << ": no data</li>" << std::endl;
                    }
                }
            }
            os << "</ul>" << std::endl;
        }
        else if(driver == "vpb")
        {
            osgEarth::Drivers::VPBOptions vpbopts(tileSourceOptions);
            os << "Base URL: <a href=\"" << vpbopts.url().value().full() << "\">"  << vpbopts.url().value().full() << "</a><br/>";
            os << "Base name: " << vpbopts.baseName().value() << "<br/>";
            os << "Primary split: " << vpbopts.primarySplitLevel().value() << "<br/>";
            os << "Secondary split: " << vpbopts.secondarySplitLevel().value() << "<br/>";
            os << "<ul>";
            for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
            {
                const osgEarth::TileKey & tilekey = *it;
                if(!tileSource->hasData(tilekey))
                    os << "<li>" << tilekey.str() << ": no data</li>" << std::endl;
                else
                {
                    std::string image_url = getVPBTerrainTile(tilekey, vpbopts);
                    if(!image_url.empty())
                    {
                        std::string full_url = baseurl + image_url;
                        urllist.push_back(full_url);
                        os << "<li>L" << tilekey.getLOD() << ": <a href=\"" << full_url << "\">" << full_url << "</a></li>" << std::endl;
                    }
                    else
                    {
                        os << "<li>L" << tilekey.getLOD() << ": no data</li>" << std::endl;
                    }
                }
            }
            os << "</ul>" << std::endl;
        }
        else if(driver == "arcgis")
        {
            osgEarth::Drivers::ArcGISOptions arcgisopts(tileSourceOptions);
            std::string url_full = arcgisopts.url().value().full();
            os << "Base URL: <a href=\"" << url_full << "\">"  << url_full << "</a><br/>";

            std::string sep = url_full.find( "?" ) == std::string::npos ? "?" : "&";
            std::string json_url = url_full + sep + std::string("f=pjson");  // request the data in JSON format

            os << "JSON URL: <a href=\"" << json_url << "\">"  << json_url << "</a><br/>";
            os << "<ul>";

            for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
            {
                const osgEarth::TileKey & tilekey = *it;
                if(!tileSource->hasData(tilekey))
                    os << "<li>" << tilekey.str() << ": no data</li>" << std::endl;
                else
                {
                    std::string image_url = getArcGISTerrainTile(tilekey, arcgisopts);
                    if(!image_url.empty())
                    {
                        std::string full_url = baseurl + image_url;
                        urllist.push_back(full_url);
                        os << "<li>L" << tilekey.getLOD() << ": <a href=\"" << full_url << "\">" << full_url << "</a></li>" << std::endl;
                    }
                    else
                    {
                        os << "<li>L" << tilekey.getLOD() << ": no data</li>" << std::endl;
                    }
                }
            }
            os << "</ul>" << std::endl;
        }
        else
        {
            os << "<i>Driver " << driver << " not yet implemented.</i>" << std::endl;
        }
        ui->urlList->setText(QString::fromStdString(os.str()));

        for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
        {
            const osgEarth::TileKey & tilekey = *it;
            if (tileSource)
            {
                TileSourceTileKeyData data(tileSource, tilekey);
                SGIHostItemOsg tskey(new TileSourceTileKey(data));
                _treeRoot->addChild(std::string(), &tskey);
            }
            else if (terrainLayer)
            {
                TileSourceTileKeyData data(terrainLayer, tilekey);
                SGIHostItemOsg tskey(new TileSourceTileKey(data));
                _treeRoot->addChild(std::string(), &tskey);
            }
            else if (cachebin)
            {
                TileSourceTileKeyData data(cachebin, tilekey);
                SGIHostItemOsg tskey(new TileSourceTileKey(data));
                _treeRoot->addChild(std::string(), &tskey);
            }
        }
    }

    if (!urllist.empty())
    {
        std::ostringstream os;

        std::string proxyOpts;
        std::string proxyUrl;
        std::string proxyUser;
        {
            osgEarth::ProxySettings proxy;
            if (!proxy.hostName().empty())
            {
                std::stringstream ss;
                ss << proxy.hostName() << ':' << proxy.port();
                proxyUrl = ss.str();
            }
            if (!proxy.userName().empty())
            {
                std::stringstream ss;
                if (proxy.password().empty())
                    ss << proxy.userName();
                else
                    ss << proxy.userName() << ':' << proxy.password();
                proxyUser = ss.str();
            }
            if (!proxyUrl.empty())
                proxyOpts += " -x " + proxyUrl;
            if (!proxyUser.empty())
                proxyOpts += " -U " + proxyUser;
        }
        os << "Proxy URL: <a href=\"" << proxyUrl << "\">" << proxyUrl << "</a><br/>";
        os << "Proxy User: " << proxyUser << "<br/>";
        os << "<ul>";
        for (stdstringlist::const_iterator it = urllist.begin(); it != urllist.end(); it++)
        {
            const std::string & url = *it;
            std::string output = url;
            if (output.compare(0, 7, "http://") == 0)
                output.erase(0, 7);

            std::replace_if(output.begin(), output.end(), IsSlash, '_');
            os << "<li>curl" << proxyOpts << " " << url << " -o " << output << "</li>" << std::endl;
        }
        os << "</ul>" << std::endl;
        ui->proxyCmdLines->setText(QString::fromStdString(os.str()));
    }

}

void TileInspectorDialog::updateMetaData()
{
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
    int index = ui->layer->currentIndex();
    QVariant data = ui->layer->itemData(index);
    QtSGIItem qitem = data.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(tileSource)
    {
        osgEarth::Config config;
        tileSource->readMetaData(config);
        tileSource->writeMetaData(config);
    }
#endif
}

void TileInspectorDialog::proxySaveScript()
{
    int index = ui->layer->currentIndex();
    QVariant data = ui->layer->itemData(index);
    QtSGIItem qitem = data.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(tileSource)
    {
        const osgEarth::Profile * profile = tileSource->getProfile();
        const osgEarth::TileSourceOptions & options = tileSource->getOptions();
        
        int idx = ui->numNeighbors->currentIndex();
        NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE;
        if(idx >= 0)
            numNeighbors = (NUM_NEIGHBORS)ui->numNeighbors->itemData(idx).toInt();
        
        int lod = -1;
        idx = ui->levelOfDetail->currentIndex();
        if(idx >= 0)
            lod = ui->levelOfDetail->itemData(idx).toInt();

        bool ok = false;
        QString input = ui->coordinate->text();
        TileKeyList tilekeylist = tileKeyListfromStringOrGpsCoordinate(ui->coordinate, profile, lod, numNeighbors, &ok);
        if(ok && !tilekeylist.empty())
        {
            std::string baseurl;
            bool invertY = false;
            osgEarth::Config layerConf = options.getConfig();
            osgEarth::optional<osgEarth::URI> url;
            layerConf.getIfSet("url", url);
            if(url.isSet())
            {
                baseurl = url.value().full();
                std::string::size_type last_slash = baseurl.rfind('/');
                if(last_slash != std::string::npos)
                    baseurl.resize(last_slash + 1);
            }
            typedef std::list<std::string> stdstringlist;
            stdstringlist urllist;

            if(options.getDriver() == "tms")
            {
                std::string tms_type;
                osgEarth::Drivers::TMSOptions tmsopts(options);
                osg::ref_ptr<osgEarth::Util::TMS::TileMap> tilemap = osgEarth::Util::TMS::TileMap::create(tileSource, profile);

                invertY = tmsopts.tmsType().value() == "google";

                for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
                {
                    const osgEarth::TileKey & tilekey = *it;
                    if(tileSource->hasData(tilekey))
                    {
                        std::string image_url = tilemap->getURL( tilekey, invertY );
                        if(!image_url.empty())
                        {
                            std::string full_url = baseurl + image_url;
                            urllist.push_back(full_url);
                        }
                    }
                }
            }
            else if(options.getDriver() == "vpb")
            {
                osgEarth::Drivers::VPBOptions vpbopts(options);
                for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
                {
                    const osgEarth::TileKey & tilekey = *it;
                    if(tileSource->hasData(tilekey))
                    {
                        std::string image_url = getVPBTerrainTile(tilekey, vpbopts);
                        if(!image_url.empty())
                        {
                            std::string full_url = baseurl + image_url;
                            urllist.push_back(full_url);
                        }
                    }
                }
            }
            else if(options.getDriver() == "arcgis")
            {
                osgEarth::Drivers::ArcGISOptions arcgisopts(options);
                for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
                {
                    const osgEarth::TileKey & tilekey = *it;
                    if(tileSource->hasData(tilekey))
                    {
                        std::string image_url = getArcGISTerrainTile(tilekey, arcgisopts);
                        if(!image_url.empty())
                        {
                            std::string full_url = baseurl + image_url;
                            urllist.push_back(full_url);
                        }
                    }
                }
            }

            if(!urllist.empty())
            {
                std::ostringstream os;

                std::string proxyOpts;
                std::string proxyUrl;
                std::string proxyUser;
                {
                    osgEarth::ProxySettings proxy;
                    if(!proxy.hostName().empty())
                    {
                        std::stringstream ss;
                        ss << proxy.hostName() << ':' << proxy.port();
                        proxyUrl = ss.str();
                    }
                    if(!proxy.userName().empty())
                    {
                        std::stringstream ss;
                        if(proxy.password().empty())
                            ss << proxy.userName();
                        else
                            ss << proxy.userName() << ':' << proxy.password();
                        proxyUser = ss.str();
                    }
                    if(!proxyUrl.empty())
                        proxyOpts += " -x " + proxyUrl;
                    if(!proxyUser.empty())
                        proxyOpts += " -U " + proxyUser;
                }
#ifdef _WIN32
                os << "#!/bin/bash" << std::endl;
#else
                os << "rem poor windoof" << std::endl;
#endif
                for(stdstringlist::const_iterator it = urllist.begin(); it != urllist.end(); it++)
                {
                    const std::string & url = *it;
                    std::string output = url;
                    if(output.compare(0, 7, "http://") == 0)
                        output.erase(0, 7);

                    std::replace_if(output.begin(), output.end(), IsSlash, '_');
                    os << "curl" << proxyOpts << " " << url << " -o " << output << std::endl;
                }
#ifdef _WIN32
                QString filters = tr("Batch files (*.bat);;All files (*.*)");
#else
                QString filters = tr("Shell scripts (*.sh);;All files (*.*)");
#endif
                static QString lastScriptFile;
                QString scriptFile = QFileDialog::getSaveFileName(this, tr("Save proxy test script"), lastScriptFile, filters);
                if(!scriptFile.isEmpty())
                {
                    lastScriptFile = scriptFile;
                    QFile f(scriptFile);
                    f.open(QIODevice::WriteOnly | QIODevice::Text);
                    QTextStream fs(&f);
                    fs << QString::fromStdString(os.str());
                    f.close();
                }
            }
        }
    }
}

void TileInspectorDialog::loadData()
{
    IObjectTreeItemPtrList children;
    _treeRoot->children(children);
    for(auto it = children.begin(); it != children.end(); ++it)
    {
        IObjectTreeItemPtr & child = *it;
        SGIItemOsg * item = dynamic_cast<SGIItemOsg *>(child->item());
        if(item)
        {
            TileSourceTileKey * tskey = dynamic_cast<TileSourceTileKey *>(item->object());
            if(tskey)
            {
                TileSourceTileKeyData & data = tskey->data();
                osgEarth::TileSource * tileSource = data.tileSource;
                osgEarth::TerrainLayer * terrainLayer = data.terrainLayer;
                osgEarth::CacheBin * cacheBin = data.cacheBin;

                if (tileSource)
                {
                    std::string ext = tileSource->getExtension();
                    bool isImageTileSource = true;
                    if (ext.compare("osgb") == 0 || ext.compare("ive") == 0)
                        isImageTileSource = (tileSource->getPixelsPerTile() >= 64);
                    else if (ext.compare("tif") == 0)
                        isImageTileSource = false;

                    if (!tileSource->hasData(data.tileKey))
                        data.status = TileSourceTileKeyData::StatusNoData;
                    else
                    {
                        if (isImageTileSource)
                        {
                            osg::ref_ptr<osg::Image> image = tileSource->createImage(data.tileKey);
                            data.tileData = image;
                        }
                        else
                        {
                            osg::ref_ptr<osg::HeightField> hf = tileSource->createHeightField(data.tileKey);
                            data.tileData = hf;
                        }
                        data.status = data.tileData.valid() ? TileSourceTileKeyData::StatusLoaded : TileSourceTileKeyData::StatusLoadFailure;
                    }
                }
                else if (terrainLayer)
                {
                    osgEarth::ImageLayer* imageLayer = dynamic_cast<osgEarth::ImageLayer*>(terrainLayer);
                    osgEarth::ElevationLayer * elevLayer = dynamic_cast<osgEarth::ElevationLayer*>(terrainLayer);
                    if (imageLayer)
                    {
                        osgEarth::GeoImage image = imageLayer->createImage(data.tileKey);
                        data.tileData = image.getImage();
                    }
                    else if (elevLayer)
                    {
                        osgEarth::GeoHeightField hf = elevLayer->createHeightField(data.tileKey);
                        data.tileData = hf.getHeightField();
                    }
                    data.status = data.tileData.valid() ? TileSourceTileKeyData::StatusLoaded : TileSourceTileKeyData::StatusLoadFailure;
                }
                else if (cacheBin)
                {
                    osgEarth::ReadResult res = cacheBin->readObject(data.tileKey.str());
                    if(res.succeeded())
                        data.tileData = res.getObject();
                    data.status = data.tileData.valid() ? TileSourceTileKeyData::StatusLoaded : TileSourceTileKeyData::StatusLoadFailure;
                }

            }
        }
        child->updateName();
    }
}

void TileInspectorDialog::itemContextMenu(IObjectTreeItem * treeItem, IContextMenuPtr & contextMenu)
{
    SGIItemBasePtr item = treeItem->item();

    if (!contextMenu)
    {
        if (_contextMenu)
        {
            _contextMenu->setObject(item, _info);
            contextMenu = _contextMenu;
        }
        else
        {
            contextMenu = _hostInterface->createContextMenu(this, item, _info);
            _contextMenu = contextMenu;
        }
    }
    
}

void TileInspectorDialog::reloadSelectedItem()
{
    IObjectTreeItemPtr selectedItem = _treeRoot->selectedItem();
    if(selectedItem.valid())
        selectedItem->reload();
}

void TileInspectorDialog::takePositionFromCamera()
{
	osgEarth::MapNode * mapnode = findMapNode(_item.get());
	if (mapnode)
	{
		osg::Vec3d eye, center, up;
		osg::Camera * camera = osgEarth::findFirstParentOfType<osg::Camera>(mapnode);
		osgViewer::View * view = NULL;
		if (camera)
		{
			view = dynamic_cast<osgViewer::View*>(camera->getView());
			camera->getViewMatrixAsLookAt(eye, center, up);

            osg::Vec3d lookdir = center - eye;
            lookdir.normalize();
            osg::Vec3d start = eye;
            osg::Vec3d end = eye + (lookdir * osg::WGS_84_RADIUS_EQUATOR * 100);
            osg::ref_ptr<osgUtil::LineSegmentIntersector> isector = new osgUtil::LineSegmentIntersector(start, end);

            osgUtil::IntersectionVisitor intersectVisitor(isector.get());
            mapnode->accept(intersectVisitor);
            if (isector->containsIntersections())
            {
                const osgUtil::LineSegmentIntersector::Intersection & first = isector->getFirstIntersection();
                osgEarth::GeoPoint geopt;
                geopt.fromWorld(mapnode->getMapSRS()->getECEF(), first.getWorldIntersectPoint());
                osgEarth::GeoPoint geoptMap = geopt.transform(mapnode->getMapSRS());

                ui->coordinate->setText(QString("%1,%2,%3").arg(geoptMap.y()).arg(geoptMap.x()).arg(geoptMap.z()));
            }
            else
            {
                ui->coordinateStatus->setText(tr("No intersections with MapNode"));
            }
		}
        else
        {
            ui->coordinateStatus->setText(tr("No camera found"));
        }

	}
    else
    {
        ui->coordinateStatus->setText(tr("No MapNode available"));
    }
}

void TileInspectorDialog::addTileKeys(osgEarth::TileSource * tileSource, const TileKeyList & tiles, bool append)
{
    if(!append)
        _treeRoot->clear();

    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
        const osgEarth::TileKey & tilekey = *it;
        TileSourceTileKeyData data(tileSource, tilekey);
        SGIHostItemOsg tskey(new TileSourceTileKey(data));
        _treeRoot->addChild(std::string(), &tskey);
    }
}

void TileInspectorDialog::addTileKeys(osgEarth::TileSource * tileSource, const TileKeySet & tiles, bool append)
{
    if(!append)
        _treeRoot->clear();

    for(auto it = tiles.begin(); it != tiles.end(); it++)
    {
        const osgEarth::TileKey & tilekey = *it;
        TileSourceTileKeyData data(tileSource, tilekey);
        SGIHostItemOsg tskey(new TileSourceTileKey(data));
        _treeRoot->addChild(std::string(), &tskey);
    }
}

void TileInspectorDialog::addTileKeys(const TileKeyList & tiles, bool append)
{
    int index = ui->layer->currentIndex();
    QVariant itemdata = ui->layer->itemData(index);
    QtSGIItem qitem = itemdata.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(!tileSource)
        return;
    addTileKeys(tileSource, tiles, append);
}

void TileInspectorDialog::addTileKeys(const TileKeySet & tiles, bool append)
{
    int index = ui->layer->currentIndex();
    QVariant itemdata = ui->layer->itemData(index);
    QtSGIItem qitem = itemdata.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(!tileSource)
        return;
    addTileKeys(tileSource, tiles, append);
}

void TileInspectorDialog::addTileKey(const osgEarth::TileKey& key)
{
    int index = ui->layer->currentIndex();
    QVariant itemdata = ui->layer->itemData(index);
    QtSGIItem qitem = itemdata.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(!tileSource)
        return;

    TileSourceTileKeyData data(tileSource, key);
    SGIHostItemOsg tskey(new TileSourceTileKey(data));
    _treeRoot->addChild(std::string(), &tskey);
}

void TileInspectorDialog::loadFromFile()
{
    int index = ui->layer->currentIndex();
    QVariant itemdata = ui->layer->itemData(index);
    QtSGIItem qitem = itemdata.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(!tileSource)
        return;

    const osgEarth::TileSourceOptions & opts = tileSource->getOptions();
    QString oldFilename = QString::fromStdString(opts.blacklistFilename().value());

    QString filename = QFileDialog::getOpenFileName(this, tr("Load tile list from file"), oldFilename, tr("List files (*.list *.txt)"));
    if(filename.isEmpty())
        return;

    osg::ref_ptr<osgEarth::TileBlacklist> blacklist = osgEarth::TileBlacklist::read(filename.toStdString());
    if(!blacklist.valid())
    {
        QMessageBox::critical(this, tr("Tile list read error"), tr("Unable to read tile key list %1").arg(filename));
        return;
    }

    _treeRoot->clear();

    const osgEarth::Profile * profile = tileSource->getProfile();

#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
    TileBlacklistAccess::TileKeySet tiles;
    static_cast<TileBlacklistAccess*>(blacklist.get())->getTileKeySet(tiles);
    for(const osgEarth::TileKey & tilekey : tiles)
    {
        osgEarth::TileKey tilekeyWithProfile(tilekey.getLOD(), tilekey.getTileX(), tilekey.getTileY(), profile);
        TileSourceTileKeyData data(tileSource, tilekeyWithProfile);
        SGIHostItemOsg tskey(new TileSourceTileKey(data));
        _treeRoot->addChild(std::string(), &tskey);
    }
#endif

}

namespace {

    void addTileForGeoPoint(TileKeyList & tilekeylist, const osgEarth::GeoPoint & pt, TileInspectorDialog::NUM_NEIGHBORS numNeighbors, int selected_lod, const osgEarth::Profile * profile)
    {
        osgEarth::TileKey tilekey = profile->createTileKey(pt.x(), pt.y(), selected_lod);
        addTileKeyAndNeighbors(tilekeylist, tilekey, (numNeighbors==TileInspectorDialog::NUM_NEIGHBORS_PARENTAL || numNeighbors == TileInspectorDialog::NUM_NEIGHBORS_PARENTAL_AND_CHILDS)? TileInspectorDialog::NUM_NEIGHBORS_NONE:numNeighbors);
    }

    void addTilesForGeoExtent(TileKeyList & tilekeylist, const osgEarth::GeoExtent & ext, TileInspectorDialog::NUM_NEIGHBORS numNeighbors, int selected_lod, const osgEarth::Profile * profile)
    {
        osgEarth::GeoPoint nw(profile->getSRS(), ext.xMin(), ext.yMin());
        osgEarth::GeoPoint ne(profile->getSRS(), ext.xMin(), ext.yMax());
        osgEarth::GeoPoint sw(profile->getSRS(), ext.xMax(), ext.yMin());
        osgEarth::GeoPoint se(profile->getSRS(), ext.xMax(), ext.yMax());

        osgEarth::GeoPoint center;
        ext.getCentroid(center);

        if(selected_lod < 0)
        {
            for (unsigned lod = 0; lod < 21; lod++)
            {
                addTileForGeoPoint(tilekeylist, center, numNeighbors, lod, profile);
                addTileForGeoPoint(tilekeylist, nw, numNeighbors, lod, profile);
                addTileForGeoPoint(tilekeylist, ne, numNeighbors, lod, profile);
                addTileForGeoPoint(tilekeylist, sw, numNeighbors, lod, profile);
                addTileForGeoPoint(tilekeylist, se, numNeighbors, lod, profile);
            }
        }
        else
        {
            addTileForGeoPoint(tilekeylist, center, numNeighbors, selected_lod, profile);
            addTileForGeoPoint(tilekeylist, nw, numNeighbors, selected_lod, profile);
            addTileForGeoPoint(tilekeylist, ne, numNeighbors, selected_lod, profile);
            addTileForGeoPoint(tilekeylist, sw, numNeighbors, selected_lod, profile);
            addTileForGeoPoint(tilekeylist, se, numNeighbors, selected_lod, profile);
        }
    }
}

void TileInspectorDialog::takeFromDataExtents()
{
    int index = ui->layer->currentIndex();
    QVariant itemdata = ui->layer->itemData(index);
    QtSGIItem qitem = itemdata.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(!tileSource)
        return;

    int idx = ui->numNeighbors->currentIndex();
    NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE;
    if(idx >= 0)
        numNeighbors = (NUM_NEIGHBORS)ui->numNeighbors->itemData(idx).toInt();

    int lod = -1;
    idx = ui->levelOfDetail->currentIndex();
    if(idx >= 0)
        lod = ui->levelOfDetail->itemData(idx).toInt();

    const osgEarth::Profile * profile = tileSource->getProfile();
    const osgEarth::DataExtentList& dataExtents = tileSource->getDataExtents();

    TileKeyList tilekeylist;
    for(osgEarth::DataExtentList::const_iterator it = dataExtents.begin(); it != dataExtents.end(); ++it)
    {
        const osgEarth::DataExtent & ext = *it;
        addTilesForGeoExtent(tilekeylist, ext, numNeighbors, (ext.maxLevel().isSet()?ext.maxLevel().value():lod), profile);
    }

    addTileKeys(tileSource, tilekeylist);
}

void TileInspectorDialog::takeFromDataExtentsUnion()
{
    int index = ui->layer->currentIndex();
    QVariant itemdata = ui->layer->itemData(index);
    QtSGIItem qitem = itemdata.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(!tileSource)
        return;

    int idx = ui->numNeighbors->currentIndex();
    NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE;
    if(idx >= 0)
        numNeighbors = (NUM_NEIGHBORS)ui->numNeighbors->itemData(idx).toInt();

    int lod = -1;
    idx = ui->levelOfDetail->currentIndex();
    if(idx >= 0)
        lod = ui->levelOfDetail->itemData(idx).toInt();

    const osgEarth::Profile * profile = tileSource->getProfile();
    const osgEarth::GeoExtent & ext = tileSource->getDataExtentsUnion();

    TileKeyList tilekeylist;
    addTilesForGeoExtent(tilekeylist, ext, numNeighbors, lod, profile);

    addTileKeys(tileSource, tilekeylist);
}

} // namespace osgearth_plugin
} // namespace sgi
