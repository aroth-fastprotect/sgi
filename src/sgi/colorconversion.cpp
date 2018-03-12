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

} // namespace sgi

