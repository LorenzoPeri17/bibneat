# Installation Guide for bibneat

`bibneat` depends on:

- The C++ standard library
- [libcurl](https://curl.se/libcurl/) (for web requests)
- [ICU4C](https://icu.unicode.org/) (for Unicode and normalization magic)

## Recommended: CMake + vcpkg (Portable, Easy)

The most portable and reliable way to build bibneat (binaries and library) is with CMake, using [vcpkg](https://github.com/microsoft/vcpkg) to manage dependencies. This project uses vcpkg in manifest mode, so all dependencies are listed in `vcpkg.json` and will be installed automatically.

### 1. Install vcpkg

Follow the [official Microsoft documentation](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-bash) for detailed instructions. But in a nutshell:

- Clone the `vcpkg` repo from the official microsoft github

```bash
git clone https://github.com/microsoft/vcpkg.git
```

- Run the bootstrap script

```bash
cd vcpkg && ./bootstrap-vcpkg.sh
```

- Configure the `VCPKG_ROOT` environment variable, either via:
  - Setting the environment variables in your shell (e.g. in `*sh`)

    ```bash
    export VCPKG_ROOT=/path/to/vcpkg
    export PATH=$VCPKG_ROOT:$PATH
    ```

  - Configuring them in your `CMakeUSerPresets.json` file

    ```json
    "environment": {
        "VCPKG_ROOT": "<path/to/vcpkg>",
        "PATH": "$env{VCPKG_ROOT}:$env{PATH}"
      }
    ```

### 2. Install dependencies and build

From your project root:

```bash
cmake --preset=release
cmake --build build
```

- This will build both the shared library (`libbibneat`) and the binaries (`bibneat`, `bibgrab`).
- Binaries will be in `bin/`, libraries in `lib/`, headers in `include/bibneat`.

> You can still use
>
> ```bash
> make clean
> ```
>
> to clean up the build directory. It has no use post-build :)

### 3. Customizing the build

If you want to modify the build presets (e.g., set environment variables like `VCPKG_ROOT`), you can create a `CMakeUserPresets.json` in your project root. See the provided `CMakeUserPresets.json.example` for a template.

---

## Alternative: Makefile (Manual Dependency Management)

If you prefer to handle dependencies yourself (e.g., with `brew install curl icu` on macOS or `apt install libcurl4-openssl-dev libicu-dev` on Ubuntu), you can use the provided Makefile:

```bash
make
```

- This will build the binaries and shared library, linking dynamically to your system's libraries.
- On macOS, you may need to ensure the C++ STL headers are available in your `CPPFLAGS` (either via `xcode-select --install` or by installing LLVM with `brew install llvm`).

---

## Summary

- **CMake + vcpkg**: Most portable, easiest for all platforms, handles dependencies for you.
- **Makefile**: For advanced users who want to manage dependencies themselves.

If you run into trouble, check the README, or open an issue. Happy TeXing!
