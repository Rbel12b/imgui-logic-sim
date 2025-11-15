# imgui-logic-sim

A c++ logic simulator built from a node editor, with imgui.

## Building

### Linux

Building just the executable (for debugging):

```bash
./setup.sh # only needed once
cmake -B ./build .
cmake build ./build
```

Building the appimage:

```bash
./build-appimage.sh
```

### Windows

The windows builds are cross compiled on linux with `mingw-w64-gcc`.

```bash
./build-win-installer.sh
```
