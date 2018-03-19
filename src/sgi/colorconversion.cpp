#include "stdafx.h"
#include "colorconversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

//-- Default constructor:
ColorGradient::ColorGradient(NamedGradient type)
{
    switch (type)
    {
    default:
    case FivePoint: createFivePointGradient(); break;
    case Monochrome: createMonochromeGradient(); break;
    case MonochromeInverse: createMonochromeInverseGradient(); break;
    case Heat: createHeatGradient(); break;
    }
}

//-- Inserts a new color point into its correct position:
void ColorGradient::addColorPoint(float red, float green, float blue, float value)
{
    for (unsigned i = 0; i < color.size(); i++) {
        if (value < color[i].val) {
            color.insert(color.begin() + i, ColorPoint(red, green, blue, value));
            return;
        }
    }
    color.emplace_back(ColorPoint(red, green, blue, value));
}

void ColorGradient::clear()
{
    color.clear();
}

bool ColorGradient::empty() const
{
    return color.empty();
}

//-- Places a 5 color heatmap gradient into the "color" vector:
void ColorGradient::createFivePointGradient()
{
    color.clear();
    color.emplace_back(ColorPoint(0, 0, 1, 0.0f));      // Blue.
    color.emplace_back(ColorPoint(0, 1, 1, 0.25f));     // Cyan.
    color.emplace_back(ColorPoint(0, 1, 0, 0.5f));      // Green.
    color.emplace_back(ColorPoint(1, 1, 0, 0.75f));     // Yellow.
    color.emplace_back(ColorPoint(1, 0, 0, 1.0f));      // Red.
    invalidColor = ColorPoint(1, 0, 0, 0.0f); // black
}

void ColorGradient::createMonochromeGradient()
{
    color.clear();
    color.emplace_back(ColorPoint(0, 0, 0, 0.0f));      // Black.
    color.emplace_back(ColorPoint(1, 1, 1, 1.0f));      // White.
    invalidColor = ColorPoint(0, 1, 1, 0.0f);     // Cyan.
}

void ColorGradient::createMonochromeInverseGradient()
{
    color.clear();
    color.emplace_back(ColorPoint(1, 1, 1, 0.0f));      // White.
    color.emplace_back(ColorPoint(0, 0, 0, 1.0f));      // Black.
    invalidColor = ColorPoint(0, 1, 1, 0.0f);     // Cyan.
}

void ColorGradient::createHeatGradient()
{
    color.clear();
    color.emplace_back(ColorPoint(0, 0, 1, 0.0f));      // Blue.
    color.emplace_back(ColorPoint(1, 0, 0, 1.0f));      // Red.
    invalidColor = ColorPoint(0, 1, 0, 0.0f);      // Green.
}

//-- Inputs a (value) between 0 and 1 and outputs the (red), (green) and (blue)
//-- values representing that position in the gradient.
void ColorGradient::getColorAtValue(const float value, float &red, float &green, float &blue) const
{
    if (color.empty())
        return;

    for (unsigned i = 0; i < color.size(); i++)
    {
        const ColorPoint &currC = color[i];
        if (value < currC.val)
        {
            const ColorPoint &prevC = color[ (i > 0) ? (i-1) : 0 ];
            float valueDiff = (prevC.val - currC.val);
            float fractBetween = (valueDiff == 0) ? 0 : (value - currC.val) / valueDiff;
            red = (prevC.r - currC.r)*fractBetween + currC.r;
            green = (prevC.g - currC.g)*fractBetween + currC.g;
            blue = (prevC.b - currC.b)*fractBetween + currC.b;
            return;
        }
    }
    red = color.back().r;
    green = color.back().g;
    blue = color.back().b;
    return;
}

QRgb ColorGradient::getColor(const float value) const
{
    float r = 0, g = 0, b = 0;
    getColorAtValue(value, r, g, b);
    return qRgba(r * 255.0f, g * 255.0f, b * 255.0f, 255);
}
void ColorGradient::getInvalidColor(float &red, float &green, float &blue) const
{
    red = invalidColor.r;
    green = invalidColor.g;
    blue = invalidColor.b;
}
QRgb ColorGradient::getInvalidColor() const
{
    return qRgba(invalidColor.r * 255.0f, invalidColor.g * 255.0f, invalidColor.b * 255.0f, 255);
}

//------------------------------------------------------------------------------
// ColorFilter
//------------------------------------------------------------------------------

namespace {

    static bool filterTranslateGrayscale(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = qGray(src_pixel) & 0xff;
        dest_pixel = qRgb(v, v, v);
        return true;
    }
    static bool filterTranslateGrayscaleInverse(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = 0xff - (qGray(src_pixel) & 0xff);
        dest_pixel = qRgb(v, v, v);
        return true;
    }
    static bool filterTranslateRed(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = qRed(src_pixel) & 0xff;
        dest_pixel = qRgb(v, 0, 0);
        return true;
    }
    static bool filterTranslateGreen(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = qGreen(src_pixel) & 0xff;
        dest_pixel = qRgb(0, v, 0);
        return true;
    }
    static bool filterTranslateBlue(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = qBlue(src_pixel) & 0xff;
        dest_pixel = qRgb(0, 0, v);
        return true;
    }
    static bool filterTranslateAlpha(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = QColor(src_pixel).lightness();
        // display the luminance channel as grayscale image
        dest_pixel = qRgb(v, v, v);
        return true;
    }

    static bool filterTranslateLuminance(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = qAlpha(src_pixel) & 0xff;
        // display the alpha channel as grayscale image
        dest_pixel = qRgb(v, v, v);
        return true;
    }
    static bool filterTranslateHue(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = QColor(src_pixel).hslHue();
        // display the hue channel as grayscale image
        dest_pixel = qRgb(v, v, v);
        return true;
    }
    static bool filterTranslateSaturation(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        int v = QColor(src_pixel).hslSaturation();
        // display the saturation channel as grayscale image
        dest_pixel = qRgb(v, v, v);
        return true;
    }
    static bool filterTranslateSwap(const QRgb & src_pixel, QRgb & dest_pixel)
    {
        if (src_pixel == 0)
            dest_pixel = qRgba(255, 255, 255, 255);
        else
        {
            int v = (qRed(src_pixel) + qGreen(src_pixel) + qBlue(src_pixel) + qAlpha(src_pixel)) / 4;
            dest_pixel = qRgba(v, v, v, 255);
        }
        return true;
    }
} // namespace

ColorFilter ColorFilter::s_filters[] = {
    /* FilterNone */ ColorFilter("None"),
    /* FilterGrayscale */ ColorFilter("Grayscale", QString(), "float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));\r\ncolor = vec4(vec3(gray), color.a);", filterTranslateGrayscale),
    /* FilterGrayscaleInverse */ ColorFilter("Grayscale Inverse", QString(), "float gray = 1.0 - dot(color.rgb, vec3(0.299, 0.587, 0.114));\r\ncolor = vec4(vec3(gray), color.a);", filterTranslateGrayscaleInverse),
    /* FilterRed */ ColorFilter("Red", QString(), "color = vec4(color.r, 0.0, 0.0, color.a);", filterTranslateRed),
    /* FilterGreen */ ColorFilter("Green", QString(), "color = vec4(0.0, color.g, 0.0, color.a);", filterTranslateGreen),
    /* FilterBlue */ ColorFilter("Blue", QString(), "color = vec4(0.0, 0.0, color.b, color.a);", filterTranslateBlue),
    /* FilterAlpha */ ColorFilter("Alpha", QString(), "color = vec4(color.a, color.a, color.a, 1.0);", filterTranslateAlpha),
    /* FilterLuminance */ ColorFilter("Luminance ", QString(), "float lum = dot(color.rgb, vec3(0.2125, 0.7154, 0.0721));\r\ncolor = vec4(vec3(lum), 1.0);", filterTranslateLuminance),
    /* FilterHue */ ColorFilter("Hue", QString(), "", filterTranslateHue),
    /* FilterSaturation */ ColorFilter("Saturation", QString(), "", filterTranslateSaturation),
    /* FilterSwap */ ColorFilter("Swap", QString(), "color.rgba = color==vec4(0)? vec4(1) : vec4(vec3((color.r+color.g+color.b+color.a)/4.0),1);", filterTranslateSwap),
    /* FilterCustom1 */ ColorFilter("Custom 1"),
    /* FilterCustom2 */ ColorFilter("Custom 2"),
    /* FilterCustom3 */ ColorFilter("Custom 3"),
};

ColorFilter::ColorFilter(const QString & name, const QString & vertexShader, const QString & fragmentShader, pixelTranslateFunc translate)
    : _name(name), _vertexShader(vertexShader), _fragmentShader(fragmentShader), _pixelTranslate(translate)
{
    int index = this - &s_filters[0];
    _type = (ColorFilter::FilterType)index;
}

const ColorFilter::FilterType & ColorFilter::type() const
{
    return _type;
}

const QString & ColorFilter::name() const
{
    return _name;
}

const QString & ColorFilter::vertexShader() const
{
    return _vertexShader;
}

const QString & ColorFilter::fragmentShader() const
{
    return _fragmentShader;
}

bool ColorFilter::apply(const QImage & src, QImage & dest) const
{
    if (src.isNull())
        return false;

    if (!_pixelTranslate)
        return false;

    bool ret = true;
    dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);
    for (int y = 0; y < src.height(); y++) {
        for (int x = 0; x < src.width(); x++) {
            QRgb src_pixel = src.pixel(x, y);
            QRgb dest_pixel = 0;
            if (!_pixelTranslate(src_pixel, dest_pixel))
                ret = false;
            dest.setPixel(x, y, dest_pixel);
        }
    }
    return ret;
}

const ColorFilter & ColorFilter::getFilter(FilterType type)
{
    if ((int)type < 0 || type > FilterMax)
        return s_filters[0];
    else
        return s_filters[(unsigned)type];
}

unsigned ColorFilter::numberOfFilters()
{
    return sizeof(s_filters) / sizeof(s_filters[0]);
}

} // namespace sgi

