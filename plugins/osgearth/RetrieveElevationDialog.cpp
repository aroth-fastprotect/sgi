#include "stdafx.h"
#include "RetrieveElevationDialog.h"
#include "RetrieveElevationDialog.moc"
#include "../osg/SGIItemOsg"

#include "ui_RetrieveElevationDialog.h"

#include <osgEarth/Version>
#include <osgEarth/ElevationQuery>

#include <sgi/plugins/SGISettingsDialogImpl>

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QProcess>

#include "ElevationQueryReferenced"
#include "osgearth_accessor.h"
#include "string_helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {

namespace {
    ElevationQueryReferenced * getElevationQuery(SGIItemBase * item)
    {
        ElevationQueryReferenced * ret = NULL;
        SGIItemOsg* osgitem = dynamic_cast<SGIItemOsg*>(item);
        if(osgitem)
            ret = dynamic_cast<ElevationQueryReferenced*>(osgitem->object());
        return ret;
    }

    QString GeoPointToString(const osgEarth::GeoPoint & point)
    {
        QString ret = QString::number(point.y(),'f',6) + "," + QString::number(point.x(),'f',6);
        return ret;
    }

    bool elevationWebQuery(const std::string & url, double & elevation, double & resolution)
    {
        bool ret = false;
        elevation = 0.0;
        resolution = 0.0;
        osgEarth::ReadResult result;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
        osgEarth::HTTPRequest req(url);
        result = osgEarth::HTTPClient::readString(req);
#else
        result = osgEarth::HTTPClient::readString(url);
#endif
        if(result.succeeded())
        {
            std::stringstream is(result.getString());
            osgEarth::Config resp;
            if(resp.fromXML(is))
            {
                const osgEarth::Config& firstChild = resp.children().front();
                osgEarth::Config result = firstChild.child("result");
                elevation = result.value<double>("elevation", 0.0);
                resolution = result.value<double>("resolution", 0.0);
                ret = true;
            }
        }
        return ret;
    }
}


RetrieveElevationDialog::RetrieveElevationDialog(QWidget * parent, SGIItemBase * item, ISettingsDialogInfo * info)
	: QDialog(parent)
    , _item(item)
    , _interface(new SettingsDialogImpl(this))
    , _info(info)
{
    Q_ASSERT(_info != NULL);

	ui = new Ui_RetrieveElevationDialog;
	ui->setupUi( this );
    ui->layer->addItem("All", QString());
    ui->layer->setCurrentIndex(0);
    ui->layer->setEnabled(false);

    loadResults();
}

RetrieveElevationDialog::~RetrieveElevationDialog()
{
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
}

QTreeWidgetItem * RetrieveElevationDialog::addResult(const osgEarth::GeoPoint & point, double elevation, double resolution, qint64 time, const QDateTime & timestamp, bool web)
{
    QDateTime resultTimestamp = timestamp;
    if(!resultTimestamp.isValid())
        resultTimestamp = QDateTime::currentDateTime();
    QTreeWidgetItem * parent = ui->results->invisibleRootItem();
    QTreeWidgetItem * item = new QTreeWidgetItem;
    item->setData(0, Qt::UserRole, QString::fromStdString(point.getConfig().toJSON()));
    item->setText(0, GeoPointToString(point));
    item->setText(1, QString::number(elevation,'f',1));
    item->setData(1, Qt::UserRole, qVariantFromValue(elevation));
    item->setText(2, QString::number(resolution,'f',1));
    item->setData(2, Qt::UserRole, qVariantFromValue(resolution));
    item->setText(3, QString("%1 ms").arg(time));
    item->setData(3, Qt::UserRole, qVariantFromValue((unsigned)time));
    item->setText(4, resultTimestamp.toString(Qt::ISODate));
    item->setData(4, Qt::UserRole, qVariantFromValue(resultTimestamp));
    item->setText(5, (web)?QString("Web"):QString("osgEarth"));
    item->setData(5, Qt::UserRole, qVariantFromValue(web));
    parent->addChild(item);
    return item;
}

bool RetrieveElevationDialog::getQueryPoint(osgEarth::GeoPoint & point)
{
    bool ret = false;
    ElevationQueryReferenced * queryRef = getElevationQuery(_item.get());
    ElevationQueryAccess * query = queryRef?(ElevationQueryAccess *)queryRef->get():NULL;
    if(query)
    {
        const osgEarth::MapInfo& mapInfo = query->getMapInfo();
        if(mapInfo.getProfile())
        {
            bool ok = false;
			CoordinateResult coordResult = coordinateFromString(ui->coordinate, mapInfo.getProfile(), 0, &ok);
            if(ok)
            {
				if (coordResult.geoPoint.isValid())
				{
					point = coordResult.geoPoint;
					ret = true;
				}
            }
        }
    }
    return ret;
}

void RetrieveElevationDialog::query()
{
    osgEarth::GeoPoint point;
    if(getQueryPoint(point))
    {
        ElevationQueryReferenced * queryRef = getElevationQuery(_item.get());
        ElevationQueryAccess * query = queryRef?(ElevationQueryAccess *)queryRef->get():NULL;
        
        qint64 start = QDateTime::currentMSecsSinceEpoch();

        double elev = 0;
        double desired_resolution = 0;
        double resolution = 0;
        query->getElevation(point, elev, desired_resolution, &resolution);
        qint64 end = QDateTime::currentMSecsSinceEpoch();
        qint64 diff = end - start;

        addResult(point, elev, resolution, diff);
    }
}

void RetrieveElevationDialog::webQuery()
{
    osgEarth::GeoPoint point;
    if(getQueryPoint(point))
    {
        QString url = QString("http://maps.googleapis.com/maps/api/elevation/xml?locations=%1,%2").arg(point.y()).arg(point.x());
        qint64 start = QDateTime::currentMSecsSinceEpoch();

        double elevation = 0;
        double resolution = 0;
        if(elevationWebQuery(url.toStdString(), elevation, resolution))
        {
            qint64 end = QDateTime::currentMSecsSinceEpoch();
            qint64 diff = end - start;

            addResult(point, elevation, resolution, diff, QDateTime(), true);
        }
    }
}

void RetrieveElevationDialog::layerChanged()
{
}

void RetrieveElevationDialog::resultItemContextMenu(const QPoint & pt)
{
    QTreeWidgetItem * item = ui->results->itemAt(pt);
    if(item)
    {
    }
}

void RetrieveElevationDialog::resultItemClicked(QTreeWidgetItem * item, int column)
{
    if(item)
    {
        QString posJSON = item->data(0, Qt::UserRole).toString();
        osgEarth::Config config;
        config.fromJSON(posJSON.toStdString());
        osgEarth::GeoPoint point(config);

        ui->coordinate->setText(GeoPointToString(point));
    }
}

void RetrieveElevationDialog::closeEvent(QCloseEvent * event)
{
    saveResults();
    QDialog::closeEvent(event);
}

void RetrieveElevationDialog::loadResults()
{
    QDir h = QDir::home();
    QSettings ini(h.absoluteFilePath(".local/sgi_osgearth.conf"), QSettings::IniFormat);
    ui->results->clear();
    int count = ini.beginReadArray("RetrieveElevation");
    for(int n = 0; n < count; n++)
    {
        ini.setArrayIndex(n);
        QString posJSON = ini.value("Position").toString();
        osgEarth::Config config;
        config.fromJSON(posJSON.toStdString());
        osgEarth::GeoPoint point(config);
        double elevation = ini.value("Elevation").toFloat();
        double resolution = ini.value("Resolution").toFloat();
        unsigned time = ini.value("Time").toUInt();
        QDateTime resultTimestamp = ini.value("ResultTimestamp").toDateTime();
        bool web = ini.value("Web").toBool();
        if(point.isValid())
            addResult(point, elevation, resolution, time, resultTimestamp, web);
    }
    ini.endArray();
}

void RetrieveElevationDialog::saveResults()
{
    QDir h = QDir::home();
    QSettings ini(h.absoluteFilePath(".local/sgi_osgearth.conf"), QSettings::IniFormat);
    QTreeWidgetItem * parent = ui->results->invisibleRootItem();

    ini.beginWriteArray("RetrieveElevation");
    for(int i = 0; i < parent->childCount(); ++i)
    {
        ini.setArrayIndex(i);
        QTreeWidgetItem * item = parent->child(i);
        QString pos = item->data(0, Qt::UserRole).toString();
        double elevation = item->data(1, Qt::UserRole).toDouble();
        double resolution = item->data(2, Qt::UserRole).toDouble();
        unsigned time = item->data(3, Qt::UserRole).toUInt();
        QDateTime resultTimestamp = item->data(4, Qt::UserRole).toDateTime();
        bool web = item->data(5, Qt::UserRole).toBool();

        ini.setValue("Position", pos);
        ini.setValue("Elevation", elevation);
        ini.setValue("Resolution", resolution);
        ini.setValue("Time", time);
        ini.setValue("ResultTimestamp", resultTimestamp);
        ini.setValue("Web", web);

    }
    ini.endArray();
}

} // namespace osgearth_plugin
} // namespace sgi
