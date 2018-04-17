#include "stdafx.h"
#include "TileInspectorDialog.h"
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

#include <osgEarth/Version>
#include <osgEarth/Registry>
#include <osgEarth/Viewpoint>
#include <osgEarth/TileKey>
#include <osgEarth/TerrainLayer>
#include <osgEarth/MapNode>
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
#include <osgEarth/MapFrame>
#endif

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
#include "geo_helpers.h"
#include "osgearth_accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {

using namespace sgi::qt_helpers;

namespace {


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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            const osgEarth::Profile * p = terrainLayer->getProfile();
            if (p)
                return terrainLayer->getCacheBin(p);
#else
            osgEarth::CacheSettings * cs = terrainLayer->getCacheSettings();
            if(cs)
                return cs->getCacheBin();
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

    ui->treeWidget->setHeaderHidden(true);
    _treeRoot = new ObjectTreeItem(ui->treeWidget, _treeImpl.get(), _hostInterface);

    osgEarth::Map * map = getMap(_item.get());
    if(map)
    {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        osgEarth::MapFrame frame(map);
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
#else
        osgEarth::LayerVector layers;
        map->getLayers(layers);
        for (auto it = layers.begin(); it != layers.end(); ++it)
        {
            osgEarth::Layer * layer = (*it).get();
            osgEarth::ImageLayer * imageLayer = dynamic_cast<osgEarth::ImageLayer *>(layer);
            osgEarth::ElevationLayer * elevLayer = dynamic_cast<osgEarth::ElevationLayer *>(layer);
            if(!imageLayer && !elevLayer)
                continue;

            SGIHostItemOsg hostitem(layer);
            SGIItemBasePtr item;
            if (_hostInterface->generateItem(item, &hostitem))
            {
                std::string name;
                _hostInterface->getObjectDisplayName(name, item.get());
                QString itemname;
                if(imageLayer)
                    itemname = tr("Image: %1").arg(fromUtf8(name));
                else if(elevLayer)
                    itemname = tr("Elevation: %1").arg(fromUtf8(name));
                ui->layer->addItem(itemname, QVariant::fromValue(QtSGIItem(item.get())));
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

    ui->numNeighbors->addItem(tr("None"), QVariant(TileKeyList::NUM_NEIGHBORS_NONE) );
    ui->numNeighbors->addItem(tr("Cross (4)"), QVariant(TileKeyList::NUM_NEIGHBORS_CROSS) );
    ui->numNeighbors->addItem(tr("Immediate (9)"), QVariant(TileKeyList::NUM_NEIGHBORS_IMMEDIATE) );
    ui->numNeighbors->addItem(tr("Childs"), QVariant(TileKeyList::NUM_NEIGHBORS_CHILDS));
	ui->numNeighbors->addItem(tr("Parental"), QVariant(TileKeyList::NUM_NEIGHBORS_PARENTAL));
	ui->numNeighbors->addItem(tr("Parental&Childs"), QVariant(TileKeyList::NUM_NEIGHBORS_PARENTAL_AND_CHILDS));

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


    osgEarth::TileSource * tileSource = getTileSource(item);
    osgEarth::TerrainLayer * terrainLayer = getTerrainLayer(item);
    osgEarth::CacheBin * cachebin = getCacheBin(item);


    ui->layerSource->clear();
    if (terrainLayer)
        ui->layerSource->addItem(tr("Layer"), QVariant(LayerDataSourceLayer));
    if (tileSource)
        ui->layerSource->addItem(tr("Tile source"), QVariant(LayerDataSourceTileSource));
    if (cachebin)
        ui->layerSource->addItem(tr("Cache"), QVariant(LayerDataSourceCache));
    if (!ui->layerSource->count())
        ui->layerSource->addItem(tr("None"), QVariant(LayerDataSourceNone));

    ui->levelOfDetail->clear();
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
    ui->levelOfDetail->addItem(tr("All"), QVariant(-1));
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::TileSource * tileSource = getTileSource(item);
    for (unsigned lod = 0; lod < 23; lod++)
    {
        if (tileSource && tileSource->hasDataAtLOD(lod))
        {
            QString text(tr("LOD%1").arg(lod));
            ui->levelOfDetail->addItem(text, QVariant(lod));
        }
    }
#endif

    refresh();
}

void TileInspectorDialog::setNodeInfo(const SGIItemBase * item)
{
    std::ostringstream os;
    if(item)
    {
        QImage qimage;
		std::string previewText;
        std::string previewTextPlain;
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
				osg_helpers::heightFieldDumpPlainText(os, hf);
                previewTextPlain = os.str();
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
						osg_helpers::heightFieldDumpPlainText(os, hf);
                        previewTextPlain = os.str();
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
            ui->previewImage->setTextFormat(Qt::RichText);
			ui->previewImage->setText(fromUtf8(previewText));
		}
        else if (!previewTextPlain.empty())
        {
            ui->previewImage->setPixmap(QPixmap());
            ui->previewImage->setTextFormat(Qt::PlainText);
            ui->previewImage->setText(fromUtf8(previewTextPlain));
        }
        else
        {
            ui->previewImage->setPixmap(QPixmap());
            ui->previewImage->setTextFormat(Qt::PlainText);
            ui->previewImage->setText(tr("No image/heightfield"));
        }
    }
    else
    {
        ui->previewImage->setPixmap(QPixmap());
        ui->previewImage->setTextFormat(Qt::PlainText);
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
    TileKeyList::NUM_NEIGHBORS numNeighbors = TileKeyList::NUM_NEIGHBORS_NONE;
    if (index >= 0)
        numNeighbors = (TileKeyList::NUM_NEIGHBORS)ui->numNeighbors->itemData(index).toInt();

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
        driver = layerConf.value("driver");
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
    TileKeyList tilekeylist;
    if(tilekeylist.fromLineEdit(ui->coordinate, profile, lod, numNeighbors) && !tilekeylist.empty())
    {
        std::ostringstream os;
        std::string baseurl;

        os << "<b>Result for " << input.toStdString() << "</b><br/>";
        os << "Driver: " << driver << "<br/>";
        if(driver == "tms")
        {
            std::string tms_type;
            osgEarth::DataExtentList dataExtents;

            osgEarth::Drivers::TMSOptions tmsopts(tileSourceOptions);
            osg::ref_ptr<osgEarth::Util::TMS::TileMap> tilemap = osgEarth::Util::TMS::TileMap::create(baseurl, profile,
                dataExtents, tmsopts.format().value(), 256, 256);

            os << "Base URL: <a href=\"" << baseurl << "\">"  << baseurl << "</a><br/>";
            os << "TMS type: " << tmsopts.tmsType().value() << "<br/>";
            os << "Format: " << tmsopts.format().value() << "<br/>";
            os << "InvertY: " << (invertY?"true":"false") << "<br/>";
            os << "<ul>";
            for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
            {
                const osgEarth::TileKey & tilekey = *it;
                if(0 /*!tileSource->hasData(tilekey)*/ )
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
                if(0 /*!tileSource->hasData(tilekey)*/ )
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
                if(0 /*!tileSource->hasData(tilekey)*/ )
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
                if (terrainLayer->mayHaveData(tilekey))
                {
                    TileSourceTileKeyData data(terrainLayer, tilekey);
                    SGIHostItemOsg tskey(new TileSourceTileKey(data));
                    _treeRoot->addChild(std::string(), &tskey);
                }
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
        //tileSource->readMetaData(config);
        //tileSource->writeMetaData(config);
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
        TileKeyList::NUM_NEIGHBORS numNeighbors = TileKeyList::NUM_NEIGHBORS_NONE;
        if(idx >= 0)
            numNeighbors = (TileKeyList::NUM_NEIGHBORS)ui->numNeighbors->itemData(idx).toInt();
        
        int lod = -1;
        idx = ui->levelOfDetail->currentIndex();
        if(idx >= 0)
            lod = ui->levelOfDetail->itemData(idx).toInt();

        bool ok = false;
        QString input = ui->coordinate->text();
        TileKeyList tilekeylist;
        if(tilekeylist.fromLineEdit(ui->coordinate, profile, lod, numNeighbors) && !tilekeylist.empty())
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
                osgEarth::DataExtentList dataExtents;

                std::string tms_type;
                osgEarth::Drivers::TMSOptions tmsopts(options);
                osg::ref_ptr<osgEarth::Util::TMS::TileMap> tilemap = osgEarth::Util::TMS::TileMap::create(baseurl, profile,
                    dataExtents, tmsopts.format().value(), 256, 256);

                invertY = tmsopts.tmsType().value() == "google";

                for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
                {
                    const osgEarth::TileKey & tilekey = *it;
                    if(1 /*tileSource->hasData(tilekey)*/)
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
                    if(1 /*tileSource->hasData(tilekey)*/)
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
                    if(1 /*tileSource->hasData(tilekey)*/)
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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                    bool isImageTileSource = true;
                    isImageTileSource = (tileSource->getPixelsPerTile() >= 64);
#else
                    std::string ext = tileSource->getExtension();
                    bool isImageTileSource = true;
                    if (ext.compare("osgb") == 0 || ext.compare("ive") == 0)
                        isImageTileSource = (tileSource->getPixelsPerTile() >= 64);
                    else if (ext.compare("tif") == 0)
                        isImageTileSource = false;
#endif

                    if (0 /*!tileSource->hasData(data.tileKey)*/)
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
                    std::string cacheKey = osgEarth::Stringify() << data.tileKey.str() << "_" << data.tileKey.getProfile()->getHorizSignature();
                    osgEarth::ReadResult res = cacheBin->readObject(cacheKey, nullptr);
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

#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
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
#endif
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
    TileKeyList::NUM_NEIGHBORS numNeighbors = TileKeyList::NUM_NEIGHBORS_NONE;
    if(idx >= 0)
        numNeighbors = (TileKeyList::NUM_NEIGHBORS)ui->numNeighbors->itemData(idx).toInt();

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
        tilekeylist.addTilesForGeoExtent(ext, profile, (ext.maxLevel().isSet()?ext.maxLevel().value():lod), numNeighbors);
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
    TileKeyList::NUM_NEIGHBORS numNeighbors = TileKeyList::NUM_NEIGHBORS_NONE;
    if(idx >= 0)
        numNeighbors = (TileKeyList::NUM_NEIGHBORS)ui->numNeighbors->itemData(idx).toInt();

    int lod = -1;
    idx = ui->levelOfDetail->currentIndex();
    if(idx >= 0)
        lod = ui->levelOfDetail->itemData(idx).toInt();

    const osgEarth::Profile * profile = tileSource->getProfile();
    //const osgEarth::GeoExtent & ext = tileSource->getDataExtentsUnion();

    TileKeyList tilekeylist;
    tilekeylist.addTilesForGeoExtent(osgEarth::GeoExtent(), profile, lod, numNeighbors);

    addTileKeys(tileSource, tilekeylist);
}

} // namespace osgearth_plugin
} // namespace sgi
