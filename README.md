# Tinyplotlib

Tiny plotting library in C++11.

Tinyplotlib uses NanoVG + NanoRT(CPU raytracer) to draw plots into memory and do not require graphics API(e.g. X11, OpenGL) and its environment(header files/libraries).

## Use case

* Add data visualization/plotting feature to your C++11 based renderer.
* Add data visualization/plotting feature to your C++11 based image processing application.
* Add data visualization/plotting feature to your C++11 based machine learning application.

## Status

Very early, proof of concept stage. No API yet.

## Requirements

* C++11 compiler
* CMake(for running tests)

### Supported colormap

`viridis`(default), `plasma`, `magma`, `inferno`, `jet`

## TODO

* [ ] Show legend.
* [ ] Show tick
* [ ] Show title/text

## License

MIT license.

### Third party license

* nanovg : zlib license.
* nanort, nanovg-nanort : MIT license.
* stb : Public domain.
* colormap : CC-0
* Roboto-Mono font : Apache 2.0 License. https://fonts.google.com/specimen/Roboto+Mono

