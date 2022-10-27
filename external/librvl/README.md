# librvl

`librvl` is the reference library for reading/writing Regular VoLumetric format.

Currenty, librvl version has not reach v1.0.0. The API and the specification are
subject to change for each minor release.

## Build and Install

`librvl` uses CMake to configure the build system.

The steps to build `librvl` and install it in a designated location are as follows:

```shell
# Generate CMake cache
cmake -S <path-to-source-tree> -B <path-to-build-tree> -D CMAKE_BUILD_TYPE=Release

# Bulid the shared library
cmake --build <path-to-build-tree>

# Install the library
cmake --install <path-to-build-tree> --prefix <path-to-install-dir>
```

## Usage

Use `librvl` in your CMake project by setting the `CMAKE_PREFIX_PATH` variable.

For example,

```shell
# Define CMAKE_PREFIX_PATH in the command-line
cmake -S <path-to-source-tree> -B <path-to-build-tree> -D CMAKE_BUILD_TYPE=Debug -D CMAKE_PREFIX_PATH=<path-to-install-dir>
```

## Documentation

The public API and its documentation are in `rvl.h` file.
