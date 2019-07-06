#ifndef TINYPLOTLIB_H_
#define TINYPLOTLIB_H_

#include <vector>
#include <string>

namespace tinyplotlib {

class Plot
{
 public:
  Plot();
  ~Plot();

  Plot &operator=(const Plot &) = delete;

  ///
  /// Check if this Plot instance is valid.
  /// If this returns false, none of Plot methods works.
  ///
  bool valid();

  ///
  /// Set antialias level.
  /// @param[in] samples Antialias samples per dimension. Total number of AA samples is `samples` * `samples`.
  ///
  void set_antialias(int samples) {
    _aa_samples = samples;
  }

  ///
  /// Plot data. Rendering is done into the memory.
  /// Thus application must call `readfig` or `savefig` afterwards.
  /// @return true upon success. You can get error message using `get_errors` when returned value is false.
  ///
  bool render();

  ///
  /// Readback figure to byte array.
  /// Pixel format is 8-byte RGBA.
  ///
  /// @param[out] width Image width
  /// @param[out] height Image height
  /// @param[out] image Image data(RGBA * width * height)
  /// @param[out] err Error message
  /// @return true upon success. You can get error message using `get_errors` when returned value is false.
  ///
  bool readfig(int *width, int *height, std::vector<uint8_t> *image);

  ///
  /// Save figure to a file.
  /// Supported file extension is `.png` or `.jpg`
  /// Image are saved as RGB for `jpg`, RGBA for `png`
  ///
  /// @param[in] filename Image filename.
  /// @param[in] height Image height
  /// @param[out] image Image data(RGBA * width * height)
  /// @return true upon success. You can get error message using `get_errors` when returned value is false.
  ///
  bool savefig(const std::string &filename);

  ///
  /// Show matrix or array data.
  ///
  bool matshow(const float *data, int width, int height);

  ///
  /// Get error messages.
  ///
  std::string get_errors() const {
    return _errs;
  }

 private:
  int _aa_samples = 1;

  int _offset[2] = {8, 8}; // FIXME(LTE): Compute good initial offset(pixel margin)

  std::string _errs;

  class Impl;
  Impl *_impl;
};

class Subplot
{
 public:
  Subplot();
  ~Subplot() = default;

  Subplot(const Subplot &) = default;
  Subplot &operator=(const Subplot &) = default;


  void set_title(const std::string &title) {
    _title = title;
  }

  const std::string title() const {
    return _title;
  }


 private:
  std::string _title;

};



} // namespace tinyplotlib

#endif // TINYPLOTLIB_H_
