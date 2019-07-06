#include "tinyplotlib.h"

#include <cstdio>
#include <cstdlib>

#if 0
// src
#include "nanovg.h"

#define NANOVG_RT_IMPLEMENTATION
#define NANORT_IMPLEMENTATION
#include "nanovg_rt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef __cplusplus
}
#endif

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
    if (fabs(len) > 1.0e-6f) {
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

inline vec3 vcross(vec3 a, vec3 b) {
  vec3 c;
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
  return c;
}

inline float vdot(vec3 a, vec3 b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

vec3 viridis(float t) {

    const vec3 c0 = vec3(0.2777273272234177, 0.005407344544966578, 0.3340998053353061);
    const vec3 c1 = vec3(0.1050930431085774, 1.404613529898575, 1.384590162594685);
    const vec3 c2 = vec3(-0.3308618287255563, 0.214847559468213, 0.09509516302823659);
    const vec3 c3 = vec3(-4.634230498983486, -5.799100973351585, -19.33244095627987);
    const vec3 c4 = vec3(6.228269936347081, 14.17993336680509, 56.69055260068105);
    const vec3 c5 = vec3(4.776384997670288, -13.74514537774601, -65.35303263337234);
    const vec3 c6 = vec3(-5.435455855934631, 4.645852612178535, 26.3124352495832);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));

}
vec3 plasma(float t) {

    const vec3 c0 = vec3(0.05873234392399702, 0.02333670892565664, 0.5433401826748754);
    const vec3 c1 = vec3(2.176514634195958, 0.2383834171260182, 0.7539604599784036);
    const vec3 c2 = vec3(-2.689460476458034, -7.455851135738909, 3.110799939717086);
    const vec3 c3 = vec3(6.130348345893603, 42.3461881477227, -28.51885465332158);
    const vec3 c4 = vec3(-11.10743619062271, -82.66631109428045, 60.13984767418263);
    const vec3 c5 = vec3(10.02306557647065, 71.41361770095349, -54.07218655560067);
    const vec3 c6 = vec3(-3.658713842777788, -22.93153465461149, 18.19190778539828);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));

}

vec3 magma(float t) {

    const vec3 c0 = vec3(-0.002136485053939582, -0.000749655052795221, -0.005386127855323933);
    const vec3 c1 = vec3(0.2516605407371642, 0.6775232436837668, 2.494026599312351);
    const vec3 c2 = vec3(8.353717279216625, -3.577719514958484, 0.3144679030132573);
    const vec3 c3 = vec3(-27.66873308576866, 14.26473078096533, -13.64921318813922);
    const vec3 c4 = vec3(52.17613981234068, -27.94360607168351, 12.94416944238394);
    const vec3 c5 = vec3(-50.76852536473588, 29.04658282127291, 4.23415299384598);
    const vec3 c6 = vec3(18.65570506591883, -11.48977351997711, -5.601961508734096);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));

}

vec3 inferno(float t) {

    const vec3 c0 = vec3(0.0002189403691192265, 0.001651004631001012, -0.01948089843709184);
    const vec3 c1 = vec3(0.1065134194856116, 0.5639564367884091, 3.932712388889277);
    const vec3 c2 = vec3(11.60249308247187, -3.972853965665698, -15.9423941062914);
    const vec3 c3 = vec3(-41.70399613139459, 17.43639888205313, 44.35414519872813);
    const vec3 c4 = vec3(77.162935699427, -33.40235894210092, -81.80730925738993);
    const vec3 c5 = vec3(-71.31942824499214, 32.62606426397723, 73.20951985803202);
    const vec3 c6 = vec3(25.13112622477341, -12.24266895238567, -23.07032500287172);

    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));
}

// https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
static float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

static float base( float val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

static float red( float gray ) {
    return base( gray - 0.5 );
}
static float green( float gray ) {
    return base( gray );
}
static float blue( float gray ) {
    return base( gray + 0.5 );
}

vec3 jet(float t) {
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

int CreateViridisColorMapImage(NVGcontext* ctx)
{
  const int w = 256;

  // Create 1D image
  std::vector<uint8_t> image_data(w * 4);

  for (size_t i = 0; i < w; i++) {
    // inclusive end: t = [0.0, 1.0]
    float t = float(i) / float(w-1);
    vec3 col = viridis(t);

    image_data[4 * i + 0] = ftoc(col[0]);
    image_data[4 * i + 1] = ftoc(col[1]);
    image_data[4 * i + 2] = ftoc(col[2]);
    image_data[4 * i + 3] = 255;

  }

  int id = nvgCreateImageRGBA(ctx, w, /* height */1, /* flags */0, image_data.data());

  return id;
}

int CreatePlasmaColorMapImage(NVGcontext* ctx)
{
  const int w = 256;

  // Create 1D image
  std::vector<uint8_t> image_data(w * 4);

  for (size_t i = 0; i < w; i++) {
    // inclusive end: t = [0.0, 1.0]
    float t = float(i) / float(w-1);
    vec3 col = plasma(t);

    image_data[4 * i + 0] = ftoc(col[0]);
    image_data[4 * i + 1] = ftoc(col[1]);
    image_data[4 * i + 2] = ftoc(col[2]);
    image_data[4 * i + 3] = 255;

  }

  int id = nvgCreateImageRGBA(ctx, w, /* height */1, /* flags */0, image_data.data());

  return id;
}

int CreateMagmaColorMapImage(NVGcontext* ctx)
{
  const int w = 256;

  // Create 1D image
  std::vector<uint8_t> image_data(w * 4);

  for (size_t i = 0; i < w; i++) {
    // inclusive end: t = [0.0, 1.0]
    float t = float(i) / float(w-1);
    vec3 col = magma(t);

    image_data[4 * i + 0] = ftoc(col[0]);
    image_data[4 * i + 1] = ftoc(col[1]);
    image_data[4 * i + 2] = ftoc(col[2]);
    image_data[4 * i + 3] = 255;

  }

  int id = nvgCreateImageRGBA(ctx, w, /* height */1, /* flags */0, image_data.data());

  return id;
}

int CreateInfernoColorMapImage(NVGcontext* ctx)
{
  const int w = 256;

  // Create 1D image
  std::vector<uint8_t> image_data(w * 4);

  for (size_t i = 0; i < w; i++) {
    // inclusive end: t = [0.0, 1.0]
    float t = float(i) / float(w-1);
    vec3 col = inferno(t);

    image_data[4 * i + 0] = ftoc(col[0]);
    image_data[4 * i + 1] = ftoc(col[1]);
    image_data[4 * i + 2] = ftoc(col[2]);
    image_data[4 * i + 3] = 255;

  }

  int id = nvgCreateImageRGBA(ctx, w, /* height */1, /* flags */0, image_data.data());

  return id;
}

int CreateJetColorMapImage(NVGcontext* ctx)
{
  const int w = 256;

  // Create 1D image
  std::vector<uint8_t> image_data(w * 4);

  for (size_t i = 0; i < w; i++) {
    // inclusive end: t = [0.0, 1.0]
    float t = float(i) / float(w-1);
    vec3 col = jet(t);

    image_data[4 * i + 0] = ftoc(col[0]);
    image_data[4 * i + 1] = ftoc(col[1]);
    image_data[4 * i + 2] = ftoc(col[2]);
    image_data[4 * i + 3] = 255;

  }

  int id = nvgCreateImageRGBA(ctx, w, /* height */1, /* flags */0, image_data.data());

  return id;
}


int main(int argc, char **argv)
{

  NVGcontext *vg = NULL;

  int winWidth = 1024;
  int winHeight = 1024;

  vg = nvgCreateRT(NVG_DEBUG, winWidth, winHeight);
  if (vg == NULL) {
    fprintf(stderr, "Could not init nanovg.\n");
    return -1;
  }

  int viridis_colormap_image_id = CreateViridisColorMapImage(vg);
  int plasma_colormap_image_id = CreatePlasmaColorMapImage(vg);
  int magma_colormap_image_id = CreateMagmaColorMapImage(vg);
  int inferno_colormap_image_id = CreateInfernoColorMapImage(vg);
  int jet_colormap_image_id = CreateJetColorMapImage(vg);

  NVGpaint viridis_img_paint = nvgImagePattern(vg, 0.0f, 0.0f, 1024.0f, 32.0f, /* angle */0.0f, viridis_colormap_image_id, /* alpha */1.0f);
  NVGpaint plasma_img_paint = nvgImagePattern(vg, 0.0f, 0.0f, 1024.0f, 32.0f, /* angle */0.0f, plasma_colormap_image_id, /* alpha */1.0f);
  NVGpaint magma_img_paint = nvgImagePattern(vg, 0.0f, 0.0f, 1024.0f, 32.0f, /* angle */0.0f, magma_colormap_image_id, /* alpha */1.0f);
  NVGpaint inferno_img_paint = nvgImagePattern(vg, 0.0f, 0.0f, 1024.0f, 32.0f, /* angle */0.0f, inferno_colormap_image_id, /* alpha */1.0f);
  NVGpaint jet_img_paint = nvgImagePattern(vg, 0.0f, 0.0f, 1024.0f, 32.0f, /* angle */0.0f, jet_colormap_image_id, /* alpha */1.0f);

  nvgClearBackgroundRT(vg, 0.3f, 0.4f, 0.54f, 1.0f);

  float pxRatio = 1.0f;
  nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

  // viridis
  {
    int loc = 0;
    nvgBeginPath(vg);
    nvgRect(vg, 0, loc, 1024, 64);
    nvgFillPaint(vg, viridis_img_paint);
    nvgFill(vg);
  }

  // plasma
  {
    int loc = 64 + 4;
    nvgBeginPath(vg);
    nvgRect(vg, 0, loc, 1024, 64);
    nvgFillPaint(vg, plasma_img_paint);
    nvgFill(vg);
  }

  // magma
  {
    int loc = 128 + 4 * 2;
    nvgBeginPath(vg);
    nvgRect(vg, 0, loc, 1024, 64);
    nvgFillPaint(vg, magma_img_paint);
    nvgFill(vg);
  }

  // inferno
  {
    int loc = 192 + 4 * 3;
    nvgBeginPath(vg);
    nvgRect(vg, 0, loc, 1024, 64);
    nvgFillPaint(vg, inferno_img_paint);
    nvgFill(vg);
  }

  // jet
  {
    int loc = 192 + 64 + 4 * 4;
    nvgBeginPath(vg);
    nvgRect(vg, 0, loc, 1024, 64);
    nvgFillPaint(vg, jet_img_paint);
    nvgFill(vg);
  }
  nvgEndFrame(vg);


  unsigned char *rgba = nvgReadPixelsRT(vg);
  stbi_write_png("colormap.png", winWidth, winHeight, 4, rgba, winWidth * 4);

  nvgDeleteImage(vg, viridis_colormap_image_id);
  nvgDeleteImage(vg, plasma_colormap_image_id);
  nvgDeleteImage(vg, magma_colormap_image_id);
  nvgDeleteImage(vg, inferno_colormap_image_id);
  nvgDeleteImage(vg, jet_colormap_image_id);
  nvgDeleteRT(vg);


  return EXIT_SUCCESS;
};
#endif
