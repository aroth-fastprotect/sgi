#include "stdafx.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QToolBar>
#include <QScrollBar>
#include <QComboBox>
#include <QDesktopWidget>
#include <QImageWriter>
#include <QImageReader>
#include <QMouseEvent>

#include "ImagePreviewDialog.h"
#include "ui_ImagePreviewDialog.h"
#include "SGIPlugin.h"
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/SGIImage.h>
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
}

bool applyColorFilterQImage(QImage & src, QImage & dest, ImagePreviewDialog::ColorFilter filter)
{
    if(src.isNull())
        return false;

    if(filter == ImagePreviewDialog::ColorFilterAll)
        dest = src;
    else
    {
        dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);
        for (int y = 0; y < src.height(); y++) {
            for (int x = 0; x < src.width(); x++) {
                QRgb src_pixel = src.pixel(x, y);
                QRgb dest_pixel = 0;
                switch(filter)
                {
                case ImagePreviewDialog::ColorFilterAll:
                    dest_pixel = src_pixel;
                    break;
                case ImagePreviewDialog::ColorFilterGrayscale:
                    {
                        int v = qGray(src_pixel) & 0xff;
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterRed:
                    {
                        int v = qRed(src_pixel) & 0xff;
                        dest_pixel = qRgb(v, 0, 0);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterGreen:
                    {
                        int v = qGreen(src_pixel) & 0xff;
                        dest_pixel = qRgb(0, v, 0);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterBlue:
                    {
                        int v = qBlue(src_pixel) & 0xff;
                        dest_pixel = qRgb(0, 0, v);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterAlpha:
                    {
                        int v = qAlpha(src_pixel) & 0xff;
                        // display the alpha channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterLuminance:
                    {
                        int v = QColor(src_pixel).lightness();
                        // display the luminance channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterHue:
                    {
                        int v = QColor(src_pixel).hslHue();
                        // display the hue channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::ColorFilterSaturation:
                    {
                        int v = QColor(src_pixel).hslSaturation();
                        // display the saturation channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                default:
                    dest_pixel = src_pixel;
                    break;
                }
                dest.setPixel(x, y, dest_pixel);
            }
        }
    }
    return true;
}

/// @note a lot of GLSL shader code for color filters can be found at
/// @see https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Shaders/Builtin/Functions/
/// e.g. RGBToHSL.glsl
bool getColorFilterShaderCode(ImagePreviewDialog::ColorFilter filter, QString & fragment, QString & vertex)
{
    /// @see http://cs.uns.edu.ar/cg/clasespdf/GraphicShaders.pdf, Chapter 11, page 241ff
    ///
    bool ret = true;
    switch(filter)
    {
    case ImagePreviewDialog::ColorFilterAll:
        fragment.clear();
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterGrayscale:
        fragment = "float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));\r\ncolor = vec4(vec3(gray), color.a);";
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterRed:
        fragment = "color = vec4(color.r, 0.0, 0.0, color.a);";
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterGreen:
        fragment = "color = vec4(0.0, color.g, 0.0, color.a);";
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterBlue:
        fragment = "color = vec4(0.0, 0.0, color.b, color.a);";
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterAlpha:
        fragment = "color = vec4(color.a, color.a, color.a, 1.0);";
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterLuminance:
        fragment = "float lum = dot(color.rgb, vec3(0.2125, 0.7154, 0.0721));\r\ncolor = vec4(vec3(lum), 1.0);";
        vertex.clear();
        break;
    case ImagePreviewDialog::ColorFilterHue:
        // http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
        break;
    case ImagePreviewDialog::ColorFilterSaturation:
        // http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
        break;
    default:
        break;
    }
    return ret;
}


class ImagePreviewDialog::ImagePreviewDialogImpl : public QObject, public IImagePreviewDialog
{
public:
    static std::map<Image::ImageFormat, QString> ImageFormatDisplayText;
	ImagePreviewDialogImpl(ImagePreviewDialog * dialog_);
	~ImagePreviewDialogImpl();
    void createToolbar();
    void updateToolbar();
    void scaleImage(double factor);
    static void adjustScrollBar(QScrollBar *scrollBar, double factor);

    Image::ImageFormat currentImageFormat() const;
    ColorFilter currentColorFilter() const;

    // Actions
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void saveImageAs();
    void openImageAs();
    void refresh();
    void flipHorizontal();
    void flipVertical();
    void selectBackgroundColor();
    void imageWidthChanged(int index);
    void imageHeightChanged(int index);
    void imageFormatChanged(int index);
    void selectedColorFilterChanged(int index);

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
    QComboBox *                     imageWidth;
    QComboBox *                     imageHeight;
    QComboBox *                     imageFormat;
    QComboBox *                     colorFilterComboBox;
    QString                         labelText;
    double                          scaleFactor;
    bool                            initialRefresh;
	Histogram						histogram;
	bool							histogramReady;
};

std::map<Image::ImageFormat, QString> ImagePreviewDialog::ImagePreviewDialogImpl::ImageFormatDisplayText;


ImagePreviewDialog::ImagePreviewDialogImpl::ImagePreviewDialogImpl(ImagePreviewDialog * dialog_)
    : _dialog(dialog_)
    , ui(NULL)
    , toolBar(NULL)
    , refreshAction(NULL)
    , zoomInAction(NULL)
    , zoomOutAction(NULL)
    , normalSizeAction(NULL)
    , fitToWindowAction(NULL)
    , flipHorizontalAction(NULL)
    , flipVerticalAction(NULL)
    , selectBackgroundColorAction(NULL)
    , imageWidth(NULL)
    , imageHeight(NULL)
    , imageFormat(NULL)
	, labelText()
	, scaleFactor(1.0)
	, initialRefresh(true)
	, histogram()
	, histogramReady(false)
{
    if(ImageFormatDisplayText.empty())
    {
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

    QPalette pal = ui->imageLabel->palette();
    const QColor default_osg_view_clear_color = QColor::fromRgbF(0.2f, 0.2f, 0.4f, 1.0f);
    pal.setColor(QPalette::Base, default_osg_view_clear_color);
    ui->imageLabel->setPalette(pal);
    ui->scrollAreaImageQt->setPalette(pal);
    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->scrollAreaImageQt->setBackgroundRole(QPalette::Base);
    ui->imageGL->setBackgroundColor(default_osg_view_clear_color);

	createToolbar();

	int w = ui->splitter->width();
	QList<int> sizes = QList<int>() << (w * 3 / 4) << (w / 4);
	ui->splitter->setSizes(sizes);

    ui->scrollAreaImageGL->setWidgetResizable(true);
    ui->scrollAreaImageQt->setWidgetResizable(true);

	ui->tabWidget->setCurrentIndex(0);
}

ImagePreviewDialog::ImagePreviewDialogImpl::~ImagePreviewDialogImpl()
{
    delete _dialog;
    delete ui;
}

void ImagePreviewDialog::ImagePreviewDialogImpl::refresh()
{
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
	scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImagePreviewDialog::ImagePreviewDialogImpl::fitToWindow()
//! [13] //! [14]
{
    refresh();
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
	connect(fitToWindowAction, &QAction::triggered, this, &ImagePreviewDialogImpl::fitToWindow);

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

    QMenu * saveMenu = new QMenu(_dialog);
    for(const auto & it : ImageFormatDisplayText)
    {
        QAction * saveAction = new QAction(it.second, saveMenu);
        if(it.first == Image::ImageFormatAutomatic)
        {
            saveAction->setShortcut(tr("Ctrl+S"));
            saveAction->setToolTip(tr("Save image..."));
        }
        else
            saveAction->setToolTip(tr("Save image as %1...").arg(it.second));
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
        QAction * openAction = new QAction(it.second, openMenu);
        if(it.first == Image::ImageFormatAutomatic)
        {
            openAction->setShortcut(tr("Ctrl+O"));
            openAction->setToolTip(tr("Open image..."));
        }
        else
            openAction->setToolTip(tr("Open image as %1...").arg(it.second));
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
    colorFilterComboBox->addItem(tr("All"), QVariant::fromValue((int)ColorFilterAll));
    colorFilterComboBox->addItem(tr("Grayscale"), QVariant::fromValue((int)ColorFilterGrayscale));
    colorFilterComboBox->addItem(tr("Red"), QVariant::fromValue((int)ColorFilterRed));
    colorFilterComboBox->addItem(tr("Green"), QVariant::fromValue((int)ColorFilterGreen));
    colorFilterComboBox->addItem(tr("Blue"), QVariant::fromValue((int)ColorFilterBlue));
    colorFilterComboBox->addItem(tr("Alpha"), QVariant::fromValue((int)ColorFilterAlpha));
    colorFilterComboBox->addItem(tr("Luminance"), QVariant::fromValue((int)ColorFilterLuminance));
    colorFilterComboBox->addItem(tr("Hue"), QVariant::fromValue((int)ColorFilterHue));
    colorFilterComboBox->addItem(tr("Saturation"), QVariant::fromValue((int)ColorFilterSaturation));

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
    ui->imageLabel->setScaledContents(true);
    ui->imageLabel->resize(scaleFactor * ui->imageLabel->pixmap()->size());

	adjustScrollBar(ui->scrollAreaImageQt->horizontalScrollBar(), factor);
	adjustScrollBar(ui->scrollAreaImageQt->verticalScrollBar(), factor);
    adjustScrollBar(ui->scrollAreaImageGL->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollAreaImageGL->verticalScrollBar(), factor);

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
    QPalette pal = ui->imageLabel->palette();
    QColor color = QColorDialog::getColor(pal.color(QPalette::Base), _dialog, tr("Select background color"));
    if(color.isValid())
    {
        pal.setColor(QPalette::Base, color);
        ui->imageLabel->setPalette(pal);
        ui->scrollAreaImageQt->setPalette(pal);
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
        case Image::ImageFormatAutomatic:
        case Image::ImageFormatRGB24:
        case Image::ImageFormatRGB32:
        case Image::ImageFormatARGB32:
        case Image::ImageFormatIndexed8:
        case Image::ImageFormatBGR24:
        case Image::ImageFormatBGR32:
        case Image::ImageFormatABGR32:
            {
                QImageReader reader(fileName);
                QImage image = reader.read();
                result = !image.isNull();
                if(result)
                {
                    originalImage = image;
                    _dialog->_workImage = new Image(&image);
                    _dialog->_workImage->reinterpretFormat(targetFormat);
                    _dialog->_image = _dialog->_workImage;
                    _dialog->_itemImage = nullptr;
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
        if(targetFormat != Image::ImageFormatAutomatic)
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
    _priv->_dialog = NULL;
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

ImagePreviewDialog::ColorFilter ImagePreviewDialog::ImagePreviewDialogImpl::currentColorFilter() const
{
    int index = colorFilterComboBox->currentIndex();
    ColorFilter ret = (ColorFilter)colorFilterComboBox->itemData(index, Qt::UserRole).toInt();
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

	refreshStatistics(qimg);

    _priv->ui->imageLabel->setText(QString());
    _priv->originalImage = qimg;
    if(!qimg.isNull())
    {
        QImage qimgDisplay;
        ColorFilter filter = _priv->currentColorFilter();
        applyColorFilterQImage(qimg, qimgDisplay, filter);
        QString fragment, vertex;
        if(getColorFilterShaderCode(filter, fragment, vertex))
        {
            _priv->ui->colorFilterFragment->blockSignals(true);
            _priv->ui->colorFilterVertex->blockSignals(true);
            _priv->ui->colorFilterFragment->setPlainText(fragment);
            _priv->ui->colorFilterVertex->setPlainText(vertex);
            _priv->ui->colorFilterFragment->blockSignals(false);
            _priv->ui->colorFilterVertex->blockSignals(false);
            _priv->ui->imageGL->setColorFilter(fragment, vertex);
        }

        QImage actualImage;
        if(_priv->flipHorizontalAction->isChecked() || _priv->flipVerticalAction->isChecked())
            actualImage = qimgDisplay.mirrored(_priv->flipHorizontalAction->isChecked(), _priv->flipVerticalAction->isChecked());
        else
            actualImage = qimgDisplay;
        _priv->ui->imageGL->setMirrored(_priv->flipHorizontalAction->isChecked(), _priv->flipVerticalAction->isChecked());

        _priv->imageWidth->setCurrentText(QString::number(_image->width()));
        _priv->imageHeight->setCurrentText(QString::number(_image->height()));
        _priv->ui->imageLabel->setImage(actualImage);
    }
    else
    {
        _priv->imageWidth->setCurrentText(tr("N/A"));
        _priv->imageHeight->setCurrentText(tr("N/A"));
        _priv->ui->imageLabel->setImage(qimg);
    }

    {
        int width = _workImage.valid() ? _workImage->width() : 0;
        int height = _workImage.valid() ? _workImage->height() : 0;
        _priv->ui->scrollAreaImageQt->horizontalScrollBar()->setMaximum(width);
        _priv->ui->scrollAreaImageQt->verticalScrollBar()->setMaximum(height);
        _priv->ui->scrollAreaImageQt->horizontalScrollBar()->setPageStep((width / 10));
        _priv->ui->scrollAreaImageQt->verticalScrollBar()->setMaximum((height / 10));
        _priv->ui->scrollAreaImageQt->horizontalScrollBar()->setValue(0);
        _priv->ui->scrollAreaImageQt->verticalScrollBar()->setValue(0);
        _priv->ui->scrollAreaImageQt->horizontalScrollBar()->setValue(0);
        _priv->ui->scrollAreaImageQt->verticalScrollBar()->setValue(0);
        _priv->ui->scrollAreaImageGL->horizontalScrollBar()->setMaximum(width);
        _priv->ui->scrollAreaImageGL->verticalScrollBar()->setMaximum(height);
        _priv->ui->scrollAreaImageGL->horizontalScrollBar()->setPageStep((width / 10));
        _priv->ui->scrollAreaImageGL->verticalScrollBar()->setMaximum((height / 10));
        _priv->ui->scrollAreaImageGL->horizontalScrollBar()->setValue(0);
        _priv->ui->scrollAreaImageGL->verticalScrollBar()->setValue(0);
        bool fitToWindow = _priv->fitToWindowAction->isChecked();
        _priv->ui->scrollAreaImageQt->setWidgetResizable(fitToWindow);
        _priv->ui->scrollAreaImageGL->setWidgetResizable(fitToWindow);
    }

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
    _priv->ui->mouseinfo->setText(QString());
    _priv->setImageInfo(_workImage.get());
    _priv->setNodeInfo(_item.get());
    _priv->updateToolbar();
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
    if (!_image.valid())
        _workImage = NULL;
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
        _workImage = NULL;
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

void ImagePreviewDialog::onMouseMoved(float x, float y)
{
    QString str;
    if (_workImage.valid())
    {
        int px_x = std::max(0, qRound(x * _workImage->width()));
        int px_y = std::max(0, qRound(y * _workImage->height()));

        if(_priv->flipHorizontalAction->isChecked())
            px_x = _workImage->width() - px_x;
        if(_priv->flipVerticalAction->isChecked())
            px_y = _workImage->height() - px_y;

        QString px_value;
        QString px_value_second;

        switch (_workImage->format())
        {
        case Image::ImageFormatABGR32:
        case Image::ImageFormatARGB32:
        case Image::ImageFormatBGRA32:
        case Image::ImageFormatRGBA32:
        case Image::ImageFormatBGR32:
        case Image::ImageFormatRGB32:
            {
                const quint32 * px = _workImage->pixel<quint32>(px_x, px_y);
                if (px)
                {
                    const quint8 * pxe = (const quint8 *)px;
                    QColor color;
                    switch (_workImage->format())
                    {
                    case Image::ImageFormatBGR32:
                    case Image::ImageFormatABGR32: color.setRgb(pxe[3], pxe[2], pxe[1], pxe[0]); break;
                    case Image::ImageFormatRGB32:
                    case Image::ImageFormatARGB32: color.setRgb(pxe[1], pxe[2], pxe[3], pxe[0]); break;
                    case Image::ImageFormatBGRA32: color.setRgb(pxe[2], pxe[1], pxe[1], pxe[3]); break;
                    case Image::ImageFormatRGBA32: color.setRgb(pxe[0], pxe[1], pxe[2], pxe[3]); break;
                    default: Q_ASSERT(false); break;
                    }
                    px_value = QColor(color).name();

                    switch (_workImage->dataType())
                    {
                    case Image::DataTypeUnsignedByte:
                    case Image::DataTypeSignedByte:
                        for(unsigned i = 0; i < 4; ++i)
                        {
                            if(i>0)
                                px_value_second += QChar(',');
                            px_value_second += QString::number(pxe[i]);
                        }
                        break;
                    case Image::DataTypeUnsignedShort:
                    case Image::DataTypeSignedShort:
                        for(unsigned i = 0; i < 2; ++i)
                        {
                            if(i>0)
                                px_value_second += QChar(',');
                            px_value_second += QString::number(((const quint16 *)px)[i]);
                        }
                        break;
                    case Image::DataTypeUnsignedInt:
                    case Image::DataTypeSignedInt:
                        px_value_second += QString::number(*px);
                        break;
                    case Image::DataTypeFloat32:
                        px_value_second += QString::number(*(const float*)px);
                        break;
                    case Image::DataTypeFloat64:
                        px_value_second += QString::number(*(const double*)px);
                        break;
                    }
                }
                else
                    px_value = tr("N/A");
            }
            break;
        case Image::ImageFormatBGR24:
        case Image::ImageFormatRGB24:
            {
                const quint8 * pxe = _workImage->pixel<quint8>(px_x, px_y);
                if (pxe)
                {
                    QColor color;
                    switch (_workImage->format())
                    {
                    case Image::ImageFormatBGR24:color.setRgb(pxe[2], pxe[1], pxe[0], 255); break;
                    case Image::ImageFormatRGB24: color.setRgb(pxe[0], pxe[1], pxe[2], 255); break;
                    default: Q_ASSERT(false); break;
                    }
                    px_value = QColor(color).name();
                }
                else
                    px_value = tr("N/A");
            }
            break;
        case Image::ImageFormatDepth:
        case Image::ImageFormatFloat:
            {
                const float * px = _workImage->pixel<float>(px_x, px_y);
                px_value = px ? QString::number(*px) : tr("N/A");
            }
            break;
        case Image::ImageFormatRed:
        case Image::ImageFormatGreen:
        case Image::ImageFormatBlue:
        case Image::ImageFormatAlpha:
        case Image::ImageFormatGray:
        case Image::ImageFormatLuminance:
        case Image::ImageFormatLuminanceAlpha:
            {
                switch (_workImage->dataType())
                {
                case Image::DataTypeUnsignedByte:
                case Image::DataTypeSignedByte:
                    {
                        const unsigned char * px = _workImage->pixel<unsigned char>(px_x, px_y);
                        px_value = px ? QString::number(*px) : tr("N/A");
                    }
                    break;
                case Image::DataTypeUnsignedShort:
                case Image::DataTypeSignedShort:
                    {
                        const unsigned short * px = _workImage->pixel<unsigned short>(px_x, px_y);
                        px_value = px ? QString::number(*px) : tr("N/A");
                    }
                    break;
                case Image::DataTypeUnsignedInt:
                case Image::DataTypeSignedInt:
                    {
                        const unsigned int * px = _workImage->pixel<unsigned int>(px_x, px_y);
                        px_value = px ? QString::number(*px) : tr("N/A");
                    }
                    break;
                case Image::DataTypeFloat32:
                    {
                        const float * px = _workImage->pixel<float>(px_x, px_y);
                        px_value = px ? QString::number(*px) : tr("N/A");
                    }
                    break;
                case Image::DataTypeFloat64:
                    {
                        const double * px = _workImage->pixel<double>(px_x, px_y);
                        px_value = px ? QString::number(*px) : tr("N/A");
                    }
                    break;
                }
            }
            break;
        }

        str = tr("X=%1, Y=%2, value=%3").arg(px_x).arg(px_y).arg(px_value);
        if(!px_value_second.isEmpty())
            str += QChar(',') + px_value_second;
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

} // namespace sgi
