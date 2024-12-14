# Piperifle Build

```bash
CXX=clang++ meson setup      build --reconfigure
CXX=clang++ meson compile -C build -j4
CXX=clang++ meson test    -C build -j4 -i
CXX=clang++ meson dist    -C build
```
