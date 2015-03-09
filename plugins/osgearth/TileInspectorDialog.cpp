#include "stdafx.h"
#include "TileInspectorDialog.h"
#include <sgi/plugins/SGIItemOsg>

#include "ui_TileInspectorDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/helpers/qt>

#include <QTextStream>
#include <QFileDialog>
#include <QMenu>

#include <osgEarth/Registry>
#include <osgEarth/Viewpoint>
#include <osgEarth/TileKey>
#include <osgEarth/TerrainLayer>

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


TileInspectorDialog::TileInspectorDialog(QWidget * parent, SGIItemOsg * item, ISettingsDialogInfo * info)
	: QDialog(parent)
    , _item(item)
    , _interface(new SettingsDialogImpl(this))
    , _info(info)
{
    Q_ASSERT(_info != NULL);

	ui = new Ui_TileInspectorDialog;
	ui->setupUi( this );
    _treeRoot = new ObjectTreeItem(ui->treeWidget->invisibleRootItem());

    QString name;
    const osgEarth::TerrainLayer * terrainLayer = getTerrainLayer();
    const osgEarth::TileSource * tileSource = getTileSource();
    if(terrainLayer)
        name = QString::fromStdString(terrainLayer->getName());
    else if(tileSource)
        name = QString::fromStdString(tileSource->getName());
    ui->layer->addItem(name);

    ui->levelOfDetail->addItem(tr("All"), QVariant(-1));
    for(unsigned lod = 0; lod < 23; lod++)
    {
        if(tileSource->hasDataAtLOD(lod))
        {
            QString text(tr("LOD%1").arg(lod));
            ui->levelOfDetail->addItem(text, QVariant(lod));
        }
    }

    ui->numNeighbors->addItem(tr("None"), QVariant(NUM_NEIGHBORS_NONE) );
    ui->numNeighbors->addItem(tr("Cross (4)"), QVariant(NUM_NEIGHBORS_CROSS) );
    ui->numNeighbors->addItem(tr("Immediate (9)"), QVariant(NUM_NEIGHBORS_IMMEDIATE) );

    refresh();
}

TileInspectorDialog::~TileInspectorDialog()
{
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
}

osgEarth::TileSource * TileInspectorDialog::getTileSource() const
{

    if(osgEarth::TileSource * tileSource = dynamic_cast<osgEarth::TileSource *>(_item->object()))
        return tileSource;
    else if(osgEarth::TerrainLayer * terrainLayer = dynamic_cast<osgEarth::TerrainLayer*>(_item->object()))
        return terrainLayer->getTileSource();
    else
        return NULL;
}

osgEarth::TerrainLayer * TileInspectorDialog::getTerrainLayer() const
{
    if(osgEarth::TerrainLayer * terrainLayer = dynamic_cast<osgEarth::TerrainLayer *>(_item->object()))
        return terrainLayer;
    else
        return NULL;
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
        onItemActivated(firstTreeItem->treeItem(), 0);
    }

    ui->treeWidget->blockSignals(false);
    setCursor(Qt::ArrowCursor);
}

void TileInspectorDialog::onItemExpanded(QTreeWidgetItem * item)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    if(!itemData.isPopulated() && itemData.hasItem())
    {
        // we are going to re-populate the item with new data,
        // so first remove the old dummy child item.
        QList<QTreeWidgetItem *> children = item->takeChildren();
        Q_FOREACH(QTreeWidgetItem * child, children)
        {
            delete child;
        }
        ObjectTreeItem treeItem(item);
        buildTree(&treeItem, itemData.item());
    }
}

void TileInspectorDialog::onItemCollapsed(QTreeWidgetItem * item)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

void TileInspectorDialog::onItemClicked(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

void TileInspectorDialog::onItemActivated(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

bool TileInspectorDialog::buildTree(IObjectTreeItem * treeItem, SGIItemBase * item)
{
    bool ret = _hostInterface->objectTreeBuildTree(treeItem, item);
    if(ret)
    {
        /*
        InternalItemData internalItemData(item);;
        SGIHostItemOsg hostItemInternal(new ReferencedInternalItemData(internalItemData));
        treeItem->addChild("Internal", &hostItemInternal);

        QTreeWidgetItem * treeItemQt = treeItem->treeItem();
        QtSGIItem itemData = treeItemQt->data(0, Qt::UserRole).value<QtSGIItem>();
        itemData.markAsPopulated();
        treeItemQt->setData(0, Qt::UserRole, QVariant::fromValue(itemData));
        */
    }
    return ret;
}

void TileInspectorDialog::onItemContextMenu(QPoint pt)
{
    QTreeWidgetItem * item = ui->treeWidget->itemAt (pt);
    QtSGIItem itemData;
    if(item)
        itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();

    QMenu * contextMenu = NULL;
    if(!_contextMenuCallback)
        _contextMenuCallback = new ContextMenuCallback(this);

    IContextMenu * objectMenu = NULL;
//     if(_info)
//         objectMenu = _info->contextMenu(this, itemData.item(), _contextMenuCallback);
    if(!objectMenu)
    {
        if(_contextMenu)
        {
            _contextMenu->setObject(itemData.item(), _contextMenuCallback);
            objectMenu = _contextMenu;
        }
        else
        {
            _hostInterface->createContextMenu(this, itemData.item(), _contextMenuCallback);
        }
    }

    if(objectMenu)
        contextMenu = objectMenu->getMenu();

    _contextMenu = objectMenu;

    if(contextMenu)
    {
        pt.ry() += ui->treeWidget->header()->height();
        QPoint globalPos = ui->treeWidget->mapToGlobal(pt);
        contextMenu->popup(globalPos);
    }
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

void TileInspectorDialog::refresh()
{
    osgEarth::TileSource * tileSource = getTileSource();
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
    osgEarth::TileSource * tileSource = const_cast<osgEarth::TileSource *>(getTileSource());
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
    const osgEarth::TileSource * tileSource = getTileSource();
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

} // namespace osgearth_plugin
} // namespace sgi
