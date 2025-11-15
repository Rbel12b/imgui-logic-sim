# {{PROJECT_NAME}}

A c++ project with imgui and cmake.

## Setup

Run the folowing to set the project name.

```bash
./init_name.py
```

After this running it again will not do anything.

For debugging, run the `setup.sh` to link the required files into the build directory (for creating bundles this isn't needed).

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
