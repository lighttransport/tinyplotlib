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

### imshow

You can use system's image viewer to view a plotted image using `imshow` method.
Internally it calls image viewer application through `system()` function.

You can change the command argument by `plt.set_imshow_command()`

In default, command is set to

* Windows: `start.exe`(Use Explorer)
* Linux: `display`(Use image magic)
* macOS: `open`(Use Finder)

`imshow()`

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

