#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QToolBar>
#include <QScrollBar>
#include <QGLWidget>
#include <QImageWriter>
#include <QImageReader>

#include <sgi/plugins/SGISettingsDialogImpl>

#include <sgi/SGIItemInternal>
#include <sgi/plugins/ImagePreviewDialogBase>
#include "ui_ImagePreviewDialogBase.h"
#include <__/__/include/sgi/plugins/ImagePreviewDialogBase.moc>

#include <sgi/helpers/qt>

namespace sgi {

ImagePreviewDialogBase::ImagePreviewDialogBase(QWidget * parent, SGIItemBase * item)
    : QDialog(parent)
    , _item(item)
    , _interface(new SettingsDialogImpl(this))
    , _scaleFactor(1.0)
    , _initialRefresh(true)
{
    init();
}

void ImagePreviewDialogBase::init()
{
    ui = new Ui_ImagePreviewDialogBase;
    ui->setupUi( this );

    //connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));

    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

    createToolbar();
}

ImagePreviewDialogBase::~ImagePreviewDialogBase()
{
}

void ImagePreviewDialogBase::showEvent(QShowEvent * event)
{
    if(_initialRefresh)
    {
        _initialRefresh = false;
        refresh();
    }
    QDialog::showEvent(event);
}

void ImagePreviewDialogBase::setLabel(const QString & label)
{
    _labelText = label;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, QImage::Format t)
{
    switch(t)
    {
    case QImage::Format_Invalid: os << "Invalid"; break;
    case QImage::Format_Mono: os << "Mono"; break;
    case QImage::Format_MonoLSB: os << "MonoLSB"; break;
    case QImage::Format_Indexed8: os << "Indexed8"; break;
    case QImage::Format_RGB32: os << "RGB32"; break;
    case QImage::Format_ARGB32: os << "ARGB32"; break;
    case QImage::Format_ARGB32_Premultiplied: os << "ARGB32_Premultiplied"; break;
    case QImage::Format_RGB16: os << "RGB16"; break;
    case QImage::Format_ARGB8565_Premultiplied: os << "ARGB8565_Premultiplied"; break;
    case QImage::Format_RGB666: os << "RGB666"; break;
    case QImage::Format_ARGB6666_Premultiplied: os << "ARGB6666_Premultiplied"; break;
    case QImage::Format_RGB555: os << "RGB555"; break;
    case QImage::Format_ARGB8555_Premultiplied: os << "ARGB8555_Premultiplied"; break;
    case QImage::Format_RGB888: os << "RGB888"; break;
    case QImage::Format_RGB444: os << "RGB444"; break;
    case QImage::Format_ARGB4444_Premultiplied: os << "ARGB4444_Premultiplied"; break;
    default: os << (int)t; break;
    }
    return os;
}

void ImagePreviewDialogBase::refresh()
{
    refreshImpl();
}

void ImagePreviewDialogBase::refreshImpl()
{
    updateImageAndLabel();
}

void ImagePreviewDialogBase::setImageInfo(const QString & name, const QString & infoText)
{
    if(_labelText.isEmpty())
        ui->label->setText(infoText);
    else
        ui->label->setText(_labelText + QString("\r\n") + infoText);

    const QPixmap * p = ui->label->pixmap();
    int width = p?p->width():0;
    int height = p?p->height():0;
    ui->scrollArea->horizontalScrollBar()->setMaximum(width);
    ui->scrollArea->verticalScrollBar()->setMaximum(height);
    ui->scrollArea->horizontalScrollBar()->setPageStep((width/10));
    ui->scrollArea->verticalScrollBar()->setMaximum((height/10));
    ui->scrollArea->horizontalScrollBar()->setValue(0);
    ui->scrollArea->verticalScrollBar()->setValue(0);

    _fitToWindowAction->setEnabled(true);

    if (_fitToWindowAction->isChecked())
        fitToWindow();
    else
        normalSize();

    updateToolbar();
    setWindowTitle(tr("Image Viewer - %1").arg(name));
}

void ImagePreviewDialogBase::setImage(const QImage & image, const QString & name, const QString & infoText)
{
    ui->imageLabel->setText(QString());
    if(!image.isNull())
        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
    else
        ui->imageLabel->setPixmap(QPixmap());

    std::stringstream ss;
    if(!infoText.isEmpty())
        ss << infoText + QString("\r\n");
    if(!image.isNull())
    {
        ss << "QImage " << image.width() << "x" << image.height() << "x" << image.depth();
        ss << " [format=" << image.format() << ";colors=" << image.colorCount() << ";bitPlaneCount=" << image.bitPlaneCount() << "]";
    }
    else
        ss << "QImage NULL";

    QString imageInfo = QString::fromStdString(ss.str());
    QString imageName = name;
    if(imageName.isEmpty())
    {
        ss.clear();
        ss << (void*)&image;
        imageName = QString::fromStdString(ss.str());
    }
    setImageInfo(imageName, imageInfo);
}

void ImagePreviewDialogBase::setImage(const QPixmap & pixmap, const QString & name, const QString & infoText)
{
    ui->imageLabel->setText(QString());
    if(pixmap.isNull())
        ui->imageLabel->setPixmap(pixmap);
    else
        ui->imageLabel->setPixmap(QPixmap());

    std::stringstream ss;
    if(!infoText.isEmpty())
        ss << infoText + QString("\r\n");
    if(!pixmap.isNull())
    {
        ss << "QPixmap " << pixmap.width() << "x" << pixmap.height() << "x" << pixmap.depth();
        ss << " [devtype=" << pixmap.devType() << ";alpha=" << pixmap.hasAlpha() << "]";
    }
    else
        ss << "QPixmap NULL";

    QString imageInfo = QString::fromStdString(ss.str());
    QString imageName = name;
    if(imageName.isEmpty())
    {
        ss.clear();
        ss << (void*)&pixmap;
        imageName = QString::fromStdString(ss.str());
    }
    setImageInfo(imageName, imageInfo);
}

//! [9]
void ImagePreviewDialogBase::zoomIn()
//! [9] //! [10]
{
    scaleImage(1.25);
}

void ImagePreviewDialogBase::zoomOut()
{
    scaleImage(0.8);
}

//! [10] //! [11]
void ImagePreviewDialogBase::normalSize()
//! [11] //! [12]
{
    ui->imageLabel->adjustSize();
    _scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImagePreviewDialogBase::fitToWindow()
//! [13] //! [14]
{
    bool fitToWindow = _fitToWindowAction->isChecked();
    ui->scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateToolbar();
}
//! [14]

//! [19]
void ImagePreviewDialogBase::createToolbar()
//! [19] //! [20]
{
    _toolBar = new QToolBar;
    QVBoxLayout * mainLayout = (QVBoxLayout *)this->layout();
    mainLayout->insertWidget(0, _toolBar);

    _refreshAction = new QAction(tr("&Refresh"), this);
    _refreshAction->setIcon(QIcon::fromTheme("view-refresh"));
    _refreshAction->setShortcut(tr("F5"));
    _refreshAction->setEnabled(_item.valid());
    connect(_refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));

    _saveAction = new QAction(tr("&Save..."), this);
    _saveAction->setIcon(QIcon::fromTheme("document-save"));
    _saveAction->setShortcut(tr("Ctrl+S"));
    _saveAction->setEnabled(_item.valid());
    connect(_saveAction, SIGNAL(triggered()), this, SLOT(save()));

    _zoomInAction = new QAction(tr("Zoom &In (25%)"), this);
    _zoomInAction->setIcon(QIcon::fromTheme("zoom-in"));
    _zoomInAction->setShortcut(tr("Ctrl++"));
    _zoomInAction->setEnabled(false);
    connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    _zoomOutAction = new QAction(tr("Zoom &Out (25%)"), this);
    _zoomOutAction->setIcon(QIcon::fromTheme("zoom-out"));
    _zoomOutAction->setShortcut(tr("Ctrl+-"));
    _zoomOutAction->setEnabled(false);
    connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    _normalSizeAction = new QAction(tr("&Normal Size"), this);
    _normalSizeAction->setIcon(QIcon::fromTheme("zoom-original"));
    _normalSizeAction->setShortcut(tr("Ctrl+O"));
    _normalSizeAction->setEnabled(false);
    connect(_normalSizeAction, SIGNAL(triggered()), this, SLOT(normalSize()));

    _fitToWindowAction = new QAction(tr("&Fit to Window"), this);
    _fitToWindowAction->setIcon(QIcon::fromTheme("zoom-fit-best"));
    _fitToWindowAction->setEnabled(false);
    _fitToWindowAction->setCheckable(true);
    _fitToWindowAction->setShortcut(tr("Ctrl+F"));
    connect(_fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    _toolBar->addAction(_refreshAction);
    _toolBar->addAction(_saveAction);
    _toolBar->addAction(_zoomInAction);
    _toolBar->addAction(_zoomOutAction);
    _toolBar->addAction(_normalSizeAction);
    _toolBar->addAction(_fitToWindowAction);
}
//! [20]

//! [21]
void ImagePreviewDialogBase::updateToolbar()
//! [21] //! [22]
{
    _zoomInAction->setEnabled(!_fitToWindowAction->isChecked());
    _zoomOutAction->setEnabled(!_fitToWindowAction->isChecked());
    _normalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
}
//! [22]

//! [23]
void ImagePreviewDialogBase::scaleImage(double factor)
//! [23] //! [24]
{
    Q_ASSERT(ui->imageLabel->pixmap());
    _scaleFactor *= factor;
    ui->imageLabel->resize(_scaleFactor * ui->imageLabel->pixmap()->size());

    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);

    _zoomInAction->setEnabled(_scaleFactor < 3.0);
    _zoomOutAction->setEnabled(_scaleFactor > 0.333);
}
//! [24]

//! [25]
void ImagePreviewDialogBase::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
//! [26]

bool ImagePreviewDialogBase::openImpl(const QString & filename)
{
    bool ret = false;
    QImageReader reader(filename);
    QImage image = reader.read();
    ret = !image.isNull();
    if(ret)
        setImage(image);
    return ret;
}

void ImagePreviewDialogBase::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open image file"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        openImpl(fileName);
    }
}

bool ImagePreviewDialogBase::saveImpl(const QString & filename)
{
    bool ret = false;
    const QPixmap * pixmap = ui->imageLabel->pixmap();
    if(pixmap)
    {
        QImageWriter writer(filename);
        ret = writer.write(pixmap->toImage());
    }
    return ret;
}

void ImagePreviewDialogBase::save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                    tr("Save image file"), QDir::currentPath());
    if (!fileName.isEmpty())
    {
        saveImpl(fileName);
    }
}

} // namespace sgi
