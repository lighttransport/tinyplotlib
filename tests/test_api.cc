#include "tinyplotlib.h"

#include <iostream>
#include <random>

std::vector<float> CreateRandomUniformMatrix(size_t w, size_t h) {
  std::vector<float> data(w * h);

  std::mt19937_64 engine;
  std::uniform_real_distribution<float> udist;

  // generate a matrix with random value
  for (size_t j = 0; j < h; j++) {
    for (size_t i = 0; i < w; i++) {
      float t = udist(engine);
      data[j * w + i] = t;
    }
  }

  return data;
}

int main(int argc, char **argv)
{
  std::vector<float> mat = CreateRandomUniformMatrix(64, 64);

  tinyplotlib::Plot plt;

  plt.matshow(mat.data(), 64, 64);

  plt.text("my awesome matrix", 64, 300 + 32, /* font_size */20);

  plt.colorbar();

  plt.render();

  bool ret = plt.savefig("test.png");
  if (!ret) {
    std::cerr << plt.get_errors() << "\n";
  }

  return 0;
}
