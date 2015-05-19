#include "stdafx.h"
#include "ImagePreviewDialog.h"

#include "ui_ImagePreviewDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QToolBar>
#include <QScrollBar>
#include <QGLWidget>
#include <QImageWriter>

#include <sgi/plugins/SGISettingsDialogImpl>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {

ImagePreviewDialog::ImagePreviewDialog(QWidget * parent, QImage * image)
    : QDialog(parent)
    , _image(image)
    , _pixmap()
    , _interface(new SettingsDialogImpl(this))
    , _scaleFactor(1.0)
{
    init();
}

ImagePreviewDialog::ImagePreviewDialog(QWidget * parent, QPixmap * pixmap)
    : QDialog(parent)
    , _image()
    , _pixmap(pixmap)
    , _interface(new SettingsDialogImpl(this))
    , _scaleFactor(1.0)
{
    init();
}

void ImagePreviewDialog::init()
{
    ui = new Ui_ImagePreviewDialog;
    ui->setupUi( this );

    //connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));

    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

    createToolbar();

    setWindowTitle(tr("Image Viewer"));

    if(_image)
        load(_image.data());
    else if(_pixmap)
        load(_pixmap.data());
}

ImagePreviewDialog::~ImagePreviewDialog()
{
}

void ImagePreviewDialog::setLabel(const QString & label)
{
    _labelText = label;
}

void ImagePreviewDialog::load(const QString & filename)
{
	QImage * img = new QImage(filename);
	if (img->isNull()) 
	{
		QMessageBox::critical(this, windowTitle(), tr("Cannot load %1.").arg(filename));
		delete img;
		return;
	}
	load(img);
}

void ImagePreviewDialog::save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                    tr("Save image file"), QDir::currentPath());
    if (!fileName.isEmpty()) {

        QImageWriter writer(fileName);
        if(_image)
            writer.write(*_image.data());
        else if(_pixmap)
            writer.write(_pixmap->toImage());
    }
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

void ImagePreviewDialog::load(const QImage * img)
{
    ui->imageLabel->setText(QString());
	ui->imageLabel->setPixmap(QPixmap::fromImage(*img));

    std::stringstream ss;
    if(img->isNull())
        ss << "Image is null";
    else
    {
        ss << img->width() << "x" << img->height() << "x" << img->depth();
        ss << " [format=" << img->format() << ";colors=" << img->colorCount() << ";bitPlaneCount=" << img->bitPlaneCount() << "]";
    }

    QString imageInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(imageInfo);
    else
        ui->label->setText(_labelText + QString("; ") + imageInfo);
	_scaleFactor = 1.0;

	_fitToWindowAction->setEnabled(true);
	updateToolbar();

	if (!_fitToWindowAction->isChecked())
		ui->imageLabel->adjustSize();
}

void ImagePreviewDialog::load(const QPixmap * pixmap)
{
    ui->imageLabel->setText(QString());
    ui->imageLabel->setPixmap(*pixmap);

    std::stringstream ss;
    if(pixmap->isNull())
        ss << "Pixmap is null";
    else
    {
        ss << pixmap->width() << "x" << pixmap->height() << "x" << pixmap->depth();
        ss << " [devtype=" << pixmap->devType() << ";alpha=" << pixmap->hasAlpha() << "]";
    }

    QString imageInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(imageInfo);
    else
        ui->label->setText(_labelText + QString("; ") + imageInfo);
    _scaleFactor = 1.0;

    _fitToWindowAction->setEnabled(true);
    updateToolbar();

    if (!_fitToWindowAction->isChecked())
        ui->imageLabel->adjustSize();
}

//! [1]
void ImagePreviewDialog::open()
//! [1] //! [2]
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open image file"), QDir::currentPath());
    if (!fileName.isEmpty()) {
		load(fileName);
    }
}
//! [4]
//! [8]

//! [9]
void ImagePreviewDialog::zoomIn()
//! [9] //! [10]
{
    scaleImage(1.25);
}

void ImagePreviewDialog::zoomOut()
{
    scaleImage(0.8);
}

//! [10] //! [11]
void ImagePreviewDialog::normalSize()
//! [11] //! [12]
{
    ui->imageLabel->adjustSize();
    _scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImagePreviewDialog::fitToWindow()
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
void ImagePreviewDialog::createToolbar()
//! [19] //! [20]
{
    _toolBar = new QToolBar;
    QVBoxLayout * mainLayout = (QVBoxLayout *)this->layout();
    mainLayout->insertWidget(0, _toolBar);

    _saveAction = new QAction(tr("&Save..."), this);
    _saveAction->setIcon(QIcon::fromTheme("document-save"));
    _saveAction->setShortcut(tr("Ctrl+S"));
    _saveAction->setEnabled(false);
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

    _toolBar->addAction(_saveAction);
    _toolBar->addAction(_zoomInAction);
    _toolBar->addAction(_zoomOutAction);
    _toolBar->addAction(_normalSizeAction);
    _toolBar->addAction(_fitToWindowAction);
}
//! [20]

//! [21]
void ImagePreviewDialog::updateToolbar()
//! [21] //! [22]
{
    _zoomInAction->setEnabled(!_fitToWindowAction->isChecked());
    _zoomOutAction->setEnabled(!_fitToWindowAction->isChecked());
    _normalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
}
//! [22]

//! [23]
void ImagePreviewDialog::scaleImage(double factor)
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
void ImagePreviewDialog::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
//! [26]

} // namespace qt_plugin
} // namespace sgi
