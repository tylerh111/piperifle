# Piperifle Build Meson+Conan

```bash
python3 -m venv .venv
source .venv/bin/activate
source .venv/bin/activate # if using fish shell

# build with conan
pip install conan
conan profile detect
conan install . --output-folder build --build missing --profile conanprofile.ini
conan build   . --output-folder build --build missing --profile conanprofile.ini

# build with meson
pip install meson
meson setup      build .
meson compile -C build

# build wiht meson (using conan deps)
pip install conan meson
conan profile detect
conan install . --output-file build --build missing --profile conanprofile.ini
source      build/build-release/conan/conanbuild.sh
bass source build/build-release/conan/conanbuild.sh # if using fish shell
meson setup      build .
meson compile -C build
source      build/build-release/conan/deactivate_conanbuild.sh
bass source build/build-release/conan/deactivate_conanbuild.sh # if using fish shell

```


# Piperifle Build Meson+Vcpkg

```bash

vcpkg new --name piperifle --version 0.0
vcpkg add port boost-test


...
```

