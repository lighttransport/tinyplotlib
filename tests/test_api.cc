#include "tinyplotlib.h"

#include <iostream>

int main(int argc, char **argv)
{
  tinyplotlib::Plot plt;

  bool ret = plt.savefig("test.png");
  if (!ret) {
    std::cerr << plt.get_errors() << "\n";
  }

  return 0;
}
