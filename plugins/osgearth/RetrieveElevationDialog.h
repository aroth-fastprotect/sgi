#pragma once

#include <QDateTime>
#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>

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

class RetrieveElevationDialog : public QDialog
{
	Q_OBJECT

public:
                                RetrieveElevationDialog(QWidget * parent, SGIItemBase * item, ISettingsDialogInfo * info=NULL);
	virtual				        ~RetrieveElevationDialog();

public:
    ISettingsDialog *           dialogInterface() { return _interface; }

public slots:
    void                        query();
    void                        webQuery();
    void                        layerChanged();
    void                        resultItemContextMenu(const QPoint & pt);
    void                        resultItemClicked(QTreeWidgetItem * item, int column);

protected:
    virtual void                closeEvent(QCloseEvent * event);

private:
    bool                        getQueryPoint(osgEarth::GeoPoint & point);
    QTreeWidgetItem *           addResult(const osgEarth::GeoPoint & point, double elevation, double resolution, qint64 time, const QDateTime & timestamp=QDateTime(), bool web=false);

    void                        loadResults();
    void                        saveResults();

private:
	Ui_RetrieveElevationDialog *    ui;
    SGIPluginHostInterface *        _hostInterface;
    ISettingsDialogPtr _interface;
    ISettingsDialogInfoPtr          _info;
    SGIItemBasePtr                  _item;
};

} // namespace osgearth_plugin
} // namespace sgi
