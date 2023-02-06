# gtam
Tiny C++ OpenGL rendering engine with C and Python bindings.

## Building

> Requirements: `clang`, `ninja`, `python (>2.7 or >=3)`

1. Run `<python> config.py` (once)
2. Building
   - Run `ninja -C gtamfx` (to build shared library, `gtamfx/build/libgtamfx.so`)<br>
   - Or run `ninja -C gtamfx test` (to build C++ example, equivalent to python example)
2. Using
   - Make your game in C/C++ or use the python bindings!
   - Or run `./gtamfx/build/main` to see example (notice the relative path)

## Python bindings

The bindings are implemented using `ctypes` and loading a shared library (TODO: or dll)

### Example

> Requirements: be in the repository root folder, `python (>=3.6)`

1. Run `<python> -m pygtamfx.example.test` (I hate python modules)
2. Enjoy

# TODO

- [ ] Make `config.py` paths more customizable.
- [ ] Test `config.py` on MacOS
- [ ] Test `config.py` on Windows and MSYS2/Cygwin
- [ ] Test `pygtamfx.py` on Windows and MSYS2/Cygwin
