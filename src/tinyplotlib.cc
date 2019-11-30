#include "tinyplotlib.h"

// Use embeded font
#include "roboto-mono-medium.inc"

#include <map>
#include <sstream>
#include <iomanip>

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


static int CreateMatImage(NVGcontext* ctx, int width, int height, const float *data, const std::string &cmap)
{
  std::vector<uint8_t> image_data(size_t(width * height * 4));

  for (size_t j = 0; j < size_t(height); j++) {
    for (size_t i = 0; i < size_t(width); i++) {
      size_t idx = j * size_t(width) + i;
      float t = data[idx];

      // clamp to [0.0, 1.0]
      t = std::min(1.0f, std::max(0.0f, t));

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

      image_data[4 * idx + 0] = ftoc(col[0]);
      image_data[4 * idx + 1] = ftoc(col[1]);
      image_data[4 * idx + 2] = ftoc(col[2]);
      image_data[4 * idx + 3] = 255;
    }
  }

  int id = nvgCreateImageRGBA(ctx, width, height, /* flags */NVG_IMAGE_NEAREST, image_data.data());

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

    // Setup fonts.
    font_ids["roboto"] = nvgCreateFontMem(vg, "roboto", reinterpret_cast<unsigned char *>(const_cast<unsigned int *>(roboto_mono_medium_data)), roboto_mono_medium_size, /* freeData */0);

    nvgClearBackgroundRT(vg, 1.0f, 1.0f, 1.0f, 1.0f);

    float pxRatio = 1.0f; // FIXME
    nvgBeginFrame(vg, width * supersampling_level, height * supersampling_level, pxRatio);

  }


  ~Impl() {
    if (vg) {

      for (auto id : image_ids) {
        nvgDeleteImage(vg, id.second);
      }

      nvgDeleteRT(vg);
      vg = nullptr;
    }
  }

  Impl(const Impl&) = delete;

  void RegisterImage(int id) {
    if (!vg) {
      return;
    }

    if (image_ids.count(id)) {
      // free it
      nvgDeleteImage(vg, id);
    }

    image_ids[id] = id;
  }

  int width = 512;
  int height = 512;
  NVGcontext *vg = nullptr;

 private:

  std::map<std::string, int> font_ids; // <font_name, nvg id>
  std::map<int, int> image_ids; // <nvg id, nvg id>


  int supersampling_level = 1; // n x n supersampling
};

Plot::Plot()
  : _aa_samples(1), _impl(new Plot::Impl) {

  if (_impl->vg == nullptr) {
    _errs = "Failed to initialize NanoVG context.\n";
  }

#if defined(_WIN32)
  _imshow_command = "start";
#elif defined(__APPLE__)
  _imshow_command = "open";
#else
  // Assume unixish sytem
  _imshow_command = "display";
#endif

  cmap = "viridis";
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

  int mat_id = CreateMatImage(_impl->vg, width, height, data, /* colormap */"viridis");

  _impl->RegisterImage(mat_id);

  // HACK
  int y_offt = 32;

  // n x n pixel for each element.
  // TODO(LTE): Compute good scaling factor based on canvas size and matrix size.
  int scale = 4;
  NVGpaint img_paint = nvgImagePattern(_impl->vg, 0.0f, 0.0f, width * scale, height * scale, /* radian */0.0f, mat_id, /* alpha */1.0f);

  nvgBeginPath(_impl->vg);
  nvgRect(_impl->vg, _offset[0], y_offt, width * scale, height * scale);
  nvgFillPaint(_impl->vg, img_paint);
  nvgFill(_impl->vg);

  return true;
}

bool Plot::text(const std::string &text, int x, int y, int font_size)
{
  if (!valid()) {
    return false;
  }

  nvgFontSize(_impl->vg, float(font_size));
  nvgFontFace(_impl->vg, "roboto");
  nvgFillColor(_impl->vg, nvgRGBA(32, 32, 32, 192));

  nvgTextAlign(_impl->vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
  nvgText(_impl->vg, x, y, text.c_str(), nullptr);

  return true;
}

bool Plot::colorbar()
{
  if (!valid()) {
    return false;
  }

  if (!valid()) {
    return false;
  }

  const size_t image_size = 256;
  std::vector<float> values(image_size);
  for (size_t i = 0; i < image_size; i++) {
    // end is inclusive.
    values[i] = float(i) / float(image_size - 1);
  }

  int mat_id = CreateMatImage(_impl->vg, image_size, 1, values.data(), cmap);

  _impl->RegisterImage(mat_id);

  int bar_width = 16;
  // TODO(LTE): horizontal
  const float radian = -90.0f * (3.141592f / 180.0f);

  float y_offt = 32.0f;
  NVGpaint img_paint = nvgImagePattern(_impl->vg, 0.0f, y_offt, image_size, bar_width, radian, mat_id, /* alpha */1.0f);

  // draw border
  nvgBeginPath(_impl->vg);
  nvgRect(_impl->vg, 314-1, y_offt, 32+2, image_size+1); // +1 for hack
  nvgFillColor(_impl->vg, nvgRGBA(8, 8, 8, 255));
  nvgFill(_impl->vg);

  // draw cmap bar
  nvgBeginPath(_impl->vg);
  nvgRect(_impl->vg, 314, y_offt+1, 32, image_size-1);
  nvgFillPaint(_impl->vg, img_paint);
  nvgFill(_impl->vg);


  {
    // show tick
    int num_ticks = 10;
    float tick_len = 8.0f;

    float y_step = float(image_size) / float(num_ticks);

    nvgStrokeWidth(_impl->vg, 1.0f);
    nvgStrokeColor(_impl->vg, nvgRGBA(0, 0, 0, 255));
    nvgBeginPath(_impl->vg);

    float x_offt = 314.0f + 32.0f;
    float tick_y = y_offt;
    for (int i = 0; i <= num_ticks; i++) {

      nvgMoveTo(_impl->vg, x_offt, tick_y);
      nvgLineTo(_impl->vg, x_offt + tick_len, tick_y);

      tick_y += y_step;
    }

    nvgStroke(_impl->vg);

    // tick label
    x_offt = 314.0f + 32.0f + 12;
    tick_y = y_offt;
    for (int i = 0; i <= num_ticks; i++) {
      std::stringstream ss;
      ss << std::setprecision(2) << float(i) / float(num_ticks);
      std::string label = ss.str();
      text(label, int(x_offt), int(tick_y), 18);
      tick_y += y_step;
    }

  }

  return true;
}

bool Plot::imshow()
{
  if (_imshow_command.empty()) {
    return false;
  }

  render();

  std::string tmp_filename = "untitled.png"; // TODO(LTE): Generate unique tmp name.

  bool ret = savefig(tmp_filename);
  if (!ret) {
    return false;
  }

  std::stringstream ss;
  ss << _imshow_command << " " << tmp_filename;

  std::string cmd = ss.str();

  int retcode = system(cmd.c_str());
  // TODO(LTE): Check retcode.
  (void)retcode;

  return true;
}


} // namespace tinyplotlib
