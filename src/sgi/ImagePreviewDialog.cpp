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
#include <QLibrary>

#include "ImagePreviewDialog.moc"
#include "ImagePreviewDialog.h"
#include "ui_ImagePreviewDialog.h"
#include "SGIPlugin.h"
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/helpers/qt>
#include <sgi/helpers/osg>

#ifdef SGI_USE_FFMPEG
namespace ffmpeg {
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
}
#endif

#include <iostream>

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

bool selectDisplayChannel(QImage & src, QImage & dest, ImagePreviewDialog::DisplayChannel channel)
{
    if(src.isNull())
        return false;

    if(channel == ImagePreviewDialog::DisplayChannelAll)
        dest = src;
    else
    {
        dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);
        for (int y = 0; y < src.height(); y++) {
            for (int x = 0; x < src.width(); x++) {
                QRgb src_pixel = src.pixel(x, y);
                QRgb dest_pixel = 0;
                switch(channel)
                {
                case ImagePreviewDialog::DisplayChannelAll:
                    dest_pixel = src_pixel;
                    break;
                case ImagePreviewDialog::DisplayChannelGrayscale:
                    {
                        int v = qGray(src_pixel) & 0xff;
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelRed:
                    {
                        int v = qRed(src_pixel) & 0xff;
                        dest_pixel = qRgb(v, 0, 0);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelGreen:
                    {
                        int v = qGreen(src_pixel) & 0xff;
                        dest_pixel = qRgb(0, v, 0);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelBlue:
                    {
                        int v = qBlue(src_pixel) & 0xff;
                        dest_pixel = qRgb(0, 0, v);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelAlpha:
                    {
                        int v = qAlpha(src_pixel) & 0xff;
                        // display the alpha channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelLuminance:
                    {
                        int v = QColor(src_pixel).lightness();
                        // display the luminance channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelHue:
                    {
                        int v = QColor(src_pixel).hslHue();
                        // display the hue channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                case ImagePreviewDialog::DisplayChannelSaturation:
                    {
                        int v = QColor(src_pixel).hslSaturation();
                        // display the saturation channel as grayscale image
                        dest_pixel = qRgb(v, v, v);
                    }
                    break;
                }
                dest.setPixel(x, y, dest_pixel);
            }
        }
    }
    return true;
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
    void setImageInfo(const std::string & infoText);

    Image::ImageFormat currentImageFormat() const;
    DisplayChannel currentDisplayChannel() const;

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
    void imageFormatChanged(int index);
    void displayChannelChanged(int index);

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
    QComboBox *                     imageFormat;
    QComboBox *                     displayChannel;
    QString                         labelText;
    double                          scaleFactor;
    bool                            initialRefresh;
	Histogram						histogram;
	bool							histogramReady;
};

std::map<Image::ImageFormat, QString> ImagePreviewDialog::ImagePreviewDialogImpl::ImageFormatDisplayText;

class SWScale
{
private:
    static QLibrary * library;
    static bool loadAttempted;
    static bool ready;
    typedef struct ffmpeg::SwsContext * (*pfn_sws_getContext)(int srcW, int srcH, enum ffmpeg::AVPixelFormat srcFormat,
                                  int dstW, int dstH, enum ffmpeg::AVPixelFormat dstFormat,
                                  int flags, ffmpeg::SwsFilter *srcFilter,
                                  ffmpeg::SwsFilter *dstFilter, const double *param);
    typedef int (*pfn_sws_scale)(struct ffmpeg::SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);
    typedef void (*pfn_sws_freeContext)(struct ffmpeg::SwsContext *swsContext);
    static pfn_sws_getContext sws_getContext;
    static pfn_sws_scale sws_scale;
    static pfn_sws_freeContext sws_freeContext;

    static bool load()
    {
        if(!loadAttempted)
        {
#ifdef _WIN32
            library = new QLibrary("swscale-4");
#else
            library = new QLibrary("swscale");
#endif
            if(library->load())
            {
                sws_getContext = (pfn_sws_getContext)library->resolve("sws_getContext");
                sws_scale = (pfn_sws_scale)library->resolve("sws_scale");
                sws_freeContext = (pfn_sws_freeContext)library->resolve("sws_freeContext");
                ready = (sws_getContext != NULL && sws_scale != NULL && sws_freeContext != NULL);
                std::cout << "Loaded swscale library " << library->fileName() << " ready=" << ready << std::endl;
            }
            else
            {
                std::cerr << "Failed to load swscale library: " << library->errorString() << std::endl;
            }
            loadAttempted = true;
        }
        return ready;
    }


    static Image::ImageFormat ImageFormatFromAVCodec(ffmpeg::AVPixelFormat pix_fmt)
    {
        Image::ImageFormat ret = Image::ImageFormatInvalid;
        switch (pix_fmt)
        {
        case ffmpeg::AV_PIX_FMT_NONE: ret = Image::ImageFormatInvalid; break;
        case ffmpeg::AV_PIX_FMT_RGB24: ret = Image::ImageFormatRGB24; break;
        case ffmpeg::AV_PIX_FMT_BGR24: ret = Image::ImageFormatBGR24; break;
        //case ffmpeg::AV_PIX_FMT_RGB32: ret = Image::ImageFormatRGB32; break;
        //case ffmpeg::AV_PIX_FMT_BGR32: ret = Image::ImageFormatBGR32; break;

        case ffmpeg::AV_PIX_FMT_PAL8:
        case ffmpeg::AV_PIX_FMT_ARGB:
        case ffmpeg::AV_PIX_FMT_RGBA:
            ret = Image::ImageFormatARGB32;
            break;
        case ffmpeg::AV_PIX_FMT_ABGR:
        case ffmpeg::AV_PIX_FMT_BGRA:
            ret = Image::ImageFormatABGR32;
            break;

        case ffmpeg::AV_PIX_FMT_YUVJ420P:
        case ffmpeg::AV_PIX_FMT_YUV420P: ret = Image::ImageFormatYUV420; break;
        case ffmpeg::AV_PIX_FMT_YUVJ422P:
        case ffmpeg::AV_PIX_FMT_YUV422P: ret = Image::ImageFormatYUV422; break;
        case ffmpeg::AV_PIX_FMT_YUVJ444P:
        case ffmpeg::AV_PIX_FMT_YUV444P: ret = Image::ImageFormatYUV444; break;
        case ffmpeg::AV_PIX_FMT_YUYV422: ret = Image::ImageFormatYUYV; break;
        case ffmpeg::AV_PIX_FMT_UYVY422: ret = Image::ImageFormatUYVY; break;
        case ffmpeg::AV_PIX_FMT_GRAY8: ret = Image::ImageFormatGray; break;
        case ffmpeg::AV_PIX_FMT_YA8: ret = Image::ImageFormatLuminanceAlpha; break;
        default:
            Q_ASSERT_X(false, __FUNCTION__, "Unhandled frame format from avcodec");
            break;
        }
        return ret;
    }
    static ffmpeg::AVPixelFormat ImageFormatToAVCodec(Image::ImageFormat format)
    {
        ffmpeg::AVPixelFormat ret = ffmpeg::AV_PIX_FMT_NONE;
        switch (format)
        {
        case Image::ImageFormatRGB24: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
        case Image::ImageFormatRGB32: ret = ffmpeg::AV_PIX_FMT_RGB32; break;
        case Image::ImageFormatARGB32: ret = ffmpeg::AV_PIX_FMT_ARGB; break;
        case Image::ImageFormatMono: ret = ffmpeg::AV_PIX_FMT_MONOBLACK; break;
        case Image::ImageFormatMonoLSB: ret = ffmpeg::AV_PIX_FMT_MONOBLACK; break;
        case Image::ImageFormatIndexed8: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
        //case Image::ImageFormatFloat: ret = ffmpeg::AV_PIX_FMT_FLOAT; break;
        case Image::ImageFormatBGR24: ret = ffmpeg::AV_PIX_FMT_BGR24; break;
        case Image::ImageFormatBGR32: ret = ffmpeg::AV_PIX_FMT_BGR32; break;
        case Image::ImageFormatABGR32: ret = ffmpeg::AV_PIX_FMT_ABGR; break;
        case Image::ImageFormatYUV420: ret = ffmpeg::AV_PIX_FMT_YUV420P; break;
        case Image::ImageFormatYUV422: ret = ffmpeg::AV_PIX_FMT_YUV422P; break;
        case Image::ImageFormatYUV444: ret = ffmpeg::AV_PIX_FMT_YUV444P; break;
        case Image::ImageFormatYUYV: ret = ffmpeg::AV_PIX_FMT_YUYV422; break;
        case Image::ImageFormatUYVY: ret = ffmpeg::AV_PIX_FMT_UYVY422; break;
        case Image::ImageFormatGray: ret = ffmpeg::AV_PIX_FMT_GRAY8; break;
        case Image::ImageFormatRed: ret = ffmpeg::AV_PIX_FMT_GRAY8; break;
        case Image::ImageFormatGreen: ret = ffmpeg::AV_PIX_FMT_GRAY8; break;
        case Image::ImageFormatBlue: ret = ffmpeg::AV_PIX_FMT_GRAY8; break;
        case Image::ImageFormatAlpha: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
        case Image::ImageFormatDepth: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
        case Image::ImageFormatLuminance: ret = ffmpeg::AV_PIX_FMT_GRAY8; break;
        case Image::ImageFormatLuminanceAlpha: ret = ffmpeg::AV_PIX_FMT_YA8; break;
        case Image::ImageFormatInvalid:
        default: ret = ffmpeg::AV_PIX_FMT_NONE; break;
        }
        return ret;
    }

    static bool convert(const sgi::Image& src, sgi::Image& dest)
    {
        ffmpeg::AVPixelFormat srcPixelFormat = ImageFormatToAVCodec(src.format());
        ffmpeg::AVPixelFormat destPixelFormat = ImageFormatToAVCodec(dest.format());
        if(srcPixelFormat == ffmpeg::AV_PIX_FMT_NONE)
            return false;

        const uint8_t* srcdata = reinterpret_cast<const uint8_t*>(src.data());
        const int srcPitch[4] = { (int)src.pitch(0), (int)src.pitch(1), (int)src.pitch(2), (int)src.pitch(3) };
        const unsigned srcPlaneOffset[4] = { src.planeOffset(0), src.planeOffset(1), src.planeOffset(2), src.planeOffset(3) };
        const uint8_t* srcPlanes[4] = { srcdata + srcPlaneOffset[0], srcdata + srcPlaneOffset[1], srcdata + srcPlaneOffset[2], srcdata + srcPlaneOffset[3] };

        uint8_t* destdata = reinterpret_cast<uint8_t*>(dest.data());
        const int destPitch[4] = { (int)dest.pitch(0), (int)dest.pitch(1), (int)dest.pitch(2), (int)dest.pitch(3) };
        const unsigned destPlaneOffset[4] = { dest.planeOffset(0), dest.planeOffset(1), dest.planeOffset(2), dest.planeOffset(3) };
        uint8_t* destPlanes[4] = { destdata + destPlaneOffset[0], destdata + destPlaneOffset[1], destdata + destPlaneOffset[2], destdata + destPlaneOffset[3] };

        Q_ASSERT(sws_getContext != NULL);
        ffmpeg::SwsContext * ctx = sws_getContext(src.width(), src.height(), srcPixelFormat,
            dest.width(), dest.height(), destPixelFormat,
            SWS_BILINEAR, NULL, NULL, NULL);
        sws_scale(ctx, srcPlanes, srcPitch, 0, src.height(), destPlanes, destPitch);
        sws_freeContext(ctx);
        return true;
    }

    static bool to_qimage_argb32(const sgi::Image& src, QImage& dest)
    {
        ffmpeg::AVPixelFormat srcPixelFormat = ImageFormatToAVCodec(src.format());
        if(srcPixelFormat == ffmpeg::AV_PIX_FMT_NONE)
            return false;

        const uint8_t* srcdata = reinterpret_cast<const uint8_t*>(src.data());
        const int srcPitch[4] = { (int)src.pitch(0), (int)src.pitch(1), (int)src.pitch(2), (int)src.pitch(3) };
        const unsigned srcPlaneOffset[4] = { src.planeOffset(0), src.planeOffset(1), src.planeOffset(2), src.planeOffset(3) };
        const uint8_t* srcPlanes[4] = { srcdata + srcPlaneOffset[0], srcdata + srcPlaneOffset[1], srcdata + srcPlaneOffset[2], srcdata + srcPlaneOffset[3] };

        dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);

        uint8_t* dstdata = reinterpret_cast<uint8_t*>(dest.bits());
        int dstPitch[1] = { (int)dest.bytesPerLine() };
        const unsigned dstPlaneOffset[1] = { 0  };
        uint8_t * dstPlanes[1] = { dstdata + dstPlaneOffset[0] };

        Q_ASSERT(sws_getContext != NULL);
        ffmpeg::SwsContext * ctx = sws_getContext(src.width(), src.height(), srcPixelFormat,
                                                  dest.width(), dest.height(),
                                                  // The QImage::Format_ARGB32 is the BGRA format on ffmpeg side, with the more
                                                  // obvious ffmpeg::AV_PIX_FMT_ARGB the red-blue colors are swapped.
                                                  ffmpeg::AV_PIX_FMT_BGRA,
                                                  SWS_BILINEAR, NULL, NULL, NULL);
        sws_scale(ctx, srcPlanes, srcPitch, 0, src.height(), dstPlanes, dstPitch);
        sws_freeContext(ctx);
        return true;
    }

public:
    static bool convert(const sgi::Image& src, Image::ImageFormat destFormat, sgi::Image& dest)
    {
        bool ret;
        if(destFormat == src.format() || destFormat == Image::ImageFormatAutomatic)
        {
            dest = src;
            ret = true;
        }
        else
        {
            ret = load();
            if(ret)
            {
                ret = dest.allocate(src.width(), src.height(), destFormat);
                if(ret)
                    ret = convert(src, dest);
            }
        }
        return ret;
    }
    static bool convertToQImage(const sgi::Image& src, Image::ImageFormat destFormat, QImage & dest)
    {
        bool ret;
        sgi::Image tmpImage;
        ret = convert(src, destFormat, tmpImage);
        if(ret)
        {
            ret = load();
            if(ret)
            {
                ret = to_qimage_argb32(tmpImage, dest);
            }
        }
        return ret;
    }
    static bool convertToQImage(const sgi::Image& src, QImage & dest)
    {
        bool ret;
        ret = load();
        if(ret)
        {
            ret = to_qimage_argb32(src, dest);
        }
        return ret;
    }
};

QLibrary * SWScale::library = NULL;
bool SWScale::loadAttempted = false;
bool SWScale::ready = false;
SWScale::pfn_sws_getContext SWScale::sws_getContext = NULL;
SWScale::pfn_sws_scale SWScale::sws_scale = NULL;
SWScale::pfn_sws_freeContext SWScale::sws_freeContext = NULL;

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
        ImageFormatDisplayText[Image::ImageFormatARGB32] = tr("ARGB 32-bit");
        ImageFormatDisplayText[Image::ImageFormatBGR24] = tr("BGR 24-bit");
        ImageFormatDisplayText[Image::ImageFormatBGR32] = tr("BGR 32-bit");
        ImageFormatDisplayText[Image::ImageFormatABGR32] = tr("ABGR 32-bit");
        ImageFormatDisplayText[Image::ImageFormatYUV420] = tr("YUV 4:2:0");
        ImageFormatDisplayText[Image::ImageFormatYUV422] = tr("YUV 4:2:2");
        ImageFormatDisplayText[Image::ImageFormatYUV444] = tr("YUV 4:4:4");
        ImageFormatDisplayText[Image::ImageFormatGray] = tr("Grayscale");
        ImageFormatDisplayText[Image::ImageFormatRed] = tr("Red");
        ImageFormatDisplayText[Image::ImageFormatGreen] = tr("Green");
        ImageFormatDisplayText[Image::ImageFormatBlue] = tr("Blue");
        ImageFormatDisplayText[Image::ImageFormatAlpha] = tr("Alpha");
    }
	ui = new Ui_ImagePreviewDialog;
	ui->setupUi(_dialog);

	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, _dialog, &ImagePreviewDialog::reject);

	ui->imageLabel->setBackgroundRole(QPalette::Base);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);

	createToolbar();

	int w = ui->splitter->width();
	QList<int> sizes = QList<int>() << (w * 3 / 4) << (w / 4);
	ui->splitter->setSizes(sizes);

    ui->scrollArea->setWidgetResizable(true);

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
    if(!_dialog->_image.valid())
    {
        if(_dialog->_hostInterface->convertToImage(_dialog->_itemImage, _dialog->_item.get()))
            _dialog->_image = _dialog->_itemImage;
    }
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

    bool fitToWindow = fitToWindowAction->isChecked();
    ui->scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
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

    imageFormat = new QComboBox(toolBar);
    imageFormat->setToolTip(tr("Re-interpret image format"));
    for(const auto & it : ImageFormatDisplayText)
        imageFormat->addItem(it.second, QVariant::fromValue((int)it.first));

    displayChannel = new QComboBox(toolBar);
    displayChannel->addItem(tr("All"), QVariant::fromValue((int)DisplayChannelAll));
    displayChannel->addItem(tr("Grayscale"), QVariant::fromValue((int)DisplayChannelGrayscale));
    displayChannel->addItem(tr("Red"), QVariant::fromValue((int)DisplayChannelRed));
    displayChannel->addItem(tr("Green"), QVariant::fromValue((int)DisplayChannelGreen));
    displayChannel->addItem(tr("Blue"), QVariant::fromValue((int)DisplayChannelBlue));
    displayChannel->addItem(tr("Alpha"), QVariant::fromValue((int)DisplayChannelAlpha));
    displayChannel->addItem(tr("Luminance"), QVariant::fromValue((int)DisplayChannelLuminance));
    displayChannel->addItem(tr("Hue"), QVariant::fromValue((int)DisplayChannelHue));
    displayChannel->addItem(tr("Saturation"), QVariant::fromValue((int)DisplayChannelSaturation));

    toolBar->addWidget(imageFormat);
    toolBar->addWidget(displayChannel);
    toolBar->addAction(refreshAction);
	toolBar->addAction(zoomInAction);
	toolBar->addAction(zoomOutAction);
	toolBar->addAction(normalSizeAction);
	toolBar->addAction(fitToWindowAction);
	toolBar->addAction(flipHorizontalAction);
	toolBar->addAction(flipVerticalAction);

    // do the connects at the end to avoid trouble when new items are added and signals fired
    connect(imageFormat, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::imageFormatChanged);
    connect(displayChannel, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImagePreviewDialogImpl::displayChannelChanged);

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

void ImagePreviewDialog::ImagePreviewDialogImpl::imageFormatChanged(int index)
{
    // just refresh the actual change is done in refreshImpl
    refresh();
}

void ImagePreviewDialog::ImagePreviewDialogImpl::displayChannelChanged(int index)
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
                    Image * loadedImage = new Image(&image);
                    loadedImage->reinterpretFormat(targetFormat);
                    _dialog->_image = loadedImage;
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
                        Image::Origin origin = Image::OriginDefault;
                        int width = 0;
                        int height = 0;
                        int depth = 0;
                        int bytesPerLine = 0;
                        _dialog->_image = new Image(targetFormat, origin, data.data(), data.size(), width, height, depth, bytesPerLine, (QImage*)NULL);
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
            result = SWScale::convert(*_dialog->_image.get(), targetFormat, tempImage);
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
    case QImage::Format_RGBX8888: os << "RGBX8888"; break;
    case QImage::Format_RGBA8888: os << "RGBA8888"; break;
    case QImage::Format_RGBA8888_Premultiplied: os << "RGBA8888_Premultiplied"; break;
    case QImage::Format_BGR30: os << "BGR30"; break;
    case QImage::Format_A2BGR30_Premultiplied: os << "A2BGR30_Premultiplied"; break;
    case QImage::Format_RGB30: os << "RGB30"; break;
    case QImage::Format_A2RGB30_Premultiplied: os << "A2RGB30_Premultiplied"; break;
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

ImagePreviewDialog::DisplayChannel ImagePreviewDialog::ImagePreviewDialogImpl::currentDisplayChannel() const
{
    int index = displayChannel->currentIndex();
    DisplayChannel ret = (DisplayChannel)displayChannel->itemData(index, Qt::UserRole).toInt();
    return ret;
}

#if !defined(SGI_USE_FFMPEG)
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
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            src++;
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
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            src++;
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
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src++;
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
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src++;
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
    QImage ret;
    if(!image)
        return ret;
    bool convertOk = false;
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
                ret = QImage((uchar*)image->data(), (int)image->width(), (int)image->height(), (int)image->pitch(0), qt_format);
                if (image->origin() == Image::OriginBottomLeft)
                    ret.mirrored(false, true);
            }
            else
            {
                QImage qimage;
                if (convertImageToQImage_RGB32(image, qimage))
                    ret = qimage;
            }
        }
        break;
    }
    return ret;
}
#endif // !defined(SGI_USE_FFMPEG)

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
    bool useReinterpretedImage = false;
    if(_image.valid())
    {
        _priv->reinterpretedImage = *_image.get();
        useReinterpretedImage = _priv->reinterpretedImage.reinterpretFormat(format);
    }
#if defined(SGI_USE_FFMPEG)
    QImage qimg;
    if(_image.valid())
        SWScale::convertToQImage( useReinterpretedImage ? _priv->reinterpretedImage : (*_image.get()), qimg);
#else
    QImage qimg = convertImageToQImage(useReinterpretedImage ? &_priv->reinterpretedImage : _image.get(), format);
#endif

	refreshStatistics(qimg);

    _priv->ui->imageLabel->setText(QString());
    _priv->originalImage = qimg;
    if(!qimg.isNull())
    {
        QImage qimgDisplay;
        DisplayChannel channel = _priv->currentDisplayChannel();
        selectDisplayChannel(qimg, qimgDisplay, channel);

        QPixmap actualDisplayedPixmap;
        if(_priv->flipHorizontalAction->isChecked() || _priv->flipVerticalAction->isChecked())
        {
            QImage mirroredImage = qimgDisplay.mirrored(_priv->flipHorizontalAction->isChecked(), _priv->flipVerticalAction->isChecked());
            actualDisplayedPixmap = QPixmap::fromImage(mirroredImage);
        }
        else
            actualDisplayedPixmap = QPixmap::fromImage(qimgDisplay);
        _priv->ui->imageLabel->setPixmap(actualDisplayedPixmap);
    }
    else
        _priv->ui->imageLabel->setPixmap(QPixmap());
    _priv->ui->imageLabel->setScaledContents(!_priv->fitToWindowAction->isChecked());

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
