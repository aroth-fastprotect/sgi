#pragma once

#include <QDateTime>
#include <sgi/plugins/SGISettingsDialogImpl>

QT_BEGIN_NAMESPACE
class Ui_RetrieveElevationDialog;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace osgEarth {
    class GeoPoint;
}

namespace sgi {
class SGIPluginHostInterface;

namespace osgearth_plugin {

class RetrieveElevationDialog : public SettingsQDialogImpl
{
	Q_OBJECT

public:
    RetrieveElevationDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
    ~RetrieveElevationDialog() override;

public slots:
    void                        query();
    void                        webQuery();
    void                        layerChanged();
    void                        resultItemContextMenu(const QPoint & pt);
    void                        resultItemClicked(QTreeWidgetItem * item, int column);

protected:
    void                        closeEvent(QCloseEvent * event) override;

private:
    bool                        getQueryPoint(osgEarth::GeoPoint & point);
    QTreeWidgetItem *           addResult(const osgEarth::GeoPoint & point, double elevation, double resolution, qint64 time, const QDateTime & timestamp=QDateTime(), bool web=false);

    void                        loadResults();
    void                        saveResults();

private:
	Ui_RetrieveElevationDialog *    ui;
    QString                     _api_key;
};

} // namespace osgearth_plugin
} // namespace sgi
