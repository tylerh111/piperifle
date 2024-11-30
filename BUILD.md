# Piperifle Build

```bash
meson setup      build --reconfigure
meson compile -C build -j4
meson test    -C build
meson dist    -C build

# or (worse)

mkdir build
cd build

meson setup . .. --reconfigure
meson compile
meson test
meson dist
```



