#version 130

const int ImageFormatAutomatic = -2;
const int ImageFormatInvalid = -1;
const int ImageFormatRGB24 = 0;
const int ImageFormatRGB32 = 1;
const int ImageFormatARGB32 = 2;
const int ImageFormatMono = 3;
const int ImageFormatMonoLSB = 4;
const int ImageFormatIndexed8 = 5;
const int ImageFormatFloat = 6;
const int ImageFormatBGR24 = 7;
const int ImageFormatBGR32 = 8;
const int ImageFormatABGR32 = 9;
const int ImageFormatYUV420 = 10;
const int ImageFormatYUV422 = 11;
const int ImageFormatYUV444 = 12;
const int ImageFormatYUYV = 13;
const int ImageFormatUYVY = 14;
const int ImageFormatGray = 15;
const int ImageFormatRed = 16;
const int ImageFormatGreen = 17;
const int ImageFormatBlue = 18;
const int ImageFormatAlpha = 19;
const int ImageFormatDepth = 20;
const int ImageFormatLuminance = 21;
const int ImageFormatLuminanceAlpha = 22;
const int ImageFormatDXT1 = 23;
const int ImageFormatDXT1Alpha = 24;
const int ImageFormatDXT3 = 25;
const int ImageFormatDXT5 = 26;
const int ImageFormatRGBA32 = 27;
const int ImageFormatBGRA32 = 28;
const int ImageFormatFloat64 = 29;
const int ImageFormatRaw = 1000;

uniform sampler2D oe_sg_sampler0;
uniform int format;
uniform vec2 texture_scale;
uniform vec2 pixel_size;
in vec2 oe_sg_texcoord0;
out vec4 color;

vec4 rgb24_planar()
{
    vec2 real_tx_r = vec2(oe_sg_texcoord0.s * texture_scale.s, (oe_sg_texcoord0.t * texture_scale.t)/3.0);
    vec2 real_tx_g = vec2(oe_sg_texcoord0.s * texture_scale.s, (1.0/3.0) + (oe_sg_texcoord0.t * texture_scale.t)/3.0);
    vec2 real_tx_b = vec2(oe_sg_texcoord0.s * texture_scale.s, (2.0/3.0) + (oe_sg_texcoord0.t * texture_scale.t)/3.0);
    float red = texture(oe_sg_sampler0, real_tx_r ).x;
    float green = texture(oe_sg_sampler0, real_tx_g).x;
    float blue = texture(oe_sg_sampler0, real_tx_b).x;
    return vec4(red, green, blue, 1.0);
}

vec4 yuv_to_rgb_hd(float Y, float Cr, float Cb)
{
    float y_scaled = 1.164383 * Y; //(Y - 0.0625);
    float red = clamp(y_scaled + (1.596027 * Cb), 0.0, 1.0);
    float green = clamp(y_scaled - (0.812968 * Cb) - (0.391762 * Cr), 0.0, 1.0);
    float blue = clamp(y_scaled + (2.017232 * Cr), 0.0, 1.0);

    return vec4(red, green, blue, 1.0);
}

vec4 yuv_to_rgb_sd(float Y, float Cr, float Cb)
{
    float red = clamp(Y + (1.402 * Cb), 0.0, 1.0);
    float green = clamp(Y - (0.34414 * Cr) - (0.71414 * Cb), 0.0, 1.0);
    float blue = clamp(Y + (1.772 * Cr), 0.0, 1.0);

    return vec4(red, green, blue, 1.0);
}

vec4 yuyv()
{
    // same as uyvy() but with luma in first and third position
    vec2 real_tx = vec2(oe_sg_texcoord0.s * texture_scale.s, oe_sg_texcoord0.t * texture_scale.t);
    vec4 c = texture(oe_sg_sampler0, real_tx);
    float Y = 0.0;
    float real_s_px = real_tx.s / pixel_size.s;
    if (mod(real_s_px, 2.0) != 0.0) {
        Y = c[2];
    }
    else {
        Y = c[0];
    }
    float Cr = c[1] - 0.5;
    float Cb = c[3] - 0.5;
    //Cr = 0.0;
    //Cb = 0.0;
    return yuv_to_rgb_hd(Y, Cr, Cb);
}

vec4 uyvy()
{
    // same as yuyv() but with luma in second and fourth position
    vec2 real_tx = vec2(oe_sg_texcoord0.s * texture_scale.s, oe_sg_texcoord0.t * texture_scale.t);
    vec4 c = texture(oe_sg_sampler0, real_tx);
    float Y = 0.0;
    float real_s_px = real_tx.s / pixel_size.s;
    if (mod(real_s_px, 2.0) != 0.0) {
        Y = c[3];
    }
    else {
        Y = c[1];
    }
    float Cr = c[0] - 0.5;
    float Cb = c[2] - 0.5;
    //Cr = 0.0;
    //Cb = 0.0;
    return yuv_to_rgb_hd(Y, Cr, Cb);
}

vec4 yuv422p()
{
    vec2 real_tx_y  = vec2(oe_sg_texcoord0.s * texture_scale.s      , (oe_sg_texcoord0.t * texture_scale.t)/2.0);
    vec2 real_tx_cr = vec2(oe_sg_texcoord0.s * texture_scale.s / 2.0, (0.50) + (oe_sg_texcoord0.t * texture_scale.t)/4.0);
    vec2 real_tx_cb = vec2(oe_sg_texcoord0.s * texture_scale.s / 2.0, (0.75) + (oe_sg_texcoord0.t * texture_scale.t)/4.0);
    float Y  = texture(oe_sg_sampler0, real_tx_y ).x;
    float Cr = texture(oe_sg_sampler0, real_tx_cr).x - 0.5;
    float Cb = texture(oe_sg_sampler0, real_tx_cb).x - 0.5;
    //Cr = 0.0;
    //Cb = 0.0;
    return yuv_to_rgb_hd(Y, Cr, Cb);
}

vec4 yuv420p()
{
    float one_sixth = 1.0 / 6.0;
    float two_third = 2.0 / 3.0;
    vec2 real_tx_y  = vec2(oe_sg_texcoord0.s * texture_scale.s     , oe_sg_texcoord0.t * texture_scale.t * two_third);
    vec2 real_tx_cr = vec2(oe_sg_texcoord0.s * texture_scale.s / 2.0, (two_third) + (oe_sg_texcoord0.t * texture_scale.t * one_sixth));
    vec2 real_tx_cb = vec2(oe_sg_texcoord0.s * texture_scale.s / 2.0, (two_third + one_sixth) + (oe_sg_texcoord0.t * texture_scale.t * one_sixth));
    float Y  = texture(oe_sg_sampler0, real_tx_y ).x;
    float Cr = texture(oe_sg_sampler0, real_tx_cr).x - 0.5;
    float Cb = texture(oe_sg_sampler0, real_tx_cb).x - 0.5;
    //Cr = 0.0;
    //Cb = 0.0;
    return yuv_to_rgb_hd(Y, Cr, Cb);
}

vec4 yuv444p()
{
    vec2 real_tx_y =  vec2(oe_sg_texcoord0.s * texture_scale.s, (oe_sg_texcoord0.t * texture_scale.t)/3.0);
    vec2 real_tx_cr = vec2(oe_sg_texcoord0.s * texture_scale.s, (1.0/3.0) + (oe_sg_texcoord0.t * texture_scale.t)/3.0);
    vec2 real_tx_cb = vec2(oe_sg_texcoord0.s * texture_scale.s, (2.0/3.0) + (oe_sg_texcoord0.t * texture_scale.t)/3.0);
    float Y  = texture(oe_sg_sampler0, real_tx_y ).x;
    float Cr = texture(oe_sg_sampler0, real_tx_cr).x - 0.5;
    float Cb = texture(oe_sg_sampler0, real_tx_cb).x - 0.5;
    //Cr = 0.0;
    //Cb = 0.0;
    return yuv_to_rgb_hd(Y, Cr, Cb);
}

vec4 rgb24()
{
    vec2 real_tx = vec2(oe_sg_texcoord0.s * texture_scale.s, oe_sg_texcoord0.t * texture_scale.t);
    vec3 c = texture(oe_sg_sampler0, real_tx ).rgb;
    return vec4(c.r, c.g, c.b, 1.0);
}

vec4 rgb32()
{
    vec2 real_tx = vec2(oe_sg_texcoord0.s * texture_scale.s, oe_sg_texcoord0.t * texture_scale.t);
    vec3 c = texture(oe_sg_sampler0, real_tx ).rgb;
    return vec4(c.r, c.g, c.b, 1.0);
}

vec4 argb32()
{
    vec2 real_tx = vec2(oe_sg_texcoord0.s * texture_scale.s, oe_sg_texcoord0.t * texture_scale.t);
    return texture(oe_sg_sampler0, real_tx );
}

vec4 applyColorFilter(vec4 color)
{
    $GLSL_COLOR_FILTER_FRAGMENT
    return color;
}

void main()
{
    if(oe_sg_texcoord0.s < 0.0)
        color = vec4(0.0, 0.0, 0.0, 1.0);
    else
    {
        switch (format)
        {
        default:
        case ImageFormatInvalid:
            color = vec4(1.0, 0.0, 0.0, 1.0);
            break;
        case ImageFormatAutomatic:
            color = vec4(0.0, 0.0, 1.0, 1.0);
            break;
        case ImageFormatBGR24:
        case ImageFormatRGB24:
            // handle bgr and rgb identical because color order is already fixed by OpenGL
            color = rgb24();
            break;
        case ImageFormatABGR32:
        case ImageFormatARGB32:
        case ImageFormatRGBA32:
        case ImageFormatBGRA32:
            // handle bgr and rgb identical because color order is already fixed by OpenGL
            color = argb32();
            break;
        case ImageFormatBGR32:
        case ImageFormatRGB32:
            // handle bgr and rgb identical because color order is already fixed by OpenGL
            color = rgb32();
            break;
        case ImageFormatYUV420:
            color = yuv420p();
            break;
        case ImageFormatYUV422:
            color = yuv422p();
            break;
        case ImageFormatYUV444:
            color = yuv444p();
            break;
        case ImageFormatYUYV:
            color = yuyv();
            break;
        case ImageFormatUYVY:
            color = uyvy();
            break;
        case ImageFormatDXT1:
            color = rgb24();
            break;
        case ImageFormatDXT1Alpha:
        case ImageFormatDXT3:
        case ImageFormatDXT5:
            color = argb32();
            break;
        }
        color = applyColorFilter(color);
    }
}

