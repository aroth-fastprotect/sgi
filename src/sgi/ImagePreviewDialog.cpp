#include "stdafx.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QToolBar>
#include <QScrollBar>
#include <QComboBox>
#include <QDesktopWidget>
#include <QImageWriter>
#include <QImageReader>

#include "ImagePreviewDialog.moc"
#include "ImagePreviewDialog.h"
#include "ui_ImagePreviewDialog.h"
#include "SGIPlugin.h"
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/helpers/qt>


namespace sgi {

class ImagePreviewDialog::Histogram
{
public:
	typedef std::vector<int> ColorChannel;
	Histogram()
	{
		minAlpha = INT_MAX; maxAlpha = INT_MIN;
		minRed = INT_MAX; maxRed = INT_MIN;
		minGreen = INT_MAX; maxGreen = INT_MIN;
		minBlue = INT_MAX; maxBlue = INT_MIN;
		minGray = INT_MAX; maxGray = INT_MIN;
		avgAlpha = 0;
		avgRed = 0;
		avgGreen = 0;
		avgBlue = 0;
		avgGray = 0;
        numTransparentPixels = 0;
	}

	void calculate(const QImage & image);

private:
	ColorChannel _alpha;
	ColorChannel _red;
	ColorChannel _green;
	ColorChannel _blue;
	ColorChannel _gray;

public:
	int minAlpha, maxAlpha;
	int minRed, maxRed;
	int minGreen, maxGreen;
	int minBlue, maxBlue;
	int minGray, maxGray;
    unsigned numTransparentPixels;
	float avgAlpha;
	float avgRed;
	float avgGreen;
	float avgBlue;
	float avgGray;
	float avgLuma;
};

namespace {
	inline void calcMinMax(int & minIdx, int & maxIdx, int & minVal, int & maxVal, unsigned index, const std::vector<int> & data)
	{
		const int & v = data[index];
		if (v < minVal)
		{
			minVal = v;
			minIdx = index;
		}
		if (v > maxVal)
		{
			maxVal = v;
			maxIdx = index;
		}
	}
}
void ImagePreviewDialog::Histogram::calculate(const QImage & image)
{
    if(image.isNull())
        return;
	_alpha = ColorChannel(256);
	_red = ColorChannel(256);
	_green = ColorChannel(256);
	_blue = ColorChannel(256);
	_gray = ColorChannel(256);

	unsigned totalAlpha = 0;
	unsigned totalRed = 0;
	unsigned totalGreen = 0;
	unsigned totalBlue = 0;
	unsigned totalGray = 0;
	unsigned totalPixels = image.width() * image.height();
    numTransparentPixels = 0;
	double totalLuma = 0;
	for (int y = 0; y < image.height(); y++) {
		for (int x = 0; x < image.width(); x++) {
			QRgb value = image.pixel(x, y);
			
			int valueAlpha = qAlpha(value) & 0xff;
			int valueRed = qRed(value) & 0xff;
			int valueGreen = qGreen(value) & 0xff;
			int valueBlue = qBlue(value) & 0xff;
			int valueGray = qGray(value) & 0xff;

			totalAlpha += valueAlpha;
			totalRed += valueRed;
			totalGreen += valueGreen;
			totalBlue += valueBlue;
			totalGray += valueGray;

			double luma = 0.2126f * valueRed + 0.7152 * valueGreen + 0.0722 * valueBlue;
			totalLuma += luma;

            if(totalAlpha < 255)
                ++numTransparentPixels;

			++_alpha[valueAlpha];
			++_red[valueRed];
			++_green[valueGreen];
			++_blue[valueBlue];
			++_gray[valueGray];
		}
	}

	avgAlpha = (float)totalAlpha / (float)totalPixels;
	avgRed = (float)totalRed / (float)totalPixels;
	avgGreen = (float)totalGreen / (float)totalPixels;
	avgBlue = (float)totalBlue / (float)totalPixels;
	avgGray = (float)totalGray / (float)totalPixels;
	avgLuma = (float)(totalLuma / (double)totalPixels);

	int minAlphaValue = INT_MAX, maxAlphaValue = INT_MIN;
	int minRedValue = INT_MAX, maxRedValue = INT_MIN;
	int minGreenValue = INT_MAX, maxGreenValue = INT_MIN;
	int minBlueValue = INT_MAX, maxBlueValue = INT_MIN;
	int minGrayValue = INT_MAX, maxGrayValue = INT_MIN;

	minAlpha = INT_MAX; maxAlpha = INT_MIN;
	minRed = INT_MAX; maxRed = INT_MIN;
	minGreen = INT_MAX; maxGreen = INT_MIN;
	minBlue = INT_MAX; maxBlue = INT_MIN;
	minGray = INT_MAX; maxGray = INT_MIN;

	for (unsigned i = 0; i < 256; i++)
	{
		calcMinMax(minAlpha, maxAlpha, minAlphaValue, maxAlphaValue, i, _alpha);
		calcMinMax(minRed, maxRed, minRedValue, maxRedValue, i, _red);
		calcMinMax(minGreen, maxGreen, minGreenValue, maxGreenValue, i, _green);
		calcMinMax(minBlue, maxBlue, minBlueValue, maxBlueValue, i, _blue);
		calcMinMax(minGray, maxGray, minGrayValue, maxGrayValue, i, _gray);
	}
};

class ImagePreviewDialog::ImagePreviewDialogImpl : public QObject, public IImagePreviewDialog
{
public:
	ImagePreviewDialogImpl(ImagePreviewDialog * dialog_);
	~ImagePreviewDialogImpl();
    void createToolbar();
    void updateToolbar();
    void scaleImage(double factor);
    static void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void setImageInfo(const std::string & infoText);

    Image::ImageFormat currentImageFormat() const;

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
    void imageFormatChanged(int index);

    void applyFlip();
	void setNodeInfo(const SGIItemBase * item);
    void setImageInfo(const Image * image);

    virtual QDialog *       getDialog() { return _dialog; }
    virtual IHostCallback * getHostCallback() { return _dialog->_hostCallback; }
    virtual void            setObject(SGIItemBase * item, IHostCallback * callback=NULL) { _dialog->setObject(item, callback); }
    virtual void            setObject(const SGIHostItemBase * item, IHostCallback * callback=NULL) { _dialog->setObject(item, callback); }
    virtual void            setObject(SGIItemBase * item, const sgi::Image * image, const std::string & description, IHostCallback * callback=NULL)
        { _dialog->setObject(item, image, description, callback); }
    virtual void            setImage(const sgi::Image * image) { _dialog->setImage(image); }
    virtual void            setDescription(const std::string & description) { _dialog->setDescription(description); }
    virtual void            show() { emit _dialog->triggerShow(); }
    virtual void            hide() { emit _dialog->triggerHide(); }
    virtual bool            isVisible() { return _dialog->isVisible(); }
    virtual int             showModal() { return _dialog->exec(); }
    virtual SGIItemBase *   item() const { return _dialog->item(); }

    ImagePreviewDialog *        _dialog;
    Ui_ImagePreviewDialog *     ui;
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
    QComboBox *                     imageFormat;
    QString                         labelText;
    double                          scaleFactor;
    bool                            initialRefresh;
	Histogram						histogram;
	bool							histogramReady;
};

ImagePreviewDialog::ImagePreviewDialogImpl::ImagePreviewDialogImpl(ImagePreviewDialog * dialog_)
    : _dialog(dialog_)
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
    , imageFormat(NULL)
	, labelText()
	, scaleFactor(1.0)
	, initialRefresh(true)
	, histogram()
	, histogramReady(false)
{
	ui = new Ui_ImagePreviewDialog;
	ui->setupUi(_dialog);

	connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, this, &ImagePreviewDialogImpl::save);
	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, _dialog, &ImagePreviewDialog::reject);

	ui->imageLabel->setBackgroundRole(QPalette::Base);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);

	createToolbar();

	int w = ui->splitter->width();
	QList<int> sizes = QList<int>() << (w * 3 / 4) << (w / 4);
	ui->splitter->setSizes(sizes);

	ui->tabWidget->setCurrentIndex(0);
}

ImagePreviewDialog::ImagePreviewDialogImpl::~ImagePreviewDialogImpl()
{
	delete ui;
    delete _dialog;
}

void ImagePreviewDialog::ImagePreviewDialogImpl::refresh()
{
	setNodeInfo(_dialog->_item.get());
    setImageInfo(_dialog->_image.get());
	_dialog->refreshImpl();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::setImageInfo(const std::string & infoText)
{
	if (labelText.isEmpty())
		ui->labelImage->setText(qt_helpers::fromLocal8Bit(infoText));
	else
		ui->labelImage->setText(labelText + QString("\r\n") + qt_helpers::fromLocal8Bit(infoText));

	const QPixmap * p = ui->labelImage->pixmap();
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
}

//! [9]
void ImagePreviewDialog::ImagePreviewDialogImpl::zoomIn()
//! [9] //! [10]
{
	scaleImage(1.25);
}

void ImagePreviewDialog::ImagePreviewDialogImpl::zoomOut()
{
	scaleImage(0.8);
}

//! [10] //! [11]
void ImagePreviewDialog::ImagePreviewDialogImpl::normalSize()
//! [11] //! [12]
{
	ui->imageLabel->adjustSize();
	scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImagePreviewDialog::ImagePreviewDialogImpl::fitToWindow()
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
void ImagePreviewDialog::ImagePreviewDialogImpl::createToolbar()
//! [19] //! [20]
{
	toolBar = new QToolBar;
	QVBoxLayout * mainLayout = (QVBoxLayout *)_dialog->layout();
	mainLayout->insertWidget(0, toolBar);

	refreshAction = new QAction(tr("&Refresh"), _dialog);
	refreshAction->setIcon(QIcon::fromTheme("view-refresh"));
	refreshAction->setShortcut(tr("F5"));
	refreshAction->setEnabled(_dialog->_item.valid());
	connect(refreshAction, &QAction::triggered, this, &ImagePreviewDialogImpl::refresh);

	saveAction = new QAction(tr("&Save..."), _dialog);
	saveAction->setIcon(QIcon::fromTheme("document-save"));
	saveAction->setShortcut(tr("Ctrl+S"));
	saveAction->setEnabled(_dialog->_item.valid());
	connect(saveAction, &QAction::triggered, this, &ImagePreviewDialogImpl::save);

	zoomInAction = new QAction(tr("Zoom &In (25%)"), _dialog);
	zoomInAction->setIcon(QIcon::fromTheme("zoom-in"));
	zoomInAction->setShortcut(tr("Ctrl++"));
	zoomInAction->setEnabled(false);
	connect(zoomInAction, &QAction::triggered, this, &ImagePreviewDialogImpl::zoomIn);

	zoomOutAction = new QAction(tr("Zoom &Out (25%)"), _dialog);
	zoomOutAction->setIcon(QIcon::fromTheme("zoom-out"));
	zoomOutAction->setShortcut(tr("Ctrl+-"));
	zoomOutAction->setEnabled(false);
	connect(zoomOutAction, &QAction::triggered, this, &ImagePreviewDialogImpl::zoomOut);

	normalSizeAction = new QAction(tr("&Normal Size"), _dialog);
	normalSizeAction->setIcon(QIcon::fromTheme("zoom-original"));
	normalSizeAction->setShortcut(tr("Ctrl+O"));
	normalSizeAction->setEnabled(false);
	connect(normalSizeAction, &QAction::triggered, this, &ImagePreviewDialogImpl::normalSize);

	fitToWindowAction = new QAction(tr("&Fit to Window"), _dialog);
	fitToWindowAction->setIcon(QIcon::fromTheme("zoom-fit-best"));
	fitToWindowAction->setEnabled(false);
	fitToWindowAction->setCheckable(true);
	fitToWindowAction->setShortcut(tr("Ctrl+F"));
	connect(fitToWindowAction, &QAction::triggered, this, &ImagePreviewDialogImpl::fitToWindow);

	flipHorizontalAction = new QAction(tr("Flip &horizontal"), _dialog);
	flipHorizontalAction->setIcon(QIcon::fromTheme("object-flip-horizontal"));
	flipHorizontalAction->setEnabled(_dialog->_item.valid());
	flipHorizontalAction->setChecked(false);
	flipHorizontalAction->setCheckable(true);
	connect(flipHorizontalAction, &QAction::triggered, this, &ImagePreviewDialogImpl::flipHorizontal);

	flipVerticalAction = new QAction(tr("Flip &vertical"), _dialog);
	flipVerticalAction->setIcon(QIcon::fromTheme("object-flip-vertical"));
	flipVerticalAction->setEnabled(_dialog->_item.valid());
	flipVerticalAction->setChecked(false);
	flipVerticalAction->setCheckable(true);
	connect(flipVerticalAction, &QAction::triggered, this, &ImagePreviewDialogImpl::flipVertical);

    imageFormat = new QComboBox(_dialog);

    imageFormat->addItem(tr("Automatic"), QVariant::fromValue((int)Image::ImageFormatAutomatic));
    imageFormat->addItem(tr("RGB 24-bit"), QVariant::fromValue((int)Image::ImageFormatRGB24));
    imageFormat->addItem(tr("RGB 32-bit"), QVariant::fromValue((int)Image::ImageFormatRGB32));
    imageFormat->addItem(tr("ARGB 32-bit"), QVariant::fromValue((int)Image::ImageFormatARGB32));
    imageFormat->addItem(tr("BGR 24-bit"), QVariant::fromValue((int)Image::ImageFormatBGR24));
    imageFormat->addItem(tr("BGR 32-bit"), QVariant::fromValue((int)Image::ImageFormatBGR32));
    imageFormat->addItem(tr("ABGR 32-bit"), QVariant::fromValue((int)Image::ImageFormatABGR32));
    imageFormat->addItem(tr("YUV 4:2:0"), QVariant::fromValue((int)Image::ImageFormatYUV420));
    imageFormat->addItem(tr("YUV 4:2:2"), QVariant::fromValue((int)Image::ImageFormatYUV422));
    imageFormat->addItem(tr("YUV 4:4:4"), QVariant::fromValue((int)Image::ImageFormatYUV444));
    connect(imageFormat, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::imageFormatChanged);

    toolBar->addWidget(imageFormat);
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
void ImagePreviewDialog::ImagePreviewDialogImpl::updateToolbar()
//! [21] //! [22]
{
	zoomInAction->setEnabled(!fitToWindowAction->isChecked());
	zoomOutAction->setEnabled(!fitToWindowAction->isChecked());
	normalSizeAction->setEnabled(!fitToWindowAction->isChecked());
}
//! [22]

//! [23]
void ImagePreviewDialog::ImagePreviewDialogImpl::scaleImage(double factor)
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
void ImagePreviewDialog::ImagePreviewDialogImpl::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
	scrollBar->setValue(int(factor * scrollBar->value()
		+ ((factor - 1) * scrollBar->pageStep() / 2)));
}
//! [26]

void ImagePreviewDialog::ImagePreviewDialogImpl::applyFlip()
{
	const QPixmap * pixmap = ui->imageLabel->pixmap();
	if (pixmap)
	{
		QImage mirroredImage = originalImage.mirrored(flipHorizontalAction->isChecked(), flipVerticalAction->isChecked());
		ui->imageLabel->setPixmap(QPixmap::fromImage(mirroredImage));
	}
}

void ImagePreviewDialog::ImagePreviewDialogImpl::flipHorizontal()
{
	applyFlip();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::flipVertical()
{
	applyFlip();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::imageFormatChanged(int index)
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::open()
{
	static QString lastDir = QDir::currentPath();
	QString fileName = QFileDialog::getOpenFileName(_dialog,
		tr("Open image file"), lastDir);
	if (!fileName.isEmpty())
	{
		lastDir = fileName;
		_dialog->openImpl(fileName);
	}
}

void ImagePreviewDialog::ImagePreviewDialogImpl::save()
{
	static QString lastDir = QDir::currentPath();
	QString fileName = QFileDialog::getSaveFileName(_dialog,
		tr("Save image file"), lastDir);
	if (!fileName.isEmpty())
	{
		lastDir = fileName;
		_dialog->saveImpl(fileName);
	}
}


void ImagePreviewDialog::ImagePreviewDialogImpl::setNodeInfo(const SGIItemBase * item)
{
	std::ostringstream os;
	if (item)
	{
		_dialog->_hostInterface->writePrettyHTML(os, item);
	}
	else
	{
		os << "<b>item is <i>NULL</i></b>";
	}
	ui->textEditItem->blockSignals(true);
	ui->textEditItem->setHtml(QString::fromStdString(os.str()));
	ui->textEditItem->blockSignals(false);
}

void ImagePreviewDialog::ImagePreviewDialogImpl::setImageInfo(const Image * image)
{
    std::ostringstream os;
    if (image)
    {
        SGIHostItemInternal imageItem(image);
        _dialog->_hostInterface->writePrettyHTML(os, &imageItem);
    }
    else
    {
        os << "<b>image is <i>NULL</i></b>";
    }
    ui->textEditImage->blockSignals(true);
    ui->textEditImage->setHtml(QString::fromStdString(os.str()));
    ui->textEditImage->blockSignals(false);
}

class ImagePreviewDialog::HostCallback : public HostCallbackFilterT<IImagePreviewDialog>
{
public:
   HostCallback(IHostCallback * original, ImagePreviewDialog * dialog)
        : HostCallbackFilterT<IImagePreviewDialog>(original, dialog->_interface) {}
};

ImagePreviewDialog::ImagePreviewDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , _item(NULL)
    , _hostInterface(SGIPlugins::instance()->hostInterface())
    , _priv(new ImagePreviewDialogImpl(this))
    , _interface()
    , _firstShow(true)
{
    init();
}

ImagePreviewDialog::ImagePreviewDialog(SGIItemBase * item, IHostCallback * callback, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , _item(item)
	, _hostInterface(SGIPlugins::instance()->hostInterface())
    , _priv(new ImagePreviewDialogImpl(this))
    , _interface()
    , _firstShow(true)
{
    init();
}

void ImagePreviewDialog::init()
{
    _interface = _priv;
    Qt::WindowFlags flags =this->windowFlags()
        | Qt::WindowMinimizeButtonHint
        | Qt::WindowMaximizeButtonHint
        | Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    connect(this, &ImagePreviewDialog::triggerOnObjectChanged, this, &ImagePreviewDialog::onObjectChanged, Qt::QueuedConnection);
    connect(this, &ImagePreviewDialog::triggerShow, this, &ImagePreviewDialog::showBesideParent, Qt::QueuedConnection);
    connect(this, &ImagePreviewDialog::triggerHide, this, &ImagePreviewDialog::hide, Qt::QueuedConnection);
}

ImagePreviewDialog::~ImagePreviewDialog()
{
}

void ImagePreviewDialog::showEvent(QShowEvent * event)
{
    if(_priv->initialRefresh)
    {
        _priv->initialRefresh = false;
        _priv->refresh();
    }
    QDialog::showEvent(event);
}


void ImagePreviewDialog::showBesideParent()
{
    QWidget::show();
    if(_firstShow)
    {
        _firstShow = false;

        QDesktopWidget * dw = QApplication::desktop();
        QWidget * parent = parentWidget();
        if(parent)
        {
            int numScreens = dw->screenCount();
            int parentScreen = dw->screenNumber(parent);
            int currentScreen = dw->screenNumber(this);

            if(parentScreen == currentScreen)
            {
                int targetScreen = (currentScreen + 1) % numScreens;
                if(targetScreen != currentScreen)
                {
                    QRect geom = frameGeometry();
                    QRect currentScreenRect = dw->screenGeometry(currentScreen);
                    QRect targetScreenRect = dw->screenGeometry(targetScreen);
                    QPoint currentTopLeft = parent->mapToGlobal(geom.topLeft());
                    QPoint currentBottomRight = parent->mapToGlobal(geom.bottomRight());
                    QPoint screenOffset = currentTopLeft - currentScreenRect.topLeft();
                    QPoint targetTopLeft = targetScreenRect.topLeft() + screenOffset;
                    QPoint targetBottomRight(targetTopLeft.x() + geom.width(), targetTopLeft.y() + geom.height());
                    if (targetScreenRect.contains(targetTopLeft))
                    {
                        targetTopLeft = parent->mapFromGlobal(targetTopLeft);
                        move(targetTopLeft);
                    }
                }
            }
        }
    }
}

void ImagePreviewDialog::onObjectChanged()
{
    refreshImpl();
}

void ImagePreviewDialog::setLabel(const QString & label)
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


Image::ImageFormat ImagePreviewDialog::ImagePreviewDialogImpl::currentImageFormat() const
{
    int index = imageFormat->currentIndex();
    Image::ImageFormat ret = (Image::ImageFormat)imageFormat->itemData(index, Qt::UserRole).toInt();
    return ret;
}

bool convertImageToQImage_RGB24(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB888);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatRGB24:
        memcpy(dest, src, image->width() * image->height() * 3);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 3;
            dest += 3;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_BGR24(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB888);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatBGR24:
        memcpy(dest, src, image->width() * image->height() * 3);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 3;
            dest += 3;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_RGB32(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB32);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatRGB32:
        memcpy(dest, src, image->width() * image->height() * 4);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_BGR32(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB32);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatBGR32:
        memcpy(dest, src, image->width() * image->height() * 4);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

QImage convertImageToQImage(const sgi::Image * image, Image::ImageFormat destFormat)
{
    bool convertOk = false;
    QImage ret;
    switch (destFormat)
    {
    case Image::ImageFormatInvalid:
        // invalid -> return emtpy QImage
        break;
    case Image::ImageFormatRGB24: 
        convertOk = convertImageToQImage_RGB24(image, ret);
        break;
    case Image::ImageFormatBGR24:
        convertOk = convertImageToQImage_BGR24(image, ret);
        break;
    case Image::ImageFormatRGB32: 
        convertOk = convertImageToQImage_RGB32(image, ret);
        break;
    case Image::ImageFormatBGR32:
        convertOk = convertImageToQImage_BGR32(image, ret);
        break;
    case Image::ImageFormatAutomatic:
        if (image->originalImageQt())
            ret = *image->originalImageQt();
        else
        {
            QImage::Format qt_format = QImage::Format_Invalid;
            switch (image->format())
            {
            case Image::ImageFormatRGB24: qt_format = QImage::Format_RGB888; break;
            case Image::ImageFormatRGB32: qt_format = QImage::Format_RGB32; break;
            case Image::ImageFormatARGB32: qt_format = QImage::Format_ARGB32; break;
            case Image::ImageFormatMono: qt_format = QImage::Format_Mono; break;
            case Image::ImageFormatMonoLSB: qt_format = QImage::Format_MonoLSB; break;
            case Image::ImageFormatIndexed8: qt_format = QImage::Format_Indexed8; break;
            case Image::ImageFormatFloat:
            case Image::ImageFormatInvalid:
            default: qt_format = QImage::Format_Invalid; break;
            }
            if (qt_format != QImage::Format_Invalid)
            {
                ret = QImage((uchar*)image->data(), (int)image->width(), (int)image->height(), (int)image->bytesPerLine(), qt_format);
                if (image->origin() == Image::OriginBottomLeft)
                    ret.mirrored(false, true);
            }
        }
        break;
    }
    return ret;
}

namespace {
	QTreeWidgetItem * addStatisticsValueImpl(QTreeWidgetItem * root, const QString & name, const QString & value)
	{
		QTreeWidgetItem * ret = new QTreeWidgetItem;
		ret->setText(0, name);
		ret->setText(1, value);
		root->addChild(ret);
		return ret;
	}

	template<typename T>
	inline QTreeWidgetItem * addStatisticsValue(QTreeWidgetItem * root, const QString & name, const T & value)
	{
		return addStatisticsValueImpl(root, name, QString("%1").arg(value));
	}
	template<>
	inline QTreeWidgetItem * addStatisticsValue<QString>(QTreeWidgetItem * root, const QString & name, const QString & value)
	{
		return addStatisticsValueImpl(root, name, value);
	}
}

void ImagePreviewDialog::refreshStatistics(const QImage & image)
{
	_priv->histogram.calculate(image);

	_priv->ui->statistics->clear();
	_priv->ui->statistics->setHeaderLabels(QStringList() << tr("Name") << tr("Value"));

	QTreeWidgetItem * root = _priv->ui->statistics->invisibleRootItem();
    addStatisticsValue(root, tr("Width"), image.width());
    addStatisticsValue(root, tr("Height"), image.height());
    addStatisticsValue(root, tr("Depth"), image.depth());
    addStatisticsValue(root, tr("Device pixel ratio"), image.devicePixelRatio());
    addStatisticsValue(root, tr("Color count"), image.colorCount());
    addStatisticsValue(root, tr("Bit plance count"), image.bitPlaneCount());
    if(image.hasAlphaChannel())
        addStatisticsValue(root, tr("Alpha"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minAlpha).arg(_priv->histogram.maxAlpha).arg(_priv->histogram.avgAlpha));
	addStatisticsValue(root, tr("Red"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minRed).arg(_priv->histogram.maxRed).arg(_priv->histogram.avgRed));
	addStatisticsValue(root, tr("Green"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minGreen).arg(_priv->histogram.maxGreen).arg(_priv->histogram.avgGreen));
	addStatisticsValue(root, tr("Blue"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minBlue).arg(_priv->histogram.maxBlue).arg(_priv->histogram.avgBlue));
	addStatisticsValue(root, tr("Gray"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minGray).arg(_priv->histogram.maxGray).arg(_priv->histogram.avgGray));
	addStatisticsValue(root, tr("Luma"), QString("avg %1").arg(_priv->histogram.avgLuma));
    if(image.hasAlphaChannel())
        addStatisticsValue(root, tr("Transparency"), QString("%1 pixels").arg(_priv->histogram.numTransparentPixels));
    else
        addStatisticsValue(root, tr("Transparency"), tr("N/A"));
}

void ImagePreviewDialog::refreshImpl()
{
    if (_item.valid())
    {
        std::string displayName;
        SGIPlugins::instance()->getObjectDisplayName(displayName, _item);
        setWindowTitle(tr("Image Viewer - %1").arg(qt_helpers::fromLocal8Bit(displayName)));
    }
    else
    {
        setWindowTitle(tr("Image Viewer - No image available"));
    }

    Image::ImageFormat format = _priv->currentImageFormat();
    QImage qimg = convertImageToQImage(_image.get(), format);

	refreshStatistics(qimg);

    _priv->ui->imageLabel->setText(QString());
    _priv->originalImage = qimg;
    if(!qimg.isNull())
        _priv->ui->imageLabel->setPixmap(QPixmap::fromImage(qimg));
    else
        _priv->ui->imageLabel->setPixmap(QPixmap());

    std::stringstream ss;
    ss << "<i>Info for displayed image:</i><br/>\r\n";
    if(!qimg.isNull())
    {
        ss << qimg.width() << "x" << qimg.height() << "x" << qimg.depth() << std::endl;
        ss << "format=" << qimg.format() << " colors=" << qimg.colorCount() << " bitPlaneCount=" << qimg.bitPlaneCount();
    }
    else
        ss << "<b>No image</b>";
    _priv->setImageInfo(ss.str());
}

bool ImagePreviewDialog::openImpl(const QString & filename)
{
    bool ret = false;
    QImageReader reader(filename);
    QImage image = reader.read();
    ret = !image.isNull();
    if(ret)
    {
        //setImage(image);
    }
    return ret;
}


bool ImagePreviewDialog::saveImpl(const QString & filename)
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

SGIItemBase * ImagePreviewDialog::getView()
{
    if(_hostCallback)
        return _hostCallback->getView();
    else
        return NULL;
}

void ImagePreviewDialog::triggerRepaint()
{
    if(_hostCallback)
        _hostCallback->triggerRepaint();
}

void ImagePreviewDialog::setObject(const SGIHostItemBase * hostitem, IHostCallback * callback)
{
    SGIItemBasePtr item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        setObject(item.get(), callback);
}

void ImagePreviewDialog::setObject(SGIItemBase * item, IHostCallback * callback)
{
    _item = item;
    if(callback)
        _hostCallback = callback;
    emit triggerOnObjectChanged();
}

void ImagePreviewDialog::setObject(SGIItemBase * item, const sgi::Image * image, const std::string & description, IHostCallback * callback)
{
    _item = item;
    _image = image;
    _priv->labelText = qt_helpers::fromLocal8Bit(description);
    if(callback)
        _hostCallback = callback;
    emit triggerOnObjectChanged();
}

void ImagePreviewDialog::setImage(const sgi::Image * image)
{
    _image = image;
    emit triggerOnObjectChanged();
}

void ImagePreviewDialog::setDescription(const std::string & description)
{
    _priv->labelText = qt_helpers::fromLocal8Bit(description);
    emit triggerOnObjectChanged();
}

SGIItemBase * ImagePreviewDialog::item() const
{
    return _item.get();
}

} // namespace sgi
