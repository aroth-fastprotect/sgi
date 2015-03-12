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

#include <osgEarth/Registry>
#include <osgEarth/Viewpoint>
#include <osgEarth/TileKey>
#include <osgEarth/TerrainLayer>
#include <osgEarth/MapNode>

#include <osgEarthDrivers/vpb/VPBOptions>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>

#include <osgEarthUtil/TMS>

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ObjectTreeImpl>

#include "ElevationQueryReferenced"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {

using namespace sgi::qt_helpers;

namespace {

    double parseCoordinate(const QString & txt, const char positiveChar, const char negativeChar, bool * ok)
    {
        double result = 0.0;

        bool is_negative = false;
        QString tmp = txt.trimmed();
        if (tmp.at(0) == '-')
            is_negative = true;

        QStringList elems = tmp.split(' ');
        int size = elems.size();
        if (size == 1)
        {
            bool degreesOk = false;
            double degrees = elems[0].toDouble(&degreesOk);
            if(degreesOk)
            {
                if(ok) *ok = true;
                result = degrees;
            }
            else
            {
                if(ok) *ok = false;
                result = 0.0;
            }
        }
        else if (size == 2)
        {
            bool degreesOk = false;
            bool minutesOk = false;
            double degrees = elems[0].toDouble(&degreesOk);
            double minutes = elems[1].toDouble(&minutesOk);

            if(degreesOk && minutesOk)
            {
                if(ok) *ok = true;
                result = degrees + (minutes/60.0);
            }
            else if(degreesOk)
            {
                elems[1].toUpper();
                QString s = elems[1].trimmed();
                if (s[0] == positiveChar && s.length() == 1)
                {
                    if(ok) *ok = true;
                    result = fabs(degrees);
                }
                else if (s[0] == negativeChar && s.length() == 1)
                {
                    if(ok) *ok = true;
                    result = -1.0 * fabs(degrees);
                }
                else if (s.length() > 0)
                {
                    if(ok) *ok = false;
                    result = 0;
                }
            }
            else
                result = 0.0;
        }
        else if (size > 2)
        {
            bool degreesOk = false;
            bool minutesOk = false;
            bool secondsOk = false;
            double degrees = elems[0].toDouble(&degreesOk);
            double minutes = elems[1].toDouble(&minutesOk);
            double seconds = elems[2].toDouble(&secondsOk);

            if(degreesOk && minutesOk && secondsOk)
            {
                result = degrees + (minutes/60.0) + (seconds/3600.0);
                if (size > 3) {
                    elems[3].toUpper();
                    QString s = elems[3].trimmed();
                    if (s[0] == positiveChar && s.length() == 1)
                    {
                        if(ok) *ok = true;
                        result = fabs(degrees);
                    }
                    else if (s[0] == negativeChar && s.length() == 1)
                    {
                        if(ok) *ok = true;
                        result = -1.0 * fabs(degrees);
                    }
                    else if (s.length() > 0)
                    {
                        if(ok) *ok = false;
                        result = 0;
                    }
                }
                else {
                    if(ok) *ok = true;
                }

            }
            else if(degreesOk && minutesOk)
            {
                result = degrees + (minutes/60.0);
                elems[2].toUpper();
                QString s = elems[2].trimmed();
                if (s[0] == positiveChar && s.length() == 1)
                {
                    if(ok) *ok = true;
                    result = fabs(degrees);
                }
                else if (s[0] == negativeChar && s.length() == 1)
                {
                    if(ok) *ok = true;
                    result = -1.0 * fabs(degrees);
                }
                else if (s.length() > 0)
                {
                    if(ok) *ok = false;
                    result = 0;
                }
            }
        }

        if (is_negative)
            result = -fabs(result);

        return result;
    }

    double parseLatitude(const QString & txt, bool * ok)
    {
        return parseCoordinate(txt, 'N', 'S', ok);
    }

    double parseLongitude(const QString & txt, bool * ok)
    {
        return parseCoordinate(txt, 'E', 'W', ok);
    }

    double parseElevation(const QString & txt, bool * ok)
    {
        double ret;
        ret = txt.toDouble(ok);
        return ret;
    }

    osgEarth::GeoPoint GeoPointFromString(const QString & str, bool * ok)
    {
        osgEarth::GeoPoint ret;

        bool latOk = false, lonOk = false, elevOk = false;

        if(!str.isEmpty())
        {
            double lat = 0.0, lon = 0.0, elev = 0.0;
            QStringList parts = str.split(',', QString::SkipEmptyParts);
            if(parts.size() == 2)
            {
                lat = parseLatitude(parts[0], &latOk);
                lon = parseLongitude(parts[1], &lonOk);
                elevOk = true;
                elev = 0;
            }
            else if(parts.size() >= 3)
            {
                lat = parseLatitude(parts[0], &latOk);
                lon = parseLongitude(parts[1], &lonOk);
                elev = parseElevation(parts[2], &elevOk);
            }
            if(latOk && lonOk && elevOk)
                ret.set(osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS(), osg::Vec3d(lat, lon, elev), osgEarth::ALTMODE_ABSOLUTE);
        }
        if(ok)
            *ok = (latOk && lonOk && elevOk);
        return ret;
    }

    typedef std::list<osgEarth::TileKey> TileKeyList;
    typedef std::set<osgEarth::TileKey> TileKeySet;
    
    void addTileKeyAndNeighbors(TileKeyList & list, const osgEarth::TileKey & tilekey, TileInspectorDialog::NUM_NEIGHBORS numNeighbors)
    {
        TileKeySet set;
        osgEarth::TileKey orgtilekey = tilekey;
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
        }
        for(TileKeySet::const_iterator it = set.begin(); it != set.end(); it++)
            list.push_back(*it);
    }
    
    TileKeyList tileKeyListfromStringOrGpsCoordinate(QString & input, const osgEarth::Profile * profile, int selectedLod, TileInspectorDialog::NUM_NEIGHBORS numNeighbors, bool * ret_ok)
    {
        TileKeyList ret;
        bool ok;
        osgEarth::GeoPoint geopt;
        const unsigned maximum_lod = 21;
        unsigned lod = maximum_lod;
        bool hasLOD = false;
        int at_char = input.indexOf('@');
        if(at_char > 0)
        {
            QString inputgps = input.left(at_char);
            QString inputlod = input.mid(at_char + 1);
            geopt = GeoPointFromString(inputgps, &ok);
            if(ok)
            {
                lod = inputlod.toUInt(&ok);
                if(ok)
                    hasLOD = true;
            }
        }
        else
        {
            geopt = GeoPointFromString(input, &ok);
        }

        if(ok)
        {
            osgEarth::GeoPoint geoptProfile = geopt.transform(profile->getSRS());

            if(selectedLod == -1)
            {
                for(unsigned lod = 0; lod < maximum_lod; lod++)
                {
                    osgEarth::TileKey tilekey = profile->createTileKey( geoptProfile.x(), geoptProfile.y(), lod);
                    addTileKeyAndNeighbors(ret, tilekey, numNeighbors);
                }
            }
            else
            {
                osgEarth::TileKey tilekey = profile->createTileKey( geoptProfile.x(), geoptProfile.y(), selectedLod);
                addTileKeyAndNeighbors(ret, tilekey, numNeighbors);
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
}


class TileInspectorDialog::ContextMenuCallback : public IContextMenuInfo
{
public:
    ContextMenuCallback(TileInspectorDialog * dialog)
        : _dialog(dialog) {}
public:
    virtual SGIHostItemBase * getView()
    {
        return _dialog->getView();
    }
    virtual void            triggerRepaint()
    {
        _dialog->triggerRepaint();
    }
    virtual bool            showSceneGraphDialog(SGIItemBase * item)
    {
        return _dialog->showSceneGraphDialog(item);
    }
    virtual bool            showObjectLoggerDialog(SGIItemBase * item)
    {
        return _dialog->newInstance(item);
    }
private:
    TileInspectorDialog * _dialog;
};

class TileInspectorDialog::SceneGraphDialogInfo : public ISceneGraphDialogInfo
{
public:
    SceneGraphDialogInfo(TileInspectorDialog * dialog)
        : _dialog(dialog) {}
public:
    virtual IContextMenu *  contextMenu(QWidget * parent, const SGIItemBase * item, IContextMenuInfo * info)
    {
        return NULL;
    }
    virtual SGIHostItemBase * getView()
    {
        return _dialog->getView();
    }
    virtual void            triggerRepaint()
    {
        _dialog->triggerRepaint();
    }
private:
    TileInspectorDialog * _dialog;
};

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
    ObjectTreeItem::s_hostInterface = _hostInterface;

    _treeRoot = new ObjectTreeItem(ui->treeWidget, _treeImpl.get(), _hostInterface);

    osgEarth::Map * map = getMap(_item.get());
    if(map)
    {
        osgEarth::MapFrame frame(map);
        for(auto it = frame.elevationLayers().begin(); it != frame.elevationLayers().end(); ++it)
        {
            SGIHostItemOsg layer(*it);
            SGIItemBasePtr item;
            if(_hostInterface->generateItem(item, &layer))
            {
                std::string name;
                _hostInterface->getObjectDisplayName(name, item.get());
                ui->layer->addItem(fromLocal8Bit(name), QVariant::fromValue(QtSGIItem(item.get())));
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
                ui->layer->addItem(fromLocal8Bit(name), QVariant::fromValue(QtSGIItem(item.get())));
            }
        }
    }
    else
    {
        std::string name;
        _hostInterface->getObjectDisplayName(name, _item.get());
        ui->layer->addItem(fromLocal8Bit(name), QVariant::fromValue(QtSGIItem(_item.get())));
    }

    ui->numNeighbors->addItem(tr("None"), QVariant(NUM_NEIGHBORS_NONE) );
    ui->numNeighbors->addItem(tr("Cross (4)"), QVariant(NUM_NEIGHBORS_CROSS) );
    ui->numNeighbors->addItem(tr("Immediate (9)"), QVariant(NUM_NEIGHBORS_IMMEDIATE) );

    ui->layer->setCurrentIndex(0);
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

SGIHostItemBase * TileInspectorDialog::getView()
{
//     if(_info)
//         return _info->getView();
//     else
        return NULL;
}

void TileInspectorDialog::triggerRepaint()
{
    if(_info)
        _info->triggerRepaint();
}

bool TileInspectorDialog::showSceneGraphDialog(SGIItemBase * item)
{
    return false;
}

bool TileInspectorDialog::showSceneGraphDialog(const SGIHostItemBase * hostitem)
{
    bool ret;
    osg::ref_ptr<SGIItemBase> item;
    if(_hostInterface->generateItem(item, hostitem))
        ret = showSceneGraphDialog(item.get());
    else
        ret = false;
    return ret;
}

bool TileInspectorDialog::newInstance(SGIItemBase * item)
{
    return false;
}

bool TileInspectorDialog::newInstance(const SGIHostItemBase * hostitem)
{
    bool ret;
    osg::ref_ptr<SGIItemBase> item;
    if(_hostInterface->generateItem(item, hostitem))
        ret = newInstance(item.get());
    else
        ret = false;
    return ret;
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
        if(tileSource && tileSource->hasDataAtLOD(lod))
        {
            QString text(tr("LOD%1").arg(lod));
            ui->levelOfDetail->addItem(text, QVariant(lod));
        }
    }

    refresh();
}

void TileInspectorDialog::setNodeInfo(const SGIItemBase * item)
{
    std::ostringstream os;
    if(item)
    {
        QImage qimage;
        _hostInterface->writePrettyHTML(os, item);
        const SGIItemOsg * osgitem = dynamic_cast<const SGIItemOsg *>(item);
        if(osgitem)
        {
            const osg::Image * image = dynamic_cast<const osg::Image *>(osgitem->object());
            if(image)
            {
                osg_helpers::osgImageToQImage(image, &qimage);
            }
            else
            {
                const TileSourceTileKey * tskey = dynamic_cast<const TileSourceTileKey *>(osgitem->object());
                if(tskey)
                {
                    const TileSourceTileKeyData & data = tskey->data();
                    const osg::Image * image = dynamic_cast<const osg::Image *>(data.tileData.get());
                    if(image)
                    {
                        osg_helpers::osgImageToQImage(image, &qimage);
                    }
                }
            }
        }
        if(!qimage.isNull())
        {
            ui->previewImage->setText(QString());
            ui->previewImage->setPixmap(QPixmap::fromImage(qimage));
        }
        else
        {
            ui->previewImage->setPixmap(QPixmap());
            ui->previewImage->setText(tr("No image"));
        }
    }
    else
    {
        ui->previewImage->setPixmap(QPixmap());
        ui->previewImage->setText(tr("No image"));
        os << "<b>item is <i>NULL</i></b>";
    }
    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(fromLocal8Bit(os.str()));
    ui->textEdit->blockSignals(false);
}

void TileInspectorDialog::refresh()
{
    int index = ui->layer->currentIndex();
    QVariant data = ui->layer->itemData(index);
    QtSGIItem qitem = data.value<QtSGIItem>();
    SGIItemOsg * item = (SGIItemOsg *)qitem.item();
    osgEarth::TileSource * tileSource = getTileSource(item);
    if(tileSource)
    {
        _treeRoot->clear();
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
        TileKeyList tilekeylist = tileKeyListfromStringOrGpsCoordinate(input, profile, lod, numNeighbors, &ok);
        if(ok && !tilekeylist.empty())
        {
            std::string baseurl;
            bool invertY = false;
            osgEarth::Config layerConf = options.getConfig();
            osgEarth::optional<osgEarth::URI> url;
            std::ostringstream os;
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

            os << "<b>Result for " << input.toStdString() << "</b><br/>";
            os << "Driver: " << options.getDriver() << "<br/>";
            if(options.getDriver() == "tms")
            {
                std::string tms_type;
                osgEarth::Drivers::TMSOptions tmsopts(options);
                osg::ref_ptr<osgEarth::Util::TMS::TileMap> tilemap = osgEarth::Util::TMS::TileMap::create(tileSource, profile);

                invertY = tmsopts.tmsType().value() == "google";

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
            else if(options.getDriver() == "vpb")
            {
                osgEarth::Drivers::VPBOptions vpbopts(options);
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
            else if(options.getDriver() == "arcgis")
            {
                osgEarth::Drivers::ArcGISOptions arcgisopts(options);
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
                os << "<i>Driver " << options.getDriver() << " not yet implemented.</i>" << std::endl;
            }
            ui->urlList->setText(QString::fromStdString(os.str()));

            for(TileKeyList::const_iterator it = tilekeylist.begin(); it != tilekeylist.end(); it++)
            {
                const osgEarth::TileKey & tilekey = *it;
                TileSourceTileKeyData data(tileSource, tilekey);
                SGIHostItemOsg tskey(new TileSourceTileKey(data));
                _treeRoot->addChild(std::string(), &tskey);
            }

            //ui->previewImage
            
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
                os << "Proxy URL: <a href=\"" << proxyUrl << "\">" << proxyUrl << "</a><br/>";
                os << "Proxy User: " << proxyUser  << "<br/>";
                os << "<ul>";
                for(stdstringlist::const_iterator it = urllist.begin(); it != urllist.end(); it++)
                {
                    const std::string & url = *it;
                    std::string output = url;
                    if(output.compare(0, 7, "http://") == 0)
                        output.erase(0, 7);

                    std::replace_if(output.begin(), output.end(), IsSlash, '_');
                    os << "<li>curl" << proxyOpts << " " << url << " -o " << output << "</li>" << std::endl;
                }
                os << "</ul>" << std::endl;
                ui->proxyCmdLines->setText(QString::fromStdString(os.str()));
            }
        }
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
        TileKeyList tilekeylist = tileKeyListfromStringOrGpsCoordinate(input, profile, lod, numNeighbors, &ok);
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
                osg::Image * image = tileSource->createImage(data.tileKey);
                data.tileData = image;
            }
        }
    }
}

void TileInspectorDialog::itemContextMenu(IObjectTreeItem * treeItem, IContextMenuPtr & contextMenu)
{
    SGIItemBasePtr item = treeItem->item();

    if (!_contextMenuCallback)
        _contextMenuCallback = new ContextMenuCallback(this);

    if (!contextMenu)
    {
        if (_contextMenu)
        {
            _contextMenu->setObject(item, _contextMenuCallback);
            contextMenu = _contextMenu;
        }
        else
        {
            contextMenu = _hostInterface->createContextMenu(this, item, _contextMenuCallback);
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

} // namespace osgearth_plugin
} // namespace sgi
