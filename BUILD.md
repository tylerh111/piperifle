# Piperifle Build

```bash
CXX=clang++ meson setup      build --reconfigure
CXX=clang++ meson compile -C build -j4
CXX=clang++ meson test    -C build
CXX=clang++ meson dist    -C build

# or (worse)

mkdir build
cd build

meson setup . .. --reconfigure
meson compile
meson test
meson dist
```



