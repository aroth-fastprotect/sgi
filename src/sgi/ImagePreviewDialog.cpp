#include "stdafx.h"

#include <osg/GL>
#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QToolBar>
#include <QScrollBar>
#include <QComboBox>
#include <QSpinBox>
#include <QDesktopWidget>
#include <QImageWriter>
#include <QImageReader>
#include <QMouseEvent>
#include <QTimer>
#include <QThread>

#include "ImagePreviewDialog.h"
#include "ui_ImagePreviewDialog.h"
#include "SGIPlugin.h"
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/SGIImage.h>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/helpers/qt>
#include <sgi/helpers/osg>

#include "dxt.h"
#include "swscale.h"
#include "colorconversion.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

class ImagePreviewDialog::Histogram
{
public:
	typedef std::vector<int> ColorChannel;
	Histogram()
        : _callback(nullptr)
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

    class HistogramCallback {
    public:
        virtual void histogramComplete() = 0;
    };

    void calculate(const Image * image, HistogramCallback * cb);
private:
    void calculateImpl();

private:
    class Thread;
	ColorChannel _alpha;
	ColorChannel _red;
	ColorChannel _green;
	ColorChannel _blue;
	ColorChannel _gray;

    ConstImagePtr _image;
    HistogramCallback * _callback;
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

class ImagePreviewDialog::Histogram::Thread : public QThread
{
    Histogram * _owner;
public:
    Thread(Histogram * h)
        : _owner(h)
    {
        start();
    }

    void run() override
    {
        std::cout << __FUNCTION__ << std::endl;
        _owner->calculateImpl();
        _owner->_callback->histogramComplete();
        std::cout << __FUNCTION__ << " complete" << std::endl;
        deleteLater();
    }
};

namespace {
	inline void calcMinMax(int & minIdx, int & maxIdx, int & minVal, int & maxVal, unsigned index, const std::vector<int> & data)
	{
		const int & v = data[index];
		if (v < minVal)
		{
			minVal = v;
            minIdx = static_cast<int>(index);
		}
		if (v > maxVal)
		{
			maxVal = v;
            maxIdx = static_cast<int>(index);
		}
	}
}

void ImagePreviewDialog::Histogram::calculate(const Image * image, HistogramCallback * cb)
{
    _image = image;
    _callback = cb;
    Thread * t = new Thread(this);
    t->start();
}

void ImagePreviewDialog::Histogram::calculateImpl()
{
    if(!_image.valid())
        return;
	_alpha = ColorChannel(256);
	_red = ColorChannel(256);
	_green = ColorChannel(256);
	_blue = ColorChannel(256);
	_gray = ColorChannel(256);

    int totalAlpha = 0;
    int totalRed = 0;
    int totalGreen = 0;
    int totalBlue = 0;
    int totalGray = 0;
    int totalPixels = _image->width() * _image->height();

    numTransparentPixels = 0;
    double totalLuma = 0;
    PixelReader reader(_image.get());
    for (unsigned y = 0; y < _image->height(); y++) {
        for (unsigned x = 0; x < _image->width(); x++) {
            Image::Pixel pixel = reader(x, y);

            totalAlpha += pixel.alpha();
            totalRed += pixel.red();
            totalGreen += pixel.green();
            totalBlue += pixel.blue();
            totalGray += pixel.gray();
            totalLuma += pixel.lumaF();

            if(totalAlpha < 255)
                ++numTransparentPixels;

            ++_alpha[static_cast<unsigned>(pixel.alpha())];
            ++_red[static_cast<unsigned>(pixel.red())];
            ++_green[static_cast<unsigned>(pixel.green())];
            ++_blue[static_cast<unsigned>(pixel.blue())];
            ++_gray[static_cast<unsigned>(pixel.gray())];
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
}




class ImagePreviewDialog::ImagePreviewDialogImpl : public QObject
        , public IImagePreviewDialog
        , public Histogram::HistogramCallback
{
public:
    static std::map<Image::ImageFormat, QString> ImageFormatDisplayText;
	ImagePreviewDialogImpl(ImagePreviewDialog * dialog_);
    ~ImagePreviewDialogImpl() override;
    void createToolbar();
    void updateToolbar();
    void scaleImage(double factor);

    Image::ImageFormat currentImageFormat() const;
    ColorFilter::FilterType currentColorFilter() const;

    // Actions
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow(bool enable);
    void saveImageAs();
    void openImageAs();
    void refresh();
    void flipHorizontal();
    void flipVertical();
    void selectBackgroundColor();
    void setColor();
    void imageWidthChanged(int index);
    void imageHeightChanged(int index);
    void imageFormatChanged(int index);
    void selectedColorFilterChanged(int index);
    void refreshTimeChanged(int n);
    void refreshTimerExpired();

	void setNodeInfo(const SGIItemBase * item);
    void setImageInfo(const Image * image);

    virtual QDialog *       getDialog() override { return _dialog; }
    virtual IHostCallback * getHostCallback() override { return _dialog->_hostCallback.get(); }
    virtual void            setObject(SGIItemBase * item, IHostCallback * callback=nullptr) override { _dialog->setObject(item, callback); }
    virtual void            setObject(const SGIHostItemBase * item, IHostCallback * callback=nullptr) override { _dialog->setObject(item, callback); }
    virtual void            setObject(SGIItemBase * item, const sgi::Image * image, const std::string & description, IHostCallback * callback=nullptr) override
        { _dialog->setObject(item, image, description, callback); }
    virtual void            setImage(const sgi::Image * image) override { _dialog->setImage(image); }
    virtual void            setDescription(const std::string & description) override { _dialog->setDescription(description); }
    virtual void            show() override { emit _dialog->triggerShow(); }
    virtual void            hide() override { emit _dialog->triggerHide(); }
    virtual bool            isVisible() override { return _dialog->isVisible(); }
    virtual SGIItemBase *   item() const override { return _dialog->item(); }

    void histogramComplete() override {
        emit _dialog->triggerReloadStatistics();
    }

    ImagePreviewDialog *        _dialog;
    Ui_ImagePreviewDialog *     ui;
    Image                           reinterpretedImage;
	QImage							originalImage;
    QToolBar *                      toolBar;
    QAction *                       refreshAction;
    QAction *                       zoomInAction;
    QAction *                       zoomOutAction;
    QAction *                       normalSizeAction;
    QAction *                       fitToWindowAction;
    QAction *                       flipHorizontalAction;
    QAction *                       flipVerticalAction;
    QAction *                       selectBackgroundColorAction;
    QAction *                       setColorAction;
    QSpinBox *                      spinBoxRefreshTime;
    QComboBox *                     imageWidth;
    QComboBox *                     imageHeight;
    QComboBox *                     imageFormat;
    QComboBox *                     colorFilterComboBox;
    QTimer *                        refreshTimer;
    QString                         labelText;
    double                          scaleFactor;
    bool                            initialRefresh;
	Histogram						histogram;
	bool							histogramReady;
    bool                            ready;
};

std::map<Image::ImageFormat, QString> ImagePreviewDialog::ImagePreviewDialogImpl::ImageFormatDisplayText;


ImagePreviewDialog::ImagePreviewDialogImpl::ImagePreviewDialogImpl(ImagePreviewDialog * dialog_)
    : _dialog(dialog_)
    , ui(nullptr)
    , toolBar(nullptr)
    , refreshAction(nullptr)
    , zoomInAction(nullptr)
    , zoomOutAction(nullptr)
    , normalSizeAction(nullptr)
    , fitToWindowAction(nullptr)
    , flipHorizontalAction(nullptr)
    , flipVerticalAction(nullptr)
    , selectBackgroundColorAction(nullptr)
    , setColorAction(nullptr)
    , imageWidth(nullptr)
    , imageHeight(nullptr)
    , imageFormat(nullptr)
    , refreshTimer(nullptr)
    , labelText()
	, scaleFactor(1.0)
    , initialRefresh(true)
    , histogram()
	, histogramReady(false)
    , ready(false)
{
    _dialog->_priv = this;
    if(ImageFormatDisplayText.empty())
    {
        ImageFormatDisplayText[Image::ImageFormatOriginal] = tr("Original");
        ImageFormatDisplayText[Image::ImageFormatAutomatic] = tr("Automatic");
        ImageFormatDisplayText[Image::ImageFormatRGB24] = tr("RGB 24-bit");
        ImageFormatDisplayText[Image::ImageFormatRGB32] = tr("RGB 32-bit");
        ImageFormatDisplayText[Image::ImageFormatRGBA32] = tr("RGBA 32-bit");
        ImageFormatDisplayText[Image::ImageFormatARGB32] = tr("ARGB 32-bit");
        ImageFormatDisplayText[Image::ImageFormatBGR24] = tr("BGR 24-bit");
        ImageFormatDisplayText[Image::ImageFormatBGR32] = tr("BGR 32-bit");
        ImageFormatDisplayText[Image::ImageFormatBGRA32] = tr("BGRA 32-bit");
        ImageFormatDisplayText[Image::ImageFormatABGR32] = tr("ABGR 32-bit");
        ImageFormatDisplayText[Image::ImageFormatYUV420] = tr("YUV 4:2:0");
        ImageFormatDisplayText[Image::ImageFormatYUV422] = tr("YUV 4:2:2");
        ImageFormatDisplayText[Image::ImageFormatYUYV] = tr("YUYV 4:2:2");
        ImageFormatDisplayText[Image::ImageFormatUYVY] = tr("UYVY 4:2:2");
        ImageFormatDisplayText[Image::ImageFormatYUV444] = tr("YUV 4:4:4");
        ImageFormatDisplayText[Image::ImageFormatGray] = tr("Grayscale");
        ImageFormatDisplayText[Image::ImageFormatRed] = tr("Red");
        ImageFormatDisplayText[Image::ImageFormatGreen] = tr("Green");
        ImageFormatDisplayText[Image::ImageFormatBlue] = tr("Blue");
        ImageFormatDisplayText[Image::ImageFormatAlpha] = tr("Alpha");
        ImageFormatDisplayText[Image::ImageFormatFloat] = tr("Float 32-bit");
    }
	ui = new Ui_ImagePreviewDialog;
	ui->setupUi(_dialog);

	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, _dialog, &ImagePreviewDialog::reject);
    connect(ui->imageLabel, &ImageQtWidget::mouseMoved, _dialog, &ImagePreviewDialog::onMouseMoved);
    connect(ui->imageGL, &ImageGLWidget::mouseMoved, _dialog, &ImagePreviewDialog::onMouseMoved);

    const QColor default_osg_view_clear_color = QColor::fromRgbF(0.2, 0.2, 0.4, 1.0);
    ui->imageGL->setBackgroundColor(default_osg_view_clear_color);
    ui->scrollAreaImageQt->setBackgroundColor(default_osg_view_clear_color);
    ui->scrollAreaImageGL->setBackgroundColor(default_osg_view_clear_color);

	createToolbar();

	int w = ui->splitter->width();
	QList<int> sizes = QList<int>() << (w * 3 / 4) << (w / 4);
	ui->splitter->setSizes(sizes);

    ui->scrollAreaImageGL->setWidgetResizable(true);
    ui->scrollAreaImageQt->setWidgetResizable(true);

	ui->tabWidget->setCurrentIndex(0);
    ui->tabWidgetImageView->setCurrentIndex(0);

    ready = true;
}

ImagePreviewDialog::ImagePreviewDialogImpl::~ImagePreviewDialogImpl()
{
    delete _dialog;
    delete ui;
}

void ImagePreviewDialog::ImagePreviewDialogImpl::refresh()
{
    // release any previous converted images and get a new copy from the original item
    _dialog->_workImage = nullptr;

	_dialog->refreshImpl();
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
    ui->imageGL->adjustSize();
	scaleFactor = 1.0;
    refresh();
}
//! [12]

//! [13]
void ImagePreviewDialog::ImagePreviewDialogImpl::fitToWindow(bool enable)
//! [13] //! [14]
{
    ui->scrollAreaImageGL->setFitToWindow(enable);
    ui->scrollAreaImageQt->setFitToWindow(enable);

    if(!enable)
        normalSize();
    else
    {
        scaleFactor = 1.0;
        refresh();
    }
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
	normalSizeAction->setShortcut(tr("Ctrl+0"));
	normalSizeAction->setEnabled(false);
	connect(normalSizeAction, &QAction::triggered, this, &ImagePreviewDialogImpl::normalSize);

	fitToWindowAction = new QAction(tr("&Fit to Window"), _dialog);
	fitToWindowAction->setIcon(QIcon::fromTheme("zoom-fit-best"));
	fitToWindowAction->setEnabled(false);
	fitToWindowAction->setCheckable(true);
    fitToWindowAction->setChecked(true);
	fitToWindowAction->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAction, &QAction::toggled, this, &ImagePreviewDialogImpl::fitToWindow);

	flipHorizontalAction = new QAction(tr("&Mirror"), _dialog);
	flipHorizontalAction->setIcon(QIcon::fromTheme("object-flip-horizontal"));
	flipHorizontalAction->setEnabled(_dialog->_item.valid());
	flipHorizontalAction->setChecked(false);
	flipHorizontalAction->setCheckable(true);
	connect(flipHorizontalAction, &QAction::triggered, this, &ImagePreviewDialogImpl::flipHorizontal);

	flipVerticalAction = new QAction(tr("&Flip"), _dialog);
	flipVerticalAction->setIcon(QIcon::fromTheme("object-flip-vertical"));
	flipVerticalAction->setEnabled(_dialog->_item.valid());
	flipVerticalAction->setChecked(false);
	flipVerticalAction->setCheckable(true);
	connect(flipVerticalAction, &QAction::triggered, this, &ImagePreviewDialogImpl::flipVertical);

	selectBackgroundColorAction = new QAction(tr("Select &background color"), _dialog);
	selectBackgroundColorAction->setIcon(QIcon::fromTheme("color-fill"));
	connect(selectBackgroundColorAction, &QAction::triggered, this, &ImagePreviewDialogImpl::selectBackgroundColor);

    setColorAction = new QAction(tr("Set &Color"), _dialog);
    setColorAction->setIcon(QIcon::fromTheme("color-fill"));
    connect(setColorAction, &QAction::triggered, this, &ImagePreviewDialogImpl::setColor);

    

    QMenu * saveMenu = new QMenu(_dialog);
    for(const auto & it : ImageFormatDisplayText)
    {
        QAction * saveAction = new QAction(it.second, saveMenu);
        switch (it.first)
        {
        case Image::ImageFormatAutomatic:
            {
                saveAction->setShortcut(tr("Ctrl+Shift+S"));
                saveAction->setToolTip(tr("Save image..."));
            }
            break;
        case Image::ImageFormatOriginal:
            {
                saveAction->setShortcut(tr("Ctrl+S"));
                saveAction->setToolTip(tr("Save original image..."));
            }
            break;
        default:
            saveAction->setToolTip(tr("Save image as %1...").arg(it.second));
            break;
        }
        saveAction->setIcon(QIcon::fromTheme("document-save"));
        saveAction->setData(QVariant::fromValue((int)it.first));
        connect(saveAction, &QAction::triggered, this, &ImagePreviewDialogImpl::saveImageAs);
        saveMenu->addAction(saveAction);
    }
    QPushButton * saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    saveButton->setMenu(saveMenu);

    QMenu * openMenu = new QMenu(_dialog);
    for(const auto & it : ImageFormatDisplayText)
    {
        if (it.first == Image::ImageFormatOriginal)
            continue;
        QAction * openAction = new QAction(it.second, openMenu);
        switch (it.first)
        {
        case Image::ImageFormatAutomatic:
            {
                openAction->setShortcut(tr("Ctrl+O"));
                openAction->setToolTip(tr("Open image..."));
            }
            break;
        default:
            openAction->setToolTip(tr("Open image as %1...").arg(it.second));
            break;
        }
        openAction->setToolTip(tr("Open image as %1").arg(it.second));
        openAction->setIcon(QIcon::fromTheme("document-open"));
        openAction->setData(QVariant::fromValue((int)it.first));
        connect(openAction, &QAction::triggered, this, &ImagePreviewDialogImpl::openImageAs);
        openMenu->addAction(openAction);
    }
    QPushButton * openButton = ui->buttonBox->button(QDialogButtonBox::Open);
    openButton->setMenu(openMenu);

    imageWidth = new QComboBox(toolBar);
    imageWidth->setEditable(true);
    imageWidth->setToolTip(tr("Width of the image"));
    for (unsigned w : { 16, 32, 64, 128, 256, 640, 720, 800, 1024, 1240, 1980, 2048 })
        imageWidth->addItem(QString::number(w), w);

    imageHeight = new QComboBox(toolBar);
    imageHeight->setEditable(true);
    imageHeight->setToolTip(tr("Height of the image"));
    for (unsigned h : { 16, 32, 64, 128, 256, 388, 480, 600, 768, 1024, 1080, 2048 })
        imageHeight->addItem(QString::number(h), h);

    imageFormat = new QComboBox(toolBar);
    imageFormat->setToolTip(tr("Re-interpret image format"));
    for(const auto & it : ImageFormatDisplayText)
        imageFormat->addItem(it.second, QVariant::fromValue((int)it.first));

    colorFilterComboBox = new QComboBox(toolBar);
    colorFilterComboBox->setToolTip(tr("Select color filter"));
    for (unsigned i = 0; i < ColorFilter::numberOfFilters(); ++i)
    {
        const ColorFilter & f = ColorFilter::getFilter((ColorFilter::FilterType)i);
        colorFilterComboBox->addItem(f.name(), QVariant::fromValue((int)f.type()));
    }

    spinBoxRefreshTime = new QSpinBox(toolBar);
    spinBoxRefreshTime->setToolTip(tr("Automatically reloads the information every X milliseconds."));
    spinBoxRefreshTime->setMinimum(0);
    spinBoxRefreshTime->setMaximum(60000);
    spinBoxRefreshTime->setSingleStep(100);
    spinBoxRefreshTime->setPrefix("Refresh ");
    spinBoxRefreshTime->setSuffix("ms");
    connect(spinBoxRefreshTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImagePreviewDialogImpl::refreshTimeChanged);

    toolBar->addWidget(imageWidth);
    toolBar->addWidget(imageHeight);
    toolBar->addSeparator();
    toolBar->addWidget(imageFormat);
    toolBar->addWidget(colorFilterComboBox);
    toolBar->addAction(refreshAction);
    toolBar->addSeparator();
	toolBar->addAction(zoomInAction);
	toolBar->addAction(zoomOutAction);
	toolBar->addAction(normalSizeAction);
	toolBar->addAction(fitToWindowAction);
    toolBar->addSeparator();
	toolBar->addAction(flipHorizontalAction);
	toolBar->addAction(flipVerticalAction);
    toolBar->addSeparator();
    toolBar->addAction(selectBackgroundColorAction);
    toolBar->addAction(setColorAction);
    toolBar->addWidget(spinBoxRefreshTime);

    // do the connects at the end to avoid trouble when new items are added and signals fired
    connect(imageWidth, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::imageWidthChanged);
    connect(imageHeight, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::imageHeightChanged);
    connect(imageFormat, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::imageFormatChanged);
    connect(colorFilterComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::selectedColorFilterChanged);

    updateToolbar();
}
//! [20]

//! [21]
void ImagePreviewDialog::ImagePreviewDialogImpl::updateToolbar()
//! [21] //! [22]
{
    bool isImageOk = _dialog->_image.valid();
    imageFormat->setEnabled(isImageOk);
    //saveAction->setEnabled(isImageOk);
    fitToWindowAction->setEnabled(isImageOk);
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
    ui->scrollAreaImageQt->setScaleFactor(scaleFactor);
    ui->scrollAreaImageGL->setScaleFactor(scaleFactor);

	zoomInAction->setEnabled(scaleFactor < 3.0);
	zoomOutAction->setEnabled(scaleFactor > 0.333);
}
//! [24]

void ImagePreviewDialog::ImagePreviewDialogImpl::flipHorizontal()
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::flipVertical()
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::selectBackgroundColor()
{
    QColor oldColor = ui->scrollAreaImageQt->backgroundColor();
    QColor color = QColorDialog::getColor(oldColor, _dialog, tr("Select background color"));
    if(color.isValid())
    {
        ui->scrollAreaImageQt->setBackgroundColor(color);
        ui->scrollAreaImageGL->setBackgroundColor(color);
    }
}

void ImagePreviewDialog::ImagePreviewDialogImpl::setColor()
{
    QColor oldColor;
    QColor color = QColorDialog::getColor(oldColor, _dialog, tr("Select color"));
    if (color.isValid())
    {
        // get image from the item if no image has been explicitly specified by the caller
        if (_dialog->_image.valid())
        {
            struct SetColor {
                Image::Pixel _color;
                bool operator()(Image::Pixel & pixel)
                {
                    pixel = _color;
                    return true;
                }
            };
            PixelVisitor<SetColor> pv;
            pv._color = Image::Pixel(sgi::Image::DataTypeUnsignedByte, color.rgba());
            const sgi::Image * image = _dialog->_image.get();
            pv.accept(const_cast<sgi::Image*>(image));
        }

        _dialog->_workImage = nullptr;
        refresh();
    }
}

void ImagePreviewDialog::ImagePreviewDialogImpl::imageWidthChanged(int index)
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::imageHeightChanged(int index)
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::imageFormatChanged(int index)
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::selectedColorFilterChanged(int index)
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::openImageAs()
{
    QAction * action = static_cast<QAction*>(sender());
    if(!action)
        return;
	static QString lastDir = QDir::currentPath();
    QString fileName;
    Image::ImageFormat targetFormat = (Image::ImageFormat)action->data().toInt();
    if(targetFormat == Image::ImageFormatAutomatic)
    {
        fileName = QFileDialog::getOpenFileName(_dialog, tr("Open image file"), lastDir);
    }
    else
    {
        QString formatText = ImageFormatDisplayText[targetFormat];
        fileName = QFileDialog::getOpenFileName(_dialog, tr("Open image file as %1").arg(formatText), lastDir);
    }
    if (!fileName.isEmpty())
    {
        bool result = false;
        lastDir = fileName;
        switch(targetFormat)
        {
        case Image::ImageFormatOriginal:
        case Image::ImageFormatAutomatic:
        case Image::ImageFormatRGB24:
        case Image::ImageFormatRGB32:
        case Image::ImageFormatARGB32:
        case Image::ImageFormatIndexed8:
        case Image::ImageFormatBGR24:
        case Image::ImageFormatBGR32:
        case Image::ImageFormatABGR32:
            {
                QByteArray format = QImageReader::imageFormat(fileName);
                if (!format.isEmpty())
                {
                    QImageReader reader(fileName);
                    QImage image = reader.read();
                    result = !image.isNull();
                    if (result)
                    {
                        originalImage = image;
                        _dialog->_workImage = new Image(&image);
                        _dialog->_workImage->reinterpretFormat(targetFormat);
                        _dialog->_image = _dialog->_workImage;
                        _dialog->_itemImage = nullptr;
                    }
                }
                else
                {
                    //_dialog->_hostInterface->readObjectFile(result, _dialog->_item.get(), fileName.toStdString(), nullptr);

                }
            }
            break;
        case Image::ImageFormatYUV420:
        case Image::ImageFormatYUV422:
        case Image::ImageFormatYUV444:
        case Image::ImageFormatYUYV:
        case Image::ImageFormatUYVY:
            {
                QFile f(fileName);
                if(f.open(QIODevice::ReadOnly))
                {
                    QByteArray data = f.readAll();
                    result = !data.isEmpty();
                    if(result)
                    {
                        originalImage = QImage();
                        _dialog->_workImage = new Image(targetFormat, Image::DataTypeUnsignedByte, data.data(), data.size());
                        std::vector<Image::ImageSize> sizes;
                        if (_dialog->_workImage->guessImageSizes(sizes) && !sizes.empty())
                        {
                            _dialog->_workImage->reinterpret(sizes.front());
                        }
                        _dialog->_image = _dialog->_workImage;
                        _dialog->_itemImage = nullptr;
                    }
                    f.close();
                }
            }
            break;
        }
        if(!result)
        {
            QMessageBox::critical(_dialog, tr("Open failed"), tr("Failed to open image from %1").arg(fileName));
        }
        else
            refresh();
    }
}

void ImagePreviewDialog::ImagePreviewDialogImpl::saveImageAs()
{
    QAction * action = static_cast<QAction*>(sender());
    if(!action)
        return;
    const QPixmap * pixmap = ui->imageLabel->pixmap();
    static QString lastDir = QDir::currentPath();
    QString fileName;
    Image::ImageFormat targetFormat = (Image::ImageFormat)action->data().toInt();
    if(targetFormat == Image::ImageFormatAutomatic)
    {
        fileName = QFileDialog::getSaveFileName(_dialog, tr("Save image file"), lastDir);
    }
    else
    {
        QString formatText = ImageFormatDisplayText[targetFormat];
        fileName = QFileDialog::getSaveFileName(_dialog, tr("Save image file as %1").arg(formatText), lastDir);
    }
    if (!fileName.isEmpty())
    {
        Image tempImage;
        bool result = true;
        if (targetFormat == Image::ImageFormatOriginal)
        {
            // no conversion needed
        }
        else  if(targetFormat != Image::ImageFormatAutomatic)
        {
#if defined(SGI_USE_FFMPEG)
            result = SWScale::convert(*_dialog->_image.get(), targetFormat, tempImage);
#else
            result = false;
#endif
        }
        lastDir = fileName;
        switch(targetFormat)
        {
        case Image::ImageFormatOriginal:
            _dialog->_hostInterface->writeObjectFile(result, _dialog->_item.get(), fileName.toStdString(), nullptr);
            break;
        case Image::ImageFormatAutomatic:
            if(pixmap)
            {
                QImageWriter writer(fileName);
                result = writer.write(pixmap->toImage());
            }
            break;
        case Image::ImageFormatRGB24:
        case Image::ImageFormatRGB32:
        case Image::ImageFormatARGB32:
        case Image::ImageFormatIndexed8:
        case Image::ImageFormatBGR24:
        case Image::ImageFormatBGR32:
        case Image::ImageFormatABGR32:
            if(pixmap)
            {
                QImageWriter writer(fileName);
                result = writer.write(pixmap->toImage());
            }
            break;
        case Image::ImageFormatYUV420:
        case Image::ImageFormatYUV422:
        case Image::ImageFormatYUV444:
        case Image::ImageFormatYUYV:
        case Image::ImageFormatUYVY:
            {
                QFile f(fileName);
                if(f.open(QIODevice::WriteOnly|QIODevice::Truncate))
                {
                    int written = f.write((const char*)tempImage.data(), tempImage.length());
                    result = (written == tempImage.length());
                    f.close();
                }
            }
            break;
        }
        if(!result)
        {
            QMessageBox::critical(_dialog, tr("Save failed"), tr("Failed to save image to %1").arg(fileName));
        }
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
        os << "<b>item is <i>nullptr</i></b>";
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
        os << "<b>image is <i>nullptr</i></b>";
    }
    ui->textEditImage->blockSignals(true);
    ui->textEditImage->setHtml(QString::fromStdString(os.str()));
    ui->textEditImage->blockSignals(false);
}

void ImagePreviewDialog::ImagePreviewDialogImpl::refreshTimeChanged(int n)
{
    if (!refreshTimer)
    {
        refreshTimer = new QTimer(this);
        connect(refreshTimer, &QTimer::timeout, this, &ImagePreviewDialogImpl::refreshTimerExpired);
    }
    const int minimumTimerInterval = 100; // ms
    const int initialTimerInterval = 2500; // ms
    if (refreshTimer->interval() == 0 && n >= minimumTimerInterval && n < initialTimerInterval)
    {
        spinBoxRefreshTime->setValue(initialTimerInterval);
        n = initialTimerInterval;
    }
    if (n >= minimumTimerInterval)
        refreshTimer->start(n);
    else
        refreshTimer->stop();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::refreshTimerExpired()
{
    refresh();
}

class ImagePreviewDialog::HostCallback : public HostCallbackFilterT<IImagePreviewDialog>
{
public:
   HostCallback(IHostCallback * original, ImagePreviewDialog * dialog)
        : HostCallbackFilterT<IImagePreviewDialog>(original, dialog->_interface) {}
};

ImagePreviewDialog::ImagePreviewDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , _item(nullptr)
    , _hostInterface(SGIPlugins::instance()->hostInterface())
    , _priv(new ImagePreviewDialogImpl(this))
    , _interface()
    , _hostCallback(nullptr)
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
    , _hostCallback(callback)
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
    connect(this, &ImagePreviewDialog::triggerReloadStatistics, this, &ImagePreviewDialog::reloadStatistics, Qt::QueuedConnection);
}

ImagePreviewDialog::~ImagePreviewDialog()
{
    _priv->_dialog = nullptr;
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

Image::ImageFormat ImagePreviewDialog::ImagePreviewDialogImpl::currentImageFormat() const
{
    int index = imageFormat->currentIndex();
    Image::ImageFormat ret = (Image::ImageFormat)imageFormat->itemData(index, Qt::UserRole).toInt();
    return ret;
}

ColorFilter::FilterType ImagePreviewDialog::ImagePreviewDialogImpl::currentColorFilter() const
{
    int index = colorFilterComboBox->currentIndex();
    ColorFilter::FilterType ret = (ColorFilter::FilterType)colorFilterComboBox->itemData(index, Qt::UserRole).toInt();
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

void ImagePreviewDialog::refreshStatistics(const sgi::Image * image)
{
    std::cout << __FUNCTION__ << std::endl;
    _priv->histogram.calculate(image, _priv);
    reloadStatistics();
}

void ImagePreviewDialog::reloadStatistics()
{
    std::cout << __FUNCTION__ << std::endl;
    _priv->ui->statistics->clear();
    _priv->ui->statistics->setHeaderLabels(QStringList() << tr("Name") << tr("Value"));

    QTreeWidgetItem * root = _priv->ui->statistics->invisibleRootItem();
    addStatisticsValue(root, tr("Width"), _workImage->width());
    addStatisticsValue(root, tr("Height"), _workImage->height());
    addStatisticsValue(root, tr("Depth"), _workImage->depth());
    //addStatisticsValue(root, tr("Device pixel ratio"), _workImage->devicePixelRatio());
    //addStatisticsValue(root, tr("Color count"), _workImage->colorCount());
    addStatisticsValue(root, tr("Bit per pixel"), _workImage->bitsPerPixel());
//    if(image->hasAlphaChannel())
//        addStatisticsValue(root, tr("Alpha"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minAlpha).arg(_priv->histogram.maxAlpha).arg(_priv->histogram.avgAlpha));
    addStatisticsValue(root, tr("Red"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minRed).arg(_priv->histogram.maxRed).arg(_priv->histogram.avgRed));
    addStatisticsValue(root, tr("Green"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minGreen).arg(_priv->histogram.maxGreen).arg(_priv->histogram.avgGreen));
    addStatisticsValue(root, tr("Blue"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minBlue).arg(_priv->histogram.maxBlue).arg(_priv->histogram.avgBlue));
    addStatisticsValue(root, tr("Gray"), QString("min %1, max %2, avg %3").arg(_priv->histogram.minGray).arg(_priv->histogram.maxGray).arg(_priv->histogram.avgGray));
    addStatisticsValue(root, tr("Luma"), QString("avg %1").arg(_priv->histogram.avgLuma));
#if 0
    if(image->hasAlphaChannel())
        addStatisticsValue(root, tr("Transparency"), QString("%1 pixels").arg(_priv->histogram.numTransparentPixels));
    else
        addStatisticsValue(root, tr("Transparency"), tr("N/A"));
#endif
}

void ImagePreviewDialog::refreshImpl()
{
    if (_item.valid())
    {
        std::string displayName;
        SGIPlugins::instance()->getObjectDisplayName(displayName, _item.get());
        setWindowTitle(tr("Image Viewer - %1").arg(qt_helpers::fromUtf8(displayName)));
    }
    else
    {
        setWindowTitle(tr("Image Viewer - No image available"));
    }

    // get image from the item if no image has been explicitly specified by the caller
    if (!_image.valid() && _item.valid())
    {
        if (_hostInterface->convertToImage(_itemImage, _item.get()))
        {
            _image = _itemImage;
            _workImage = _itemImage;
        }
    }

    if (!_workImage.valid())
        _workImage = const_cast<sgi::Image*>(_image.get());

    Image::ImageFormat format = _priv->currentImageFormat();
    if (_workImage.valid())
        _workImage->reinterpretFormat(format);

    QImage qimg;
#if defined(SGI_USE_FFMPEG)
    if (_workImage.valid())
    {
        if(SWScale::load())
            SWScale::convertToQImage(*_workImage.get(), format, qimg);
        else
            qt_helpers::convertImageToQImage(_workImage.get(), format, qimg);
    }
#else
    qt_helpers::convertImageToQImage(_workImage.get(), format, qimg);
#endif

    _priv->ui->imageGL->setImage(_workImage.get());

    // trigger the statistic update if the page is selected
    tabWidgetCurrentChanged(0);


    _priv->ui->imageLabel->setText(QString());
    _priv->originalImage = qimg;
    if(!qimg.isNull())
    {
        QImage qimgDisplay;
        ColorFilter::FilterType filterType = _priv->currentColorFilter();
        const ColorFilter & filter = ColorFilter::getFilter(filterType);
        if (!filter.apply(qimg, qimgDisplay))
            qimgDisplay = qimg;

        _priv->ui->colorFilterFragment->blockSignals(true);
        _priv->ui->colorFilterVertex->blockSignals(true);
        _priv->ui->colorFilterFragment->setPlainText(filter.fragmentShader());
        _priv->ui->colorFilterVertex->setPlainText(filter.vertexShader());
        _priv->ui->colorFilterFragment->blockSignals(false);
        _priv->ui->colorFilterVertex->blockSignals(false);
        _priv->ui->imageGL->setColorFilter(filter.fragmentShader(), filter.vertexShader());

        QImage actualImage;
        if(_priv->flipHorizontalAction->isChecked() || _priv->flipVerticalAction->isChecked())
            actualImage = qimgDisplay.mirrored(_priv->flipHorizontalAction->isChecked(), _priv->flipVerticalAction->isChecked());
        else
            actualImage = qimgDisplay;
        _priv->ui->imageGL->setMirrored(_priv->flipHorizontalAction->isChecked(), _priv->flipVerticalAction->isChecked());

        _priv->imageWidth->setCurrentText(QString::number(_image->width()));
        _priv->imageHeight->setCurrentText(QString::number(_image->height()));
        _priv->ui->imageLabel->setImage(actualImage);
        _priv->ui->scrollAreaImageGL->setImage(actualImage);
        _priv->ui->scrollAreaImageQt->setImage(actualImage);
    }
    else
    {
        _priv->imageWidth->setCurrentText(tr("N/A"));
        _priv->imageHeight->setCurrentText(tr("N/A"));
        _priv->ui->imageLabel->setImage(qimg);
        _priv->ui->scrollAreaImageGL->setImage(qimg);
        _priv->ui->scrollAreaImageQt->setImage(qimg);
    }
    _priv->ui->scrollAreaImageGL->setFitToWindow(_priv->fitToWindowAction->isChecked());
    _priv->ui->scrollAreaImageQt->setFitToWindow(_priv->fitToWindowAction->isChecked());

    std::stringstream ss;
    ss << "<i>Info for displayed image:</i><br/>\r\n";
    if(!qimg.isNull())
    {
        ss << qimg.width() << "x" << qimg.height() << "x" << qimg.depth() << std::endl;
        ss << "format=" << qimg.format() << " colors=" << qimg.colorCount() << " bitPlaneCount=" << qimg.bitPlaneCount();
    }
    else
        ss << "<b>No image</b>";
    _priv->ui->labelImage->setText(qt_helpers::fromUtf8(ss.str()));
    std::string itemName;
    _hostInterface->getObjectDisplayName(itemName, _item.get(), true);
    _priv->ui->openItem->setText(qt_helpers::fromUtf8(itemName));
    _priv->ui->mouseinfo->setText(QString());
    _priv->setImageInfo(_workImage.get());
    _priv->setNodeInfo(_item.get());
    _priv->updateToolbar();
}

SGIItemBase * ImagePreviewDialog::getView()
{
    if(_hostCallback.valid())
        return _hostCallback->getView();
    else
        return nullptr;
}

void ImagePreviewDialog::triggerRepaint()
{
    if(_hostCallback.valid())
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
    if (!_image.valid())
        _workImage = nullptr;
    else
        _workImage = new Image(*image);
    _priv->labelText = qt_helpers::fromUtf8(description);
    if(callback)
        _hostCallback = callback;
    emit triggerOnObjectChanged();
}

void ImagePreviewDialog::setImage(const sgi::Image * image)
{
    _image = image;
    if (!_image.valid())
        _workImage = nullptr;
    else
        _workImage = new Image(*image);
    emit triggerOnObjectChanged();
}

void ImagePreviewDialog::setDescription(const std::string & description)
{
    _priv->labelText = qt_helpers::fromUtf8(description);
    emit triggerOnObjectChanged();
}

SGIItemBase * ImagePreviewDialog::item() const
{
    return _item.get();
}

void ImagePreviewDialog::openItem()
{
    if(_item.valid())
    {
        ISceneGraphDialogPtr dialog;
        if(_hostCallback.valid())
            dialog = _hostCallback->showSceneGraphDialog(this, _item.get());
        else
            dialog = _hostInterface->showSceneGraphDialog(this, _item.get(), _hostCallback.get());
        if(dialog.valid())
        {
            dialog->show();
        }
    }
}

void ImagePreviewDialog::onMouseMoved(float x, float y)
{
    QString str;
    if (_workImage.valid())
    {
        unsigned px_x = 0;
        if(x >= 0.0f && x < 1.0f)
            px_x = static_cast<unsigned>(x * _workImage->width());
        unsigned px_y = 0;
        if(y >= 0.0f && y < 1.0f)
            px_y = static_cast<unsigned>(y * _workImage->height());

        if(_priv->flipHorizontalAction->isChecked())
            px_x = _workImage->width() - px_x;
        if(_priv->flipVerticalAction->isChecked())
            px_y = _workImage->height() - px_y;

        Image::Pixel px = _workImage->pixel(px_x, px_y);

        QString px_value = QString::fromStdString(px.toString(true));

        str = tr("X=%1, Y=%2, value=%3").arg(px_x).arg(px_y).arg(px_value);
    }
    else
        str = tr("X=%1, Y=%2").arg(x).arg(y);
    _priv->ui->mouseinfo->setText(str);
}

void ImagePreviewDialog::colorFilterChanged()
{
    QString fragment = _priv->ui->colorFilterFragment->toPlainText();
    QString vertex = _priv->ui->colorFilterVertex->toPlainText();
    _priv->ui->imageGL->setColorFilter(fragment, vertex);
}

void ImagePreviewDialog::tabWidgetCurrentChanged(int)
{
    if(!_priv->ready)
        return;

    if(_priv->ui->tabWidget->currentWidget() == _priv->ui->tabStatistics)
    {
        refreshStatistics(_workImage.get());
    }
}

} // namespace sgi
