#pragma once

namespace sgi {

/// @see http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
class ColorGradient
{
private:
    struct ColorPoint  // Internal class used to store colors at different points in the gradient.
    {
        float r, g, b;      // Red, green and blue values of our color.
        float val;        // Position of our color along the gradient (between 0 and 1).
        ColorPoint(float red=0, float green=0, float blue=0, float value=0)
            : r(red), g(green), b(blue), val(value) {}
    };
    std::vector<ColorPoint> color;      // An array of color points in ascending value.
    ColorPoint invalidColor;

public:
    /// @see https://en.wikipedia.org/wiki/Heat_map
    enum NamedGradient {
        FivePoint,
        Monochrome,
        MonochromeInverse,
        Heat,
    };
    //-- Default constructor:
    ColorGradient(NamedGradient type=FivePoint);

    //-- Inserts a new color point into its correct position:
    void addColorPoint(float red, float green, float blue, float value);

    void clear();
    bool empty() const;

    //-- Places a 5 color heatmap gradient into the "color" vector:
    void createFivePointGradient();

    void createMonochromeGradient();
    void createMonochromeInverseGradient();
    void createHeatGradient();

    //-- Inputs a (value) between 0 and 1 and outputs the (red), (green) and (blue)
    //-- values representing that position in the gradient.
    void getColorAtValue(const float value, float &red, float &green, float &blue) const;

    QRgb getColor(const float value) const;
    void getInvalidColor(float &red, float &green, float &blue) const;
    QRgb getInvalidColor() const;
};

} // namespace sgi
