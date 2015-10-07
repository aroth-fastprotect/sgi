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

class ImagePreviewDialogBase::ImagePreviewDialogBasePrivate : public QObject
{
public:
	ImagePreviewDialogBasePrivate(ImagePreviewDialogBase * owner_);
	~ImagePreviewDialogBasePrivate();
    void createToolbar();
    void updateToolbar();
    void scaleImage(double factor);
    static void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void setImageInfo(const QString & name, const QString & infoText);

    // Actions
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void save();
    void open();
    void refresh();
    void flipHorizontal();
    void flipVertical();

    void applyFlip();
	void setNodeInfo(const SGIItemBase * item);


    ImagePreviewDialogBase *        owner;
    Ui_ImagePreviewDialogBase *     ui;
	QImage							originalImage;
    QToolBar *                      toolBar;
    QAction *                       refreshAction;
    QAction *                       saveAction;
    QAction *                       zoomInAction;
    QAction *                       zoomOutAction;
    QAction *                       normalSizeAction;
    QAction *                       fitToWindowAction;
    QAction *                       flipHorizontalAction;
    QAction *                       flipVerticalAction;
    QString                         labelText;
    double                          scaleFactor;
    bool                            initialRefresh;
};

ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::ImagePreviewDialogBasePrivate(ImagePreviewDialogBase * owner_)
	: owner(owner_)
	, ui(NULL)
	, toolBar(NULL)
	, refreshAction(NULL)
	, saveAction(NULL)
	, zoomInAction(NULL)
	, zoomOutAction(NULL)
	, normalSizeAction(NULL)
	, fitToWindowAction(NULL)
	, flipHorizontalAction(NULL)
	, flipVerticalAction(NULL)
	, labelText()
	, scaleFactor(1.0)
	, initialRefresh(true)
{
	ui = new Ui_ImagePreviewDialogBase;
	ui->setupUi(owner);

	connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, this, &ImagePreviewDialogBasePrivate::save);
	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, owner, &ImagePreviewDialogBase::reject);

	ui->imageLabel->setBackgroundRole(QPalette::Base);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);

	createToolbar();

	int w = ui->splitter->width();
	QList<int> sizes = QList<int>() << (w * 3 / 4) << (w / 4);
	ui->splitter->setSizes(sizes);

	ui->tabWidget->setCurrentIndex(0);
}

ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::~ImagePreviewDialogBasePrivate()
{
	delete ui;
}

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::refresh()
{
	setNodeInfo(owner->_item.get());
	owner->refreshImpl();
}

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::setImageInfo(const QString & name, const QString & infoText)
{
	if (labelText.isEmpty())
		ui->label->setText(infoText);
	else
		ui->label->setText(labelText + QString("\r\n") + infoText);

	const QPixmap * p = ui->label->pixmap();
	int width = p ? p->width() : 0;
	int height = p ? p->height() : 0;
	ui->scrollArea->horizontalScrollBar()->setMaximum(width);
	ui->scrollArea->verticalScrollBar()->setMaximum(height);
	ui->scrollArea->horizontalScrollBar()->setPageStep((width / 10));
	ui->scrollArea->verticalScrollBar()->setMaximum((height / 10));
	ui->scrollArea->horizontalScrollBar()->setValue(0);
	ui->scrollArea->verticalScrollBar()->setValue(0);

	fitToWindowAction->setEnabled(true);

	if (fitToWindowAction->isChecked())
		fitToWindow();
	else
		normalSize();

	updateToolbar();
	owner->setWindowTitle(tr("Image Viewer - %1").arg(name));
}

//! [9]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::zoomIn()
//! [9] //! [10]
{
	scaleImage(1.25);
}

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::zoomOut()
{
	scaleImage(0.8);
}

//! [10] //! [11]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::normalSize()
//! [11] //! [12]
{
	ui->imageLabel->adjustSize();
	scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::fitToWindow()
//! [13] //! [14]
{
	bool fitToWindow = fitToWindowAction->isChecked();
	ui->scrollArea->setWidgetResizable(fitToWindow);
	if (!fitToWindow) {
		normalSize();
	}
	updateToolbar();
}
//! [14]

//! [19]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::createToolbar()
//! [19] //! [20]
{
	toolBar = new QToolBar;
	QVBoxLayout * mainLayout = (QVBoxLayout *)owner->layout();
	mainLayout->insertWidget(0, toolBar);

	refreshAction = new QAction(tr("&Refresh"), owner);
	refreshAction->setIcon(QIcon::fromTheme("view-refresh"));
	refreshAction->setShortcut(tr("F5"));
	refreshAction->setEnabled(owner->_item.valid());
	connect(refreshAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::refresh);

	saveAction = new QAction(tr("&Save..."), owner);
	saveAction->setIcon(QIcon::fromTheme("document-save"));
	saveAction->setShortcut(tr("Ctrl+S"));
	saveAction->setEnabled(owner->_item.valid());
	connect(saveAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::save);

	zoomInAction = new QAction(tr("Zoom &In (25%)"), owner);
	zoomInAction->setIcon(QIcon::fromTheme("zoom-in"));
	zoomInAction->setShortcut(tr("Ctrl++"));
	zoomInAction->setEnabled(false);
	connect(zoomInAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::zoomIn);

	zoomOutAction = new QAction(tr("Zoom &Out (25%)"), owner);
	zoomOutAction->setIcon(QIcon::fromTheme("zoom-out"));
	zoomOutAction->setShortcut(tr("Ctrl+-"));
	zoomOutAction->setEnabled(false);
	connect(zoomOutAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::zoomOut);

	normalSizeAction = new QAction(tr("&Normal Size"), owner);
	normalSizeAction->setIcon(QIcon::fromTheme("zoom-original"));
	normalSizeAction->setShortcut(tr("Ctrl+O"));
	normalSizeAction->setEnabled(false);
	connect(normalSizeAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::normalSize);

	fitToWindowAction = new QAction(tr("&Fit to Window"), owner);
	fitToWindowAction->setIcon(QIcon::fromTheme("zoom-fit-best"));
	fitToWindowAction->setEnabled(false);
	fitToWindowAction->setCheckable(true);
	fitToWindowAction->setShortcut(tr("Ctrl+F"));
	connect(fitToWindowAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::fitToWindow);

	flipHorizontalAction = new QAction(tr("Flip &horizontal"), owner);
	flipHorizontalAction->setIcon(QIcon::fromTheme("object-flip-horizontal"));
	flipHorizontalAction->setEnabled(owner->_item.valid());
	flipHorizontalAction->setChecked(false);
	flipHorizontalAction->setCheckable(true);
	connect(flipHorizontalAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::flipHorizontal);

	flipVerticalAction = new QAction(tr("Flip &vertical"), owner);
	flipVerticalAction->setIcon(QIcon::fromTheme("object-flip-vertical"));
	flipVerticalAction->setEnabled(owner->_item.valid());
	flipVerticalAction->setChecked(false);
	flipVerticalAction->setCheckable(true);
	connect(flipVerticalAction, &QAction::triggered, this, &ImagePreviewDialogBasePrivate::flipVertical);

	toolBar->addAction(refreshAction);
	toolBar->addAction(saveAction);
	toolBar->addAction(zoomInAction);
	toolBar->addAction(zoomOutAction);
	toolBar->addAction(normalSizeAction);
	toolBar->addAction(fitToWindowAction);
	toolBar->addAction(flipHorizontalAction);
	toolBar->addAction(flipVerticalAction);
}
//! [20]

//! [21]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::updateToolbar()
//! [21] //! [22]
{
	zoomInAction->setEnabled(!fitToWindowAction->isChecked());
	zoomOutAction->setEnabled(!fitToWindowAction->isChecked());
	normalSizeAction->setEnabled(!fitToWindowAction->isChecked());
}
//! [22]

//! [23]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::scaleImage(double factor)
//! [23] //! [24]
{
	Q_ASSERT(ui->imageLabel->pixmap());
	scaleFactor *= factor;
	ui->imageLabel->resize(scaleFactor * ui->imageLabel->pixmap()->size());

	adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);

	zoomInAction->setEnabled(scaleFactor < 3.0);
	zoomOutAction->setEnabled(scaleFactor > 0.333);
}
//! [24]

//! [25]
void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
	scrollBar->setValue(int(factor * scrollBar->value()
		+ ((factor - 1) * scrollBar->pageStep() / 2)));
}
//! [26]

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::applyFlip()
{
	const QPixmap * pixmap = ui->imageLabel->pixmap();
	if (pixmap)
	{
		QImage mirroredImage = originalImage.mirrored(flipHorizontalAction->isChecked(), flipVerticalAction->isChecked());
		ui->imageLabel->setPixmap(QPixmap::fromImage(mirroredImage));
	}
}

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::flipHorizontal()
{
	applyFlip();
}

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::flipVertical()
{
	applyFlip();
}


void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::open()
{
	static QString lastDir = QDir::currentPath();
	QString fileName = QFileDialog::getOpenFileName(owner,
		tr("Open image file"), lastDir);
	if (!fileName.isEmpty())
	{
		lastDir = fileName;
		owner->openImpl(fileName);
	}
}

void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::save()
{
	static QString lastDir = QDir::currentPath();
	QString fileName = QFileDialog::getSaveFileName(owner,
		tr("Save image file"), lastDir);
	if (!fileName.isEmpty())
	{
		lastDir = fileName;
		owner->saveImpl(fileName);
	}
}


void ImagePreviewDialogBase::ImagePreviewDialogBasePrivate::setNodeInfo(const SGIItemBase * item)
{
	std::ostringstream os;
	if (item)
	{
		owner->_hostInterface->writePrettyHTML(os, item);
	}
	else
	{
		os << "<b>item is <i>NULL</i></b>";
	}
	ui->textEdit->blockSignals(true);
	ui->textEdit->setHtml(QString::fromStdString(os.str()));
	ui->textEdit->blockSignals(false);
}

ImagePreviewDialogBase::ImagePreviewDialogBase(SGIPluginHostInterface * hostInterface, SGIItemBase * item, QWidget * parent)
    : QDialog(parent)
    , _item(item)
	, _hostInterface(hostInterface)
    , _interface(new SettingsDialogImpl(this))
    , _priv(new ImagePreviewDialogBasePrivate(this))
{
}

ImagePreviewDialogBase::~ImagePreviewDialogBase()
{
	delete _priv;
}

void ImagePreviewDialogBase::showEvent(QShowEvent * event)
{
    if(_priv->initialRefresh)
    {
        _priv->initialRefresh = false;
        _priv->refresh();
    }
    QDialog::showEvent(event);
}

void ImagePreviewDialogBase::setLabel(const QString & label)
{
    _priv->labelText = label;
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

void ImagePreviewDialogBase::refreshImpl()
{
    updateImageAndLabel();
}


void ImagePreviewDialogBase::setImage(const QImage & image, const QString & name, const QString & infoText)
{
    _priv->ui->imageLabel->setText(QString());
	_priv->originalImage = image;
    if(!image.isNull())
        _priv->ui->imageLabel->setPixmap(QPixmap::fromImage(image));
    else
        _priv->ui->imageLabel->setPixmap(QPixmap());

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
    _priv->setImageInfo(imageName, imageInfo);
}

void ImagePreviewDialogBase::setImage(const QPixmap & pixmap, const QString & name, const QString & infoText)
{
    _priv->ui->imageLabel->setText(QString());
    if(pixmap.isNull())
        _priv->ui->imageLabel->setPixmap(pixmap);
    else
        _priv->ui->imageLabel->setPixmap(QPixmap());
	_priv->originalImage = pixmap.toImage();

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
    _priv->setImageInfo(imageName, imageInfo);
}

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


bool ImagePreviewDialogBase::saveImpl(const QString & filename)
{
    bool ret = false;
    const QPixmap * pixmap = _priv->ui->imageLabel->pixmap();
    if(pixmap)
    {
        QImageWriter writer(filename);
        ret = writer.write(pixmap->toImage());
    }
    return ret;
}

} // namespace sgi
