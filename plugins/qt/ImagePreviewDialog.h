#pragma once

#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QToolBar;
class QScrollBar;
class Ui_ImagePreviewDialog;
QT_END_NAMESPACE

namespace sgi {
namespace qt_plugin {

class ImagePreviewDialog : public QDialog
{
    Q_OBJECT

public:
	ImagePreviewDialog(QWidget * parent, QImage * image);
    ImagePreviewDialog(QWidget * parent, QPixmap * pixmap);
    virtual ~ImagePreviewDialog();

public:
    ISettingsDialog *   dialogInterface() { return _interface; }

public slots:
    void setLabel(const QString & label);

private slots:
    void open();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
	void load(const QString & filename);
	void load(const QImage * image);
    void load(const QPixmap * pixmap);

private:
    void init();
    void createToolbar();
    void updateToolbar();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

private:
    Ui_ImagePreviewDialog *         ui;
    QSharedPointer<QImage>          _image;
    QSharedPointer<QPixmap>         _pixmap;
    ISettingsDialogPtr              _interface;
    QToolBar *                      _toolBar;
    double                          _scaleFactor;
    QAction *                       _zoomInAction;
    QAction *                       _zoomOutAction;
    QAction *                       _normalSizeAction;
    QAction *                       _fitToWindowAction;
    QString                         _labelText;
};

} // namespace qt_plugin
} // namespace sgi
