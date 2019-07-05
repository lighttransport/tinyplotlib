# Tinyplotlib

Tiny plotting library in C++11.

Tinyplotlib is primarily designed for generating plot images in offscreen rendering(e.g. on the server).

Tinyplotlib uses NanoVG + NanoRT(CPU raytracer) to draw plots into memory and do not require graphics API(e.g. X11, OpenGL) and its environment(header files/libraries).

## Requirements

* C++11 compiler
* CMake(for running tests)

## License

MIT license.

### Third party license

* nanovg : zlib license.
* nanort, nanovg-nanort : MIT license.
* stb : Public domain.
* colormap : CC-0

