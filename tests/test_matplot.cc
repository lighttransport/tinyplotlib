#include "tinyplotlib.h"

#include <cstdio>
#include <cstdlib>
#include <random>

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

inline uint8_t ftoc(float x)
{
  int i = int(x * 255.0f);
  return uint8_t(std::max(0, std::min(255, i)));
}

static int CreateImageOfMatrix(NVGcontext* ctx, int w, int h)
{
  // RGBA
  std::vector<uint8_t> image_data(w * h * 4, 0);

  std::mt19937_64 engine;
  std::uniform_real_distribution<float> udist;

  // generate a matrix with random value
  for (size_t j = 0; j < h; j++) {
    for (size_t i = 0; i < w; i++) {
      float t = udist(engine);
      vec3 col = viridis(t);

      image_data[4 * (j * w + i) + 0] = ftoc(col[0]);
      image_data[4 * (j * w + i) + 1] = ftoc(col[1]);
      image_data[4 * (j * w + i) + 2] = ftoc(col[2]);
      image_data[4 * (j * w + i) + 3] = 255;

    }
  }

  int id = nvgCreateImageRGBA(ctx, w, h, /* flags */NVG_IMAGE_NEAREST, image_data.data());

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

  int matsize = 32;
  int matdispsize = 128;

  int mat_image_id = CreateImageOfMatrix(vg, matsize, matsize);

  NVGpaint mat_img_paint = nvgImagePattern(vg, 0.0f, 0.0f, float(matdispsize), float(matdispsize), /* angle */0.0f, mat_image_id, /* alpha */1.0f);

  nvgClearBackgroundRT(vg, 0.3f, 0.4f, 0.54f, 1.0f);

  float pxRatio = 1.0f;
  nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

  // viridis
  {
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, float(matdispsize), float(matdispsize));
    nvgFillPaint(vg, mat_img_paint);
    nvgFill(vg);
  }

  nvgEndFrame(vg);

  unsigned char *rgba = nvgReadPixelsRT(vg);
  stbi_write_png("matplot.png", winWidth, winHeight, 4, rgba, winWidth * 4);

  nvgDeleteImage(vg, mat_image_id);
  nvgDeleteRT(vg);


  return EXIT_SUCCESS;
};
