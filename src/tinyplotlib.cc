#include "tinyplotlib.h"

#include <map>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#include "nanovg.h"

#define NANOVG_RT_IMPLEMENTATION
#define NANORT_IMPLEMENTATION
#include "nanovg_rt.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace tinyplotlib {

struct vec3 {
  vec3() {}
  vec3(float xx, float yy, float zz) {
    x = xx;
    y = yy;
    z = zz;
  }
  vec3(const float *p) {
    x = p[0];
    y = p[1];
    z = p[2];
  }

  vec3 operator*(float f) const { return vec3(x * f, y * f, z * f); }
  vec3 operator-(const vec3 &f2) const {
    return vec3(x - f2.x, y - f2.y, z - f2.z);
  }
  vec3 operator*(const vec3 &f2) const {
    return vec3(x * f2.x, y * f2.y, z * f2.z);
  }
  vec3 operator+(const vec3 &f2) const {
    return vec3(x + f2.x, y + f2.y, z + f2.z);
  }
  vec3 &operator+=(const vec3 &f2) {
    x += f2.x;
    y += f2.y;
    z += f2.z;
    return (*this);
  }
  vec3 operator/(const vec3 &f2) const {
    return vec3(x / f2.x, y / f2.y, z / f2.z);
  }
  float operator[](int i) const { return (&x)[i]; }
  float &operator[](int i) { return (&x)[i]; }

  vec3 neg() { return vec3(-x, -y, -z); }

  float length() { return sqrtf(x * x + y * y + z * z); }

  void normalize() {
    float len = length();
    if (std::fabs(len) > 1.0e-6f) {
      float inv_len = 1.0f / len;
      x *= inv_len;
      y *= inv_len;
      z *= inv_len;
    }
  }

  float x, y, z;
  // float pad;  // for alignment
};

inline vec3 operator*(float f, const vec3 &v) {
  return vec3(v.x * f, v.y * f, v.z * f);
}

// --------------------------------------------------------------------==
// https://www.shadertoy.com/view/WlfXRN
//
// fitting polynomials to matplotlib colormaps
//
// License CC0 (public domain)
//   https://creativecommons.org/share-your-work/public-domain/cc0/
//
// feel free to use these in your own work!
//
// similar to https://www.shadertoy.com/view/XtGGzG but with a couple small adjustments:
//
//  - use degree 6 instead of degree 5 polynomials
//  - use nested horner representation for polynomials
//  - polynomials were fitted to minimize maximum error (as opposed to least squares)
//
// data fitted from https://github.com/BIDS/colormap/blob/master/colormaps.py
// (which is licensed CC0)

static vec3 viridis(float t) {

    const vec3 c0 = vec3(0.2777273272234177f, 0.005407344544966578f, 0.3340998053353061f);
    const vec3 c1 = vec3(0.1050930431085774f, 1.404613529898575f, 1.384590162594685f);
    const vec3 c2 = vec3(-0.3308618287255563f, 0.214847559468213f, 0.09509516302823659f);
    const vec3 c3 = vec3(-4.634230498983486f, -5.799100973351585f, -19.33244095627987f);
    const vec3 c4 = vec3(6.228269936347081f, 14.17993336680509f, 56.69055260068105f);
    const vec3 c5 = vec3(4.776384997670288f, -13.74514537774601f, -65.35303263337234f);
    const vec3 c6 = vec3(-5.435455855934631f, 4.645852612178535f, 26.3124352495832f);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));

}

static vec3 plasma(float t) {

    const vec3 c0 = vec3(0.05873234392399702f, 0.02333670892565664f, 0.5433401826748754f);
    const vec3 c1 = vec3(2.176514634195958f, 0.2383834171260182f, 0.7539604599784036f);
    const vec3 c2 = vec3(-2.689460476458034f, -7.455851135738909f, 3.110799939717086f);
    const vec3 c3 = vec3(6.130348345893603f, 42.3461881477227f, -28.51885465332158f);
    const vec3 c4 = vec3(-11.10743619062271f, -82.66631109428045f, 60.13984767418263f);
    const vec3 c5 = vec3(10.02306557647065f, 71.41361770095349f, -54.07218655560067f);
    const vec3 c6 = vec3(-3.658713842777788f, -22.93153465461149f, 18.19190778539828f);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));

}

static vec3 magma(float t) {

    const vec3 c0 = vec3(-0.002136485053939582f, -0.000749655052795221f, -0.005386127855323933f);
    const vec3 c1 = vec3(0.2516605407371642f, 0.6775232436837668f, 2.494026599312351f);
    const vec3 c2 = vec3(8.353717279216625f, -3.577719514958484f, 0.3144679030132573f);
    const vec3 c3 = vec3(-27.66873308576866f, 14.26473078096533f, -13.64921318813922f);
    const vec3 c4 = vec3(52.17613981234068f, -27.94360607168351f, 12.94416944238394f);
    const vec3 c5 = vec3(-50.76852536473588f, 29.04658282127291f, 4.23415299384598f);
    const vec3 c6 = vec3(18.65570506591883f, -11.48977351997711f, -5.601961508734096f);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));

}

static vec3 inferno(float t) {

    const vec3 c0 = vec3(0.0002189403691192265f, 0.001651004631001012f, -0.01948089843709184f);
    const vec3 c1 = vec3(0.1065134194856116f, 0.5639564367884091f, 3.932712388889277f);
    const vec3 c2 = vec3(11.60249308247187f, -3.972853965665698f, -15.9423941062914f);
    const vec3 c3 = vec3(-41.70399613139459f, 17.43639888205313f, 44.35414519872813f);
    const vec3 c4 = vec3(77.162935699427f, -33.40235894210092f, -81.80730925738993f);
    const vec3 c5 = vec3(-71.31942824499214f, 32.62606426397723f, 73.20951985803202f);
    const vec3 c6 = vec3(25.13112622477341f, -12.24266895238567f, -23.07032500287172f);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));
}
// --------------------------------------------------------------------==

// https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
static float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

static float base( float val ) {
    if ( val <= -0.75f ) return 0.0f;
    else if ( val <= -0.25f ) return interpolate( val, 0.0f, -0.75f, 1.0f, -0.25f );
    else if ( val <= 0.25f ) return 1.0f;
    else if ( val <= 0.75f ) return interpolate( val, 1.0f, 0.25f, 0.0f, 0.75f );
    else return 0.0f;
}

static float red( float gray ) {
    return base( gray - 0.5f );
}
static float green( float gray ) {
    return base( gray );
}
static float blue( float gray ) {
    return base( gray + 0.5f );
}

static vec3 jet(float t) {
  vec3 col;
  // [0, 1] to [-1, 1]
  col[0] = red(2.0f * t - 1.0f);
  col[1] = green(2.0f * t - 1.0f);
  col[2] = blue(2.0f * t - 1.0f);

  return col;
}

inline uint8_t ftoc(float x)
{
  int i = int(x * 255.0f);
  return uint8_t(std::max(0, std::min(255, i)));
}


static int CreateColorMapImage(NVGcontext* ctx, const std::string &cmap)
{
  const int w = 256; // TODO

  // Create 1D image
  std::vector<uint8_t> image_data(w * 4);

  for (size_t i = 0; i < w; i++) {
    // inclusive end: t = [0.0, 1.0]
    float t = float(i) / float(w-1);

    vec3 col;
    if (cmap.compare("magma") == 0) {
      col = magma(t);
    } else if (cmap.compare("jet") == 0) {
      col = jet(t);
    } else if (cmap.compare("plasma") == 0) {
      col = plasma(t);
    } else if (cmap.compare("inferno") == 0) {
      col = inferno(t);
    } else if (cmap.compare("viridis") == 0) {
      col = viridis(t);
    } else {
      col = viridis(t);
    }

    image_data[4 * i + 0] = ftoc(col[0]);
    image_data[4 * i + 1] = ftoc(col[1]);
    image_data[4 * i + 2] = ftoc(col[2]);
    image_data[4 * i + 3] = 255;

  }

  int id = nvgCreateImageRGBA(ctx, w, /* height */1, /* flags */0, image_data.data());

  return id;
}



class Plot::Impl
{
 public:
  Impl() {
    vg = nvgCreateRT(NVG_DEBUG, width * supersampling_level, height * supersampling_level);

    if (!vg) {
      return;
    }

    // Create color map
    cmap_ids["viridis"] = CreateColorMapImage(vg, "viridis");
    cmap_ids["jet"] = CreateColorMapImage(vg, "jet");
    cmap_ids["magma"] = CreateColorMapImage(vg, "magma");
    cmap_ids["inferno"] = CreateColorMapImage(vg, "inferno");
    cmap_ids["plasma"] = CreateColorMapImage(vg, "plasma");

    nvgClearBackgroundRT(vg, 1.0f, 1.0f, 1.0f, 1.0f);

    float pxRatio = 1.0f; // FIXME
    nvgBeginFrame(vg, width * supersampling_level, height * supersampling_level, pxRatio);

  }


  ~Impl() {
    if (vg) {

      for (auto id : cmap_ids) {
        nvgDeleteImage(vg, id.second);
      }

      nvgDeleteRT(vg);
      vg = nullptr;
    }
  }

  Impl(const Impl&) = delete;

  int GetColorMapImage(const std::string &name) {
    if (cmap_ids.count(name)) {
      return cmap_ids[name];
    } else {
      // Use default colormap.
      // TODO(LTE): Print warnings
      return cmap_ids["viridis"];
    }
  }

  int width = 512;
  int height = 512;
  NVGcontext *vg = nullptr;

 private:

  std::map<std::string, int> cmap_ids;

  int supersampling_level = 2; // 2x2 supersampling
};

Plot::Plot()
  : _aa_samples(1), _impl(new Plot::Impl) {

  if (_impl->vg == nullptr) {
    _errs = "Failed to initialize NanoVG context.\n";
  }
}

Plot::~Plot()
{
  delete _impl;
}

bool Plot::valid() {
  if (_impl && _impl->vg) {
    return true;
  }

  return false;
}

bool Plot::render()
{
  if (!valid()) {
    return false;
  }

  nvgEndFrame(_impl->vg);

  return true;
}

bool Plot::readfig(int *width, int *height, std::vector<uint8_t> *image)
{
  if (!valid()) {
    return false;
  }

  if (!width  || !height || !image) {
    _errs += "Input argument is null for readfig()\n";
    return false;
  }

  unsigned char *rgba = nvgReadPixelsRT(_impl->vg);

  int w = _impl->width;
  int h = _impl->height;

  (*width) = w;
  (*height) = h;
  image->resize(size_t(w * h * 4));
  memcpy(image->data(), rgba, size_t(w * h * 4));

  return false;
}


static std::string GetFileExtension(const std::string &filename) {
  if (filename.find_last_of(".") != std::string::npos)
    return filename.substr(filename.find_last_of(".") + 1);
  return "";
}

bool Plot::savefig(const std::string &filename)
{
  if (!valid()) {
    return false;
  }

  std::string ext = GetFileExtension(filename);

  if ((ext.compare("png") == 0) ||
      (ext.compare("jpg") == 0)) {
    // ok
  } else {
    _errs += "No extension in filename or unsupported file format(note that filename is case-sensitive). ext = " + ext + "\n";
    return false;
  }

  unsigned char *rgba = nvgReadPixelsRT(_impl->vg);

  // TODO(LTE): Apply sRGB conversion?

  if (ext.compare("png") == 0) {

    int n = stbi_write_png(filename.c_str(), _impl->width, _impl->height, 4, rgba, _impl->width * 4);
    return (n > 0);

  } else if (ext.compare("jpg") == 0) {
    int n = stbi_write_jpg(filename.c_str(), _impl->width, _impl->height, 4, rgba, /* quality */97);
    return (n > 0);
  }

  return false;
}

bool Plot::matshow(const float *data, int width, int height)
{
  if (!valid()) {
    return false;
  }

  // 2x2 pixel for each element.
  int cmap_id = _impl->GetColorMapImage("viridis");

  NVGpaint img_paint = nvgImagePattern(_impl->vg, 0.0f, 0.0f, width * 2, height * 2, /* angle */0.0f, cmap_id, /* alpha */1.0f);

  nvgBeginPath(_impl->vg);
  nvgRect(_impl->vg, _offset[0], _offset[1], width * 2, height * 2);
  nvgFillPaint(_impl->vg, img_paint);
  nvgFill(_impl->vg);
}

} // namespace tinyplotlib
